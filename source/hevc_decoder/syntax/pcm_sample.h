#ifndef _PCM_SAMPLE_H_
#define _PCM_SAMPLE_H_

class BitStream;

class PCMSample
{
public:
    PCMSample();
    ~PCMSample();

    bool Parse(BitStream* bit_stream);
};

#endif
