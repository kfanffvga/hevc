﻿#ifndef _HEVC_DECODER_H_
#define _HEVC_DECODER_H_

#include <memory>
#include <list>

#include "base/basic_types.h"
#include "hevc_decoder/hevc_decoder.h"

class Frame;
class NalOrganizer;
enum HEVC_ERROR;

class HEVCDecoder
{
public:
    HEVCDecoder();
    ~HEVCDecoder();

    bool Init(uint32 thread_count);

    HEVC_ERROR Decode(const int8* buffer, uint32 len, 
                      std::list<std::unique_ptr<Frame>>* frames);

    HEVC_ERROR Flush();

    void Close();

private:
    std::unique_ptr<NalOrganizer> organizer_;
};
#endif