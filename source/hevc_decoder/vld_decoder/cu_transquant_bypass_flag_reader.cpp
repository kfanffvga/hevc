#include "hevc_decoder/vld_decoder/cu_transquant_bypass_flag_reader.h"

#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

CUTransquantBypassFlagReader::CUTransquantBypassFlagReader(
    CABACReader* cabac_reader, CABACInitType init_type)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(CU_TRANSQUANT_BYPASS_FLAG, 
                                                init_type))
{

}

CUTransquantBypassFlagReader::~CUTransquantBypassFlagReader()
{

}

bool CUTransquantBypassFlagReader::Read()
{
    auto bit_reader = bind(&CUTransquantBypassFlagReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(1);
}

uint32_t CUTransquantBypassFlagReader::GetArithmeticContextIndex(
    uint16_t bin_idx)
{
    return lowest_context_index_;
}

SyntaxElementName CUTransquantBypassFlagReader::GetSyntaxElementName()
{
    return CU_TRANSQUANT_BYPASS_FLAG;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    CUTransquantBypassFlagReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}

