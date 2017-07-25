#ifndef _SUB_LAGER_HRD_PARAMETERS_H_
#define _SUB_LAGER_HRD_PARAMETERS_H_

#include "hevc_decoder/syntax/base_syntax.h"

#include "hevc_decoder/base/basic_types.h"

class SubLayerHrdParameters : public BaseSyntax
{
public:
    SubLayerHrdParameters(uint32 cpb_cnt_value,
                          bool has_sub_pic_hrd_params_present);
    virtual ~SubLayerHrdParameters();

    virtual bool Parse(BitStream* bit_stream) override;

private:
    uint32 cpb_cnt_value_;
    bool has_sub_pic_hrd_params_present_;
};


#endif