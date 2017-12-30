#ifndef _REFERENCE_PICTURE_LISTS_MODIFICATION_H_
#define _REFERENCE_PICTURE_LISTS_MODIFICATION_H_

#include <stdint.h>
#include <vector>

enum SliceType;
class BitStream;

class ReferencePictureListsModification
{
public:
    ReferencePictureListsModification();
    ~ReferencePictureListsModification();

    bool Parse(BitStream* bit_stream, SliceType slice_type,
               uint32_t num_ref_idx_negative_active, 
               uint32_t num_ref_idx_positive_active,
               uint32_t reference_idx_bit_length);

    const std::vector<uint32_t>& GetListEntryOfNegative() const;
    const std::vector<uint32_t>& GetListEntryOfPositive() const;

private:
    std::vector<uint32_t> list_entry_of_negative_;
    std::vector<uint32_t> list_entry_of_positive_;

};
#endif