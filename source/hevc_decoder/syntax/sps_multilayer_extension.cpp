#include "hevc_decoder/syntax/sps_multilayer_extension.h"

#include "hevc_decoder/base/stream/bit_stream.h"

SPSMultilayerExtension::SPSMultilayerExtension()
{

}

SPSMultilayerExtension::~SPSMultilayerExtension()
{

}

bool SPSMultilayerExtension::Parse(BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    bool is_inter_view_mv_vert_constraint = bit_stream->ReadBool();
    return true;
}
