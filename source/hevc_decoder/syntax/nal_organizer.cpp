#include "hevc_decoder/syntax/nal_organizer.h"

#include <assert.h>

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/syntax/nal_unit.h"
#include "hevc_decoder/syntax/syntax_dispatcher.h"
#include "hevc_decoder/syntax/parameters_manager.h"

using std::move;

NALOrganizer::NALOrganizer(SyntaxDispatcher* dispatcher) 
    : unused_length_(0)
    , raw_nal_unit_data_(4096)
    , cache_pos_(0)
    , dispatcher_(dispatcher)
    , has_start_prefix_(false)
{
    memset(cache_, 0, 4);
}

NALOrganizer::~NALOrganizer()
{

}

bool NALOrganizer::Decode(const int8_t* data, uint32_t length)
{
    if (!data || (length <= 0))
        return false;
     
    raw_nal_unit_data_.SetSize(length + unused_length_);
    for (uint32_t i = 0; i < length; ++i)
    {
        if (((data[i] != 3) && (data[i] != 1) && (data[i] != 0)) || 
            (((3 == data[i]) || (1 == data[i])) && (cache_pos_ < 2)))
        {
            if (cache_pos_ > 0)
            {
                for (uint32_t j = 0; j < cache_pos_; ++j)
                {
                    raw_nal_unit_data_.GetBuffer()[unused_length_] = cache_[j];
                    ++unused_length_;
                }
                ClearCache();
            }
            raw_nal_unit_data_.GetBuffer()[unused_length_] = data[i];
            ++unused_length_;
            continue;
        }
        
        cache_[cache_pos_] = data[i];
        ++cache_pos_;
        
        if (3 == cache_pos_)
        {
            if (3 == cache_[2])
            {
                int8_t* p = raw_nal_unit_data_.GetBuffer() + unused_length_;
                *reinterpret_cast<uint16_t*>(p) = 0;
                unused_length_ += 2;
                ClearCache();
            }
            else if (1 == cache_[2])
            {
                if (!DispatchNalUnit())
                    return false;

                ClearCache();
            }
        }
        else if (4 == cache_pos_)
        {
            if (3 == cache_[3])
            {
                for (uint32_t j = 0; j < 3; ++j)
                {
                    int8_t* p = raw_nal_unit_data_.GetBuffer() + unused_length_;
                    p[j] = cache_[j];
                    ++unused_length_;
                }
                ClearCache();
            }
            else if (1 == cache_[3])
            {
                if (!DispatchNalUnit())
                    return false;

                ClearCache();
            }
            else
            {
                assert(false);
            }
        }
    }
    return true;
}

bool NALOrganizer::DispatchNalUnit()
{
    if (has_start_prefix_)
    {
        unique_ptr<NalUnit> nal_unit(
            new NalUnit(raw_nal_unit_data_.GetBuffer(), unused_length_));
        bool success = dispatcher_->CreateSyntaxAndDispatch(move(nal_unit));
        unused_length_ = 0;
        return success;
    }
    else
    {
        has_start_prefix_ = true;
    }
    return true;
}

void NALOrganizer::ClearCache()
{
    *reinterpret_cast<uint32_t*>(cache_) = 0;
    cache_pos_ = 0;
}
