#include "hevc_decoder/syntax/sample_adaptive_offset.h"

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/base/color_util.h"
#include "hevc_decoder/syntax/sample_adaptive_offset_context.h"
#include "hevc_decoder/vld_decoder/sao_merge_flag_reader.h"
#include "hevc_decoder/vld_decoder/sao_type_index_reader.h"
#include "hevc_decoder/vld_decoder/sao_offset_abs_reader.h"
#include "hevc_decoder/vld_decoder/sao_offset_sign_reader.h"
#include "hevc_decoder/vld_decoder/sao_band_position_reader.h"
#include "hevc_decoder/vld_decoder/sao_eo_class_reader.h"

using std::vector;
using std::array;

SampleAdaptiveOffset::SampleAdaptiveOffset()
    : sao_type_indices_()
    , sao_offsets_()
    , sao_band_positions_()
    , sao_eo_class_luma_(0)
    , sao_eo_class_chroma_(0)
{

}

SampleAdaptiveOffset::~SampleAdaptiveOffset()
{

}

bool SampleAdaptiveOffset::Parse(CABACReader* cabac_reader, 
                                 ISampleAdaptiveOffsetContext* context)
{
    if (!cabac_reader || !context)
        return false;

    bool is_allow_sao_merge_left = false;
    
    if ((context->GetCurrentCoordinate().x > 0) &&
        context->HasLeftCTBInSliceSegment() && context->HasLeftCTBInTile())
    {
        SAOMergeFlagReader reader(cabac_reader, context->GetCABACInitType());
        is_allow_sao_merge_left = reader.Read();
    }

    bool is_allow_sao_merge_up = false;
    if ((context->GetCurrentCoordinate().y > 0) && is_allow_sao_merge_left &&
        context->HasUpCTBInSliceSegment() && context->HasUpCTBInTile())
    {
        SAOMergeFlagReader reader(cabac_reader, context->GetCABACInitType());
        is_allow_sao_merge_up = reader.Read();
    }

    if (!is_allow_sao_merge_left && !is_allow_sao_merge_up)
        return ParseIndependentSAOInfo(cabac_reader, context);
    
    return CopyDependentSAOInfo(context, is_allow_sao_merge_left, 
                                is_allow_sao_merge_up);
}

const vector<SampleAdaptiveOffsetType>& 
    SampleAdaptiveOffset::GetSAOTypeIndices() const
{
    return sao_type_indices_;
}

const vector<array<int, 4>>& SampleAdaptiveOffset::GetSAOOffsets() const
{
    return sao_offsets_;
}

const vector<uint32_t> SampleAdaptiveOffset::GetSAOBandPositions() const
{
    return sao_band_positions_;
}

uint32_t SampleAdaptiveOffset::GetSAOEoClassLuma() const
{
    return sao_eo_class_luma_;
}

uint32_t SampleAdaptiveOffset::GetSAOEoClassChroma() const
{
    return sao_eo_class_chroma_;
}

bool SampleAdaptiveOffset::ParseIndependentSAOInfo(
    CABACReader* cabac_reader, ISampleAdaptiveOffsetContext* context)
{
    const uint32_t color_component_count = 
        GetColorCompomentCount(context->GetChromaFormatType());

    sao_type_indices_.resize(color_component_count);
    sao_offsets_.resize(color_component_count);
    sao_band_positions_.resize(color_component_count);
    for (uint32_t color_index = 0; color_index < color_component_count; 
         ++color_index)
    {
        if ((!(context->IsSliceSAOLuma() && (0 == color_index))) && 
            (!(context->IsSliceSAOChroma() && (color_index > 0))))
            continue;

        if (0 == color_index)
        {
            SAOTypeIndexReader sao_type_index_reader(
                cabac_reader, context->GetCABACInitType());
            uint32_t sao_type_idx_luma = sao_type_index_reader.Read();
            sao_type_indices_[0] = 
                static_cast<SampleAdaptiveOffsetType>(sao_type_idx_luma);
        }
        else if (1 == color_index)
        {
            SAOTypeIndexReader sao_type_index_reader(
                cabac_reader, context->GetCABACInitType());
            uint32_t sao_type_idx_chroma = sao_type_index_reader.Read();
            sao_type_indices_[1] = 
                static_cast<SampleAdaptiveOffsetType>(sao_type_idx_chroma);
            sao_type_indices_[2] = 
                static_cast<SampleAdaptiveOffsetType>(sao_type_idx_chroma);
        }
        if (sao_type_indices_[color_index] != NOT_APPLIED)
        {
            uint32_t sample_bit_depth = 
                (0 == color_index) ? context->GetBitDepthLuma() : 
                context->GetBitDepthChroma();

            bool success = ParseSingleColorSAODetailInfo(
                cabac_reader, sample_bit_depth, color_index, 
                sao_type_indices_[color_index], &sao_offsets_[color_index], 
                &sao_band_positions_[color_index]);
            if (!success)
                return false;
        }
    }
    return true;
}

bool SampleAdaptiveOffset::ParseSingleColorSAODetailInfo(
    CABACReader* cabac_reader, uint32_t sample_bit_depth, uint32_t color_index, 
    SampleAdaptiveOffsetType sao_type, array<int, 4>* sao_offset, 
    uint32_t* sao_band_position)
{
    if (!cabac_reader || !sao_offset || !sao_band_position)
        return false;

    SAOOffsetAbsReader sao_offset_abs_reader(cabac_reader, sample_bit_depth);
    for (uint32_t i = 0; i < 4; ++i)
        (*sao_offset)[i] = sao_offset_abs_reader.Read();
    
    if (BAND_OFFSET == sao_type)
    {
        for (uint32_t i = 0; i < 4; ++i)
        {
            if ((*sao_offset)[i] != 0)
            {
                SAOOffsetSignReader sao_offset_sign_reader(cabac_reader);
                bool is_positive_sign = sao_offset_sign_reader.Read();
                if (!is_positive_sign)
                    (*sao_offset)[i] = -(*sao_offset)[i];
            }
        }
        *sao_band_position = SAOBandPositionReader(cabac_reader).Read();
    }
    else
    {
        SAOEOClassReader sao_eo_class_reader(cabac_reader);
        if (0 == color_index)
            sao_eo_class_luma_ = sao_eo_class_reader.Read();
        else if (1 == color_index)
            sao_eo_class_chroma_ = sao_eo_class_reader.Read();
    }
    return true;
}

bool SampleAdaptiveOffset::CopyDependentSAOInfo(
    ISampleAdaptiveOffsetContext* context, bool is_allow_sao_merge_left,
    bool is_allow_sao_merge_up)
{
    const SampleAdaptiveOffset* neighbour_sao = nullptr;
    if (is_allow_sao_merge_left)
    {
        neighbour_sao = context->GetLeftNeighbourSAO();
        if (!neighbour_sao)
            return false;
    }
    else if (is_allow_sao_merge_up)
    {
        neighbour_sao = context->GetUpNeighbourSAO();
        if (!neighbour_sao)
            return false;
    }
    if (neighbour_sao)
    {
        sao_type_indices_ = neighbour_sao->GetSAOTypeIndices();
        sao_offsets_ = neighbour_sao->GetSAOOffsets();
        sao_band_positions_ = neighbour_sao->GetSAOBandPositions();
        sao_eo_class_luma_ = neighbour_sao->GetSAOEoClassLuma();
        sao_eo_class_chroma_ = neighbour_sao->GetSAOEoClassChroma();
    }
    else
    {
        const uint32_t color_component_count = 
            GetColorCompomentCount(context->GetChromaFormatType());
            
        sao_type_indices_.resize(color_component_count);
        sao_offsets_.resize(color_component_count);
        sao_band_positions_.resize(color_component_count);
        sao_eo_class_luma_ = 0;
        sao_eo_class_chroma_ = 0;
    }
    return true;
}

