#include "hevc_decoder/syntax/palette_table.h"

using std::vector;

PaletteTable::PaletteTable()
    : palette_table_()
    , color_compoment_count_(0)
{

}

PaletteTable::~PaletteTable()
{

}

void PaletteTable::Init(uint32_t color_compoment_count)
{
    color_compoment_count_ = color_compoment_count;
    empty_entry_.resize(color_compoment_count);
}

bool PaletteTable::SetValue(uint32_t entry_index, 
                            uint32_t color_compoment_index, uint32_t value)
{
    if ((entry_index >= palette_table_.size()) || 
        (color_compoment_index >= color_compoment_count_))
        return false;

    palette_table_[entry_index][color_compoment_index] = value;
    return true;
}

uint32_t PaletteTable::GetValue(uint32_t entry_index, 
                                uint32_t color_compoment_index)
{
    if ((entry_index >= palette_table_.size()) || 
        (color_compoment_index >= color_compoment_count_))
        return 0;

    return palette_table_[entry_index][color_compoment_index];
}

const vector<uint32_t>& PaletteTable::GetEntry(uint32_t entry_index)
{
    if (entry_index >= palette_table_.size())
        return empty_entry_;

    return palette_table_[entry_index];
}

bool PaletteTable::PushEntry(const vector<uint32_t>& entry)
{
    if (entry.size() != color_compoment_count_)
        return false;

    palette_table_.push_back(entry);
    return true;
}

uint32_t PaletteTable::GetColorCompomentCount()
{
    return color_compoment_count_;
}

uint32_t PaletteTable::GetEntriesCount()
{
    return palette_table_.size();
}

void PaletteTable::Clear()
{
    palette_table_.clear();
}
