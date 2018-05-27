#ifndef _SPS_SCREEN_CONTENT_CONDING_EXTENSION_H_
#define _SPS_SCREEN_CONTENT_CONDING_EXTENSION_H_

#include <stdint.h>
#include <memory>

class BitStream;
class PaletteTable;

class SPSScreenContentCodingExtension
{
public:
    SPSScreenContentCodingExtension();
    virtual ~SPSScreenContentCodingExtension();

    bool Parse(BitStream* bit_stream, uint32_t chroma_format_idc, 
               uint32_t bit_depth_luma, uint32_t bit_depth_chroma);

    uint8_t GetMotionVectorResolutionControlIDC() const;
    bool IsPaletteModeEnabled() const;
    bool HasSPSPalettePredictorInitializerPresent() const;
    const PaletteTable& GetPalettePredictorInitializer() const;
    uint32_t GetPaletteMaxSize() const;
    uint32_t GetPredictorPaletteMaxSize() const;

private:
    uint8_t motion_vector_resolution_control_idc_;
    bool is_palette_mode_enabled_;
    bool has_sps_palette_predictor_initializer_present_;
    std::unique_ptr<PaletteTable> palette_predictor_initializer_;
    uint32_t palette_max_size_;
    uint32_t predictor_palette_max_size_;
};

#endif
