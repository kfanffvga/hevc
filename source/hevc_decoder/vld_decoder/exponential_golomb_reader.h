#ifndef _EXPONENTIAL_GOLOMB_READER_H_
#define _EXPONENTIAL_GOLOMB_READER_H_

#include <functional>

class ExponentialGolombReader
{
public:
    typedef std::function<uint8_t ()> BitReader;

    ExponentialGolombReader(const BitReader& reader);
    ~ExponentialGolombReader();

    uint32_t Read(uint32_t exp_value);

private:
    BitReader bit_reader_;

};

#endif
