#ifndef _HRD_PARMETERS_H_
#define _HRD_PARMETERS_H_

#include "hevc_decoder/syntax/base_syntax.h"

class BitStream;

class HrdParmeters : public BaseSyntax
{
public:
    HrdParmeters(bool commin_inf_present, int max_num_sub_layers);
    ~HrdParmeters();

    virtual bool Parse(BitStream* bit_stream) override;

private:
    bool commin_inf_present_;
    int max_num_sub_layers_;
};

#endif