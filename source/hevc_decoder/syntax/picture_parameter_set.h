#ifndef _PICTURE_PARAMETER_SET_H_
#define _PICTURE_PARAMETER_SET_H_

#include <stdint.h>
#include <vector>

#include "hevc_decoder/syntax/base_syntax.h"

class BitStream;

class PictureParameterSet : public BaseSyntax
{
public:
    PictureParameterSet();
    virtual ~PictureParameterSet();

    virtual bool Parse(BitStream* bit_stream) override;

private:
    struct TileInfo
    {
        bool is_uniform_spacing;
        std::vector<uint32_t> column_width;
        std::vector<uint32_t> row_height;
        bool loop_filter_across_tiles_enabled;
    };

    struct DeblockingFilterControlInfo
    {
        bool deblocking_filter_override_enabled;
        bool pps_deblocking_filter_disabled;
        int32_t beta_offset;
        int32_t tc_offset;
    };

    void ParseTileInfo(BitStream* bit_stream);
    void ParseDeblockingFilterControlInfo(BitStream* bit_stream);
    bool ParsePPSExtensionInfo(bool is_transform_skip_enabled, 
                               BitStream* bit_stream);
};
#endif