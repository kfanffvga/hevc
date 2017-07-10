#include "hevc_decoder/syntax/hrd_parameters.h"

HrdParmeters::HrdParmeters(BitStream* bit_stream, bool commin_inf_present,
                           int max_num_sub_layers)
                           : BaseSyntax()
                           , bit_stream_(bit_stream)
                           , commin_inf_present_(commin_inf_present)
                           , max_num_sub_layers_(max_num_sub_layers)
{

}

HrdParmeters::~HrdParmeters()
{

}

bool HrdParmeters::Parser()
{
    return true;
}
