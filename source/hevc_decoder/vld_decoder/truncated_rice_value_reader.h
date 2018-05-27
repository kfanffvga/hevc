#ifndef _TRUNCATED_RICE_VALUE_READER_H_
#define _TRUNCATED_RICE_VALUE_READER_H_

#include <stdint.h>
#include <functional>

class TruncatedRiceValueReader
{
public:
    typedef std::function<uint8_t ()> BitReader;

    TruncatedRiceValueReader(const BitReader& bit_reader);
    ~TruncatedRiceValueReader();

    uint32_t Read(uint32_t max_value, uint32_t rice_param);

private:
    BitReader bit_reader_;
};

#endif
