#ifndef _BIT_STREAM_H_
#define _BIT_STREAM_H_

#include <memory>
#include "hevc_decoder/base/basic_types.h"

class BitStream
{
public:
    BitStream(const int8* data, int length);
    ~BitStream();

    uint32 Read(int length);
    bool Seek(int byte_position, int bit_position);
    int GetSize();
    int GetBytePosition();
    int GetBitPosition();

private:
    inline int ReadBitInByte(int length, uint8* read_result);
    
    const int8* data_;
    int byte_inner_sequence_;
    const int8* current_pos_ptr_;
    const int8* end_ptr_;
};

#endif