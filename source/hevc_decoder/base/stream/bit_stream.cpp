#include "hevc_decoder/base/stream/bit_stream.h"

using std::min;

BitStream::BitStream(const int8* data, int length)
    : data_(data)
    , byte_inner_sequence_(0)
    , current_pos_ptr_(data)
    , end_ptr_(data + length)
{
}

BitStream::~BitStream()
{
    current_pos_ptr_ = nullptr;
    end_ptr_ = nullptr;
}

int BitStream::GetSize()
{
    return end_ptr_ - data_;
}

bool BitStream::Seek(int byte_position, int bit_position)
{
    if ((byte_position < 0) && byte_position > GetSize() &&
        (bit_position < 0) && (bit_position > 7))
        return false;

    current_pos_ptr_ = data_ + byte_position;
    byte_inner_sequence_ = bit_position;
    return true;
}

int BitStream::GetBytePosition()
{
    return (current_pos_ptr_ - data_);
}

int BitStream::GetBitPosition()
{
    return byte_inner_sequence_;
}

int BitStream::ReadBitInByte(int length, int64* read_result)
{
    if (!read_result || (current_pos_ptr_ == end_ptr_))
        return -1;

    assert(byte_inner_sequence_ >= 0 && byte_inner_sequence_ < 8);
    int raw_bits = 8 - byte_inner_sequence_;
    int read_reality_length = min(raw_bits, length);
    uint8 offset = raw_bits - read_reality_length;
    uint8 mask = ((1 << read_reality_length) - 1) << offset;
    *read_result = (*current_pos_ptr_ & mask) >> offset;
    byte_inner_sequence_ += read_reality_length;
    if (8 == byte_inner_sequence_)
    {
        byte_inner_sequence_ = 0;
        if (current_pos_ptr_ < end_ptr_)
            ++current_pos_ptr_;
    }

    return read_reality_length;
}

bool BitStream::ReadBool()
{
    int64 read_result = 0;
    ReadBitInByte(1, &read_result);
    return !!read_result;
}

void BitStream::SkipBits(uint32 bits)
{
    uint32 bit_pos = byte_inner_sequence_ + bits;
    uint32 skip_bytes = bit_pos >> 3;
    uint32 max_skip_bytes = end_ptr_ - current_pos_ptr_;
    if (max_skip_bytes < skip_bytes)
    {
        current_pos_ptr_ += max_skip_bytes;
        byte_inner_sequence_ = 0;
    }
    else
    {
        current_pos_ptr_ += skip_bytes;
        byte_inner_sequence_ = bit_pos & 0x7;
    }
}
