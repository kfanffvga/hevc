#include "hevc_decoder/syntax/palette_coding.h"

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/base/color_util.h"
#include "hevc_decoder/partitions/block_scan_order_provider.h"
#include "hevc_decoder/syntax/palette_coding_context.h"
#include "hevc_decoder/syntax/palette_table.h"
#include "hevc_decoder/syntax/delta_qp_context.h"
#include "hevc_decoder/syntax/delta_qp.h"
#include "hevc_decoder/syntax/chroma_qp_offset_context.h"
#include "hevc_decoder/syntax/chroma_qp_offset.h"
#include "hevc_decoder/vld_decoder/palette_predictor_run_reader.h"
#include "hevc_decoder/vld_decoder/num_signalled_palette_entries_reader.h"
#include "hevc_decoder/vld_decoder/new_palette_entries_reader.h"
#include "hevc_decoder/vld_decoder/palette_escape_val_present_flag_reader.h"
#include "hevc_decoder/vld_decoder/num_palette_indices_reader.h"
#include "hevc_decoder/vld_decoder/palette_index_idc_reader.h"
#include "hevc_decoder/vld_decoder/copy_above_indices_for_final_run_flag_reader.h"
#include "hevc_decoder/vld_decoder/palette_transpose_flag_reader.h"
#include "hevc_decoder/vld_decoder/copy_above_palette_indices_flag_reader.h"
#include "hevc_decoder/vld_decoder/palette_run_prefix_reader.h"
#include "hevc_decoder/vld_decoder/palette_run_suffix_reader.h"
#include "hevc_decoder/vld_decoder/palette_escape_val_reader.h"


using std::shared_ptr;
using std::unique_ptr;
using std::vector;
using std::make_pair;
using boost::multi_array;

class DeltaQPContextForPaletteCoding : public IDeltaQPContext
{
public:
    DeltaQPContextForPaletteCoding(IPaletteCodingContext* palette_coding_context)
        : palette_coding_context_(palette_coding_context)
    {

    }

    virtual ~DeltaQPContextForPaletteCoding()
    {

    }

    virtual bool IsCUQPDeltaEnabled() const override
    {
        return palette_coding_context_->IsCUQPDeltaEnabled();
    }

    virtual bool IsCUQPDeltaCoded() const override
    {
        return palette_coding_context_->IsCUQPDeltaCoded();
    }

    virtual void SetCUQPDeltaVal(int32_t cu_qp_delta_val) override
    {
        palette_coding_context_->SetCUQPDeltaVal(cu_qp_delta_val);
    }

    virtual CABACInitType GetCABACInitType() const override
    {
        return palette_coding_context_->GetCABACInitType();
    }

private:
    IPaletteCodingContext* palette_coding_context_;
};

class ChromaQPOffsetContextForPaletteCoding : public IChromaQPOffsetContext
{
public:
    ChromaQPOffsetContextForPaletteCoding(
        IPaletteCodingContext* palette_coding_context)
        : palette_coding_context_(palette_coding_context)
    {

    }

    virtual ~ChromaQPOffsetContextForPaletteCoding()
    {

    }

    virtual CABACInitType GetCABACInitType() const override
    {
        return palette_coding_context_->GetCABACInitType();
    }

    virtual bool IsCUChromaQPOffsetEnable() const override
    {
        return palette_coding_context_->IsCUChromaQPOffsetEnable();
    }

    virtual bool IsCUChromaQPOffsetCoded() const override
    {
        return palette_coding_context_->IsCUChromaQPOffsetCoded();
    }

    virtual uint32_t GetChromaQPOffsetListtLen() const override
    {
        return palette_coding_context_->GetChromaQPOffsetListtLen();
    }

    virtual void SetCUChromaQPOffsetIndex(uint32_t cu_chroma_qp_offset_index)
        override
    {
        palette_coding_context_->SetCUChromaQPOffsetIndex(
            cu_chroma_qp_offset_index);
    }

private:
    IPaletteCodingContext* palette_coding_context_;
};

PaletteCoding::PaletteCoding(uint32_t cb_size_y)
    : cb_size_y_(cb_size_y)
    , current_palette_table_(new PaletteTable())
    , is_copy_above_indices_(boost::extents[cb_size_y][cb_size_y])
    , palette_index_map_(boost::extents[cb_size_y][cb_size_y])
    , palette_escape_values_()
{

}

PaletteCoding::~PaletteCoding()
{

}

bool PaletteCoding::Parse(CABACReader* cabac_reader, 
                          IPaletteCodingContext* context)
{
    if (!ParseCurrentPaletteTable(cabac_reader, context))
        return false;

    bool has_palette_escape_val_present = true;
    if (current_palette_table_->GetEntriesCount() != 0)
    {
        PaletteEscapeValPresentFlagReader reader(cabac_reader);
        has_palette_escape_val_present = reader.Read();
    }

    vector<uint32_t> palette_index_idc;
    bool is_copy_above_indices_for_final_run = false;
    bool is_palette_transpose = false;

    bool success = ParseSampleMapPaletteTableInfo(
        cabac_reader, context, has_palette_escape_val_present, 
        &palette_index_idc, &is_copy_above_indices_for_final_run, 
        &is_palette_transpose);
    if (!success)
        return false;

    if (has_palette_escape_val_present)
    {
        DeltaQP delta_qp;
        DeltaQPContextForPaletteCoding delta_qp_context(context);
        if (!delta_qp.Parse(cabac_reader, &delta_qp_context))
            return false;

        if (context->IsCUTransquantBypass())
        {
            ChromaQPOffset chroma_qp_offset;
            ChromaQPOffsetContextForPaletteCoding 
                chroma_qp_offset_context(context);

            if (!chroma_qp_offset.Parse(cabac_reader, &chroma_qp_offset_context))
                return false;
        }
    }
    success = ParseSampleIndexMapInfo(cabac_reader, context, 
                                      is_copy_above_indices_for_final_run, 
                                      palette_index_idc);
    if (!success)
        return false;

    if (has_palette_escape_val_present)
    {
        return ParseEscapeValueInfo(cabac_reader, context, is_palette_transpose);
    }   

    return true;
}

bool PaletteCoding::ParseCurrentPaletteTable(CABACReader* cabac_reader, 
                                             IPaletteCodingContext* context)
{
    vector<uint32_t> reuse_entry_indices;
    bool success = ParsePredictorPaletteTableReuseEntryIndices(
        cabac_reader, context, &reuse_entry_indices);

    if (!success)
        return false;

    multi_array<uint32_t, 2> new_palette_entries;
    success = ParsePaletteTableOfSelfEntries(cabac_reader, context, 
                                             reuse_entry_indices, 
                                             &new_palette_entries);
    if (!success)
        return false;

    ProductCurrentPaletteTableAndUpdatePredictorPaletteTable(
        context, reuse_entry_indices, new_palette_entries);

    return true;
}

bool PaletteCoding::ParsePredictorPaletteTableReuseEntryIndices(
    CABACReader* cabac_reader, IPaletteCodingContext* context, 
    vector<uint32_t>* reuse_entry_indices)
{
    if (!reuse_entry_indices)
        return false;

    shared_ptr<PaletteTable> predictor_palette_table =
        context->GetPredictorPaletteTable();

    uint32_t color_component_count = 
        GetColorCompomentCount(context->GetChromaFormatType());

    if (color_component_count !=
        predictor_palette_table->GetColorCompomentCount())
        return false;

    // 从预测调色板处得到当前cu需要用的色彩值
    current_palette_table_->Init(color_component_count);
    uint32_t predictor_extries_index = 0;
    while (predictor_extries_index < predictor_palette_table->GetEntriesCount())
    {
        PalettePredictorRunReader reader(cabac_reader);
        uint32_t palette_predictor_run = reader.Read();
        if (1 == palette_predictor_run)
            break;

        if (palette_predictor_run > 1)
            predictor_extries_index += palette_predictor_run - 1;

        reuse_entry_indices->push_back(predictor_extries_index);
        ++predictor_extries_index;
    }
    return true;
}

bool PaletteCoding::ParsePaletteTableOfSelfEntries(
    CABACReader* cabac_reader, IPaletteCodingContext* context, 
    const vector<uint32_t>& reuse_entry_indices, 
    multi_array<uint32_t, 2>* new_palette_entries)
{
    if (!new_palette_entries)
        return false;

    uint32_t color_component_count = 
        GetColorCompomentCount(context->GetChromaFormatType());

    // 读取当前cu自带的调色板的色彩值
    uint32_t num_signalled_palette_entries = 0;
    if (reuse_entry_indices.size() < context->GetPaletteMaxSize())
    {
        NumSignalledPaletteEntriesReader num_signalled_palette_entries_reader(
            cabac_reader);
        num_signalled_palette_entries =
            num_signalled_palette_entries_reader.Read();

        new_palette_entries->resize(
            boost::extents[num_signalled_palette_entries][color_component_count]);
        for (uint32_t i = 0; i < color_component_count; ++i)
        {
            uint32_t bit_depth = 0 == i ? context->GetBitDepthLuma() :
                context->GetBitDepthChroma();

            for (uint32_t j = 0; j < num_signalled_palette_entries; ++j)
            {
                NewPaletteEntriesReader new_palette_entries_reader(cabac_reader);
                uint32_t v = new_palette_entries_reader.Read(bit_depth);
                (*new_palette_entries)[j][i] = v;
            }
        }
    }
    return true;
}

void PaletteCoding::ProductCurrentPaletteTableAndUpdatePredictorPaletteTable(
    IPaletteCodingContext* context, const vector<uint32_t>& reuse_entry_indices, 
    const multi_array<uint32_t, 2>& new_palette_entries)
{
    shared_ptr<PaletteTable> predictor_palette_table = 
        context->GetPredictorPaletteTable();

    current_palette_table_->Init(
        predictor_palette_table->GetColorCompomentCount());

    for (const auto& i : reuse_entry_indices)
        current_palette_table_->PushEntry(predictor_palette_table->GetEntry(i));

    for (const auto& row : new_palette_entries)
    {
        vector<uint32_t> entry;
        for (const auto& value : row)
            entry.push_back(value);

        current_palette_table_->PushEntry(entry);
    }

    PaletteTable cache_palette_table;
    cache_palette_table = *current_palette_table_;
    for (uint32_t i = 0, j = 0; i < predictor_palette_table->GetEntriesCount(); 
         ++i)
    {
        if (cache_palette_table.GetEntriesCount() >= 
            context->GetPredictorPaletteMaxSize())
            break;

        if ((j < reuse_entry_indices.size()) && (reuse_entry_indices[j] == i))
        {
            ++j;
            continue;
        }
        cache_palette_table.PushEntry(predictor_palette_table->GetEntry(i));
    }
    predictor_palette_table->Clear();
    // 此处的更新是更新到了外面，但考虑到这样写太隐晦，因此，将来把其改为setXXX
    *predictor_palette_table = cache_palette_table;
}

bool PaletteCoding::ParseSampleMapPaletteTableInfo(
    CABACReader* cabac_reader, IPaletteCodingContext* context, 
    bool has_palette_escape_val_present, 
    vector<uint32_t>* palette_index_idc_values, 
    bool* is_copy_above_indices_for_final_run, bool* is_palette_transpose)
{
    if (!palette_index_idc_values || !is_copy_above_indices_for_final_run || 
        !is_palette_transpose)
        return false;

    *is_copy_above_indices_for_final_run = false;
    // 猜测,这里的值不重要,因为,可以理解为只要需要用到palette mode,就肯定有
    // is_palette_transpose的值
    *is_palette_transpose = false;

    uint32_t max_palette_index = current_palette_table_->GetEntriesCount() - 1;
    if (has_palette_escape_val_present)
        ++max_palette_index;

    if (max_palette_index > 0)
    {
        NumPaletteIndicesReader num_palette_indices_reader(cabac_reader);
        uint32_t num_palette_indices = 
            num_palette_indices_reader.Read(max_palette_index) + 1;
        if ((max_palette_index - 1) == 0)
        {
            PaletteIndexIdcReader palette_index_idc_reader(cabac_reader);
            uint32_t palette_index_idc = 
                palette_index_idc_reader.Read(true, max_palette_index);

            palette_index_idc_values->push_back(palette_index_idc);
        }
        else
        {
            bool first_invoked = false;
            for (uint32_t i = 0; i < num_palette_indices; ++i)
            {
                PaletteIndexIdcReader palette_index_idc_reader(cabac_reader);
                uint32_t palette_index_idc = palette_index_idc_reader.Read(
                    first_invoked, max_palette_index);

                palette_index_idc_values->push_back(palette_index_idc);
                first_invoked = false;
            }
        }
        CopyAboveIndicesForFinalRunFlagReader 
            copy_above_indices_for_final_flag_reader(
                cabac_reader, context->GetCABACInitType());

        *is_copy_above_indices_for_final_run = 
            copy_above_indices_for_final_flag_reader.Read();

        PaletteTransposeFlagReader palette_transpose_flag_reader(
            cabac_reader, context->GetCABACInitType());
        *is_palette_transpose = palette_transpose_flag_reader.Read();
    }
    return true;
}

bool PaletteCoding::ParseSampleIndexMapInfo(
    CABACReader* cabac_reader, IPaletteCodingContext* context,
    bool is_copy_above_indices_for_final_run,
    const vector<uint32_t>& palette_index_idc_values)
{
    BlockScanOrderProvider::BlockSize block_size = 
        BlockScanOrderProvider::GetInstance()->GetBlockSizeType(cb_size_y_);

    if (BlockScanOrderProvider::UNKNOWN_BLOCK_SIZE == block_size)
        return false;

    uint32_t palette_scan_pos = 0;
    uint32_t total_scan_size = cb_size_y_ *cb_size_y_;
    uint32_t max_palette_index = current_palette_table_->GetEntriesCount() - 1;
    uint32_t current_use_palette_index_idc_index = 0;
    while (palette_scan_pos < total_scan_size)
    {
        Coordinate begin = 
            BlockScanOrderProvider::GetInstance()->GetScanPosition(
                block_size, BlockScanOrderProvider::TRAVERSE_SCAN, 
                palette_scan_pos);

        Coordinate previous = {0, 0};
        if (palette_scan_pos > 0)
        {
            previous = BlockScanOrderProvider::GetInstance()->GetScanPosition(
                block_size, BlockScanOrderProvider::TRAVERSE_SCAN,
                palette_scan_pos - 1);
        }
        // 对应于标准上的PaletteRun
        uint32_t remain_max_palette_run = total_scan_size - palette_scan_pos - 1;
        is_copy_above_indices_[begin.y][begin.x] = false;

        if (max_palette_index > 0)
        {
            bool is_copy_above_indices = false;
            bool has_remaining_palette_index_idc =
                (palette_index_idc_values.size() -
                 current_use_palette_index_idc_index - 1) > 0;

            bool success = DeriveCopyAbovePaletteIndicesFlag(
                cabac_reader, context, has_remaining_palette_index_idc,
                palette_scan_pos, total_scan_size, previous,
                &is_copy_above_indices_[begin.y][begin.x]);

            if (!success)
                return false;
        }

        uint32_t current_palette_index = 0;
        if (!is_copy_above_indices_[begin.y][begin.x])
        {
            current_palette_index =
                palette_index_idc_values[current_use_palette_index_idc_index];

            ++current_use_palette_index_idc_index;
        }
        uint32_t adjusted_ref_palette_index = max_palette_index + 1;
        if (palette_scan_pos > 0)
        {
            if (!is_copy_above_indices_[previous.y][previous.x])
            {
                adjusted_ref_palette_index = 
                    palette_index_map_[previous.y][previous.x];
            }
            else
            {
                adjusted_ref_palette_index = 
                    palette_index_map_[begin.y][begin.x];
            }
        }
        if (current_palette_index >= adjusted_ref_palette_index)
            ++current_palette_index;

        uint32_t remaining_palette_index_idc_count = 
            palette_index_idc_values.size() - 
                current_use_palette_index_idc_index - 1;

        uint32_t palette_run = 0;
        if (max_palette_index > 0)
        {
            bool success = ParsePaletteRunValue(
                cabac_reader, context, total_scan_size, palette_scan_pos, 
                current_palette_index, remaining_palette_index_idc_count,
                is_copy_above_indices_[begin.y][begin.x], 
                is_copy_above_indices_for_final_run, &palette_run);

            if (!success)
                return false;
        }
        uint32_t last_pos = palette_scan_pos + palette_run;
        for (uint32_t pos = palette_scan_pos; pos < last_pos; ++pos)
        {
            Coordinate c = 
                BlockScanOrderProvider::GetInstance()->GetScanPosition(
                    block_size, BlockScanOrderProvider::TRAVERSE_SCAN, pos);

            if (!is_copy_above_indices_[begin.y][begin.x])
            {
                is_copy_above_indices_[c.y][c.x] = false;
                palette_index_map_[c.y][c.x] = current_palette_index;
            }
            else
            {
                is_copy_above_indices_[c.y][c.x] = true;
                palette_index_map_[c.y][c.x] = palette_index_map_[c.y - 1][c.x];
            }
        }
        palette_scan_pos = last_pos;
    }
    return true;
}

bool PaletteCoding::DeriveCopyAbovePaletteIndicesFlag(
    CABACReader* cabac_reader, IPaletteCodingContext* context, 
    bool has_remaining_palette_index_idc, uint32_t palette_scan_pos, 
    uint32_t total_scan_size, const Coordinate& previous, 
    bool* is_copy_above_palette_indices)
{
    if (!is_copy_above_palette_indices)
        return false;

    *is_copy_above_palette_indices = false;
    if ((palette_scan_pos < cb_size_y_) || 
        is_copy_above_indices_[previous.y][previous.x])
        return true;

    if (has_remaining_palette_index_idc)
    {
        if (palette_scan_pos < total_scan_size - 1)
        {
            CopyAboveIndicesForFinalRunFlagReader reader(
                cabac_reader, context->GetCABACInitType());
            *is_copy_above_palette_indices = reader.Read();
        }
    }
    else
    {
        *is_copy_above_palette_indices = true;
    }
    return true;
}

bool PaletteCoding::ParsePaletteRunValue(
    CABACReader* cabac_reader, IPaletteCodingContext* context, 
    uint32_t total_scan_size, uint32_t palette_scan_pos, 
    uint32_t palette_index_idc, uint32_t remaining_palette_index_idc_count, 
    bool is_copy_above_index, bool is_copy_above_indices_for_final_run, 
    uint32_t* palette_run)
{
    if (!palette_run)
        return false;

    uint32_t palette_max_run = total_scan_size - palette_scan_pos - 1 -
        remaining_palette_index_idc_count - 
        (is_copy_above_indices_for_final_run ? 1 : 0);

    if ((remaining_palette_index_idc_count > 0) ||
        (is_copy_above_index != is_copy_above_indices_for_final_run))
    {
        if (palette_max_run > 0)
        {
            PaletteRunPrefixReader prefix_reader(
                cabac_reader, context->GetCABACInitType(), 
                is_copy_above_indices_for_final_run, palette_index_idc);
            uint32_t palette_run_prefix = prefix_reader.Read(palette_max_run);
            uint32_t prefix_offset = 0;
            if (palette_run_prefix > 1)
            {
                prefix_offset = 1 << (palette_run_prefix - 1);
                if (palette_max_run != (1 << (palette_run_prefix - 1)))
                {
                    PaletteRunSuffixReader suffix_reader(cabac_reader);
                    uint32_t palette_run_suffix =
                        suffix_reader.Read(prefix_offset, palette_max_run);
                    *palette_run = prefix_offset + palette_run_suffix;
                }
            }
            else
            {
                *palette_run = palette_run_prefix;
            }
            return true;
        }
    }
    return true;
}

bool PaletteCoding::ParseEscapeValueInfo(CABACReader* cabac_reader, 
                                         IPaletteCodingContext* context,
                                         bool is_palette_transpose)
{
    const ChromaFormatType chroma_format_type = context->GetChromaFormatType();
    uint32_t color_component_count = GetColorCompomentCount(chroma_format_type);
    BlockScanOrderProvider::BlockSize block_size =
        BlockScanOrderProvider::GetInstance()->GetBlockSizeType(cb_size_y_);

    if (BlockScanOrderProvider::UNKNOWN_BLOCK_SIZE == block_size)
        return false;

    uint32_t max_palette_index = current_palette_table_->GetEntriesCount() - 1;
    for (uint32_t color_index = 0; color_index < color_component_count;
         ++color_index)
    {
        uint32_t cb_area_size = cb_size_y_ * cb_size_y_;
        for (uint32_t i = 0; i < cb_area_size; ++i)
        {
            Coordinate c = 
                BlockScanOrderProvider::GetInstance()->GetScanPosition(
                    block_size, BlockScanOrderProvider::TRAVERSE_SCAN, i);
            if (palette_index_map_[c.y][c.x] == max_palette_index)
            {
                bool has_palette_escape_val = true;
                do {
                    if (0 == color_component_count)
                        break;

                    if (YUV_MONO_CHROME == chroma_format_type)
                        break;

                    if (((c.x & 0x1) == 0) && ((c.y & 0x1) == 0) && 
                        (YUV_420 == chroma_format_type))
                        break;

                    if (YUV_422 == chroma_format_type)
                    {
                        if (!is_palette_transpose && ((c.x & 0x1) == 0))
                            break;

                        if (is_palette_transpose && ((c.y & 0x1) == 0))
                            break;
                    }
                    has_palette_escape_val = false;
                } while (false);
                if (has_palette_escape_val)
                {
                    uint32_t color_bit_depth = 
                        0 == color_index ? context->GetBitDepthLuma() : 
                        context->GetBitDepthChroma();

                    PaletteEscapeValReader reader(cabac_reader);
                    uint32_t palette_escape_val = 
                        reader.Read(context->IsCUTransquantBypass(), 
                                    color_bit_depth);
                    
                    uint64_t key = (static_cast<uint64_t>(color_index) << 32) || 
                        (c.y << 16) || c.x;
                    palette_escape_values_.insert(
                        make_pair(key, palette_escape_val));
                }
            }
        }
    }
    return true;
}
