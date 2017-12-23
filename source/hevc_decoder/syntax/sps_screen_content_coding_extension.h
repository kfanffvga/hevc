#ifndef _SPS_SCREEN_CONTENT_CONDING_EXTENSION_H_
#define _SPS_SCREEN_CONTENT_CONDING_EXTENSION_H_

#include <stdint.h>

class BitStream;

class SPSScreenContentCodingExtension
{
public:
    SPSScreenContentCodingExtension(uint32_t num_of_color_compoments);
    virtual ~SPSScreenContentCodingExtension();

    bool Parse(BitStream* bit_stream);

    uint8_t GetMotionVectorResolutionControlIDC() const;

private:
    uint32_t num_of_color_compoments_;
    uint8_t motion_vector_resolution_control_idc_;
};

#endif