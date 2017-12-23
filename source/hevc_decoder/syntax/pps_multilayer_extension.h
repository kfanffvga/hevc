#ifndef _PPS_MULTILAYER_EXTENSION_H_
#define _PPS_MULTILAYER_EXTENSION_H_

#include <stdint.h>

class BitStream;

class PPSMultilayerExtension
{
public:
    PPSMultilayerExtension();
    ~PPSMultilayerExtension();

    bool Parse(BitStream* bit_stream);

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