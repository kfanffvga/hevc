#include "hevc_decoder/syntax/predictor_palette_table_storage.h"

#include <cassert>

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/syntax/frame_syntax.h"
#include "hevc_decoder/partitions/frame_partition.h"
#include "hevc_decoder/syntax/palette_table.h"

using std::shared_ptr;
using std::make_pair;
using std::pair;

PredictorPaletteTableStorage::PredictorPaletteTableStorage(
    FrameSyntax* frame_syntax)
    : frame_syntax_(frame_syntax)
    , default_table_()
    , palette_tables_()
    , current_palette_table_()
{

}

PredictorPaletteTableStorage::~PredictorPaletteTableStorage()
{

}

bool PredictorPaletteTableStorage::SetInitialzationPredictorPaletteTable(
    const shared_ptr<PaletteTable>& table)
{
    assert(table);
    if (!table)
        return false;

    default_table_ = table;
    return true;
}

shared_ptr<PaletteTable> PredictorPaletteTableStorage::GetPredictorPaletteTable(
    bool is_the_first_ctu_in_slice_segment, uint32_t ctb_size_y,
    bool is_entropy_coding_sync_enabled, bool is_dependent_slice_segment, 
    const Coordinate& c)
{
    shared_ptr<FramePartition> partition = frame_syntax_->GetFramePartition();
    assert(partition);
    if (!partition)
        return shared_ptr<PaletteTable>(new PaletteTable());

    if (is_the_first_ctu_in_slice_segment || partition->IsTheFirstCTBInTile(c) ||
        (is_entropy_coding_sync_enabled && 
         partition->IsTheFirstCTBInRowOfTile(c)))
    {
        current_palette_table_ = GetInitialzedPredictorPaletteTable(
            is_the_first_ctu_in_slice_segment, ctb_size_y, 
            is_entropy_coding_sync_enabled, is_dependent_slice_segment, c);
    }
    return current_palette_table_;
}

void PredictorPaletteTableStorage::SaveCTUPredictorPaletteTable(
    const Coordinate& c, const shared_ptr<PaletteTable>& palette_table)
{
    // 此处可以不用全部保存
    palette_tables_.insert(make_pair(make_pair(c.x, c.y), palette_table));
}

void PredictorPaletteTableStorage::Clear()
{
    default_table_.reset(new PaletteTable());
    current_palette_table_.reset(new PaletteTable());
    palette_tables_.clear();
}

shared_ptr<PaletteTable> 
    PredictorPaletteTableStorage::GetInitialzedPredictorPaletteTable(
        bool is_the_first_ctu_in_slice_segment, uint32_t ctb_size_y, 
        bool is_entropy_coding_sync_enabled, bool is_dependent_slice_segment, 
        const Coordinate& c)
{
    if (is_the_first_ctu_in_slice_segment && !is_dependent_slice_segment)
        return default_table_;

    shared_ptr<FramePartition> frame_partition =
        frame_syntax_->GetFramePartition();
    if (!frame_partition)
        return default_table_;

    if (frame_partition->IsTheFirstCTBInTile(c))
        return default_table_;

    if (is_entropy_coding_sync_enabled &&
        (frame_partition->IsTheFirstCTBInRowOfFrame(c) ||
         frame_partition->IsTheFirstCTBInRowOfTile(c)))
    {
        Coordinate neighbour_ctb = {c.x + ctb_size_y, c.y - ctb_size_y};
        bool available = frame_syntax_->IsZScanOrderNeighbouringBlockAvailable(
            c, neighbour_ctb);
        if (available)
        {
            auto predictor_palette_table = palette_tables_.find(
                make_pair(neighbour_ctb.x, neighbour_ctb.y));
            if (predictor_palette_table != palette_tables_.end())
                return predictor_palette_table->second;
        }
        return default_table_;
    }

    if (is_dependent_slice_segment && is_dependent_slice_segment)
        return current_palette_table_;

    return default_table_;
}