#ifndef _PREDIICTOR_PALETTE_TABLE_STORAGE_H_
#define _PREDIICTOR_PALETTE_TABLE_STORAGE_H_

#include <memory>
#include <map>

struct Coordinate;
class PaletteTable;
class FrameSyntax;

// fix me: 此处应该需要和cabac的存储做一个统一的架构

class PredictorPaletteTableStorage
{
public:
    PredictorPaletteTableStorage(FrameSyntax* frame_syntax);
    ~PredictorPaletteTableStorage();

    bool SetInitialzationPredictorPaletteTable(
        const std::shared_ptr<PaletteTable>& table);

    std::shared_ptr<PaletteTable> GetPredictorPaletteTable(
        bool is_the_first_ctu_in_slice_segment, uint32_t ctb_size_y,
        bool is_entropy_coding_sync_enabled, bool is_dependent_slice_segment, 
        const Coordinate& c);

    void SaveCTUPredictorPaletteTable(
        const Coordinate& c, const std::shared_ptr<PaletteTable>& palette_table);

    void Clear();

private:
    std::shared_ptr<PaletteTable> GetInitialzedPredictorPaletteTable(
        bool is_the_first_ctu_in_slice_segment, uint32_t ctb_size_y,
        bool is_entropy_coding_sync_enabled, bool is_dependent_slice_segment,
        const Coordinate& c);

    FrameSyntax* frame_syntax_;
    std::shared_ptr<PaletteTable> default_table_;
    std::map<std::pair<uint32_t, uint32_t>, std::shared_ptr<PaletteTable>> 
        palette_tables_;

    std::shared_ptr<PaletteTable> current_palette_table_;

};

#endif