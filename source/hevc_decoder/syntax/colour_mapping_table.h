#ifndef _COLOUR_MAPPING_TABLE_H_
#define _COLOUR_MAPPING_TABLE_H_

class BitStream;

class ColourMappingTable
{
public:
    ColourMappingTable();
    ~ColourMappingTable();

    bool Parse(BitStream* bit_stream);
};

#endif