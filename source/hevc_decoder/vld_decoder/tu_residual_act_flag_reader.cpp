#include "hevc_decoder/vld_decoder/tu_residual_act_flag_reader.h"

#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

TUResidualActFlagReader::TUResidualActFlagReader(CABACReader* cabac_reader, 
                                                 CABACInitType init_type)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(TU_RESIDUAL_ACT_FLAG, init_type))
{

}

TUResidualActFlagReader::~TUResidualActFlagReader()
{

}

bool TUResidualActFlagReader::Read()
{
    auto bit_reader = bind(&TUResidualActFlagReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(1);
}

uint32_t TUResidualActFlagReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return lowest_context_index_;
}

SyntaxElementName TUResidualActFlagReader::GetSyntaxElementName()
{
    return TU_RESIDUAL_ACT_FLAG;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    TUResidualActFlagReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}
