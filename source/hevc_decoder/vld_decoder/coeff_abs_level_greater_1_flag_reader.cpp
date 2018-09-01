#include "hevc_decoder/vld_decoder/coeff_abs_level_greater_1_flag_reader.h"

#include <cassert>

#include "hevc_decoder/vld_decoder/coeff_abs_level_greater_number_flag_reader_context.h"
#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;
using std::min;

CoeffAbsLevelGreater1FlagReader::CoeffAbsLevelGreater1FlagReader(
    CABACReader* cabac_reader, CABACInitType init_type, 
    CoeffAbsLevelGreaterNumberFlagReaderContext* context, uint32_t color_index)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(COEFF_ABS_LEVEL_GREATER_1_FLAG, 
                                                init_type))
    , color_index_(color_index)
    , context_(context)
{

}

CoeffAbsLevelGreater1FlagReader::~CoeffAbsLevelGreater1FlagReader()
{

}

bool CoeffAbsLevelGreater1FlagReader::Read()
{
    assert(context_->GetContextSet() >= 0);
    auto bit_reader = bind(&CoeffAbsLevelGreater1FlagReader::ReadBit, this);
    bool val = !!FixedLengthValueReader(bit_reader).Read(1);
    context_->UpdateContext(val);
    return val;
}

uint32_t CoeffAbsLevelGreater1FlagReader::GetArithmeticContextIndex(
    uint16_t bin_idx)
{
    uint32_t context_inc = (context_->GetContextSet() << 2) + 
        min(3, context_->GetGreaterNumberContext());

    if (color_index_ > 0)
        context_inc += 16;

    return lowest_context_index_ + context_inc;
}

SyntaxElementName CoeffAbsLevelGreater1FlagReader::GetSyntaxElementName()
{
    return COEFF_ABS_LEVEL_GREATER_1_FLAG;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    CoeffAbsLevelGreater1FlagReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}