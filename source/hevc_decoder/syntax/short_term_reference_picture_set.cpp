#include "hevc_decoder/syntax/short_term_reference_picture_set.h"

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"
#include "hevc_decoder/syntax/short_term_reference_picture_set_context.h"

using std::vector;

// positive_delta_pocs_, negative_delta_pocs_的意义是说明短期参考帧的编号与当前解码帧
// 的差值,按照标准里的描述, 两个数组代表的是不同的方向而已,如下图描述:
// 
// |x| <---negative_delta_pocs          0       positive_delta_pocs_--> |y|
// -------------------------------------|-------------------------------
// |0  <---can_use_delta             |x|^        can_use_delta--->     |x|+|y|
//                                      |
// 对于can_use_delta来说 箭头大小negative_delta_pocs.size(),两个坐标的起始点是不同的
// is_used_by_curr_pic坐标与can_use_delta一样
// 因此推导两个数组的值的时候,要么从前往后,要么从后往前

ShortTermReferencePictureSet::ShortTermReferencePictureSet(
    const ShortTermReferencePictureSetContext* context, 
    uint32_t current_rps_index)
    : context_(context)
    , positive_delta_pocs_()
    , negative_delta_pocs_()
    , current_rps_index_(current_rps_index)
{

}

ShortTermReferencePictureSet::~ShortTermReferencePictureSet()
{

}

bool ShortTermReferencePictureSet::Parse(BitStream* bit_stream)
{
    if (!context_ || !bit_stream)
        return false;

    bool is_inter_ref_pic_set_prediction = false;
    if (current_rps_index_ != 0)
        is_inter_ref_pic_set_prediction = bit_stream->ReadBool();

    if (is_inter_ref_pic_set_prediction)
    {
        ParsePredictionReferencePOCs(bit_stream);
        return true;
    }

    GolombReader golomb_reader(bit_stream);

    uint32_t num_negative_pics = golomb_reader.ReadUnsignedValue();
    uint32_t num_positive_pics = golomb_reader.ReadUnsignedValue();

    for (uint32_t i = 0; i < num_negative_pics; ++i)
    {
        uint32_t delta_poc_s0 = golomb_reader.ReadUnsignedValue();
        bool is_used_by_curr_pic_s0 = bit_stream->ReadBool();
        negative_delta_pocs_.push_back({delta_poc_s0, is_used_by_curr_pic_s0});
    }

    for (uint32_t i = 0; i < num_positive_pics; ++i)
    {
        uint32_t delta_poc_s1 = golomb_reader.ReadUnsignedValue();
        bool is_used_by_curr_pic_s1 = bit_stream->ReadBool();
        positive_delta_pocs_.push_back({delta_poc_s1, is_used_by_curr_pic_s1});
    }
    return true;
}

const ShortTermReferencePictureSet::ReferenceDeltaPOCs& 
    ShortTermReferencePictureSet::GetPositiveDeltaPOCs() const
{
    return positive_delta_pocs_;
}

const ShortTermReferencePictureSet::ReferenceDeltaPOCs& 
    ShortTermReferencePictureSet::GetNegativeDeltaPOCs() const
{
    return negative_delta_pocs_;
}

void ShortTermReferencePictureSet::ParsePredictionReferencePOCs(
    BitStream* bit_stream)
{
    uint32_t delta_idx = 0;
    GolombReader golomb_reader(bit_stream);
    if (context_->GetShortTermReferencePictureSetCount() == current_rps_index_)
        delta_idx = golomb_reader.ReadUnsignedValue();

    bool is_positive_delta_rps_sign = bit_stream->ReadBool();
    uint32_t abs_delta_rps = golomb_reader.ReadUnsignedValue();
    // delta_rps 是当前推导的每个delta_poc与参考rps_set的每个值在同一位置上的差值
    int delta_rps = is_positive_delta_rps_sign ? 
        static_cast<int>(abs_delta_rps) : -static_cast<int>(abs_delta_rps);

    uint32_t reference_rps_index = current_rps_index_ - delta_idx;
    const ShortTermReferencePictureSet* ref_short_term_reference_picture_set = 
        context_->GetShortTermReferencePictureSet(reference_rps_index);

    const ReferenceDeltaPOCs ref_positive_delta_pocs = 
        ref_short_term_reference_picture_set->GetPositiveDeltaPOCs();

    const ReferenceDeltaPOCs ref_negative_delta_pocs = 
        ref_short_term_reference_picture_set->GetNegativeDeltaPOCs();

    uint32_t delta_pocs_count = 
        ref_negative_delta_pocs.size() + ref_positive_delta_pocs.size();

    vector<bool> can_use_delta;
    vector<bool> is_used_by_curr_pic;
    for (uint32_t i = 0; i <= delta_pocs_count; ++i)
    {
        is_used_by_curr_pic.push_back(bit_stream->ReadBool());
        can_use_delta.push_back(
            is_used_by_curr_pic[i] ? bit_stream->ReadBool() : true);
    }

    // 整理并生成可用的delta_pocs
    DeriveDeltaPOCs(ref_positive_delta_pocs, ref_negative_delta_pocs, delta_rps,
                    can_use_delta, is_used_by_curr_pic);
}

void ShortTermReferencePictureSet::DeriveDeltaPOCs(
    const ReferenceDeltaPOCs& ref_positive_delta_pocs, 
    const ReferenceDeltaPOCs& ref_negative_delta_pocs, int delta_rps_of_item, 
    const vector<bool>& can_use_delta, const vector<bool>& is_used_by_curr_pic)
{
    DeriveNegativeDeltaPOCs(ref_positive_delta_pocs, ref_negative_delta_pocs, 
                            delta_rps_of_item, can_use_delta, 
                            is_used_by_curr_pic);

    DerivePositiveDeltaPOCs(ref_positive_delta_pocs, ref_negative_delta_pocs,
                            delta_rps_of_item, can_use_delta, 
                            is_used_by_curr_pic);
}

void ShortTermReferencePictureSet::DeriveNegativeDeltaPOCs(
    const ReferenceDeltaPOCs& ref_positive_delta_pocs, 
    const ReferenceDeltaPOCs& ref_negative_delta_pocs, int delta_rps_of_item, 
    const vector<bool>& can_use_delta, const vector<bool>& is_used_by_curr_pic)
{
    int ref_positive_delta_pocs_length =
        static_cast<int>(ref_positive_delta_pocs.size());
    for (int i = ref_positive_delta_pocs_length - 1; i >= 0; --i)
    {
        int delta_poc_of_current_item =
            ref_positive_delta_pocs[i].delta_poc_of_current_decode_picture +
            delta_rps_of_item;

        uint32_t flag_array_index = ref_negative_delta_pocs.size() + i;
        if ((delta_poc_of_current_item < 0) && can_use_delta[flag_array_index])
        {
            negative_delta_pocs_.push_back(
                {delta_poc_of_current_item, 
                is_used_by_curr_pic[flag_array_index]});
        }
    }
    
    if ((delta_rps_of_item < 0) && can_use_delta.back())
    {
        negative_delta_pocs_.push_back(
            {delta_rps_of_item, is_used_by_curr_pic.back()});
    }

    for (uint32_t i = 0; i < ref_negative_delta_pocs.size(); ++i)
    {
        int delta_poc_of_current_item = 
            ref_negative_delta_pocs[i].delta_poc_of_current_decode_picture + 
            delta_rps_of_item;

        if ((delta_poc_of_current_item < 0) && can_use_delta[i])
        {
            negative_delta_pocs_.push_back(
                {delta_poc_of_current_item, is_used_by_curr_pic[i]});
        }
    }
}

void ShortTermReferencePictureSet::DerivePositiveDeltaPOCs(
    const ReferenceDeltaPOCs& ref_positive_delta_pocs, 
    const ReferenceDeltaPOCs& ref_negative_delta_pocs, int delta_rps_of_item, 
    const vector<bool>& can_use_delta, const vector<bool>& is_used_by_curr_pic)
{
    int ref_negative_delta_pocs_length = 
        static_cast<int>(ref_negative_delta_pocs.size());
    for (int i = ref_negative_delta_pocs_length - 1; i >= 0; --i)
    {
        int delta_poc_of_current_item = 
            ref_negative_delta_pocs[i].delta_poc_of_current_decode_picture + 
            delta_rps_of_item;

        if ((delta_poc_of_current_item > 0) && can_use_delta[i])
        {
            positive_delta_pocs_.push_back(
                {delta_poc_of_current_item, is_used_by_curr_pic[i]});
        }
    }
    if ((delta_rps_of_item > 0) && can_use_delta.back())
    {
        positive_delta_pocs_.push_back(
            {delta_rps_of_item, is_used_by_curr_pic.back()});
    }
    for (uint32_t i = 0; i < ref_positive_delta_pocs.size(); ++i)
    {
        int delta_poc_of_current_item = 
            ref_positive_delta_pocs[i].delta_poc_of_current_decode_picture +
            delta_rps_of_item;

        uint32_t flag_array_index = i + ref_negative_delta_pocs.size();
        if ((delta_poc_of_current_item > 0) && can_use_delta[flag_array_index])
        {
            positive_delta_pocs_.push_back(
            {delta_poc_of_current_item, is_used_by_curr_pic[flag_array_index]});
        }
    }
}
