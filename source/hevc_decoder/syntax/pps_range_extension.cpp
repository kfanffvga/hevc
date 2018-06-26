﻿#include "hevc_decoder/syntax/pps_range_extension.h"

#include <vector>

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"

using std::vector;

PPSRangeExtension::PPSRangeExtension()
    : is_chroma_qp_offset_list_enabled_(false)
    , diff_cu_chroma_qp_offset_depth_(0)
    , cb_qp_offset_list_()
    , cr_qp_offset_list_()
    , is_cross_component_prediction_enabled_(false)
{

}

PPSRangeExtension::~PPSRangeExtension()
{

}

bool PPSRangeExtension::Parse(BitStream* bit_stream, 
                              bool is_transform_skip_enabled)
{
    if (!bit_stream)
        return false;

    GolombReader golomb_reader(bit_stream);
    if (is_transform_skip_enabled)
    {
        uint32_t log2_max_transform_skip_block_size = 
            golomb_reader.ReadUnsignedValue();
    }
    is_cross_component_prediction_enabled_ = bit_stream->ReadBool();
    is_chroma_qp_offset_list_enabled_ = bit_stream->ReadBool();
    if (is_chroma_qp_offset_list_enabled_)
    {
        diff_cu_chroma_qp_offset_depth_ = golomb_reader.ReadUnsignedValue();
        uint32_t chroma_qp_offset_list_len = golomb_reader.ReadUnsignedValue();
        cb_qp_offset_list_.resize(chroma_qp_offset_list_len);
        cr_qp_offset_list_.resize(chroma_qp_offset_list_len);
        for (uint32_t i = 0; i < chroma_qp_offset_list_len; ++i)
        {
            cb_qp_offset_list_[i] = golomb_reader.ReadSignedValue();
            cr_qp_offset_list_[i] = golomb_reader.ReadSignedValue();
        }
        uint32_t sao_offset_scale_luma = golomb_reader.ReadUnsignedValue();
        uint32_t sao_offset_scale_chroma = golomb_reader.ReadUnsignedValue();
    }
    return true;
}

bool PPSRangeExtension::IsChromaQPOffsetListEnabled() const
{
    return is_chroma_qp_offset_list_enabled_;
}

uint32_t PPSRangeExtension::GetDiffCUChromaQPOffsetDepth() const
{
    return diff_cu_chroma_qp_offset_depth_;
}

uint32_t PPSRangeExtension::GetChromaQPOffsetListtLen() const
{
    return cb_qp_offset_list_.size();
}

const vector<int32_t>& PPSRangeExtension::GetCbQPOffsetList() const
{
    return cb_qp_offset_list_;
}

const vector<int32_t>& PPSRangeExtension::GetCrQPOffsetList() const
{
    return cr_qp_offset_list_;
}

bool PPSRangeExtension::IsCrossComponentPredictionEnabled() const
{
    return is_cross_component_prediction_enabled_;
}
