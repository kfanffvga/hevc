#include "hevc_decoder/vld_decoder/prev_intra_luma_pred_flag_reader.h"

#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"
#include "hevc_decoder/vld_decoder/cabac_context_storage.h"

using std::bind;

PrevIntraLumaPredFlagReader::PrevIntraLumaPredFlagReader(
    CABACReader* cabac_reader, CABACInitType init_type)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(PART_MODE, init_type))
{

}

PrevIntraLumaPredFlagReader::~PrevIntraLumaPredFlagReader()
{

}

bool PrevIntraLumaPredFlagReader::Read()
{
    auto bit_reader = bind(&PrevIntraLumaPredFlagReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(1);
}

uint32_t PrevIntraLumaPredFlagReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return lowest_context_index_;
}

SyntaxElementName PrevIntraLumaPredFlagReader::GetSyntaxElementName()
{
    return PREV_INTRA_LUMA_PRED_FLAG;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    PrevIntraLumaPredFlagReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}
