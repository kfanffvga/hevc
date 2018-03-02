#include "hevc_decoder/syntax/palette_coding.h"

PaletteCoding::PaletteCoding(uint32_t cb_size_y)
    : cb_size_y_(cb_size_y)
{

}

PaletteCoding::~PaletteCoding()
{

}

bool PaletteCoding::Parse(CABACReader* cabac_reader)
{
    return false;
}
