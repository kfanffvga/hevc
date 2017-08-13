#ifndef _COLOUR_MAPPING_TABLE_H_
#define _COLOUR_MAPPING_TABLE_H_

#include "hevc_decoder/syntax/base_syntax.h"

class ColourMappingTable : public BaseSyntax
{
public:
    ColourMappingTable();
    virtual ~ColourMappingTable();

    virtual bool Parse(BitStream* bit_stream) override;
};

#endif