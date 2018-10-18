#include "hevc_decoder/syntax/slice_segment_data.h"

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/syntax/slice_segment_data_context.h"
#include "hevc_decoder/syntax/slice_segment_data_context.h"
#include "hevc_decoder/syntax/slice_segment_header.h"
#include "hevc_decoder/syntax/coding_tree_unit.h"
#include "hevc_decoder/syntax/coding_tree_unit_context.h"
#include "hevc_decoder/syntax/sps_screen_content_coding_extension.h"
#include "hevc_decoder/syntax/pps_screen_content_coding_extension.h"
#include "hevc_decoder/syntax/pps_range_extension.h"
#include "hevc_decoder/syntax/byte_alignment.h"
#include "hevc_decoder/vld_decoder/cabac_reader.h"
#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/vld_decoder/frame_info_provider_for_cabac.h"
#include "hevc_decoder/vld_decoder/end_of_slice_segment_flag_reader.h"
#include "hevc_decoder/partitions/frame_partition.h"
#include "hevc_decoder/partitions/slice_segment_address_provider.h"
#include "hevc_decoder/syntax/palette_table.h"

using std::shared_ptr;

class CABACReaderInfoProvider : public IFrameInfoProviderForCABAC
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
        bool success = frame_partition->GetRasterScanIndexByCTBCoordinate(
            block, &block_raster_scan_index);
        if (!success)
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
        ISliceSegmentDataContext* context = slice_segment_data_context_;
        return context->IsZScanOrderNeighbouringBlockAvailable(
            current_block, neighbouring_block);
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
        bool success = frame_partition->GetTileScanIndexByCTBCoordinate(
            block, &tile_scan_index);
        if (!success)
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

private:
    ISliceSegmentDataContext* slice_segment_data_context_;
    const SliceSegmentHeader* header_;
    const SliceSegmentData* slice_segment_data_;
};

// 由于代码规则上要求调用者在构造函数传递指针类型的时候必须保证指针在该类的对象的生命周期里
// 安全，因此此处的frame_partition是用构造函数传入，而非用从slice_segment_data_context
// 中获取
class CodingTreeUnitContext : public ICodingTreeUnitContext
{
public:
    CodingTreeUnitContext(const shared_ptr<FramePartition>& frame_partition,
                          SliceSegmentHeader* header, SliceSegmentData* data, 
                          uint32_t tile_scan_index, 
                          bool is_the_first_ctu_in_slice_segment,
                          CABACInitType cabac_init_type,
                          ISliceSegmentDataContext* slice_segment_data_context)
        : frame_partition_(frame_partition)
        , header_(header)
        , data_(data)
        , tile_scan_index_(tile_scan_index)
        , is_the_first_ctu_in_slice_segment_(is_the_first_ctu_in_slice_segment)
        , cabac_init_type_(cabac_init_type)
        , slice_segment_data_context_(slice_segment_data_context)
    {

    }

    virtual ~CodingTreeUnitContext()
    {

    }

    virtual shared_ptr<FramePartition> GetFramePartition() override
    {
        return frame_partition_;
    }

    virtual bool IsSliceSAOLuma() const override
    {
        return header_->IsSliceSAOLuma();
    }

    virtual bool IsSliceSAOChroma() const override
    {
        return header_->IsSliceSAOChroma();
    }

    virtual uint32_t GetSliceSegmentAddress() const override
    {
        return header_->GetSliceSegmentAddress();
    }

    virtual CABACInitType GetCABACInitType() const override
    {
        return cabac_init_type_;
    }

    virtual ChromaFormatType GetChromaFormatType() const override
    {
        return header_->GetChromaFormatType();
    }

    virtual uint32_t GetBitDepthOfLuma() const
    {
        return header_->GetBitDepthOfLuma();
    }

    virtual uint32_t GetBitDepthOfChroma() const
    {
        return header_->GetBitDepthOfChroma();
    }

    virtual uint32_t GetFrameHeightInLumaSamples() const override
    {
        return header_->GetHeight();
    }

    virtual uint32_t GetFrameWidthInLumaSamples() const override
    {
        return header_->GetWidth();
    }

    virtual uint32_t GetMinCBLog2SizeY() const override
    {
        return header_->GetMinCBLog2SizeY();
    }

    virtual uint32_t GetMinCBSizeY() const override
    {
        return header_->GetMinCBSizeY();
    }

    virtual bool IsCUQPDeltaEnabled() const override
    {
        return header_->IsCUQPDeltaEnabled();
    }

    virtual uint32_t GetLog2MinCUQPDeltaSize() const override
    {
        return header_->GetLog2MinCUQPDeltaSize();
    }

    virtual bool IsCUChromaQPOffsetEnabled() const override
    {
        return header_->IsCUChromaQPOffsetEnabled();
    }

    virtual uint32_t GetLog2MinCUChromaQPOffsetSize() const override
    {
        return header_->GetLog2MinCUChromaQPOffsetSize();
    }

    virtual bool IsTransquantBypassEnabled() const override
    {
        return header_->IsTransquantBypassEnabled();
    }

    virtual SliceType GetSliceType() const override
    {
        return header_->GetSliceType();
    }

    virtual bool IsPaletteModeEnabled() const override
    {
        const SPSScreenContentCodingExtension& sps_scc =
            header_->GetSPSScreenContentCodingExtension();
        return sps_scc.IsPaletteModeEnabled();
    }

    virtual uint32_t GetMaxTransformBlockSizeY() const override
    {
        return header_->GetMaxTransformBlockSizeY();
    }

    virtual uint32_t GetMinTransformBlockSizeY() const override
    {
        return header_->GetMinTransformBlockSizeY();
    }

    virtual uint32_t GetMinPCMCodingBlockSizeY() const override
    {
        return header_->GetMinPCMCodingBlockSizeY();
    }

    virtual uint32_t GetMaxPCMCodingBlockSizeY() const override
    {
        return header_->GetMaxPCMCodingBlockSizeY();
    }

    virtual bool IsAsymmetricMotionPartitionsEnabled() const override
    {
        return header_->IsAMPEnabled();
    }

    virtual bool IsPCMEnabled() const override
    {
        return header_->IsPCMEnabled();
    }

    virtual uint32_t GetMaxTransformHierarchyDepthIntra() const override
    {
        return header_->GetMaxTransformHierarchyDepthIntra();
    }

    virtual uint32_t GetMaxTransformHierarchyDepthInter() const override
    {
        return header_->GetMaxTransformHierarchyDepthInter();
    }

    virtual bool IsNeighbourBlockAvailable(
        const Coordinate& current, const Coordinate& neighbour) const override
    {
        ISliceSegmentDataContext* context = slice_segment_data_context_;
        return context->IsZScanOrderNeighbouringBlockAvailable(current, 
                                                               neighbour);
    }

    virtual CodingTreeUnit* GetLeftNeighbourCTU() const override
    {
        Coordinate c = {};
        bool success = frame_partition_->GetCoordinateByTileScanIndex(
            tile_scan_index_, &c);
        if (!success)
            return nullptr;

        c.OffsetX(-static_cast<int32_t>(header_->GetCTBHeight()));
        if (c.GetX() < 0)
            return nullptr;

        uint32_t neighbour_tile_scan_index = 0;
        success = frame_partition_->GetTileScanIndexByCTBCoordinate(
            c, &neighbour_tile_scan_index);
        if (!success)
            return nullptr;
        
        auto ctu = data_->GetCodingTreeUnit(neighbour_tile_scan_index);
        return ctu.get();
    }

    virtual CodingTreeUnit* GetUpNeighbourCTU() const override
    {
        Coordinate c = { };
        bool success = frame_partition_->GetCoordinateByTileScanIndex(
            tile_scan_index_, &c);
        if (!success)
            return nullptr;

        c.OffsetY(-static_cast<int32_t>(header_->GetCTBHeight()));
        if (c.GetY() < 0)
            return nullptr;

        uint32_t neighbour_tile_scan_index = 0;
        success = frame_partition_->GetTileScanIndexByCTBCoordinate(
            c, &neighbour_tile_scan_index);
        if (!success)
            return nullptr;

        auto ctu = data_->GetCodingTreeUnit(neighbour_tile_scan_index);
        return ctu.get();
    }

    virtual shared_ptr<PaletteTable> GetPredictorPaletteTable(
        const Coordinate& point) override
    {
        return slice_segment_data_context_->GetPredictorPaletteTable(
            is_the_first_ctu_in_slice_segment_ ,point);
    }

    virtual void SavePredictorPaletteTable(
        const Coordinate& point, const shared_ptr<PaletteTable>& palette_table) 
        override
    {
        slice_segment_data_context_->SavePredictorPaletteTable(point, 
                                                               palette_table);
    }

    virtual uint32_t GetPaletteMaxSize() const override
    {
        return header_->GetPaletteMaxSize();
    }

    virtual uint32_t GetPredictorPaletteMaxSize() const override
    {
        return header_->GetPredictorPaletteMaxSize();
    }

    virtual uint32_t GetChromaQPOffsetListtLen() const override
    {
        return header_->GetChromaQPOffsetListtLen();
    }

    virtual const std::vector<int32_t>& GetCbQPOffsetList() const override
    {
        return header_->GetCbQPOffsetList();
    }

    virtual const std::vector<int32_t>& GetCrQPOffsetList() const override
    {
        return header_->GetCrQPOffsetList();
    }

    virtual bool IsResidualAdaptiveColorTransformEnabled() const override
    {
        const PPSScreenContentCodingExtension& pps_scc = 
            header_->GetPPSScreenContentCodingExtension();

        return pps_scc.IsResidualAdaptiveColorTransformEnabled();
    }

    virtual bool IsCrossComponentPredictionEnabled() const override
    {
        const PPSRangeExtension& range_extension = 
            header_->GetPPSRangeExtension();

        return range_extension.IsCrossComponentPredictionEnabled();
    }

    virtual bool IsTransformSkipEnabled() const override
    {
        return header_->IsTransformSkipEnabled();
    }

    virtual uint32_t GetMaxTransformSkipSize() const override
    {
        return header_->GetMaxTransformSkipSize();
    }

    virtual bool IsExplicitRDPCMEnabled() const override
    {
        return header_->IsExplicitRDPCMEnabled();
    }

    virtual bool IsZScanOrderNeighbouringBlockAvailable(
        const Coordinate& current_block,
        const Coordinate& neighbouring_block) override
    {
        return slice_segment_data_context_->IsZScanOrderNeighbouringBlockAvailable(
            current_block, neighbouring_block);
    }

    virtual const shared_ptr<CodingTreeUnit> GetCodingTreeUnit(
        const Coordinate& p) const override
    {
        uint32_t log2_ctb_size_y = header_->GetCTBLog2SizeY();
        Coordinate c = 
        {
            (p.GetX() >> log2_ctb_size_y) << log2_ctb_size_y,
            (p.GetY() >> log2_ctb_size_y) << log2_ctb_size_y
        };
        uint32_t title_scan_index = 0;
        bool located = frame_partition_->GetTileScanIndexByCTBCoordinate(
            c, &title_scan_index);
        if (!located)
            return shared_ptr<CodingTreeUnit>();

        auto ctu = data_->GetCodingTreeUnit(title_scan_index);
        if (ctu)
            return ctu;

        return slice_segment_data_context_->GetCodingTreeUnit(title_scan_index);
    }

    virtual bool IsTransformSkipContextEnabled() const override
    {
        return header_->IsTransformSkipContextEnabled();
    }

    virtual bool IsImplicitRDPCMEnabled() const override
    {
        return header_->IsImplicitRDPCMEnabled();
    }

    virtual bool IsCABACBypassAlignmentEnabled() const override
    {
        return header_->IsCABACBypassAlignmentEnabled();
    }

    virtual bool IsSignDataHidingEnabled() const override
    {
        return header_->IsSignDataHidingEnabled();
    }

    virtual bool IsPersistentRiceAdaptationEnabled() const override
    {
        return header_->IsPersistentRiceAdaptationEnabled();
    }

    virtual bool HasExtendedPrecisionProcessing() const override
    {
        return header_->HasExtendedPrecisionProcessing();
    }

private:
    shared_ptr<FramePartition> frame_partition_;
    SliceSegmentHeader* header_;
    SliceSegmentData* data_;
    uint32_t tile_scan_index_;
    bool is_the_first_ctu_in_slice_segment_;
    CABACInitType cabac_init_type_;
    ISliceSegmentDataContext* slice_segment_data_context_;
};

SliceSegmentData::SliceSegmentData()
    : start_ctu_index_of_tile_scan_(0)
    , cabac_context_storage_index_(0)
    , ctus_()
    , predictor_palette_table_(new PaletteTable())
    , is_ctu_parsing_(false)
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

    SliceSegmentHeader* header = context->GetSliceSegmentHeader();
    if (!header || !context->GetFramePartition())
        return false;

    start_ctu_index_of_tile_scan_ = context->GetFirstCTUIndexOfTileScan();
    CABACReaderInfoProvider cabac_reader_info_provider(
        context, this, context->GetSliceSegmentHeader());
    CABACReader cabac_reader(storage, bit_stream, &cabac_reader_info_provider);

    return Parse(&cabac_reader, context);
}

bool SliceSegmentData::Parse(CABACReader* reader, 
                             ISliceSegmentDataContext* context)
{
    assert(reader);
    assert(context);

    SliceSegmentHeader* header = context->GetSliceSegmentHeader();
    shared_ptr<FramePartition> frame_partition = context->GetFramePartition();
    bool is_end_of_slice_segment = false;
    CABACInitType cabac_init_type =
        CABACContextStorage::GetInitType(header->GetSliceType(),
                                         header->IsUsedCABACInit());
    bool is_the_first_ctu_in_slice_segment = false;
    do {
        is_ctu_parsing_ = false;
        uint32_t tile_scan_index = start_ctu_index_of_tile_scan_ + ctus_.size();
        Coordinate c = {};
        bool success = 
            frame_partition->GetCoordinateByTileScanIndex(tile_scan_index, &c);
        if (!success)
            return false;

        is_ctu_parsing_ = true;
        uint32_t ctb_log2_size_y = 
            context->GetSliceSegmentHeader()->GetCTBLog2SizeY();
        shared_ptr<CodingTreeUnit> ctu(
            new CodingTreeUnit(tile_scan_index, c, ctb_log2_size_y));

        CodingTreeUnitContext codeing_tree_unit_context(
            context->GetFramePartition(), context->GetSliceSegmentHeader(), 
            this, tile_scan_index, is_the_first_ctu_in_slice_segment, 
            cabac_init_type, context);

        if (!ctu->Parse(reader, &codeing_tree_unit_context))
            return false;

        is_ctu_parsing_ = false;
        ctus_.push_back(ctu);
        is_end_of_slice_segment = EndOfSliceSegmentFlagReader(reader).Read();

        uint32_t next_tile_scan_index = tile_scan_index + 1;
        
        if (!is_end_of_slice_segment && 
            IsNextCTUNeedCABACInit(next_tile_scan_index, header, frame_partition))
        {
            bool end_of_sub_set_one_bit = EndOfSubSetOneBitReader(reader).Read();
            assert(end_of_sub_set_one_bit);
            if (!end_of_sub_set_one_bit)
                return false;

            ByteAlignment byte_alignment;
            if (!byte_alignment.Parse(reader->GetSourceBitStream()))
                return false;
        }
    } while (!is_end_of_slice_segment);

    return true;
}

bool SliceSegmentData::IsNextCTUNeedCABACInit(
    uint32_t tile_scan_index_of_ctu, SliceSegmentHeader* header, 
    const std::shared_ptr<FramePartition>& frame_partition)
{
    if (header->IsTilesEnabled())
    {
        bool is_the_first_ctu_in_tile = 
            frame_partition->IsTheFirstCTBInTileByTileScanIndex(
                tile_scan_index_of_ctu);
        if (is_the_first_ctu_in_tile)
            return true;
    }
    if (header->IsEntropyCodingSyncEnabled())
    {
        bool is_the_first_ctu_in_row = 
            frame_partition->IsTheFirstCTBInRowOfFrameByTileScanIndex(
                tile_scan_index_of_ctu);
        if (is_the_first_ctu_in_row)
            return true;

        bool is_the_first_ctu_in_row_of_tile = 
            frame_partition->IsTheFirstCTBInRowOfTileByTileScanIndex(
                tile_scan_index_of_ctu);
        if (is_the_first_ctu_in_row_of_tile)
            return true;
    }
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
        (index < start_ctu_index_of_tile_scan_ + ctus_.size() + 
            (is_ctu_parsing_ ? 1 : 0));
}

const shared_ptr<CodingTreeUnit>
    SliceSegmentData::GetCodingTreeUnit(uint32_t index) const
{
    index -= start_ctu_index_of_tile_scan_;
    if (index >= ctus_.size())
        return nullptr;

    return ctus_[index];
}

const PaletteTable& SliceSegmentData::GetSliceSegmentPaletteTable() const
{
    return *predictor_palette_table_;    
}
