#ifndef _NAL_UNIT_H_
#define _NAL_UNIT_H_

#include <memory>
#include <stdint.h>

#include "hevc_decoder/syntax/nal_unit_types.h"

class BitStream;
using std::unique_ptr;

class NalUnit
{
public:
    NalUnit(const int8_t* nal_unit, int length);
    ~NalUnit();

    NalUnitType GetNalUnitType() const;

    uint8_t GetNuhLayerID() const;

    uint8_t GetNuhTemporalID() const;

    BitStream* GetBitSteam();

private:
    bool Parser();

    struct NalUnitHeader
    {
        uint8_t forbidden_zeor_bit;
        NalUnitType nal_unit_type;
        uint8_t nuh_layer_id;
        uint8_t nuh_temporal_id;
    };

    NalUnitHeader nal_unit_header_;    
    unique_ptr<BitStream> bit_stream_;
};

#endif