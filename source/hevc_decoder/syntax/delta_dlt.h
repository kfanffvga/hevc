#ifndef _DELTA_DLT_H_
#define _DELTA_DLT_H_

#include <stdint.h>

class BitStream;

class DeltaDlt
{
public:
    DeltaDlt(uint32_t depth_max_value);
    ~DeltaDlt();

    bool Parse(BitStream* bit_stream);

private:
    uint32_t depth_max_value_;
};

#endif