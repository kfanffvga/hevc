#ifndef _BIT_STREAM_H_
#define _BIT_STREAM_H_

#include <memory>
#include "hevc_decoder/base/basic_types.h"

class BitStream
{
public:
    BitStream(std::unique_ptr<int8[]>& data, int length);
    ~BitStream();

    uint32 Read(int length);
    bool Seek(int position,  int byte_inner);
    int GetSize();

private:
    inline int ReadBitInByte(int length, uint8* read_result);
    
    std::unique_ptr<int8[]> data_;
    int byte_inner_sequence_;
    int8* current_pos_ptr_;
    int8* end_ptr_;
};

#endif