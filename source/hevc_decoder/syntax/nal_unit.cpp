#include "hevc_decoder/syntax/nal_unit.h"
#include "hevc_decoder/base/stream/bit_stream.h"

#include <assert.h>

NalUnit::NalUnit(const int8* nal_unit, int length)
    : nal_unit_header_()
    , bit_stream_(new BitStream(nal_unit, length))
{
    Parser();
}

NalUnit::~NalUnit()
{

}

bool NalUnit::Parser()
{
    nal_unit_header_.forbidden_zeor_bit = bit_stream_->Read<uint8>(1);
    nal_unit_header_.nal_unit_type = static_cast<NalUnitType>(
        bit_stream_->Read<uint8>(6));
    nal_unit_header_.nuh_layer_id = bit_stream_->Read<uint8>(6);
    nal_unit_header_.nuh_temporal_id = bit_stream_->Read<uint8>(3) - 1;
    return true;
}

NalUnitType NalUnit::GetNalUnitType() const
{
    return nal_unit_header_.nal_unit_type;
}

uint8 NalUnit::GetNuhLayerID() const
{
    return nal_unit_header_.nuh_layer_id;
}

uint8 NalUnit::GetNuhTemporalID() const
{
    return nal_unit_header_.nuh_temporal_id;
}

BitStream* NalUnit::GetBitSteam()
{
    return bit_stream_.get();
}
