#include "hevc_decoder/syntax/reference_picture_lists_modification.h"

#include "hevc_decoder/base/stream/bit_stream.h"

using std::vector;

ReferencePictureListsModification::ReferencePictureListsModification(
    SliceType slice_type, uint32_t num_ref_idx_negative_active,
    uint32_t num_ref_idx_positive_active, uint32_t reference_idx_bit_length)
    : slice_type_(slice_type)
    , num_ref_idx_negative_active_(num_ref_idx_negative_active)
    , num_ref_idx_positive_active_(num_ref_idx_positive_active)
    , reference_idx_bit_length_(reference_idx_bit_length)
    , list_entry_of_positive_()
    , list_entry_of_negative_()
{

}

ReferencePictureListsModification::~ReferencePictureListsModification()
{

}

bool ReferencePictureListsModification::Parse(BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    bool has_ref_pic_list_modification_of_negative = bit_stream->ReadBool();
    if (has_ref_pic_list_modification_of_negative)
    {
        for (uint32_t i = 0; i < num_ref_idx_negative_active_; ++i)
        {
            uint32_t v = bit_stream->Read<uint32_t>(reference_idx_bit_length_);
            list_entry_of_negative_.push_back(v);
        }
    }

    if (B_SLICE == slice_type_)
    {
        bool is_ref_pic_list_modification_of_positive = bit_stream->ReadBool();
        if (is_ref_pic_list_modification_of_positive)
        {
            for (uint32_t i = 0; i < num_ref_idx_positive_active_; ++i)
            {
                uint32_t v = 
                    bit_stream->Read<uint32_t>(reference_idx_bit_length_);
                list_entry_of_positive_.push_back(v);
            }
        }
    }
    return true;
}

const vector<uint32_t>& 
    ReferencePictureListsModification::GetListEntryOfNegative() const
{
    return list_entry_of_negative_;
}

const vector<uint32_t>& 
    ReferencePictureListsModification::GetListEntryOfPositive() const
{
    return list_entry_of_positive_;
}
