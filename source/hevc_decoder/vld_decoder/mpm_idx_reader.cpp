#include "hevc_decoder/vld_decoder/mpm_idx_reader.h"

#include "hevc_decoder/vld_decoder/truncated_rice_value_reader.h"

using std::bind;

MPMIdxReader::MPMIdxReader(CABACReader* cabac_reader)
    : CommonCABACSyntaxReader(cabac_reader)
{

}

MPMIdxReader::~MPMIdxReader()
{

}

uint32_t MPMIdxReader::Read()
{
    auto bit_reader = bind(&MPMIdxReader::ReadBit, this);
    return TruncatedRiceValueReader(bit_reader).Read(2);
}

uint32_t MPMIdxReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return 0;
}

SyntaxElementName MPMIdxReader::GetSyntaxElementName()
{
    return SYNTAX_ELEMENT_NAME_COUNT;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    MPMIdxReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::BYPASS_READER;
}

