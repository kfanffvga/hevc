#include "hevc_decoder/vld_decoder/res_scale_sign_flag_reader.h"

#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

ResScaleSignFlagReader::ResScaleSignFlagReader(CABACReader* cabac_reader, 
                                               CABACInitType init_type, 
                                               uint32_t color_index_dec1)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(LOG2_RES_SCALE_ABS, init_type))
    , color_index_dec1_(color_index_dec1)
{

}

ResScaleSignFlagReader::~ResScaleSignFlagReader()
{

}

bool ResScaleSignFlagReader::Read()
{
    auto bit_reader = bind(&ResScaleSignFlagReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(1);
}

uint32_t ResScaleSignFlagReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return lowest_context_index_ + color_index_dec1_;
}

SyntaxElementName ResScaleSignFlagReader::GetSyntaxElementName()
{
    return RES_SCALE_SIGN_FLAG;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    ResScaleSignFlagReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}
