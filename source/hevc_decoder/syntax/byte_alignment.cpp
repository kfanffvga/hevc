#include "hevc_decoder/syntax/byte_alignment.h"

#include "hevc_decoder/base/stream/bit_stream.h"

ByteAlignment::ByteAlignment()
{

}

ByteAlignment::~ByteAlignment()
{

}

bool ByteAlignment::Parse(BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    bool is_alignment_bit_equal_to_one = bit_stream->ReadBool();
    if (!is_alignment_bit_equal_to_one)
        return false;

    bit_stream->ByteAlign();
    return true;
}
