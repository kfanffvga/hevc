#include "hevc_decoder/vld_decoder/rqt_root_cbf_reader.h"

#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

RQTRootCBFReader::RQTRootCBFReader(CABACReader* cabac_reader, 
                                   CABACInitType init_type)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(RQT_ROOT_CBF, init_type))
{

}

RQTRootCBFReader::~RQTRootCBFReader()
{

}

bool RQTRootCBFReader::Read()
{
    auto bit_reader = bind(&RQTRootCBFReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(1);
}

uint32_t RQTRootCBFReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return lowest_context_index_;
}

SyntaxElementName RQTRootCBFReader::GetSyntaxElementName()
{
    return RQT_ROOT_CBF;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    RQTRootCBFReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}
