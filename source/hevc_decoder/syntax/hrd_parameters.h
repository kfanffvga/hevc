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
    HrdParmeters();
    ~HrdParmeters();

    bool Parse(BitStream* bit_stream, bool has_commin_inf_present, 
               uint32_t max_num_sub_layers);

};

#endif