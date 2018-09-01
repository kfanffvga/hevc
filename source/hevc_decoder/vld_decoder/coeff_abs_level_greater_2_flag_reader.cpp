#include "hevc_decoder/vld_decoder/coeff_abs_level_greater_2_flag_reader.h"

#include <cassert>

#include "hevc_decoder/vld_decoder/coeff_abs_level_greater_number_flag_reader_context.h"
#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

CoeffAbsLevelGreater2FlagReader::CoeffAbsLevelGreater2FlagReader(
    CABACReader* cabac_reader, CABACInitType init_type, 
    CoeffAbsLevelGreaterNumberFlagReaderContext* context, uint32_t color_index)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(COEFF_ABS_LEVEL_GREATER_2_FLAG, 
                                                init_type))
    , color_index_(color_index)
    , context_(context)

{

}

CoeffAbsLevelGreater2FlagReader::~CoeffAbsLevelGreater2FlagReader()
{

}

bool CoeffAbsLevelGreater2FlagReader::Read()
{
    assert(context_->GetContextSet() >= 0);
    auto bit_reader =
        bind(&CoeffAbsLevelGreater2FlagReader::ReadBit, this);

    bool val = !!FixedLengthValueReader(bit_reader).Read(1);
    context_->UpdateContext(val);
    return val;
}

uint32_t CoeffAbsLevelGreater2FlagReader::GetArithmeticContextIndex(
    uint16_t bin_idx)
{
    return lowest_context_index_ + context_->GetContextSet() + 
        (0 == color_index_ ? 0 : 4);
}

SyntaxElementName CoeffAbsLevelGreater2FlagReader::GetSyntaxElementName()
{
    return COEFF_ABS_LEVEL_GREATER_2_FLAG;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    CoeffAbsLevelGreater2FlagReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}
