#include "hevc_decoder/syntax/sub_layer_hrd_parameters.h"

#include <assert.h>
#include <vector>
#include <boost/multi_array.hpp>

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"

using std::vector;
using boost::multi_array;
using boost::extents;

SubLayerHrdParameters::SubLayerHrdParameters()
{

}

SubLayerHrdParameters::~SubLayerHrdParameters()
{

}

bool SubLayerHrdParameters::Parse(BitStream* bit_stream, uint32_t cpb_cnt,
                                  bool has_sub_pic_hrd_params_present)
{
    if (!bit_stream)
        return false;

    vector<uint32_t> bit_rate_value;
    vector<uint32_t> cpb_size_value;
    vector<uint32_t> cpb_size_du_value;
    vector<uint32_t> bit_rate_du_value;
    multi_array<bool, 1> is_cbr_mode(extents[cpb_cnt]);
    GolombReader golom_readr(bit_stream);
    for (uint32_t i = 0; i <= cpb_cnt; ++i)
    {
        bit_rate_value[i] = golom_readr.ReadUnsignedValue() + 1;
        cpb_size_value[i] = golom_readr.ReadUnsignedValue() + 1;
        if (has_sub_pic_hrd_params_present)
        {
            cpb_size_du_value[i] = golom_readr.ReadUnsignedValue() + 1;
            bit_rate_du_value[i] = golom_readr.ReadUnsignedValue() + 1;
        }

        is_cbr_mode[i] = bit_stream->ReadBool();
    }

    return true;
}
