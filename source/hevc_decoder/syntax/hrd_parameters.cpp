#include "hevc_decoder/syntax/hrd_parameters.h"

HrdParmeters::HrdParmeters(bool commin_inf_present, int max_num_sub_layers)
                           : BaseSyntax()
                           , commin_inf_present_(commin_inf_present)
                           , max_num_sub_layers_(max_num_sub_layers)
{

}

HrdParmeters::~HrdParmeters()
{

}

bool HrdParmeters::Parse(BitStream* bit_stream)
{
    return true;
}
