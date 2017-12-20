#ifndef _NAL_ORGANIZER_H_
#define _NAL_ORGANIZER_H_

#include <stdint.h>
#include <vector>

#include "hevc_decoder/base/growing_buffer.h"

class NalUnit;
class ParametersManager;
class SyntaxDispatcher;

class NALOrganizer
{
public:
    NALOrganizer(SyntaxDispatcher* dispatcher);
    ~NALOrganizer();

    bool Decode(const int8_t* data, uint32_t length);

private:
    inline void ClearCache();
    bool DispatchNalUnit();

    int unused_length_;
    GrowingBuffer<int8_t> raw_nal_unit_data_;
    int8_t cache_[4];
    uint32_t cache_pos_;
    SyntaxDispatcher* dispatcher_;
    bool has_start_prefix_;
};

#endif