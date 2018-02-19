#include "hevc_decoder/base/stream/bit_stream.h"

using std::min;

BitStream::BitStream(const int8_t* data, int length)
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

uint32_t BitStream::GetSize()
{
    return end_ptr_ - data_;
}

bool BitStream::Seek(uint32_t byte_position, uint32_t bit_position)
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

int BitStream::ReadBitInByte(int length, int64_t* read_result)
{
    if (!read_result || (current_pos_ptr_ == end_ptr_))
        return 0;

    assert((byte_inner_sequence_ >= 0) && (byte_inner_sequence_ < 8));
    int max_provide_bits = 8 - byte_inner_sequence_;
    int read_reality_bits = min(max_provide_bits, length);
    *read_result = static_cast<uint8_t>(
        *current_pos_ptr_ << byte_inner_sequence_) >> (8 - read_reality_bits);

    byte_inner_sequence_ += read_reality_bits;
    if (8 == byte_inner_sequence_)
    {
        byte_inner_sequence_ = 0;
        ++current_pos_ptr_;
    }
    return read_reality_bits;
}

bool BitStream::ReadBool()
{
    int64_t read_result = 0;
    ReadBitInByte(1, &read_result);
    return !!read_result;
}

void BitStream::SkipBits(uint32_t bits)
{
    uint32_t bit_pos = byte_inner_sequence_ + bits;
    uint32_t skip_bytes = bit_pos >> 3;
    uint32_t max_skip_bytes = end_ptr_ - current_pos_ptr_;
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


void BitStream::ByteAlign()
{
    current_pos_ptr_ += (byte_inner_sequence_ > 0) ? 1 : 0;
    byte_inner_sequence_ = 0;
}

bool BitStream::IsEof()
{
    return (current_pos_ptr_ == end_ptr_) && (7 == byte_inner_sequence_);
}
