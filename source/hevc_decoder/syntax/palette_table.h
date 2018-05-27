#ifndef _PALETTE_TABLE_H_
#define _PALETTE_TABLE_H_

#include <stdint.h>
#include <vector>

//FIXME: 该table有很大的优化空间，待第一版完成后，该类需进行重构

class PaletteTable
{
public:
     PaletteTable();
    ~PaletteTable();

    void Init(uint32_t color_compoment_count);
    bool SetValue(uint32_t entry_index, uint32_t color_compoment_index, 
                  uint32_t value);

    uint32_t GetValue(uint32_t entry_index, uint32_t color_compoment_index);
    const std::vector<uint32_t>& GetEntry(uint32_t entry_index);
    bool PushEntry(const std::vector<uint32_t>& entry);
    uint32_t GetColorCompomentCount();
    uint32_t GetEntriesCount();
    void Clear();

private:
    std::vector<std::vector<uint32_t>> palette_table_;
    uint32_t color_compoment_count_;
    std::vector<uint32_t> empty_entry_;

};

#endif
