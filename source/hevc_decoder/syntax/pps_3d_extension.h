#ifndef _PPS_3D_EXTENSION_H_
#define _PPS_3D_EXTENSION_H_

class BitStream;

class PPS3DExtension
{
public:
    PPS3DExtension();
    ~PPS3DExtension();

    bool Parse(BitStream* bit_stream);
};

#endif