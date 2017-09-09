#include "hevc_decoder/syntax/sub_layer_hrd_parameters.h"

#include <assert.h>
#include <vector>
#include <boost/multi_array.hpp>

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"

using std::vector;
using boost::multi_array;
using boost::extents;

SubLayerHrdParameters::SubLayerHrdParameters(uint32_t cpb_cnt_value,
                                             bool has_sub_pic_hrd_params_present)
    : BaseSyntax()
    , cpb_cnt_value_(cpb_cnt_value)
    , has_sub_pic_hrd_params_present_(has_sub_pic_hrd_params_present)
{
    assert(cpb_cnt_value_ >= 0 && cpb_cnt_value_ <= 31);
}

SubLayerHrdParameters::~SubLayerHrdParameters()
{

}

bool SubLayerHrdParameters::Parse(BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    vector<uint32_t> bit_rate_value;
    vector<uint32_t> cpb_size_value;
    vector<uint32_t> cpb_size_du_value;
    vector<uint32_t> bit_rate_du_value;
    multi_array<bool, 1> is_cbr_mode(extents[cpb_cnt_value_]);
    GolombReader golom_readr(bit_stream);
    for (uint32_t i = 0; i <= cpb_cnt_value_; ++i)
    {
        bit_rate_value[i] = golom_readr.ReadUnsignedValue() + 1;
        cpb_size_value[i] = golom_readr.ReadUnsignedValue() + 1;
        if (has_sub_pic_hrd_params_present_)
        {
            cpb_size_du_value[i] = golom_readr.ReadUnsignedValue() + 1;
            bit_rate_du_value[i] = golom_readr.ReadUnsignedValue() + 1;
        }

        is_cbr_mode[i] = bit_stream->ReadBool();
    }

    return true;
}
