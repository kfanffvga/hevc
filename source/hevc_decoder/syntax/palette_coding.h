#ifndef _PALETTE_CODING_H_
#define _PALETTE_CODING_H_

#include <stdint.h>
#include <map>
#include <boost/multi_array.hpp>

class Coordinate;
class CABACReader;
class PaletteTable;
class IPaletteCodingContext;

class PaletteCoding
{
public:
    PaletteCoding(uint32_t cb_size_y);
    ~PaletteCoding();

    bool Parse(CABACReader* cabac_reader, IPaletteCodingContext* context);

private:
    bool ParseCurrentPaletteTable(CABACReader* cabac_reader, 
                                  IPaletteCodingContext* context);

    bool ParsePredictorPaletteTableReuseEntryIndices(
        CABACReader* cabac_reader, IPaletteCodingContext* context,
        std::vector<uint32_t>* reuse_entry_indices);

    bool ParsePaletteTableOfSelfEntries(
        CABACReader* cabac_reader, IPaletteCodingContext* context,
        const std::vector<uint32_t>& reuse_entry_indices, 
        boost::multi_array<uint32_t, 2>* new_palette_entries);

    void ProductCurrentPaletteTableAndUpdatePredictorPaletteTable(
        IPaletteCodingContext* context, 
        const std::vector<uint32_t>& reuse_entry_indices,
        const boost::multi_array<uint32_t, 2>& new_palette_entries);

    bool ParseSampleMapPaletteTableInfo(
        CABACReader* cabac_reader, IPaletteCodingContext* context, 
        bool has_palette_escape_val_present, 
        std::vector<uint32_t>* palette_index_idc_values,
        bool* is_copy_above_indices_for_final_run, bool* is_palette_transpose);

    bool ParseSampleIndexMapInfo(
        CABACReader* cabac_reader, IPaletteCodingContext* context,
        bool is_copy_above_indices_for_final_run,
        const std::vector<uint32_t>& palette_index_idc_values);

    bool DeriveCopyAbovePaletteIndicesFlag(CABACReader* cabac_reader, 
                                           IPaletteCodingContext* context,
                                           bool has_remaining_palette_index_idc,
                                           uint32_t palette_scan_pos, 
                                           uint32_t total_scan_size,
                                           const Coordinate& previous,
                                           bool* is_copy_above_palette_indices);

    bool ParsePaletteRunValue(CABACReader* cabac_reader, 
                              IPaletteCodingContext* context, 
                              uint32_t total_scan_size,
                              uint32_t palette_scan_pos,
                              uint32_t palette_index_idc, 
                              uint32_t remaining_palette_index_idc_count,
                              bool is_copy_above_index, 
                              bool is_copy_above_indices_for_final_run,
                              uint32_t* palette_run);

    bool ParseEscapeValueInfo(CABACReader* cabac_reader, 
                              IPaletteCodingContext* context,
                              bool is_palette_transpose);
    
    uint32_t cb_size_y_;
    std::unique_ptr<PaletteTable> current_palette_table_;
    boost::multi_array<bool, 2> is_copy_above_indices_;
    boost::multi_array<uint32_t, 2> palette_index_map_;

    // key = cidx << 32 | y << 16 | x;
    std::map<uint64_t, uint32_t> palette_escape_values_;
};

#endif
