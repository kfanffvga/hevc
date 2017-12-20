#ifndef _SPS_SCREEN_CONTENT_CONDING_EXTENSION_H_
#define _SPS_SCREEN_CONTENT_CONDING_EXTENSION_H_

#include <stdint.h>

#include "hevc_decoder/syntax/base_syntax.h"

class SPSScreenContentCodingExtension : public BaseSyntax
{
public:
    SPSScreenContentCodingExtension(uint32_t num_of_color_compoments);
    virtual ~SPSScreenContentCodingExtension();

    virtual bool Parse(BitStream* bit_stream) override;

    uint8_t GetMotionVectorResolutionControlIDC() const;

private:
    uint32_t num_of_color_compoments_;
    uint8_t motion_vector_resolution_control_idc_;
};

#endif