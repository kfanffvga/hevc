#include "hevc_decoder/vld_decoder/pred_mode_flag_reader.h"

#include <cassert>

#include "hevc_decoder/vld_decoder/cabac_reader.h"
#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

PredModeFlagReader::PredModeFlagReader(CABACReader* cabac_reader, 
                                       CABACInitType init_type)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(PRED_MODE_FLAG, init_type))
{
    assert(init_type != FIRST_TYPE);
}

PredModeFlagReader::~PredModeFlagReader()
{
    
}

PredModeType PredModeFlagReader::Read()
{
    auto bit_reader = bind(&PredModeFlagReader::ReadBit, this);
    return static_cast<PredModeType>(FixedLengthValueReader(bit_reader).Read(1));
}

uint32_t PredModeFlagReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return lowest_context_index_;
}

SyntaxElementName PredModeFlagReader::GetSyntaxElementName()
{
    return PRED_MODE_FLAG;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    PredModeFlagReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}

