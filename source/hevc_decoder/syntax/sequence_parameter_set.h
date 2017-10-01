#ifndef _SEQUENCE_PARAMETER_SET_H_
#define _SEQUENCE_PARAMETER_SET_H_

#include <stdint.h>
#include <vector>
#include <memory>

#include "hevc_decoder/syntax/base_syntax.h"
#include "hevc_decoder/syntax/short_term_reference_picture_set_context.h"

class GolombReader;

class SequenceParameterSet : public BaseSyntax
                           , public ShortTermReferencePictureSetContext
{
public:
    SequenceParameterSet();
    virtual ~SequenceParameterSet();

    virtual bool Parse(BitStream* bit_stream) override;

    uint32_t GetSequenceParameterSetID();

private:
    struct SubLayerOrderingInfo
    {
        uint32_t sps_max_dec_pic_buffering;
        uint32_t sps_max_num_reorder_pics;
        int sps_max_latency_increase;
    };

    struct LongTermReferencePictureOrderCountInfo
    {
        uint32_t lt_ref_pic_poc_lsb_sps;
        bool is_used_by_curr_pic_lt_sps_flag;
    };

    virtual uint32_t GetShortTermReferencePictureSetCount() override;
    virtual const ShortTermReferencePictureSet*
        GetShortTermReferencePictureSet(uint32_t index) override;

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
};

#endif