#ifndef _SPS_SCREEN_CONTENT_CONDING_EXTENSION_H_
#define _SPS_SCREEN_CONTENT_CONDING_EXTENSION_H_

#include <stdint.h>

class BitStream;

class SPSScreenContentCodingExtension
{
public:
    SPSScreenContentCodingExtension();
    virtual ~SPSScreenContentCodingExtension();

    bool Parse(BitStream* bit_stream, uint32_t chroma_format_idc);

    uint8_t GetMotionVectorResolutionControlIDC() const;
    bool IsPaletteModeEnabled() const;

private:
    uint8_t motion_vector_resolution_control_idc_;
    bool is_palette_mode_enabled_;
};

#endif
