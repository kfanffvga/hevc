#ifndef _NAL_ORGANIZER_H_
#define _NAL_ORGANIZER_H_

#include <memory>
#include <functional>

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/base/growing_buffer.h"

class NalUnit;

class NalOrganizer
{
public:
    NalOrganizer();
    ~NalOrganizer();

    bool Decode(const int8* data, int length);

private:
    int zero_count_;
    int nal_unit_unused_length_;
    GrowingBuffer<int8> raw_nal_unit_data_;
};

#endif