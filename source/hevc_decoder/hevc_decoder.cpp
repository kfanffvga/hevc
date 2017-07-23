#include "hevc_decoder/hevc_decoder.h"

#include "hevc_decoder/hevc_decoder_def.h"
#include "hevc_decoder/syntax/nal_organizer.h"

using std::unique_ptr;
using std::list;

HEVCDecoder::HEVCDecoder()
    : organizer_(new NalOrganizer())
{

}

HEVCDecoder::~HEVCDecoder()
{

}

bool HEVCDecoder::Init(uint32_t thread_count)
{
    return false;
}

HEVC_ERROR HEVCDecoder::Decode(const int8_t* buffer, uint32_t len, 
                               list<unique_ptr<Frame>>* frames)
{    
    organizer_->Decode(buffer, len);
    return DECODE_SUCCESS;
}

HEVC_ERROR HEVCDecoder::Flush()
{
    return DECODE_SUCCESS;
}

void HEVCDecoder::Close()
{

}

