#ifndef _HRD_PARMETERS_H_
#define _HRD_PARMETERS_H_

#include "hevc_decoder/syntax/base_syntax.h"

class BitStream;

class HrdParmeters : public BaseSyntax
{
public:
    HrdParmeters(BitStream* bit_stream, bool commin_inf_present,
                 int max_num_sub_layers);
    ~HrdParmeters();

    virtual bool Parser() override;

private:
    bool commin_inf_present_;
    int max_num_sub_layers_;
    BitStream* bit_stream_;
};

#endif