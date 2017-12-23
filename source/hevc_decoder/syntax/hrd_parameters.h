#ifndef _HRD_PARMETERS_H_
#define _HRD_PARMETERS_H_

#include <vector>
#include <memory>

class BitStream;
class GolombReader;
class SubLayerHrdParameters;

class HrdParmeters
{
public:
    HrdParmeters(bool commin_inf_present, uint32_t max_num_sub_layers);
    ~HrdParmeters();

    bool Parse(BitStream* bit_stream);

private:
    bool commin_inf_present_;
    uint32_t max_num_sub_layers_;
    std::vector<std::unique_ptr<SubLayerHrdParameters>> 
        nal_sub_layer_hrd_parameters_;
    std::vector<std::unique_ptr<SubLayerHrdParameters>>
        vcl_sub_layer_hrd_parameters_;
};

#endif