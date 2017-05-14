#ifndef _NAL_ORGANIZER_H_
#define _NAL_ORGANIZER_H_

#include <memory>
#include <functional>

#include "hevc_decoder/base/basic_types.h"

class NalUnit;

class NalOrganizer
{
public:
    NalOrganizer();
    ~NalOrganizer();

    bool Decode(const int8* data, int length);

private:
    std::unique_ptr<int8[]> incomplete_nal_data_;

};

#endif