#include "hevc_decoder/vld_decoder/coeff_abs_level_remaining_reader.h"

#include <cassert>
#include <algorithm>

#include "hevc_decoder/vld_decoder/cabac_reader.h"
#include "hevc_decoder/vld_decoder/truncated_rice_value_reader.h"
#include "hevc_decoder/vld_decoder/limited_exponential_golomb_reader.h"
#include "hevc_decoder/vld_decoder/exponential_golomb_reader.h"

using std::min;
using std::max;
using std::bind;

CoeffAbsLevelRemainingReader::CoeffAbsLevelRemainingReader(
    CABACReader* cabac_reader, ICoeffAbsLevelRemainingReaderContext* context)
    : CommonCABACSyntaxReader(cabac_reader)
    , context_(context)
{

}

CoeffAbsLevelRemainingReader::~CoeffAbsLevelRemainingReader()
{

}

uint32_t CoeffAbsLevelRemainingReader::Read(uint32_t base_level)
{
    uint32_t rice_param = 0;
    uint32_t abs_level = 0;
    uint32_t state_coeff_index = 0;
    if (!context_->IsUpdatedRiceParamAndAbsLevel())
    {
        state_coeff_index = 
            GetInitialzationRiceParamAndAbsLevel(&rice_param, &abs_level);
    }
    else
    {
        rice_param = context_->GetRiceParam();
        abs_level = context_->GetAbsLevel();
    }
    if (abs_level > (3ui32 * (1 << rice_param)))
        ++rice_param;

    if (!context_->IsPersistentRiceAdaptationEnabled())
        rice_param = min(rice_param, 4ui32);

    auto bit_reader = bind(&CoeffAbsLevelRemainingReader::ReadBit, this);
    uint32_t max_value = 4 << rice_param;
    uint32_t prefix = 
        TruncatedRiceValueReader(bit_reader).Read(max_value, rice_param);

    uint32_t result = prefix;
    if (prefix >= max_value)
    { 
        uint32_t suffix = 0;
        if (context_->HasExtendedPrecisionProcessing())
        {
            uint32_t bit_depth = context_->GetColorIndex() == 0 ?
                context_->GetBitDepthOfLuma() : context_->GetBitDepthOfChroma();

            uint32_t log2_transform_range = max(15ui32, bit_depth + 6);
            LimitedExponentialGolombReader limit_egr(bit_reader);
            suffix = limit_egr.Read(log2_transform_range, rice_param + 1);
        }
        else
        {
            suffix = ExponentialGolombReader(bit_reader).Read(rice_param + 1);
        }
        result += suffix;
    }
    if (!context_->IsUpdatedRiceParamAndAbsLevel() && 
        context_->IsPersistentRiceAdaptationEnabled())
    {
        uint32_t state_coeff = 
            GetCABACReader()->GetStateCoefficient(state_coeff_index);
        if (result >= (3ui32 << (state_coeff >> 2)))
            ++state_coeff;
        else if ((result << 1) < (1ui32 << (state_coeff >> 2)) && (state_coeff > 0))
            --state_coeff;

        GetCABACReader()->UpdateStateCoefficient(state_coeff_index, state_coeff);
    }
    abs_level = base_level + result;
    context_->UpdateRiceParamAndAbsLevel(rice_param, abs_level);
    return result;
}

uint32_t CoeffAbsLevelRemainingReader::GetInitialzationRiceParamAndAbsLevel(
    uint32_t* rice_param, uint32_t* abs_level)
{
    assert(rice_param && abs_level);
    *abs_level = 0;
    *rice_param = 0;
    if (!context_->IsPersistentRiceAdaptationEnabled())
        return 0;

    uint32_t state_coeff_index = 0; // sbType
    if (context_->IsTransformSkip() || context_->IsCUTransquantBypass())
        state_coeff_index = context_->GetColorIndex() == 0 ? 3 : 1;
    else 
        state_coeff_index = context_->GetColorIndex() == 0 ? 2 : 0;

    *rice_param = GetCABACReader()->GetStateCoefficient(state_coeff_index);
    return state_coeff_index;
}

uint32_t CoeffAbsLevelRemainingReader::GetArithmeticContextIndex(
    uint16_t bin_idx)
{
    return 0;
}

SyntaxElementName CoeffAbsLevelRemainingReader::GetSyntaxElementName()
{
    return SYNTAX_ELEMENT_NAME_COUNT;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    CoeffAbsLevelRemainingReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::BYPASS_READER;
}
