#ifndef _PPS_SCREEN_CONTENT_CODING_EXTENSION_H_
#define _PPS_SCREEN_CONTENT_CODING_EXTENSION_H_

class BitStream;

class PPSScreenContentCodingExtension
{
public:
    PPSScreenContentCodingExtension();
    virtual ~PPSScreenContentCodingExtension();

    bool Parse(BitStream* bit_stream);

    bool IsPPSCurrentPictureReferenceEnabled() const;
    bool HasPPSSliceActQPOffsetsPresent() const;

private:
    bool is_pps_curr_pic_ref_enabled_;
    bool has_pps_slice_act_qp_offsets_present_;
};

#endif