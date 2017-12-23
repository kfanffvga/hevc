#ifndef _SUB_LAGER_HRD_PARAMETERS_H_
#define _SUB_LAGER_HRD_PARAMETERS_H_

#include "hevc_decoder/base/basic_types.h"

class BitStream;

class SubLayerHrdParameters
{
public:
    SubLayerHrdParameters(uint32_t cpb_cnt_value,
                          bool has_sub_pic_hrd_params_present);
    ~SubLayerHrdParameters();

    bool Parse(BitStream* bit_stream);

private:
    uint32_t cpb_cnt_value_;
    bool has_sub_pic_hrd_params_present_;
};


#endif