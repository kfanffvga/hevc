#include "hevc_decoder/vld_decoder/cbf_cb_reader.h"

#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

CBFCBReader::CBFCBReader(CABACReader* cabac_reader, CABACInitType init_type, 
                         uint32_t layer)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(CBF_CHROMA, init_type))
    , layer_(layer)
{

}

CBFCBReader::~CBFCBReader()
{

}

bool CBFCBReader::Read()
{
    auto bit_reader = bind(&CBFCBReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(1);
}

uint32_t CBFCBReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return layer_ + lowest_context_index_;
}

SyntaxElementName CBFCBReader::GetSyntaxElementName()
{
    return CBF_CHROMA;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    CBFCBReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}
