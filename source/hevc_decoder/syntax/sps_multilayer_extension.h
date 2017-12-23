#ifndef _SPS_MULTILAYER_EXTENSION_H_
#define _SPS_MULTILAYER_EXTENSION_H_

class BitStream;

class SPSMultilayerExtension
{
public:
    SPSMultilayerExtension();
    ~SPSMultilayerExtension();

    bool Parse(BitStream* bit_stream);
};

#endif