#ifndef _FIXED_LENGTH_VALUE_READER_H_
#define _FIXED_LENGTH_VALUE_READER_H_

#include <stdint.h>
#include <functional>

class FixedLengthValueReader
{
public:
    typedef std::function<uint8_t ()> BitReader;

    FixedLengthValueReader(const BitReader& bit_reader);
    ~FixedLengthValueReader();

    uint32_t Read(uint32_t read_bits);

private:
    BitReader bit_reader_;

};

#endif
