#include "hevc_decoder/syntax/delta_dlt.h"

#include "hevc_decoder/base/stream/bit_stream.h"

DeltaDlt::DeltaDlt(uint32_t depth_max_value)
    : depth_max_value_(depth_max_value)
{

}

DeltaDlt::~DeltaDlt()
{

}

bool DeltaDlt::Parse(BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    uint32_t num_val_delta_dlt = 0;
    if (depth_max_value_ > 0)
        num_val_delta_dlt = bit_stream->Read<uint32_t>(depth_max_value_);

    if (num_val_delta_dlt > 0)
    {
        uint32_t max_diff = 0;
        if (num_val_delta_dlt > 1)
            max_diff = bit_stream->Read<uint32_t>(depth_max_value_);

        uint32_t min_diff = 0;
        if ((num_val_delta_dlt > 2) && (max_diff > 0))
            min_diff = bit_stream->Read<uint32_t>(depth_max_value_) + 1;

        uint32_t delta_dlt_val0 = bit_stream->Read<uint32_t>(depth_max_value_);
        if (max_diff > min_diff)
        {
            for (uint32_t k = 0; k < num_val_delta_dlt; ++k)
            {
                uint32_t delta_val_diff_minus_min = 
                    bit_stream->Read<uint32_t>(depth_max_value_);
            }   
        }
    }
    return true;
}
