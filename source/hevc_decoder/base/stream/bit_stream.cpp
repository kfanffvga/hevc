#include "hevc_decoder/base/stream/bit_stream.h"

#include <assert.h>
#include <algorithm>

BitStream::BitStream(std::unique_ptr<int8[]> data, int length)
    : data_(std::move(data))
    , byte_inner_sequence_(0)
    , current_pos_ptr_(nullptr)
    , end_ptr_(nullptr)
{
    current_pos_ptr_ = data_.get();
    end_ptr_ = current_pos_ptr_ + (length - 1) * sizeof(int8);
}

BitStream::~BitStream()
{
    current_pos_ptr_ = nullptr;
    end_ptr_ = nullptr;
}

uint32 BitStream::Read(int length)
{
    assert(length >= 0 && length < 32);
    uint32 result = 0;
    int remain_length = length;
    uint8 read_result = 0;
    do 
    {
        int read_bits = ReadBitInByte(remain_length, &read_result);
        result = (result << read_bits) | read_result;
        remain_length -= read_bits;
    } while (remain_length > 0);

    return result;
}

int BitStream::GetSize()
{
    return (end_ptr_ - data_.get());
}

int BitStream::ReadBitInByte(int length, uint8* read_result)
{
    if (!read_result)
        return 0;

    assert(byte_inner_sequence_ >= 0 && byte_inner_sequence_ < 8);
    int raw_bits = 8 - byte_inner_sequence_;
    int read_reality_length = std::min(raw_bits, length);
    uint8 offset = raw_bits - read_reality_length;
    uint8 mask = ((1 << read_reality_length) - 1) << offset;
    *read_result = (*current_pos_ptr_ & mask) >> offset;
    byte_inner_sequence_ += read_reality_length;
    if (byte_inner_sequence_ >= 8)
    {
        byte_inner_sequence_ = 0;
        current_pos_ptr_ != end_ptr_ ? current_pos_ptr_++ : end_ptr_;
    }

    return read_reality_length;
}

bool BitStream::Seek(int byte_position, int bit_position)
{
    if ((byte_position < 0) && byte_position > GetSize() &&
        (bit_position < 0) && (bit_position > 7))
        return false;

    current_pos_ptr_ = data_.get() + byte_position;
    byte_inner_sequence_ = bit_position;
    return true;
}

int BitStream::GetBytePosition()
{
    return (current_pos_ptr_ - data_.get());
}

int BitStream::GetBitPosition()
{
    return byte_inner_sequence_;
}
