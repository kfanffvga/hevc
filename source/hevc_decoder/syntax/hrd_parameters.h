#ifndef _HRD_PARMETERS_H_
#define _HRD_PARMETERS_H_

#include <vector>
#include <memory>

#include "hevc_decoder/syntax/base_syntax.h"

class BitStream;
class GolombReader;
class SubLayerHrdParameters;

class HrdParmeters : public BaseSyntax
{
public:
    HrdParmeters(bool commin_inf_present, int max_num_sub_layers);
    ~HrdParmeters();

    virtual bool Parse(BitStream* bit_stream) override;

private:
    bool commin_inf_present_;
    int max_num_sub_layers_;
    std::vector<std::unique_ptr<SubLayerHrdParameters>> 
        nal_sub_layer_hrd_parameters_;
    std::vector<std::unique_ptr<SubLayerHrdParameters>>
        vcl_sub_layer_hrd_parameters_;
};

#endif