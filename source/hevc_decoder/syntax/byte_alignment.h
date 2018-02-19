#ifndef _BYTE_ALIGNMENT_H_
#define _BYTE_ALIGNMENT_H_

class BitStream;

class ByteAlignment
{
public:
    ByteAlignment();
    ~ByteAlignment();

    bool Parse(BitStream* bit_stream);
};

#endif
