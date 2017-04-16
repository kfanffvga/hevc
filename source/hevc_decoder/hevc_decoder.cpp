#include "hevc_decoder/hevc_decoder.h"

#include "hevc_decoder/hevc_decoder_def.h"

using std::unique_ptr;
using std::list;

HEVCDecoder::HEVCDecoder()
{

}

HEVCDecoder::~HEVCDecoder()
{

}

bool HEVCDecoder::Init(uint32 thread_count)
{
    return false;
}

HEVC_ERROR HEVCDecoder::Decode(const int8* buffer, uint32 len, 
                               list<unique_ptr<Frame>>* frames)
{
    return DECODE_SUCCESS;
}

HEVC_ERROR HEVCDecoder::Flush()
{
    return DECODE_SUCCESS;
}

void HEVCDecoder::Close()
{

}

