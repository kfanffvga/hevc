#ifndef _TRUNCATED_RICE_VALUE_READER_H_
#define _TRUNCATED_RICE_VALUE_READER_H_

#include <stdint.h>
#include <functional>

// 此处是TR解码的特例情况，TR解码最多读cmax >> cRiceParam位，当在读这么多位的过程中，
// 读到0的时候，就当结束，如读不到0，则读完cmax >> cRiceParam个，结果就是数读到多少个1
// 但由于HEVC的cRiceParam都是0，因此此处的cMax就相当于bit_count,假如将来要把其放到别处，
// 切记要修改，cMax读取结果的最大值，也就是编码时候的最大值，cRiceParam是指后缀有多少位

class TruncatedRiceValueReader
{
public:
    typedef std::function<uint8_t ()> BitReader;

    TruncatedRiceValueReader(const BitReader& bit_reader);
    ~TruncatedRiceValueReader();

    uint32_t Read(uint32_t bit_count);

private:
    BitReader bit_reader_;
};

#endif
