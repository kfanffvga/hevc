﻿#ifndef _SEQUENCE_PARAMETER_SET_H_
#define _SEQUENCE_PARAMETER_SET_H_

#include <stdint.h>
#include <vector>
#include <memory>

#include "hevc_decoder/syntax/short_term_reference_picture_set_context.h"

class GolombReader;
class SPSRangeExtension;
class SPSScreenContentCodingExtension;
class BitStream;

struct LongTermReferenceLSBPictureOrderCountInfo
{
    uint32_t lt_ref_pic_poc_lsb_sps;
    bool is_used_by_curr_pic_lt_sps_flag;
};

enum ChromaFormatType
{
    MONO_CHROME = 0,
    YUV_420 = 1,
    YUV_422 = 2,
    YUV_444 = 3,
    YUV_MONO_CHROME = 0xffff   // 此处可以认为chroma_array_type为0
};

class SequenceParameterSet : public ShortTermReferencePictureSetContext
{
public:
    SequenceParameterSet();
    ~SequenceParameterSet();

    bool Parse(BitStream* bit_stream);

    uint32_t GetSequenceParameterSetID();
    uint32_t GetSliceSegmentAddressBitLength() const;
    uint32_t GetPicOrderCountLSBBitLength() const;
    uint32_t GetMaxLSBOfPicOrderCount() const;
    bool HasLongTermReferencePicturesPresent() const;
    const std::vector<LongTermReferenceLSBPictureOrderCountInfo>&
        GetLongTermReferencePictureOrderCountInfos() const;

    bool IsTemporalMVPEnabled() const;
    bool IsSampleAdaptiveOffsetEnabled() const;
    ChromaFormatType GetChromaArrayType() const;
    uint32_t GetBitDepthChroma() const;

    const SPSRangeExtension* GetSPSRangeExtension() const;
    const SPSScreenContentCodingExtension* GetSPSScreenContentCodingExtension() 
        const;

    virtual uint32_t GetShortTermReferencePictureSetCount() const override;
    virtual const ShortTermReferencePictureSet*
        GetShortTermReferencePictureSet(uint32_t index) const override;

private:
    struct SubLayerOrderingInfo
    {
        uint32_t sps_max_dec_pic_buffering;
        uint32_t sps_max_num_reorder_pics;
        int sps_max_latency_increase;
    };

    void ParseSubLayerOrderingInfo(GolombReader* golomb_reader, 
                                   uint32_t sps_max_sub_layers,
                                   bool has_sps_sub_layer_ordering_info_present);

    bool ParseReferencePicturesInfo(BitStream* bit_stream, 
                                    uint32_t log2_max_pic_order_cnt_lsb);

    bool ParseExtensionInfo(BitStream* bit_stream, uint32_t ctb_log2_size_y,
                            uint32_t chroma_format_idc);

    uint32_t sps_seq_parameter_set_id_;
    std::vector<std::unique_ptr<ShortTermReferencePictureSet>> 
        short_term_reference_picture_sets_;
    uint32_t slice_segment_address_bit_length_;
    uint32_t log2_max_pic_order_cnt_lsb_;
    bool has_long_term_ref_pics_present_;
    std::vector<LongTermReferenceLSBPictureOrderCountInfo> lt_ref_poc_infos_;
    bool is_sps_temporal_mvp_enabled_;
    bool is_sample_adaptive_offset_enabled_;
    ChromaFormatType chroma_array_type_;
    uint32_t max_lsb_of_pic_order_count_;
    std::unique_ptr<SPSRangeExtension> sps_range_extension_;
    std::unique_ptr<SPSScreenContentCodingExtension> sps_scc_extension_;
    uint32_t bit_depth_chroma_;
};

#endif