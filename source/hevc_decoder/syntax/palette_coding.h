#ifndef _PALETTE_CODING_H_
#define _PALETTE_CODING_H_

#include <stdint.h>

class CABACReader;

class PaletteCoding
{
public:
    PaletteCoding(uint32_t cb_size_y);
    ~PaletteCoding();

    bool Parse(CABACReader* cabac_reader);

private:
    uint32_t cb_size_y_;
};

#endif
