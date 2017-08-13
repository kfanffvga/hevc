#include "hevc_decoder/syntax/pps_3d_extension.h"

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/syntax/delta_dlt.h"

PPS3DExtension::PPS3DExtension()
{

}

PPS3DExtension::~PPS3DExtension()
{

}

bool PPS3DExtension::Parse(BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    bool has_dlts_present = bit_stream->ReadBool();
    if (has_dlts_present)
    {
        uint8_t pps_depth_layers = bit_stream->Read<uint8_t>(6) + 1;
        uint8_t pps_bit_depth_for_depth_layers = 
            bit_stream->Read<uint8_t>(6) + 8;

        uint32_t depth_max_value = (1 << pps_bit_depth_for_depth_layers) - 1;
        for (uint8_t i = 0; i < pps_depth_layers; ++i)
        {
            bool is_dlt = bit_stream->ReadBool();
            if (is_dlt)
            {
                bool is_dlt_pred = bit_stream->ReadBool();
                bool has_dlt_val_flags_present = false;
                if (!is_dlt_pred)
                    has_dlt_val_flags_present = bit_stream->ReadBool();

                if (has_dlt_val_flags_present)
                {
                    for (uint32_t j = 0; j < depth_max_value; ++j)
                        bool has_dlt_value = bit_stream->ReadBool();
                }
                else
                {
                    DeltaDlt delta_dlt(depth_max_value);
                    if (!delta_dlt.Parse(bit_stream))
                        return false;
                }
            }
        }
    }
    return true;
}
