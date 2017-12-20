#ifndef _PPS_SCREEN_CONTENT_CODING_EXTENSION_H_
#define _PPS_SCREEN_CONTENT_CODING_EXTENSION_H_

#include "hevc_decoder/syntax/base_syntax.h"

class PPSScreenContentCodingExtension : public BaseSyntax
{
public:
    PPSScreenContentCodingExtension();
    virtual ~PPSScreenContentCodingExtension();

    virtual bool Parse(BitStream* bit_stream) override;

    bool IsPPSCurrentPictureReferenceEnabled() const;
    bool HasPPSSliceActQPOffsetsPresent() const;

private:
    bool is_pps_curr_pic_ref_enabled_;
    bool has_pps_slice_act_qp_offsets_present_;
};

#endif