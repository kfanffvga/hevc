#include "hevc_decoder/vld_decoder/cu_qp_delta_sign_flag_reader.h"

#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

CUQPDeltaSignFlagReader::CUQPDeltaSignFlagReader(CABACReader* reader)
    : CommonCABACSyntaxReader(reader)
{

}

CUQPDeltaSignFlagReader::~CUQPDeltaSignFlagReader()
{

}

bool CUQPDeltaSignFlagReader::Read()
{
    auto bit_reader = bind(&CUQPDeltaSignFlagReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(1);
}

uint32_t CUQPDeltaSignFlagReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return 0;
}

SyntaxElementName CUQPDeltaSignFlagReader::GetSyntaxElementName()
{
    return SYNTAX_ELEMENT_NAME_COUNT;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    CUQPDeltaSignFlagReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::BYPASS_READER;
}
