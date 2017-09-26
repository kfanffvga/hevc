#ifndef _NAL_ORGANIZER_H_
#define _NAL_ORGANIZER_H_

#include <memory>
#include <functional>
#include <stdint.h>

#include "hevc_decoder/base/growing_buffer.h"

class NalUnit;
class ParametersManager;
class SyntaxDispatcher;

class NALOrganizer
{
public:
    NALOrganizer(ParametersManager* parameters_namager);
    ~NALOrganizer();

    bool Decode(const int8_t* data, int length);

private:
    int zero_count_;
    int nal_unit_unused_length_;
    GrowingBuffer<int8_t> raw_nal_unit_data_;
    std::unique_ptr<SyntaxDispatcher> dispatcher_;
};

#endif