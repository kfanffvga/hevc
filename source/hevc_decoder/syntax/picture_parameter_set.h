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

    void ParseTileInfo(BitStream* bit_stream);
};
#endif