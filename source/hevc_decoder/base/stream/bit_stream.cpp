#include "hevc_decoder/base/stream/bit_stream.h"

#include <assert.h>

BitStream::BitStream(std::unique_ptr<int8[]>& data, int length)
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
        byte_inner_sequence_ += read_bits;
        if (byte_inner_sequence_ >= 8)
        {
            byte_inner_sequence_ = 0;
            current_pos_ptr_ != end_ptr_ ? current_pos_ptr_++ : end_ptr_;
        }

        result |= (read_result << (remain_length - read_bits));
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
    int read_reality_length = 0;
    if ((length >= 8) || ((byte_inner_sequence_ + length) >= 8))
        read_reality_length = 8 - byte_inner_sequence_;
    else
        read_reality_length =  length;

    uint8 data = *current_pos_ptr_;
    uint8 offset = 8 - byte_inner_sequence_ - read_reality_length;
    uint8 mask = ((1 << read_reality_length) - 1) << offset;
    *read_result = (data & mask) >> offset;
    return read_reality_length;
}

bool BitStream::Seek(int position, int byte_inner)
{
    if ((position < 0) && position > GetSize() && 
        (byte_inner < 0) && (byte_inner > 7))
        return false;

    current_pos_ptr_ = data_.get() + position;
    byte_inner_sequence_ = byte_inner;
    return true;
}
