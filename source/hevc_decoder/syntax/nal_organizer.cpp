#include "hevc_decoder/syntax/nal_organizer.h"

#include <assert.h>

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/syntax/nal_unit.h"
#include "hevc_decoder/syntax/syntax_dispatcher.h"
#include "hevc_decoder/syntax/parameters_manager.h"

using std::move;

NALOrganizer::NALOrganizer(ParametersManager* parameters_manager) 
    : zero_count_(0)
    , nal_unit_unused_length_(0)
    , raw_nal_unit_data_(4096)
    , dispatcher_(new SyntaxDispatcher(parameters_manager))
{

}

NALOrganizer::~NALOrganizer()
{

}

bool NALOrganizer::Decode(const int8_t* data, int length)
{
    raw_nal_unit_data_.SetSize(length + nal_unit_unused_length_);
    const int24 end_mark = int_to_int24(0x10000);
    const int8_t* raw_nal_current_point = data;
    const int8_t* raw_nal_end_point = data + length;
    int8_t* raw_nal_unit_point = raw_nal_unit_data_.GetBuffer();
    int8_t raw_nal_byte_data = 0;
    while (raw_nal_current_point != raw_nal_end_point)
    {
        if ((*reinterpret_cast<const int*>(raw_nal_current_point) == 0x1000000) ||
            (*reinterpret_cast<const int24*>(raw_nal_current_point) == end_mark))
        {
            ++raw_nal_current_point;
            if (*reinterpret_cast<const int24*>(raw_nal_current_point) == end_mark)
            {
                raw_nal_current_point += 3;
            }

            if (nal_unit_unused_length_ > 0)
            {
                unique_ptr<NalUnit> nal_unit(
                    new NalUnit(raw_nal_unit_point, nal_unit_unused_length_));
                dispatcher_->CreateSyntaxAndDispatch(move(nal_unit));
                nal_unit_unused_length_ = 0;
            }
        }

        raw_nal_byte_data = *raw_nal_current_point;
        if ((2 == zero_count_) && (0x03 == raw_nal_byte_data))
        {
            zero_count_ = 0;
            ++raw_nal_current_point;
            continue;
        }

        if (0x00 == raw_nal_byte_data)
            ++zero_count_;
        else
            zero_count_ = 0;

        raw_nal_unit_point[nal_unit_unused_length_] = raw_nal_byte_data;
        ++raw_nal_current_point;
        ++nal_unit_unused_length_;
    }

    return true;
}
