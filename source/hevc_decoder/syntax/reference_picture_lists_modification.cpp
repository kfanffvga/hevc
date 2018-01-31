#include "hevc_decoder/syntax/reference_picture_lists_modification.h"

#include "hevc_decoder/base/stream/bit_stream.h"

using std::vector;

ReferencePictureListsModification::ReferencePictureListsModification(
    uint32_t num_ref_idx_negative_active, uint32_t num_ref_idx_positive_active)
    : list_entry_of_positive_()
    , list_entry_of_negative_()
{
    for (uint32_t i = 0; i < num_ref_idx_negative_active; ++i)
        list_entry_of_negative_.push_back(i);

    for (uint32_t i = 0; i < num_ref_idx_positive_active; ++i)
        list_entry_of_positive_.push_back(i);
}

ReferencePictureListsModification::~ReferencePictureListsModification()
{

}

bool ReferencePictureListsModification::Parse(BitStream* bit_stream, 
                                              SliceType slice_type, 
                                              uint32_t reference_idx_bit_length)
{
    if (!bit_stream)
        return false;

    bool has_ref_pic_list_modification_of_negative = bit_stream->ReadBool();
    if (has_ref_pic_list_modification_of_negative)
    {
        for (auto& i : list_entry_of_negative_)
            i = bit_stream->Read<uint32_t>(reference_idx_bit_length);
    }

    if (B_SLICE == slice_type)
    {
        bool is_ref_pic_list_modification_of_positive = bit_stream->ReadBool();
        if (is_ref_pic_list_modification_of_positive)
        {
            for (auto& i : list_entry_of_positive_)
                i = bit_stream->Read<uint32_t>(reference_idx_bit_length);
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
