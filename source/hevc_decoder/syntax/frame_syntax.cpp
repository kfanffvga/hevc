#include "hevc_decoder/syntax/frame_syntax.h"

#include <cassert>

#include "hevc_decoder/syntax/slice_segment_syntax.h"
#include "hevc_decoder/syntax/slice_segment_header.h"
#include "hevc_decoder/syntax/slice_syntax.h"
#include "hevc_decoder/syntax/coded_video_sequence.h"
#include "hevc_decoder/syntax/slice_segment_data.h"
#include "hevc_decoder/syntax/nal_unit.h"
#include "hevc_decoder/syntax/predictor_palette_table_storage.h"
#include "hevc_decoder/syntax/palette_table.h"
#include "hevc_decoder/syntax/slice_segment_context.h"
#include "hevc_decoder/partitions/frame_partition_creator_info_provider_impl.h"
#include "hevc_decoder/partitions/frame_partition_manager.h"
#include "hevc_decoder/partitions/frame_partition.h"

using std::shared_ptr;
using std::unique_ptr;
using std::vector;

class SliceSegmentContext : public ISliceSegmentContext
{
public:
    SliceSegmentContext(NalUnit* nal_unit, FrameSyntax* frame_syntax,
                        const IFrameSyntaxContext* frame_context)
        : frame_syntax_(frame_syntax)
        , frame_context_(frame_context)
        , nal_unit_(nal_unit)
    {

    }

    virtual ~SliceSegmentContext()
    {

    }

    virtual void SetPictureOrderCountByLSB(uint32_t lsb, uint32_t max_lsb)
        override
    {
        frame_syntax_->SetPictureOrderCountByLSB(lsb, max_lsb);
    }

    virtual PictureOrderCount GetPictureOrderCount() const override
    {
        return frame_syntax_->GetPictureOrderCount();
    }

    virtual uint8_t GetNuhLayerIDByPOCValue(uint32_t poc_value) const override
    {
        return frame_context_->GetLayerID(poc_value);
    }

    virtual bool OnSliceSegmentHeaderParsed(const SliceSegmentHeader& header)
        override
    {
        if (!InitFramePartition(header))
            return false;
        
        return SetDefaultPaletteTable(header);
    }

    virtual shared_ptr<FramePartition> GetFramePartition() override
    {
        return frame_syntax_->GetFramePartition();
    }

    virtual CABACContextStorage* GetCABACContextStorage() override
    {
        return frame_context_->GetCABACContextStorage();
    }

    virtual bool GetSliceSegmentAddressByTileScanIndex(
        uint32_t tile_scan_index, uint32_t* slice_segment_address)
    {
        return frame_syntax_->GetSliceSegmentAddress(tile_scan_index, 
                                                     slice_segment_address);
    }

    virtual uint32_t GetFirstCTUIndexOfTileScan() override
    {
        uint32_t ctu_count = frame_syntax_->GetContainCTUCountByTileScan();
        return 0 == ctu_count ? 0 : ctu_count - 1;
    }

    virtual uint32_t GetCABACContextIndexInLastParsedSliceSegment()
        override
    {
        return
            frame_syntax_->GetCABACContextIndexInLastParsedSliceSegment();
    }

    virtual const ParametersManager& GetParametersManager() const
    {
        return frame_context_->GetParametersManager();
    }

    virtual NalUnitType GetNalUnitType() const override
    {
        return nal_unit_->GetNalUnitType();
    }

    virtual uint8_t GetNuhLayerID() const override
    {
        return nal_unit_->GetNuhLayerID();
    }

    virtual bool IsZScanOrderNeighbouringBlockAvailable(
        const Coordinate& current_block, const Coordinate& neighbouring_block)
        const override
    {
        return frame_syntax_->IsZScanOrderNeighbouringBlockAvailable(
            current_block, neighbouring_block);
    }

    virtual shared_ptr<PaletteTable> GetPredictorPaletteTable(
        bool is_the_first_ctu_in_slice_segment, uint32_t ctb_size_y,
        bool is_entropy_coding_sync_enabled, bool is_dependent_slice_segment,
        const Coordinate& point) override
    {
        PredictorPaletteTableStorage* storage = 
            frame_syntax_->GetPredictorPaletteTableStorage();
        if (!storage)
            return shared_ptr<PaletteTable>(new PaletteTable());

        return storage->GetPredictorPaletteTable(
            is_the_first_ctu_in_slice_segment, ctb_size_y, 
            is_entropy_coding_sync_enabled, is_dependent_slice_segment, point);
    }

    virtual void SavePredictorPaletteTable(
        const Coordinate& point,
        const shared_ptr<PaletteTable>& palette_table) override
    {
        PredictorPaletteTableStorage* storage =
            frame_syntax_->GetPredictorPaletteTableStorage();
        if (!storage)
            return;

        storage->SaveCTUPredictorPaletteTable(point, palette_table);
    }

private:
    bool InitFramePartition(const SliceSegmentHeader& header)
    {
        if (!header.IsFirstSliceSegmentInPic())
            return frame_syntax_->HasFramePartition();

        FramePartitionManager* frame_partition_manager =
            frame_context_->GetFramePartitionManager();
        if (!frame_partition_manager)
            return false;

        FramePartitionCreatorInfoProviderImpl* provider_impl =
            new FramePartitionCreatorInfoProviderImpl(header.GetWidth(),
                                                      header.GetHeight());

        shared_ptr<IFramePartitionCreatorInfoProvider> provider(provider_impl);
        bool success = false;
        const TileInfo& tile_info = header.GetTileInfo();
        if (tile_info.IsUniformSpacing())
        {
            success = provider_impl->Init(tile_info.GetNumTileColumns(),
                                          tile_info.GetNumTileRows(),
                                          header.GetCTBLog2SizeY(),
                                          header.GetMinTBLog2SizeY());
        }
        else
        {
            success = provider_impl->Init(tile_info.GetColumnWidth(),
                                          tile_info.GetRowHeight(),
                                          header.GetCTBLog2SizeY(),
                                          header.GetMinTBLog2SizeY());
        }
        if (!success)
            return false;

        frame_syntax_->SetFramePartition(frame_partition_manager->Get(provider));
        return true;
    }

    bool SetDefaultPaletteTable(const SliceSegmentHeader& header)
    {
        PredictorPaletteTableStorage* storage = 
            frame_syntax_->GetPredictorPaletteTableStorage();
        if (!storage)
            return false;

        shared_ptr<PaletteTable> palette_table(new PaletteTable());
        *palette_table = header.GetPalettePredictorInitializer();
        storage->SetInitialzationPredictorPaletteTable(palette_table);
        return true;
    }

    FrameSyntax* frame_syntax_;
    const IFrameSyntaxContext* frame_context_;
    NalUnit* nal_unit_;
};


FrameSyntax::FrameSyntax(IFrameSyntaxContext* frame_syntax_context)
    : slices_()
    , picture_order_count_()
    , frame_syntax_context_(frame_syntax_context)
    , frame_partition_()
    , ctu_count_by_tile_scan_(0)
    , parsing_slice_segment_(new SliceSegmentSyntax())
    , palette_table_storage_(new PredictorPaletteTableStorage(this))
{

}

FrameSyntax::~FrameSyntax()
{

}

bool FrameSyntax::ParseSliceSegment(NalUnit* nal_unit, 
                                    IFrameSyntaxContext* context)
{
    assert(parsing_slice_segment_);
    SliceSegmentContext slice_segment_context(nal_unit, this, 
                                              frame_syntax_context_);
    bool success = parsing_slice_segment_->Parse(nal_unit->GetBitSteam(),
                                                 &slice_segment_context);
    if (!success)
        return false;

    if (!AddSliceSegment(parsing_slice_segment_))
        return false;

    parsing_slice_segment_.reset(new SliceSegmentSyntax());
    return true;
}

bool FrameSyntax::AddSliceSegment(shared_ptr<SliceSegmentSyntax> slice_segment)
{
    if (!slices_.empty() && slices_.back()->AddSliceSegment(slice_segment))
        return true;

    if (slice_segment->GetSliceSegmentHeader().IsDependentSliceSegment())
        return false;

    unique_ptr<SliceSyntax> slice(new SliceSyntax(frame_syntax_context_));
    if (!slice->AddSliceSegment(slice_segment))
        return false;
     
    ctu_count_by_tile_scan_ += 
        slice_segment->GetSliceSegmentData().GetCTUCount();
    slices_.push_back(move(slice));
    return true;
}

bool FrameSyntax::HasFramePartition() const
{
    return !!frame_partition_;
}

bool FrameSyntax::SetPictureOrderCountByLSB(uint32_t lsb, uint32_t max_lsb)
{
    PictureOrderCount preview_poc;
    bool success = 
        frame_syntax_context_->GetPreviewPictureOrderCount(&preview_poc);
    if (!success)
        return false;

    picture_order_count_ = FrameSyntax::CalcPictureOrderCount(preview_poc.msb, 
                                                              preview_poc.lsb,
                                                              false, max_lsb, 
                                                              lsb);
    return true;
}

void FrameSyntax::SetFramePartition(
    const shared_ptr<FramePartition>& frame_partition)
{
    frame_partition_ = frame_partition;
}

const PictureOrderCount& FrameSyntax::GetPictureOrderCount() const
{
    return picture_order_count_;
}

shared_ptr<FramePartition> FrameSyntax::GetFramePartition()
{
    return frame_partition_;
}

bool FrameSyntax::GetSliceSegmentAddress(uint32_t tile_scan_index, 
                                         uint32_t* address) const
{
    if (!address)
        return false;

    const SliceSegmentData& data = parsing_slice_segment_->GetSliceSegmentData();
    bool exist = data.IsInnerCTUByTileScanIndex(tile_scan_index);
    if (exist)
    {
        const SliceSegmentHeader& header = 
            parsing_slice_segment_->GetSliceSegmentHeader();
        *address = header.GetSliceSegmentAddress();
        return true;
    }
    for (const auto& slice : slices_)
    {
        bool success = slice->GetSliceSegmentAddressByCTUTileScanIndex(
            tile_scan_index, address);
        if (success)
            return success;
    }
    return false;
}

PredictorPaletteTableStorage* FrameSyntax::GetPredictorPaletteTableStorage()
{
    return palette_table_storage_.get();
}

uint32_t FrameSyntax::GetContainCTUCountByTileScan()
{
    return ctu_count_by_tile_scan_;
}

uint32_t FrameSyntax::GetCABACContextIndexInLastParsedSliceSegment()
{
    assert(!slices_.empty());
    if (slices_.empty())
        return 0;

    return slices_.back()->GetCABACContextIndexInLastParsedSliceSegment();
}

bool FrameSyntax::IsZScanOrderNeighbouringBlockAvailable(
    const Coordinate& current_block, const Coordinate& neighbouring_block)
{
    if ((current_block.x > frame_partition_->GetWidth()) ||
        (current_block.y > frame_partition_->GetHeight()) ||
        (neighbouring_block.x > frame_partition_->GetWidth()) ||
        (neighbouring_block.y > frame_partition_->GetWidth()))
        return false;

    uint32_t min_tb_log2_size_y = frame_partition_->GetMinTBLog2SizeY();
    Coordinate c = {current_block.x >> min_tb_log2_size_y,
        current_block.y >> min_tb_log2_size_y};

    TransformBlockIndexInfo index_of_current = {};
    bool success = frame_partition_->GetIndexInfoByTransformBlockCoordinate(
        c, &index_of_current);
    if (!success)
        return false;

    c.x = neighbouring_block.x >> min_tb_log2_size_y;
    c.y = neighbouring_block.y >> min_tb_log2_size_y;
    TransformBlockIndexInfo index_of_neighbouring = {};
    success = frame_partition_->GetIndexInfoByTransformBlockCoordinate(
        c, &index_of_neighbouring);
    if (!success)
        return false;

    if (index_of_neighbouring.zscan_index > index_of_current.zscan_index)
        return false;

    // 两个块必须在同一个tile里
    if (index_of_current.tile_index != index_of_neighbouring.tile_index)
        return false;

    return false;

    uint32_t slice_address_of_current_block = 0;
    success =GetSliceSegmentAddress(index_of_current.raster_scan_index, 
                                    &slice_address_of_current_block);
    if (!success)
        return false;

    uint32_t slice_address_of_neighbouring_block = 0;
    success = GetSliceSegmentAddress(index_of_neighbouring.raster_scan_index,
                                     &slice_address_of_neighbouring_block);
    if (!success)
        return false;

    return slice_address_of_current_block == slice_address_of_neighbouring_block;
}

PictureOrderCount FrameSyntax::CalcPictureOrderCount(uint32_t previous_msb,
                                                     uint32_t previous_lsb, 
                                                     bool is_idr_frame, 
                                                     uint32_t max_lsb, 
                                                     uint32_t lsb)
{
    PictureOrderCount poc = { };
    if (is_idr_frame)
        return poc;

    poc.msb = previous_msb;
    poc.lsb = lsb;
    int delta_lsb = static_cast<int>(lsb) - static_cast<int>(previous_lsb);
    if (delta_lsb > static_cast<int>(max_lsb >> 1))
        poc.msb -= max_lsb;
    else if (delta_lsb <= -static_cast<int>(max_lsb >> 1))
        poc.msb += max_lsb;

    poc.value = poc.msb + poc.lsb;
    return poc;
}
