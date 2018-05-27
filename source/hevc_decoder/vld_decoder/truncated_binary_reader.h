#ifndef _TRUNCATED_BINARY_READER_H_
#define _TRUNCATED_BINARY_READER_H_

#include <functional>

class TruncatedBinaryReader
{
public:
    typedef std::function<uint32_t ()> BitReader;
    TruncatedBinaryReader(const BitReader& bit_reader);
    ~TruncatedBinaryReader();

    uint32_t Read(uint32_t max_value);

private:
    BitReader bit_reader_;
};

#endif
