#include "hevc_decoder/syntax/residual_coding.h"

#include <cmath>

#include "hevc_decoder/base/color_util.h"
#include "hevc_decoder/base/coordinate.h"
#include "hevc_decoder/base/plane_util.h"
#include "hevc_decoder/syntax/residual_coding_context.h"
#include "hevc_decoder/vld_decoder/cabac_reader.h"
#include "hevc_decoder/vld_decoder/transform_skip_flag_reader.h"
#include "hevc_decoder/vld_decoder/explicit_rdpcm_flag_reader.h"
#include "hevc_decoder/vld_decoder/explicit_rdpcm_dir_flag_reader.h"
#include "hevc_decoder/vld_decoder/last_sig_coeff_x_prefix_reader.h"
#include "hevc_decoder/vld_decoder/last_sig_coeff_y_prefix_reader.h"
#include "hevc_decoder/vld_decoder/last_sig_coeff_x_suffix_reader.h"
#include "hevc_decoder/vld_decoder/last_sig_coeff_y_suffix_reader.h"
#include "hevc_decoder/vld_decoder/coded_sub_block_flag_reader.h"
#include "hevc_decoder/vld_decoder/sig_coeff_flag_reader.h"
#include "hevc_decoder/vld_decoder/coeff_abs_level_greater_number_flag_reader_context.h"
#include "hevc_decoder/vld_decoder/coeff_abs_level_greater_1_flag_reader.h"
#include "hevc_decoder/vld_decoder/coeff_abs_level_greater_2_flag_reader.h"
#include "hevc_decoder/vld_decoder/coeff_sign_flag_reader.h"
#include "hevc_decoder/vld_decoder/coeff_abs_level_remaining_reader.h"

using boost::multi_array;
using std::array;

class SigCoeffFlagReaderContext : public ISigCoeffFlagReaderContext
{
public:
    SigCoeffFlagReaderContext(const ResidualCoding* residual_coding, 
                              const IResidualCodingContext* rc_context,
                              const Coordinate& current, 
                              bool has_coded_sub_block_on_right,
                              bool has_coded_sub_block_on_bottom,
                              BlockScanOrderProvider::ScanType scan_type)
        : residual_coding_(residual_coding)
        , rc_context_(rc_context)
        , current_(current)
        , has_coded_sub_block_on_right_(has_coded_sub_block_on_right)
        , has_coded_sub_block_on_bottom_(has_coded_sub_block_on_bottom)
        , scan_type_(scan_type)
    {

    }

    virtual ~SigCoeffFlagReaderContext()
    {

    }

private:
    virtual uint32_t GetColorIndex() const override
    {
        return residual_coding_->GetColorIndex();
    }

    virtual bool IsTransformSkipContextEnabled() const override
    {
        return rc_context_->IsTransformSkipContextEnabled();
    }

    virtual bool IsTransformSkip() const override
    {
        return residual_coding_->IsTransformSkip();
    }

    virtual bool IsCUTransquantBypass() const override
    {
        return rc_context_->IsCUTransquantBypass();
    }

    virtual uint32_t GetLog2TransformSizeY() const override
    {
        return residual_coding_->GetLog2TransformSizeY();
    }

    virtual const Coordinate& GetCurrentRelativeCoordinate() const override
    {
        return current_;
    }

    virtual bool HasCodedSubBlockOnRight() const override
    {
        return has_coded_sub_block_on_right_;
    }

    virtual bool HasCodedSubBlockOnBottom() const override
    {
        return has_coded_sub_block_on_bottom_;
    }

    virtual uint32_t GetScanType() const override
    {
        return scan_type_;
    }

    const IResidualCodingContext* rc_context_;
    const ResidualCoding* residual_coding_;
    Coordinate current_;
    bool has_coded_sub_block_on_right_;
    bool has_coded_sub_block_on_bottom_;
    BlockScanOrderProvider::ScanType scan_type_;
};

class CoeffAbsLevelRemainingReaderContext :
    public ICoeffAbsLevelRemainingReaderContext
{
public:
    CoeffAbsLevelRemainingReaderContext(const IResidualCodingContext* context, 
                                        const ResidualCoding* residual_coding)
        : rc_context_(context)
        , residual_coding_(residual_coding)
        , rice_param_(0)
        , abs_level_(0)
        , already_updated_(false)
    {

    }

    virtual ~CoeffAbsLevelRemainingReaderContext()
    {

    }

    virtual bool IsPersistentRiceAdaptationEnabled() const override
    {
        return rc_context_->IsPersistentRiceAdaptationEnabled();
    }

    virtual bool IsTransformSkip() const override
    {
        return residual_coding_->IsTransformSkip();
    }

    virtual bool IsCUTransquantBypass() const override
    {
        return rc_context_->IsCUTransquantBypass();
    }

    virtual uint32_t GetColorIndex() const override
    {
        return residual_coding_->GetColorIndex();
    }

    virtual uint32_t GetRiceParam() const override
    {
        return rice_param_;
    }

    virtual void UpdateRiceParamAndAbsLevel(uint32_t rice_param,
                                            uint32_t abs_level) override
    {
        rice_param_ = rice_param;
        abs_level_ = abs_level;
        already_updated_ = true;
    }

    virtual bool IsUpdatedRiceParamAndAbsLevel() const override
    {
        return already_updated_;
    }

    virtual uint32_t GetAbsLevel() const override
    {
        return abs_level_;
    }

    virtual uint32_t GetBitDepthOfLuma() const override
    {
        return rc_context_->GetBitDepthOfLuma();
    }

    virtual uint32_t GetBitDepthOfChroma() const override
    {
        return rc_context_->GetBitDepthOfChroma();
    }

    virtual bool HasExtendedPrecisionProcessing() const override
    {
        return rc_context_->HasExtendedPrecisionProcessing();
    }

private:
    const IResidualCodingContext* rc_context_;
    const ResidualCoding* residual_coding_;
    uint32_t rice_param_;
    uint32_t abs_level_;
    bool already_updated_;

};

ResidualCoding::ResidualCoding(const Coordinate& current, 
                               uint32_t log2_transform_size_y,
                               uint32_t color_index)
    : color_index_(color_index)
    , current_(current)
    , transform_size_y_(1 << log2_transform_size_y)
    , log2_transform_size_y_(log2_transform_size_y)
    , scan_type_(BlockScanOrderProvider::UP_RIGHT_SCAN)
    , is_transform_skip_(false)
    , is_explicit_rdpcm_(false)
    , transform_coeff_level_(
        boost::extents[transform_size_y_][transform_size_y_])
{

}

ResidualCoding::~ResidualCoding()
{

}

bool ResidualCoding::Parse(CABACReader* cabac_reader, 
                           IResidualCodingContext* context)
{
    if (!cabac_reader || !context)
        return false;

    if (context->IsTransformSkipEnabled() && !context->IsCUTransquantBypass() &&
        (context->GetMaxTransformSkipSize() >= transform_size_y_))
    {
        TransformSkipFlagReader reader(cabac_reader, context->GetCABACInitType(), 
                                       color_index_);

        is_transform_skip_ = reader.Read();
    }
    if ((context->GetCUPredMode() == MODE_INTER) &&
        context->IsExplicitRDPCMEnabled() &&
        (is_transform_skip_ || context->IsCUTransquantBypass()))
    {
        // rdpcm = residual differential pcm, 
        // is_explicit_rdpcm用来标识该残差是否为帧间预测下pcm的残差值
        ExplicitRDPCMFlagReader explicit_rdpcm_flag_reader(
            cabac_reader, context->GetCABACInitType(), color_index_);

        is_explicit_rdpcm_ = explicit_rdpcm_flag_reader.Read();
        if (is_explicit_rdpcm_)
        {
            ExplicitRDPCMDirFlagReader explicit_rdpcm_dir_flag_reader(
                cabac_reader, context->GetCABACInitType(), color_index_);
            bool is_explicit_rdpcm_dir = explicit_rdpcm_dir_flag_reader.Read();
        }
    }
    scan_type_ = DeriveScanType(context);

    uint32_t last_sig_coeff_x = 0;
    uint32_t last_sig_coeff_y = 0;
    bool success = ParseLastSigCoeffPositionValue(cabac_reader, context, 
                                                  &last_sig_coeff_x, 
                                                  &last_sig_coeff_y);
    if (!success)
        return false;

	int32_t last_sub_block_index = 0;
	int32_t last_scan_index = 0;
	success = LocateLastSubBlockAndLastScanPos(last_sig_coeff_x, 
											   last_sig_coeff_y, scan_type_, 
											   &last_sub_block_index, 
											   &last_scan_index);
	if (!success)
		return false;

    uint32_t transform_width_in_sub_block = transform_size_y_ >> 2;
    int32_t sub_block_count =
        static_cast<int32_t>(pow(transform_width_in_sub_block, 2));

    Coordinate last_sig_coeff_coordinate = {last_sig_coeff_x, last_sig_coeff_y};
    multi_array<bool, 1> has_coded_sub_blocks(boost::extents[sub_block_count]);
    auto block_size = BlockScanOrderProvider::GetInstance()->GetBlockSizeType(
        transform_width_in_sub_block);

    CoeffAbsLevelGreaterNumberFlagReaderContext greater_1_flag_reader_context;
    for (int32_t i = last_sub_block_index; i >= 0; --i)
    {
        greater_1_flag_reader_context.Reset();
        success = ParseSingleBlockTransformCoeffLevel(
            cabac_reader, context, &greater_1_flag_reader_context, i, 
            last_sig_coeff_coordinate, last_sub_block_index, last_scan_index, 
            block_size, &has_coded_sub_blocks);

        if (!success)
            return false;
    }

    return true;
}

uint32_t ResidualCoding::GetColorIndex() const
{
    return color_index_;
}

bool ResidualCoding::IsTransformSkip() const
{
    return is_transform_skip_;
}

uint32_t ResidualCoding::GetLog2TransformSizeY() const
{
    return log2_transform_size_y_;
}

bool ResidualCoding::ParseLastSigCoeffPositionValue(
    CABACReader* cabac_reader, IResidualCodingContext* context, 
    uint32_t* last_sig_coeff_x, uint32_t* last_sig_coeff_y)
{
    if (!last_sig_coeff_x || !last_sig_coeff_y)
        return false;

    LastSigCoeffXPrefixReader last_sig_coeff_x_prefix_reader(
        cabac_reader, context->GetCABACInitType(), log2_transform_size_y_, 
        color_index_);
    uint32_t last_sig_coeff_x_prefix = last_sig_coeff_x_prefix_reader.Read();

    LastSigCoeffYPrefixReader last_sig_coeff_y_prefix_reader(
        cabac_reader, context->GetCABACInitType(), log2_transform_size_y_,
        color_index_);
    uint32_t last_sig_coeff_y_prefix = last_sig_coeff_y_prefix_reader.Read();

    auto combine_prefix_and_suffix = [](uint32_t prefix, uint32_t suffix)
    {
        return ((2ui32 + (prefix & 1)) << ((prefix >> 1) - 1)) + suffix;
    };
    if (last_sig_coeff_x_prefix > 3)
    {
        LastSigCoeffXSuffixReader last_sig_coeff_x_suffix_reader(cabac_reader);
        uint32_t last_sig_coeff_x_suffix = 0;
        last_sig_coeff_x_suffix = 
            last_sig_coeff_x_suffix_reader.Read(last_sig_coeff_x_prefix);

        *last_sig_coeff_x = combine_prefix_and_suffix(last_sig_coeff_x_prefix,
                                                      last_sig_coeff_x_suffix);
    }
    else
    {
        *last_sig_coeff_x = last_sig_coeff_x_prefix;
    }
    
    if (last_sig_coeff_y_prefix > 3)
    {
        LastSigCoeffXSuffixReader last_sig_coeff_y_suffix_reader(cabac_reader);
        uint32_t last_sig_coeff_y_suffix = 0;
        last_sig_coeff_y_suffix =
            last_sig_coeff_y_suffix_reader.Read(last_sig_coeff_y_prefix);

        *last_sig_coeff_y = combine_prefix_and_suffix(last_sig_coeff_y_prefix,
                                                      last_sig_coeff_y_suffix);
    }
    else
    {
        *last_sig_coeff_y = last_sig_coeff_y_prefix;
    }
    if (BlockScanOrderProvider::VERTICAL_SCAN == scan_type_)
    {
        uint32_t v = *last_sig_coeff_y;
        *last_sig_coeff_y = *last_sig_coeff_x;
        *last_sig_coeff_x = v;
    }
    return true;
}

bool ResidualCoding::ParseSingleBlockTransformCoeffLevel(
	CABACReader* cabac_reader, IResidualCodingContext* context, 
    CoeffAbsLevelGreaterNumberFlagReaderContext* greater_1_reader_context,
    int32_t sub_block_index, const Coordinate& last_sig_coeff_c, 
    int32_t last_sub_block_index, int32_t last_scan_index, 
    BlockScanOrderProvider::BlockSize transform_block_size,
    multi_array<bool, 1>* has_coded_sub_blocks)
{
    if (!has_coded_sub_blocks)
        return false;

    bool has_coded_sub_block_on_right = false;
    uint32_t right_index = 0;
    bool success = 
        BlockScanOrderProvider::GetInstance()->GetRightIndexByScanType(
            scan_type_, transform_block_size, sub_block_index, &right_index);
    if (success)
        has_coded_sub_block_on_right = (*has_coded_sub_blocks)[right_index];

    bool has_coded_sub_block_on_bottom = false;
    uint32_t bottom_index = 0;
    success =
        BlockScanOrderProvider::GetInstance()->GetBottomIndexByScanType(
            scan_type_, transform_block_size, sub_block_index, &bottom_index);

    if (success)
        has_coded_sub_block_on_bottom = (*has_coded_sub_blocks)[bottom_index];

	bool need_read_the_first_value = true;  // inferSbDcSigCoeffFlag = 0;
    bool is_coded_sub_block = false;
    if ((sub_block_index < last_sub_block_index) && (sub_block_index > 0))
    {
        CodedSubBlockFlagReader reader(cabac_reader, context->GetCABACInitType(),
                                       has_coded_sub_block_on_right, 
                                       has_coded_sub_block_on_bottom,
                                       color_index_);
        (*has_coded_sub_blocks)[sub_block_index] = reader.Read();
        need_read_the_first_value = false;
    }
    else
    {
        (*has_coded_sub_blocks)[sub_block_index] = true;
    }

    bool has_sig_coeff[16] = {};
    // 块有值并且第一个是不需要读取的话，那它必须是有值
    if ((*has_coded_sub_blocks)[sub_block_index] && !need_read_the_first_value)
        has_sig_coeff[0] = true;

    // 如果是指定的最后一个系数的位置，那这个位置应该是有值的，不然就会产生矛盾
    if (last_sub_block_index == sub_block_index)
        has_sig_coeff[last_scan_index] = true;

    int32_t scan_pos =
        sub_block_index == last_sub_block_index ? last_scan_index - 1 : 15;

    Coordinate sub_block_begin_c = 
        BlockScanOrderProvider::GetInstance()->GetScanPosition(
            transform_block_size, scan_type_, sub_block_index);

    uint32_t x_length = sub_block_begin_c.GetX() << 2;
    uint32_t y_length = sub_block_begin_c.GetY() << 2;
    for (; scan_pos >= 0; --scan_pos)
    {
        if ((*has_coded_sub_blocks)[sub_block_index] &&
            ((scan_pos > 0) || (need_read_the_first_value)))
        {
            auto c = BlockScanOrderProvider::GetInstance()->GetScanPosition(
                BlockScanOrderProvider::BLOCK_SIZE_4X4, scan_type_, scan_pos);

            c.OffsetX(x_length);
            c.OffsetY(y_length);
            SigCoeffFlagReaderContext sig_coeff_flag_reader_context(
                this, context, c, has_coded_sub_block_on_right, 
                has_coded_sub_block_on_bottom, scan_type_);

            SigCoeffFlagReader sig_coeff_flag_reader(
                cabac_reader, context->GetCABACInitType(), 
                &sig_coeff_flag_reader_context);

            has_sig_coeff[scan_pos] = sig_coeff_flag_reader.Read();
            if (has_sig_coeff[scan_pos])
                need_read_the_first_value = true;
        }
    }

    return ParseAndDerivedTransformCoeffLevel(cabac_reader, context, 
                                              sub_block_begin_c, sub_block_index,
                                              greater_1_reader_context, 
                                              has_sig_coeff);
}

bool ResidualCoding::ParseAndDerivedTransformCoeffLevel(
    CABACReader* cabac_reader, IResidualCodingContext* context, 
    const Coordinate& sub_block_begin_c, int32_t sub_block_index, 
    CoeffAbsLevelGreaterNumberFlagReaderContext* greater_1_reader_context,
    bool has_sig_coeff[16])
{
    int32_t first_sig_scan_pos = 16;
    int32_t last_sig_scan_pos = -1;
    uint32_t count_of_greater_1 = 0;  // numGreater1Flag
    int32_t last_greater_1_scan_pos = -1;
    array<bool, 16> is_coeff_abs_level_greater_1;
    is_coeff_abs_level_greater_1.fill(false);
    bool has_escape_data_present = false;
    for (int32_t i = 15; i >= 0; --i)
    {
        if (!has_sig_coeff[i])
            continue;

        if (-1 == last_sig_scan_pos)
            last_sig_scan_pos = i;

        first_sig_scan_pos = i;

        if (count_of_greater_1 >= 8)
        {
            has_escape_data_present = true;
            continue;
        }
        greater_1_reader_context->Init(sub_block_index, color_index_);
        CoeffAbsLevelGreater1FlagReader reader(cabac_reader, 
                                               context->GetCABACInitType(), 
                                               greater_1_reader_context, 
                                               color_index_);

        is_coeff_abs_level_greater_1[i] = reader.Read();
        ++count_of_greater_1;

        if (is_coeff_abs_level_greater_1[i] && (-1 == last_greater_1_scan_pos))
            last_greater_1_scan_pos = i;
        else if (is_coeff_abs_level_greater_1[i])
            has_escape_data_present = true;
    }

    bool is_sign_hidden = (last_sig_scan_pos - first_sig_scan_pos > 3);
    if (context->IsCUTransquantBypass() || 
        ((context->GetCUPredMode() == MODE_INTRA) && 
         context->IsImplicitRDPCMEnabled() && is_transform_skip_ &&
         ((context->GetIntraLumaPredMode(current_) == INTRA_ANGULAR10) ||
         (context->GetIntraLumaPredMode(current_) == INTRA_ANGULAR26))) ||
        is_explicit_rdpcm_)
        is_sign_hidden = false;

    array<bool, 16> is_coeff_abs_level_greater_2;
    is_coeff_abs_level_greater_2.fill(false);
    if (last_greater_1_scan_pos != -1)
    {
        CoeffAbsLevelGreater2FlagReader reader(cabac_reader, 
                                               context->GetCABACInitType(), 
                                               greater_1_reader_context, 
                                               color_index_);
        is_coeff_abs_level_greater_2[last_greater_1_scan_pos] = reader.Read();
        if (is_coeff_abs_level_greater_2[last_greater_1_scan_pos])
            has_escape_data_present = true;
    }
    if (has_escape_data_present && context->IsCABACBypassAlignmentEnabled())
        cabac_reader->Align();

    array<bool, 16> is_negative_coeff; // coeff_sign_flag
    is_negative_coeff.fill(false);
    is_sign_hidden &= context->IsSignDataHidingEnabled();
    for (int32_t i = 15; i >= 0; --i)
    {
        if (has_sig_coeff[i] && (!is_sign_hidden || (i != first_sig_scan_pos)))
        {
            CoeffSignFlagReader coeff_sign_flag_reader(
                cabac_reader, context->GetCABACInitType());

            is_negative_coeff[i] = coeff_sign_flag_reader.Read(); 
        }
    }

    return CombineTransformCoeffLevel(cabac_reader, context, 
                                      last_greater_1_scan_pos, 
                                      first_sig_scan_pos, is_sign_hidden, 
                                      sub_block_begin_c, has_sig_coeff, 
                                      is_coeff_abs_level_greater_1,
                                      is_coeff_abs_level_greater_2, 
                                      is_negative_coeff);
}

bool ResidualCoding::CombineTransformCoeffLevel(
    CABACReader* cabac_reader, IResidualCodingContext* context, 
    int32_t last_greater_1_scan_pos, int32_t first_sig_scan_pos, 
    bool is_sign_hidden, const Coordinate& sub_block_begin_c, 
    bool has_sig_coeff[16], 
    const array<bool, 16>& is_coeff_abs_level_greater_1, 
    const array<bool, 16>& is_coeff_abs_level_greater_2, 
    const array<bool, 16>& is_negative_sign)
{
    uint32_t count_of_sig_coeff = 0; // numSigCoeff
    uint32_t sum_abs_level = 0;

    CoeffAbsLevelRemainingReaderContext reader_context(context, this);
    uint32_t x_length = sub_block_begin_c.GetX() << 2;
    uint32_t y_length = sub_block_begin_c.GetY() << 2;
    for (int32_t i = 15; i >= 0; --i)
    {
        if (!has_sig_coeff[i])
            continue;

        uint32_t base_level = 1;
        if (is_coeff_abs_level_greater_1[i])
            ++base_level;

        if (is_coeff_abs_level_greater_2[i])
            ++base_level;

        int32_t max_base_level_value = 1;
        if (count_of_sig_coeff < 8)
            max_base_level_value = (i == last_greater_1_scan_pos) ? 3 : 2;

        uint32_t coeff_abs_level_remaining = 0;
        // 如果等于最大值的话，也就是说有可能会有剩余的值
        if (max_base_level_value == base_level)
        {
            CoeffAbsLevelRemainingReader reader(cabac_reader, &reader_context);
            coeff_abs_level_remaining = reader.Read(base_level);
        }
        int32_t coeff_abs_level_value = 
            static_cast<int32_t>(base_level + coeff_abs_level_remaining);

        if (is_negative_sign[i])
            coeff_abs_level_value = -coeff_abs_level_value;

        if (is_sign_hidden)
        {
            sum_abs_level += abs(coeff_abs_level_value);
            if ((first_sig_scan_pos == i) && ((sum_abs_level & 1) == 1))
                coeff_abs_level_value = -coeff_abs_level_value;
        }
        auto c = BlockScanOrderProvider::GetInstance()->GetScanPosition(
            BlockScanOrderProvider::BLOCK_SIZE_4X4, scan_type_, i);

        uint32_t x = x_length + c.GetX();
        uint32_t y = y_length + c.GetY();
        transform_coeff_level_[y][x] = coeff_abs_level_value;
        ++count_of_sig_coeff;
    }
    return true;
}

BlockScanOrderProvider::ScanType ResidualCoding::DeriveScanType(
    IResidualCodingContext* context)
{
    if (context->GetCUPredMode() != MODE_INTRA)
        return BlockScanOrderProvider::UP_RIGHT_SCAN;

    if ((2 == log2_transform_size_y_) ||
        ((3 == log2_transform_size_y_) &&
         ((0 == color_index_) ||
          IsSingleChromaPerPixel(context->GetChromaFormatType()))))
    {
        IntraPredModeType intra_predicted_mode = INTRA_PLANAR;
        if (0 == color_index_)
            intra_predicted_mode = context->GetIntraLumaPredMode(current_);
        else
            intra_predicted_mode = context->GetIntraChromaPredMode(current_);

        if ((intra_predicted_mode >= INTRA_ANGULAR6) && 
            (intra_predicted_mode <= INTRA_ANGULAR14))
            return BlockScanOrderProvider::VERTICAL_SCAN;
        
        if ((intra_predicted_mode >= INTRA_ANGULAR22) &&
            (intra_predicted_mode <= INTRA_ANGULAR30))
            return BlockScanOrderProvider::HORIZONTAL_SCAN;
    }
    return BlockScanOrderProvider::UP_RIGHT_SCAN;
}

bool ResidualCoding::LocateLastSubBlockAndLastScanPos(
    uint32_t last_sig_coeff_x, uint32_t last_sig_coeff_y, 
    BlockScanOrderProvider::ScanType scan_type, int32_t* last_sub_block_index, 
    int32_t* last_scan_index)
{
    if (!last_scan_index || !last_sub_block_index)
        return false;

    uint32_t transform_width_in_sub_block = 1 << (log2_transform_size_y_ - 2);
    int32_t sub_block_count = 
        static_cast<int32_t>(pow(transform_width_in_sub_block, 2));

    auto block_size = BlockScanOrderProvider::GetInstance()->GetBlockSizeType(
		transform_width_in_sub_block);

	if (BlockScanOrderProvider::UNKNOWN_BLOCK_SIZE == block_size)
		return false;

    const Coordinate last_sig_coordinate(last_sig_coeff_x, last_sig_coeff_y);
    Coordinate sub_block_begin_point;
    for (*last_sub_block_index = sub_block_count - 1; *last_sub_block_index >= 0;
         --*last_sub_block_index)
    {
        sub_block_begin_point = 
            BlockScanOrderProvider::GetInstance()->GetScanPosition(
                block_size, scan_type, *last_sub_block_index);

        sub_block_begin_point.SetX(sub_block_begin_point.GetX() << 2);
        sub_block_begin_point.SetY(sub_block_begin_point.GetY() << 2);
        if (IsPointInSquare(last_sig_coordinate, sub_block_begin_point, 4))
            break;
    }
    if (*last_sub_block_index < 0)
        return false;

	block_size = BlockScanOrderProvider::GetInstance()->GetBlockSizeType(4);
	for (*last_scan_index = 15; *last_scan_index >= 0; --*last_scan_index)
	{
		Coordinate scan_point = 
			BlockScanOrderProvider::GetInstance()->GetScanPosition(
				block_size, scan_type, *last_scan_index);

		scan_point.OffsetX(sub_block_begin_point.GetX());
		scan_point.OffsetY(sub_block_begin_point.GetY());
		if (last_sig_coordinate == scan_point)
			break;
	}
	return *last_scan_index >= 0;
}