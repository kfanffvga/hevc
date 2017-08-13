#ifndef _PPS_MULTILAYER_EXTENSION_H_
#define _PPS_MULTILAYER_EXTENSION_H_

#include <stdint.h>

#include "hevc_decoder/syntax/base_syntax.h"

class PPSMultilayerExtension : public BaseSyntax
{
public:
    PPSMultilayerExtension();
    virtual ~PPSMultilayerExtension();

    virtual bool Parse(BitStream* bit_stream) override;

private:
    struct ScaledRefLayerInfo
    {
        int32_t left_offset;
        int32_t top_offset;
        int32_t right_offset;
        int32_t bottom_offset;
    };

    struct RefRegionInfo
    {
        int32_t left_offset;
        int32_t top_offset;
        int32_t right_offset;
        int32_t bottom_offset;
    };

    struct PhaseInfo
    {
        int32_t hor_luma;
        int32_t ver_luma;
        int32_t hor_chroma;
        int32_t ver_chroma;
    };
};

#endif