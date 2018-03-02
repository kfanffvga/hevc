#include "hevc_decoder/syntax/coding_tree_unit.h"

#include "hevc_decoder/partitions/frame_partition.h"
#include "hevc_decoder/vld_decoder/cabac_reader.h"
#include "hevc_decoder/syntax/coding_tree_unit_context.h"
#include "hevc_decoder/syntax/sample_adaptive_offset_context.h"
#include "hevc_decoder/syntax/slice_segment_header.h"
#include "hevc_decoder/syntax/sample_adaptive_offset.h"
#include "hevc_decoder/syntax/coding_quadtree.h"
#include "hevc_decoder/syntax/coding_quadtree_context.h"
#include "hevc_decoder/partitions/slice_segment_address_provider.h"

using std::shared_ptr;

class SampleAdaptiveOffsetContext : public ISampleAdaptiveOffsetContext
{
public:
    SampleAdaptiveOffsetContext(
        const shared_ptr<FramePartition>& frame_partition, 
        ICodingTreeUnitContext* ctu_context, Coordinate& ctb_point,
        uint32_t ctb_size_y)
        : frame_partition_(frame_partition)
        , ctu_context_(ctu_context)
        , ctb_point_(ctb_point)
        , ctb_size_y_(ctb_size_y)
    {
        
    }

    virtual ~SampleAdaptiveOffsetContext()
    {

    }

    virtual bool HasLeftCTBInSliceSegment() const override
    {
        uint32_t raster_scan_index = 0;
        bool success = frame_partition_->GetRasterScanIndex(ctb_point_, 
                                                            &raster_scan_index);
        if (!success)
            return false;

        return raster_scan_index > ctu_context_->GetSliceSegmentAddress();
    }

    virtual bool HasLeftCTBInTile() const override
    {
        uint32_t current_tile_index = 0;
        bool success = 
            frame_partition_->GetTileIndex(ctb_point_, &current_tile_index);
        if (!success)
            return false;

        Coordinate left_neighbour_ctb_point = 
            {ctb_point_.x - ctb_size_y_, ctb_point_.y};
        
        uint32_t left_neighbour_ctb_tile_index = 0;
        success = frame_partition_->GetTileIndex(left_neighbour_ctb_point, 
                                                 &left_neighbour_ctb_tile_index);
        if (!success)
            return false;
        
        return current_tile_index == left_neighbour_ctb_tile_index;
    }

    virtual bool HasUpCTBInSliceSegment() const override
    {
        Coordinate up_neighbour_ctb_point =
            {ctb_point_.x, ctb_point_.y - ctb_size_y_};

        uint32_t neighbour_raster_scan_index = 0;
        bool success = frame_partition_->GetRasterScanIndex(
            up_neighbour_ctb_point, &neighbour_raster_scan_index);
        if (!success)
            return false;

        return 
            neighbour_raster_scan_index > ctu_context_->GetSliceSegmentAddress();
    }

    virtual bool HasUpCTBInTile() const override
    {
        uint32_t current_tile_index = 0;
        bool success =
            frame_partition_->GetTileIndex(ctb_point_, &current_tile_index);
        if (!success)
            return false;
        
        Coordinate up_neighbour_ctb_point =
            {ctb_point_.x, ctb_point_.y - ctb_size_y_};
        uint32_t neighbour_tile_index = 0;
        success = frame_partition_->GetTileIndex(up_neighbour_ctb_point, 
                                                 &neighbour_tile_index);
        if (!success)
            return false;

        return current_tile_index == neighbour_tile_index;
    }

    virtual const Coordinate& GetCurrentCoordinate() const override
    {
        return ctb_point_;
    }

    virtual CABACInitType GetCABACInitType() const override
    {
        return ctu_context_->GetCABACInitType();
    }

    virtual ChromaFormatType GetChromaFormatType() const override
    {
        return ctu_context_->GetChromaFormatType();
    }

    virtual bool IsSliceSAOLuma() const override
    {
        return ctu_context_->IsSliceSAOLuma();
    }
    
    virtual bool IsSliceSAOChroma() const override
    {
        return ctu_context_->IsSliceSAOChroma();
    }
    
    virtual uint32_t GetBitDepthLuma() const
    {
        return ctu_context_->GetBitDepthLuma();
    }

    virtual uint32_t GetBitDepthChroma() const
    {
        return ctu_context_->GetBitDepthChroma();
    }

    virtual const SampleAdaptiveOffset* GetLeftNeighbourSAO() const override
    {
        CodingTreeUnit* ctu = ctu_context_->GetLeftNeighbourCTU();
        if (!ctu)
            return nullptr;

        return ctu->GetSampleAdaptiveOffset();
    }

    virtual const SampleAdaptiveOffset* GetUpNeighbourSAO() const override
    {
        CodingTreeUnit* ctu = ctu_context_->GetUpNeighbourCTU();
        if (!ctu)
            return nullptr;

        return ctu->GetSampleAdaptiveOffset();
    }

private:
    shared_ptr<FramePartition> frame_partition_;
    ICodingTreeUnitContext* ctu_context_;
    Coordinate ctb_point_;
    uint32_t ctb_size_y_;
};

class CodingQuadtreeContext : public ICodingQuadtreeContext
{
public:
    CodingQuadtreeContext(ICodingTreeUnitContext* ctu_context,
                          CodingTreeUnit* ctu)
        : ctu_context_(ctu_context)
        , ctu_(ctu)
    {

    }

    virtual ~CodingQuadtreeContext()
    {

    }

    virtual uint32_t GetFrameWidthInLumaSamples() const override
    {
        return ctu_context_->GetFrameWidthInLumaSamples();
    }

    virtual uint32_t GetFrameHeightInLumaSamples() const override
    {
        return ctu_context_->GetFrameHeightInLumaSamples();
    }

    virtual uint32_t GetMinCBLog2SizeY() const override
    {
        return ctu_context_->GetMinCBLog2SizeY();
    }

    virtual uint32_t GetMinCBSizeY() const override
    {
        return ctu_context_->GetMinCBSizeY();
    }

    virtual bool IsCUQPDeltaEnabled() const override
    {
        return ctu_context_->IsCUQPDeltaEnabled();
    }

    virtual uint32_t GetLog2MinCUQPDeltaSize() const override
    {
        return ctu_context_->GetLog2MinCUQPDeltaSize();
    }

    virtual bool IsCUChromaQPOffsetEnabled() const override
    {
        return ctu_context_->IsCUChromaQPOffsetEnabled();
    }

    virtual uint32_t GetLog2MinCUChromaQPOffsetSize() const override
    {
        return ctu_context_->GetLog2MinCUChromaQPOffsetSize();
    }

    virtual CABACInitType GetCABACInitType() const override
    {
        return ctu_context_->GetCABACInitType();
    }

    virtual bool IsTransquantBypassEnabled() const override
    {
        return ctu_context_->IsTransquantBypassEnabled();
    }

    virtual SliceType GetSliceType() const override
    {
        return ctu_context_->GetSliceType();
    }

    virtual bool IsPaletteModeEnabled() const override
    {
        return ctu_context_->IsPaletteModeEnabled();
    }

    virtual uint32_t GetMaxTransformBlockSizeY() const override
    {
        return ctu_context_->GetMaxTransformBlockSizeY();
    }

    virtual uint32_t GetMinPCMCodingBlockSizeY() const override
    {
        return ctu_context_->GetMinPCMCodingBlockSizeY();
    }

    virtual uint32_t GetMaxPCMCodingBlockSizeY() const override
    {
        return ctu_context_->GetMaxPCMCodingBlockSizeY();
    }

    virtual bool IsAsymmetricMotionPartitionsEnabled() const override
    {
        return ctu_context_->IsAsymmetricMotionPartitionsEnabled();
    }

    virtual bool IsPCMEnabled() const override
    {
        return ctu_context_->IsPCMEnabled();
    }

    virtual ChromaFormatType GetChromaFormatType() const override
    {
        return ctu_context_->GetChromaFormatType();
    }

    virtual uint32_t GetMaxTransformHierarchyDepthIntra() const override
    {
        return ctu_context_->GetMaxTransformHierarchyDepthIntra();
    }

    virtual uint32_t GetMaxTransformHierarchyDepthInter() const override
    {
        return ctu_context_->GetMaxTransformHierarchyDepthInter();
    }

    virtual bool IsNeighbourBlockAvailable(
        const Coordinate& current, const Coordinate& neighbour) const override
    {
        return ctu_context_->IsNeighbourBlockAvailable(current, neighbour);
    }

    virtual uint32_t GetNearestCULayerByCoordinate(const Coordinate& point)
        const override
    {
        return ctu_->GetNearestCULayerByCoordinate(point);
    }

private:
    ICodingTreeUnitContext* ctu_context_;
    CodingTreeUnit* ctu_;
};

CodingTreeUnit::CodingTreeUnit(uint32_t tile_scan_index, 
                               const Coordinate& point, 
                               uint32_t ctb_log2_size_y)
    : tile_scan_index_(tile_scan_index)
    , point_(point)
    , ctb_log2_size_y_(ctb_log2_size_y)
    , ctb_size_y_(1 << ctb_log2_size_y)
    , cabac_context_storage_index_(0)
    , coding_quadtree_(new CodingQuadtree(point, ctb_log2_size_y, 0))
    , sample_adaptive_offset_()
{

}

CodingTreeUnit::~CodingTreeUnit()
{

}

bool CodingTreeUnit::Parse(CABACReader* reader, ICodingTreeUnitContext* context)
{
    if (!reader || !context)
        return false;

    shared_ptr<FramePartition> frame_partition = context->GetFramePartition();
    if (!frame_partition)
        return false;

    if (!reader->StartToReadWithNewCTU(point_))
        return false;

    if (context->IsSliceSAOLuma() || context->IsSliceSAOChroma())
    {
        sample_adaptive_offset_.reset(new SampleAdaptiveOffset());
        SampleAdaptiveOffsetContext sao_context(frame_partition, context, 
                                                point_, ctb_size_y_);
        if (!sample_adaptive_offset_->Parse(reader, &sao_context))
            return false;
    }
    CodingQuadtreeContext coding_quadtree_context(context, this);
    if (!coding_quadtree_->Parse(reader, &coding_quadtree_context))
        return false;

    return reader->FinishToReadInCTU(&cabac_context_storage_index_);
}

uint32_t CodingTreeUnit::GetCABACContextStorageIndex() const
{
    return cabac_context_storage_index_;
}

const SampleAdaptiveOffset* CodingTreeUnit::GetSampleAdaptiveOffset() const
{
    return sample_adaptive_offset_.get();
}

uint32_t CodingTreeUnit::GetNearestCULayerByCoordinate(const Coordinate& point) 
    const
{
    return coding_quadtree_->GetNearestCULayerByCoordinate(point);
}

