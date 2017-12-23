#include "hevc_decoder/syntax/nal_unit.h"
#include "hevc_decoder/base/stream/bit_stream.h"

#include <assert.h>

NalUnit::NalUnit(const int8_t* data, int length)
    : nal_unit_header_()
    , bit_stream_(new BitStream(data, length))
{
    Parse();
}

NalUnit::~NalUnit()
{

}

void NalUnit::Parse()
{
    nal_unit_header_.forbidden_zeor_bit = bit_stream_->Read<uint8_t>(1);
    nal_unit_header_.nal_unit_type = 
        static_cast<NalUnitType>(bit_stream_->Read<uint8_t>(6));
    nal_unit_header_.nuh_layer_id = bit_stream_->Read<uint8_t>(6);
    nal_unit_header_.nuh_temporal_id = bit_stream_->Read<uint8_t>(3) - 1;
}

NalUnitType NalUnit::GetNalUnitType() const
{
    return nal_unit_header_.nal_unit_type;
}

uint8_t NalUnit::GetNuhLayerID() const
{
    return nal_unit_header_.nuh_layer_id;
}

BitStream* NalUnit::GetBitSteam()
{
    return bit_stream_.get();
}
