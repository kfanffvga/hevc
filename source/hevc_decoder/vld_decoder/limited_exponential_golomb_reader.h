#ifndef _LIMITED_EXPONENTIAL_GOLOMB_READER_H_
#define _LIMITED_EXPONENTIAL_GOLOMB_READER_H_

#include <functional>

class LimitedExponentialGolombReader
{
public:
    typedef std::function<uint8_t ()> BitReader;

    LimitedExponentialGolombReader(const BitReader& bit_reader);
    ~LimitedExponentialGolombReader();
    
    uint32_t Read(uint32_t log2_transform_range, uint32_t rice_param);

private:
    BitReader bit_reader_;
};

#endif