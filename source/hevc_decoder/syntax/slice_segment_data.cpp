#include "hevc_decoder/syntax/slice_segment_data.h"

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/syntax/slice_segment_data_context.h"
#include "hevc_decoder/syntax/slice_segment_data_context.h"
#include "hevc_decoder/syntax/slice_segment_header.h"
#include "hevc_decoder/vld_decoder/cabac_reader.h"
#include "hevc_decoder/vld_decoder/frame_info_provider_for_cabac.h"
#include "hevc_decoder/partitions/frame_partition.h"
#include "hevc_decoder/partitions/slice_segment_address_provider.h"
#include "hevc_decoder/syntax/coded_tree_unit_syntax.h"

using std::shared_ptr;

class CABACReaderInfoProvider : public IFrameInfoProviderForCABAC
                              , public ISliceSegmentAddressProvider
{
public:
    CABACReaderInfoProvider(ISliceSegmentDataContext* slice_segment_data_context,
                            const SliceSegmentData* slice_segment_data,
                            const SliceSegmentHeader* header)
        : slice_segment_data_context_(slice_segment_data_context)
        , header_(header)
        , slice_segment_data_(slice_segment_data)
    {

    }

    virtual ~CABACReaderInfoProvider()
    {

    }

    virtual bool IsTheFirstBlockInTile(const Coordinate& block) const override
    {
        shared_ptr<FramePartition> frame_partition = 
            slice_segment_data_context_->GetFramePartition();
        if (!frame_partition)
            return false;

        return frame_partition->IsTheFirstCTBInTile(block);;
    }

    virtual bool IsTheFirstBlockInRowOfTile(const Coordinate& block) const
        override
    {
        shared_ptr<FramePartition> frame_partition =
            slice_segment_data_context_->GetFramePartition();
        if (!frame_partition)
            return false;

        return frame_partition->IsTheFirstCTBInRowOfTile(block);
    }

    virtual bool IsTheFirstBlockInRowOfFrame(const Coordinate& block) const
        override
    {
        shared_ptr<FramePartition> frame_partition =
            slice_segment_data_context_->GetFramePartition();
        if (!frame_partition)
            return false;

        return frame_partition->IsTheFirstCTBInRowOfFrame(block);
    }

    virtual bool IsTheFirstBlockInSliceSegment(const Coordinate& block) const
    {
        shared_ptr<FramePartition> frame_partition =
            slice_segment_data_context_->GetFramePartition();
        if (!frame_partition)
            return false;

        uint32_t block_raster_scan_index = 0;
        if (!frame_partition->GetRasterScanIndex(block, &block_raster_scan_index))
            return false;

        return header_->GetSliceSegmentAddress() == block_raster_scan_index;
    }

    virtual uint32_t GetCTBHeight() const override
    {
        return header_->GetCTBHeight();
    }

    virtual bool IsZScanOrderNeighbouringBlockAvailable(
        const Coordinate& current_block,
        const Coordinate& neighbouring_block) const override
    {
        shared_ptr<FramePartition> frame_partition =
            slice_segment_data_context_->GetFramePartition();
        if (!frame_partition)
            return false;

        return frame_partition->IsZScanOrderNeighbouringBlockAvailable(
            current_block, neighbouring_block, this);
    }

    virtual bool GetCABACContextIndexInCTU(
        const Coordinate& block, uint32_t* cabac_context_storage_index) const 
        override
    {
        if (!cabac_context_storage_index)
            return false;

        shared_ptr<FramePartition> frame_partition =
            slice_segment_data_context_->GetFramePartition();
        if (!frame_partition)
            return false;

        uint32_t tile_scan_index = 0;
        if (!frame_partition->GetTileScanIndex(block, &tile_scan_index))
            return false;

        return slice_segment_data_->GetCABACContextStorageIndexByTileScanIndex(
            tile_scan_index, cabac_context_storage_index);
    }

    virtual uint32_t GetQuantizationParameter() const override
    {
        return header_->GetQuantizationParameter();
    }

    virtual bool IsEntropyCodingSyncEnabled() const override
    {
        return header_->IsEntropyCodingSyncEnabled();
    }

    virtual bool IsDependentSliceSegment() const override
    {
        return header_->IsDependentSliceSegment();
    }

    virtual uint32_t GetCABACContextIndexInPriorSliceSegment() const 
        override
    {
        ISliceSegmentDataContext* context = slice_segment_data_context_;
        return context->GetCABACContextIndexInPriorSliceSegment();
    }

    virtual bool GetSliceAddressByRasterScanIndex(
        uint32_t index, uint32_t* address) const override
    {
        shared_ptr<FramePartition> frame_partition =
            slice_segment_data_context_->GetFramePartition();
        if (!frame_partition)
            return false;

        uint32_t tile_scan_index = 0;
        bool success = frame_partition->RasterScanIndexToTileScanIndex(
            index, &tile_scan_index);
        if (!success)
            return false;

        return GetSliceAddressByTileScanIndex(tile_scan_index, address);
    }

    virtual bool GetSliceAddressByTileScanIndex(
        uint32_t index, uint32_t* address) const override
    {
        if (!address)
            return false;

        uint32_t index_end = slice_segment_data_->GetStartCTUIndexOfTileScan() + 
            slice_segment_data_->GetCTUCount();
        if (index > index_end)
            return false;

        if (index >= slice_segment_data_->GetStartCTUIndexOfTileScan())
        {
            *address = header_->GetSliceSegmentAddress();
            return true;
        }
        return slice_segment_data_context_->GetSliceSegmentAddressByTileScanIndex(
            index, address);
    }

    virtual bool GetSliceAddress(const Coordinate& coordinate, 
                                 uint32_t* address) const override
    {
        shared_ptr<FramePartition> frame_partition =
            slice_segment_data_context_->GetFramePartition();
        if (!frame_partition)
            return false;

        uint32_t tile_scan_index = 0;
        bool success = frame_partition->GetTileScanIndex(coordinate, 
                                                         &tile_scan_index);
        if (success)
            return false;

        return GetSliceAddressByTileScanIndex(tile_scan_index, address);
    }

private:
    ISliceSegmentDataContext* slice_segment_data_context_;
    const SliceSegmentHeader* header_;
    const SliceSegmentData* slice_segment_data_;
};

SliceSegmentData::SliceSegmentData()
    : start_ctu_index_of_tile_scan_(0)
    , cabac_context_storage_index_(0)
    , ctus_()
{

}

SliceSegmentData::~SliceSegmentData()
{

}

bool SliceSegmentData::Parse(BitStream* bit_stream, 
                             ISliceSegmentDataContext* context)
{
    if (!bit_stream || !context)
        return false;

    CABACContextStorage* storage = context->GetCABACContextStorage();
    if (!storage)
        return false;

    if (!context->GetSliceSegmentHeader())
        return false;

    start_ctu_index_of_tile_scan_ = context->GetFirstCTUIndexOfTileScan();

    CABACReaderInfoProvider cabac_reader_info_provider(
        context, this, context->GetSliceSegmentHeader());
    CABACReader cabac_reader(storage, bit_stream, &cabac_reader_info_provider);
    return false;
}

uint32_t SliceSegmentData::GetCTUCount() const
{
    return ctus_.size();
}

uint32_t SliceSegmentData::GetStartCTUIndexOfTileScan() const
{
    return start_ctu_index_of_tile_scan_;
}

bool SliceSegmentData::GetCABACContextStorageIndexByTileScanIndex(
    uint32_t tile_scan_index, uint32_t* cabac_context_storage_index) const
{
    if (!cabac_context_storage_index)
        return false;

    if (!IsInnerCTUByTileScanIndex(tile_scan_index))
        return false;

    *cabac_context_storage_index = 
        ctus_[tile_scan_index]->GetCABACContextStorageIndex();
    return true;
}

uint32_t SliceSegmentData::GetCABACContextStorageIndex() const
{
    return cabac_context_storage_index_;
}

bool SliceSegmentData::IsInnerCTUByTileScanIndex(uint32_t index) const
{
    return (index >= start_ctu_index_of_tile_scan_) &&
        (index < start_ctu_index_of_tile_scan_ + ctus_.size());
}
