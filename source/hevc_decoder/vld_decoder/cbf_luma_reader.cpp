#include "hevc_decoder/vld_decoder/cbf_luma_reader.h"

#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

CBFLumaReader::CBFLumaReader(CABACReader* cabac_reader, 
                             CABACInitType init_type, uint32_t depth)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(CBF_LUMA, init_type))
    , depth_(depth)
{

}

CBFLumaReader::~CBFLumaReader()
{

}

bool CBFLumaReader::Read()
{
    auto bit_reader = bind(&CBFLumaReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(1);
}

uint32_t CBFLumaReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return 0 == depth_ ? 1 : 0;
}

SyntaxElementName CBFLumaReader::GetSyntaxElementName()
{
    return CBF_LUMA;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    CBFLumaReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}

