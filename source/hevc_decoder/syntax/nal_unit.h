#ifndef _NAL_UNIT_H_
#define _NAL_UNIT_H_

#include <memory>

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/syntax/nal_unit_types.h"

class BitStream;
using std::unique_ptr;

class NalUnit
{
public:
    NalUnit(const int8* nal_unit, int length);
    ~NalUnit();

    NalUnitType GetNalUnitType() const;

    uint8 GetNuhLayerID() const;

    uint8 GetNuhTemporalID() const;

    BitStream* GetBitSteam();

private:
    bool Parser();

    struct NalUnitHeader
    {
        uint8 forbidden_zeor_bit;
        NalUnitType nal_unit_type;
        uint8 nuh_layer_id;
        uint8 nuh_temporal_id;
    };

    NalUnitHeader nal_unit_header_;    
    unique_ptr<BitStream> bit_stream_;
};

#endif