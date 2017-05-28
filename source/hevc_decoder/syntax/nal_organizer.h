#ifndef _NAL_ORGANIZER_H_
#define _NAL_ORGANIZER_H_

#include <memory>
#include <functional>

#include "hevc_decoder/base/basic_types.h"

class NalUnit;
using std::unique_ptr;

class NalOrganizer
{
public:
    NalOrganizer();
    ~NalOrganizer();

    bool Decode(const int8* data, int length);

private:
    const int buffer_size_;
    int zero_count_;
    int nal_unit_size_;
    int nal_unit_length_;
    unique_ptr<int8[]> nal_unit_data_;
};

#endif