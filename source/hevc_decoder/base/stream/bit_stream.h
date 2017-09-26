#ifndef _BIT_STREAM_H_
#define _BIT_STREAM_H_

#include <assert.h>
#include <memory>
#include <algorithm>

#include "hevc_decoder/base/basic_types.h"

class BitStream
{
public:
    BitStream(const int8_t* data, int length);
    ~BitStream();

    template<typename T>
    T Read(uint32_t length)
    {
        assert((sizeof(T) << 3) >= length);

        uint64_t result = 0;
        int remain_length = length;
        int64_t read_result = 0;
        do
        {
            int read_bits = ReadBitInByte(remain_length, &read_result);
            if (read_bits <= 0)
                break;

            result = (result << read_bits) | read_result;
            remain_length -= read_bits;
        } while (remain_length > 0);

        return static_cast<T>(result);
    }

    bool ReadBool();
    bool Seek(int byte_position, int bit_position);
    int GetSize();
    int GetBytePosition();
    int GetBitPosition();
    void SkipBits(uint32_t bits);

private:
    inline int ReadBitInByte(int length, int64_t* read_result);

    const int8_t* data_;
    int byte_inner_sequence_;
    const int8_t* current_pos_ptr_;
    const int8_t* end_ptr_;
};

#endif