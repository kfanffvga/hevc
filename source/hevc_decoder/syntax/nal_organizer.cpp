#include "hevc_decoder/syntax/nal_organizer.h"

#include <assert.h>

#include "hevc_decoder/syntax/nal_unit.h"
#include "hevc_decoder/syntax/syntax_manager.h"


using std::move;

NalOrganizer::NalOrganizer()
    : buffer_size_(4096)
    , nal_unit_data_(new int8[buffer_size_])
    , nal_unit_size_(buffer_size_)    
    , zero_count_(0)
    , nal_unit_length_(0)
{

}

NalOrganizer::~NalOrganizer()
{

}

bool NalOrganizer::Decode(const int8* data, int length)
{
    if (nal_unit_length_ >= (nal_unit_size_ / 3))
    {
        nal_unit_size_ = nal_unit_size_ << 1;
        unique_ptr<int8[]> buffer(new int8[nal_unit_length_]);
        memcpy(buffer.get(), nal_unit_data_.get(), nal_unit_length_);
        nal_unit_data_.reset(new int8[nal_unit_size_]);
        memcpy(nal_unit_data_.get(), buffer.get(), nal_unit_length_);
    }

    const int8* raw_nal_begin_point = data;
    const int8* raw_nal_end_point = data + length;
    int8 raw_nal_byte_data = 0;
    while (raw_nal_begin_point != raw_nal_end_point)
    {
        if ((*reinterpret_cast<const int*>(raw_nal_begin_point) == 0x1000000) ||
            (int24_to_int(*reinterpret_cast<const int24*>(raw_nal_begin_point)) == 0x10000))
        {
            ++raw_nal_begin_point;
            if (int24_to_int(*reinterpret_cast<const int24*>(raw_nal_begin_point)) == 0x10000)
            {
                raw_nal_begin_point += 3;
            }

            if (nal_unit_length_ > 0)
            {
                unique_ptr<NalUnit> nal_unit(new NalUnit(nal_unit_data_.get(),
                                                         nal_unit_length_));
                // TO DO : 未完成 暂时注释代码
//                 SyntaxManager manager;
//                 manager.CreateSyntaxElement(std::move(nal_unit));
                nal_unit_length_ = 0;
            }
        }

        raw_nal_byte_data = *raw_nal_begin_point;
        if (zero_count_ == 2 && raw_nal_byte_data == 0x03)
        {
            zero_count_ = 0;
            ++raw_nal_begin_point;
            continue;
        }

        zero_count_ = (raw_nal_byte_data == 0x00) ? ++zero_count_ : 0;
        nal_unit_data_[nal_unit_length_] = raw_nal_byte_data;
        ++raw_nal_begin_point;
        ++nal_unit_length_;
    }

    return true;
}
