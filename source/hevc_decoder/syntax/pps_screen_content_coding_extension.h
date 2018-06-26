#ifndef _PPS_SCREEN_CONTENT_CODING_EXTENSION_H_
#define _PPS_SCREEN_CONTENT_CODING_EXTENSION_H_

#include <memory>

class BitStream;
class PaletteTable;

class PPSScreenContentCodingExtension
{
public:
    PPSScreenContentCodingExtension();
    virtual ~PPSScreenContentCodingExtension();

    bool Parse(BitStream* bit_stream);

    bool IsPPSCurrentPictureReferenceEnabled() const;
    bool HasPPSSliceActQPOffsetsPresent() const;
    bool HasPPSPalettePredictorInitializerPresent() const;
    const PaletteTable& GetPalettePredictorInitializer() const;
    bool IsResidualAdaptiveColorTransformEnabled() const;

private:
    bool is_pps_curr_pic_ref_enabled_;
    bool has_pps_slice_act_qp_offsets_present_;
    bool has_pps_palette_predictor_initializer_present_;
    std::unique_ptr<PaletteTable> palette_predictor_initializer_;
    bool is_residual_adaptive_colour_transform_enabled_;
};

#endif