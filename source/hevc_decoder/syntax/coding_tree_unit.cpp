#include "hevc_decoder/syntax/coding_tree_unit.h"

#include "hevc_decoder/base/plane_util.h"
#include "hevc_decoder/base/coordinate.h"
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
        bool success = frame_partition_->GetRasterScanIndexByCTBCoordinate(
            ctb_point_, &raster_scan_index);
        if (!success)
            return false;

        return raster_scan_index > ctu_context_->GetSliceSegmentAddress();
    }

    virtual bool HasLeftCTBInTile() const override
    {
        uint32_t current_tile_index = 0;
        bool success = frame_partition_->GetTileIndexByCTBCoordinate(
            ctb_point_, &current_tile_index);
        if (!success)
            return false;

        Coordinate left_neighbour_ctb_point(ctb_point_.GetX() - ctb_size_y_, 
                                            ctb_point_.GetY());

        uint32_t left_neighbour_ctb_tile_index = 0;
        success = frame_partition_->GetTileIndexByCTBCoordinate(
            left_neighbour_ctb_point, &left_neighbour_ctb_tile_index);
        if (!success)
            return false;

        return current_tile_index == left_neighbour_ctb_tile_index;
    }

    virtual bool HasUpCTBInSliceSegment() const override
    {
        Coordinate up_neighbour_ctb_point(ctb_point_.GetX(), 
                                          ctb_point_.GetY() - ctb_size_y_);

        uint32_t neighbour_raster_scan_index = 0;
        bool success = frame_partition_->GetRasterScanIndexByCTBCoordinate(
            up_neighbour_ctb_point, &neighbour_raster_scan_index);
        if (!success)
            return false;

        return
            neighbour_raster_scan_index >= ctu_context_->GetSliceSegmentAddress();
    }

    virtual bool HasUpCTBInTile() const override
    {
        uint32_t current_tile_index = 0;
        bool success = frame_partition_->GetTileIndexByCTBCoordinate(
            ctb_point_, &current_tile_index);
        if (!success)
            return false;

        Coordinate up_neighbour_ctb_point(ctb_point_.GetX(), 
                                          ctb_point_.GetY() - ctb_size_y_);
        uint32_t neighbour_tile_index = 0;
        success = frame_partition_->GetTileIndexByCTBCoordinate(
            up_neighbour_ctb_point, &neighbour_tile_index);
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

    virtual uint32_t GetBitDepthOfLuma() const
    {
        return ctu_context_->GetBitDepthOfLuma();
    }

    virtual uint32_t GetBitDepthOfChroma() const
    {
        return ctu_context_->GetBitDepthOfChroma();
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

// 此处的is_cu_qp_delta_coded_直接设置为false是因为一在一个ctu开始的时候，本来就应该要
// 初始化is_cu_qp_delta_coded_的值，但在CodingQuadtreeContextInMyself就需要考虑是否
// 满足is_cu_qp_delta_coded_为true的条件了, is_cu_chroma_qp_offset_coded_ 同理
class CodingQuadtreeContextInCTU : public ICodingQuadtreeContext
{
public:
    CodingQuadtreeContextInCTU(ICodingTreeUnitContext* ctu_context,
                               CodingTreeUnit* ctu, CodingQuadtree* quadtree,
    const shared_ptr<PaletteTable>& palette_table)
        : ctu_context_(ctu_context)
        , ctu_(ctu)
        , palette_table_(palette_table)
        , is_cu_qp_delta_coded_(false)
        , is_cu_chroma_qp_offset_coded_(false)
        , coding_quadtree_(quadtree)
    {

    }

    virtual ~CodingQuadtreeContextInCTU()
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

    virtual uint32_t GetMinTransformBlockSizeY() const override
    {
        return ctu_context_->GetMinTransformBlockSizeY();
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

    virtual bool GetNearestCULayerByCoordinate(const Coordinate& point, 
                                               uint32_t* layer) const override
    {
        bool success = ctu_->GetNearestCULayerByCoordinate(point, layer);
        if (success)
            return success;

        auto ctu = ctu_context_->GetCodingTreeUnit(point);
        if (!ctu)
            return false;

        return ctu->GetNearestCULayerByCoordinate(point, layer);
    }

    virtual shared_ptr<PaletteTable> GetPredictorPaletteTable() const override
    {
        return palette_table_;
    }

    virtual uint32_t GetPaletteMaxSize() const override
    {
        return ctu_context_->GetPaletteMaxSize();
    }

    virtual uint32_t GetBitDepthOfLuma() const override
    {
        return ctu_context_->GetBitDepthOfLuma();
    }

    virtual uint32_t GetBitDepthOfChroma() const override
    {
        return ctu_context_->GetBitDepthOfChroma();
    }

    virtual uint32_t GetPredictorPaletteMaxSize() const override
    {
        return ctu_context_->GetPredictorPaletteMaxSize();
    }

    virtual bool IsCUQPDeltaCoded() const override
    {
        return is_cu_qp_delta_coded_;
    }

    virtual void SetCUQPDeltaVal(int32_t cu_qp_delta_val) override
    {
        assert(!is_cu_qp_delta_coded_);
        is_cu_qp_delta_coded_ = true;
        coding_quadtree_->SetCUQPDeltaVal(cu_qp_delta_val);
    }

    virtual bool IsCUChromaQPOffsetCoded() const override
    {
        return is_cu_chroma_qp_offset_coded_;
    }

    virtual void SetCUChromaQPOffsetCrAndCb(
        int32_t cu_chroma_qp_offset_cr, int32_t cu_chroma_qp_offset_cb) override
    {
        assert(!is_cu_chroma_qp_offset_coded_);
        is_cu_chroma_qp_offset_coded_ = true;
        coding_quadtree_->SetCUChromaQPOffsetCr(cu_chroma_qp_offset_cr);
        coding_quadtree_->SetCUChromaQPOffsetCb(cu_chroma_qp_offset_cb);
    }

    virtual uint32_t GetChromaQPOffsetListtLen() const override
    {
        return ctu_context_->GetChromaQPOffsetListtLen();
    }

    virtual const std::vector<int32_t>& GetCbQPOffsetList() const override
    {
        return ctu_context_->GetCbQPOffsetList();
    }

    virtual const std::vector<int32_t>& GetCrQPOffsetList() const override
    {
        return ctu_context_->GetCrQPOffsetList();
    }

    virtual bool IsResidualAdaptiveColorTransformEnabled() const override
    {
        return ctu_context_->IsResidualAdaptiveColorTransformEnabled();
    }

    virtual bool IsCrossComponentPredictionEnabled() const override
    {
        return ctu_context_->IsCrossComponentPredictionEnabled();
    }

    virtual bool IsTransformSkipEnabled() const override
    {
        return ctu_context_->IsTransformSkipEnabled();
    }

    virtual uint32_t GetMaxTransformSkipSize() const override
    {
        return ctu_context_->GetMaxTransformSkipSize();
    }

    virtual bool IsExplicitRDPCMEnabled() const override
    {
        return ctu_context_->IsExplicitRDPCMEnabled();
    }

    virtual bool IsZScanOrderNeighbouringBlockAvailable(
        const Coordinate& current_block,
        const Coordinate& neighbouring_block) override
    {
        return ctu_context_->IsZScanOrderNeighbouringBlockAvailable(
            current_block, neighbouring_block);
    }

    virtual const shared_ptr<CodingUnit> GetCodingUnit(const Coordinate& p) 
        const override
    {
        const shared_ptr<CodingUnit> cu = ctu_->GetCodingUnit(p);
        if (cu)
            return cu;

        auto ctu = ctu_context_->GetCodingTreeUnit(p);
        return ctu ? ctu->GetCodingUnit(p) : shared_ptr<CodingUnit>();
    }

    virtual bool IsTransformSkipContextEnabled() const override
    {
        return ctu_context_->IsTransformSkipContextEnabled();
    }

    virtual bool IsImplicitRDPCMEnabled() const override
    {
        return ctu_context_->IsImplicitRDPCMEnabled();
    }

    virtual bool IsCABACBypassAlignmentEnabled() const override
    {
        return ctu_context_->IsCABACBypassAlignmentEnabled();
    }

    virtual bool IsSignDataHidingEnabled() const override
    {
        return ctu_context_->IsSignDataHidingEnabled();
    }

    virtual bool IsPersistentRiceAdaptationEnabled() const override
    {
        return ctu_context_->IsPersistentRiceAdaptationEnabled();
    }

    virtual bool HasExtendedPrecisionProcessing() const override
    {
        return ctu_context_->HasExtendedPrecisionProcessing();
    }

private:
    ICodingTreeUnitContext* ctu_context_;
    CodingTreeUnit* ctu_;
    std::shared_ptr<PaletteTable> palette_table_;
    bool is_cu_qp_delta_coded_;
    bool is_cu_chroma_qp_offset_coded_;
    CodingQuadtree* coding_quadtree_;
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
    shared_ptr<PaletteTable> predictor_palette_table =
        context->GetPredictorPaletteTable(point_);
    CodingQuadtreeContextInCTU coding_quadtree_context(context, this, 
                                                       coding_quadtree_.get(),
                                                       predictor_palette_table);
    if (!coding_quadtree_->Parse(reader, &coding_quadtree_context))
        return false;

    if (!reader->FinishToReadInCTU(&cabac_context_storage_index_))
        return false;

    context->SavePredictorPaletteTable(point_, predictor_palette_table);
    return true;
}

uint32_t CodingTreeUnit::GetCABACContextStorageIndex() const
{
    return cabac_context_storage_index_;
}

const SampleAdaptiveOffset* CodingTreeUnit::GetSampleAdaptiveOffset() const
{
    return sample_adaptive_offset_.get();
}

bool CodingTreeUnit::GetNearestCULayerByCoordinate(const Coordinate& point, 
                                                   uint32_t* layer) const
{
    return coding_quadtree_->GetNearestCULayerByCoordinate(point, layer);
}

const shared_ptr<CodingUnit> CodingTreeUnit::GetCodingUnit(
    const Coordinate& point) const
{
    return coding_quadtree_->GetCodingUnit(point);
}
