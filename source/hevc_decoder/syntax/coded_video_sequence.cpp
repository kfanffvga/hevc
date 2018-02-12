#include "hevc_decoder/syntax/coded_video_sequence.h"

#include <cassert>

#include "hevc_decoder/decode_processor_manager.h"
#include "hevc_decoder/syntax/frame_syntax.h"
#include "hevc_decoder/syntax/slice_segment_syntax.h"
#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/tile_info.h"
#include "hevc_decoder/syntax/nal_unit.h"
#include "hevc_decoder/syntax/slice_segment_context.h"
#include "hevc_decoder/partitions/frame_partition_creator_info_provider_impl.h"
#include "hevc_decoder/partitions/frame_partition_manager.h"
#include "hevc_decoder/syntax/slice_segment_header.h"

using std::pair;
using std::make_pair;
using std::shared_ptr;
using std::find_if;

class SliceSegmentContext : public ISliceSegmentContext
{
public:
    SliceSegmentContext(FrameSyntax* frame_syntax, 
                        const IFrameSyntaxContext* frame_context,
                        FramePartitionManager* frame_partition_manager,
                        CABACContextStorage* cabac_context_storage)
        : frame_syntax_(frame_syntax)
        , frame_context_(frame_context)
        , frame_partition_manager_(frame_partition_manager)
    {
        assert(frame_syntax);
        assert(frame_context);
        assert(frame_partition_manager);
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
        if (!header.IsFirstSliceSegmentInPic())
            return frame_syntax_->HasFramePartition();

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

        frame_syntax_->SetFramePartition(
            frame_partition_manager_->Get(provider));
        return true;
    }

    virtual shared_ptr<FramePartition> GetFramePartition() override
    {
        return frame_syntax_->GetFramePartition();
    }

    virtual CABACContextStorage* GetCABACContextStorage() override
    {
        return cabac_context_storage_;
    }

    virtual bool GetSliceSegmentAddressByTileScanIndex(
        uint32_t tile_scan_index, uint32_t* slice_segment_address)
    {
        return frame_syntax_->GetSliceSegmentAddressByTileScanIndex(
            tile_scan_index, slice_segment_address);
    }

    virtual uint32_t GetFirstCTUIndexOfTileScan() override
    {
        return frame_syntax_->GetContainCTUCountByTileScan() - 1;
    }

    virtual uint32_t GetCABACContextIndexInLastParsedSliceSegment() 
        override
    {
        return 
            frame_syntax_->GetCABACContextIndexInLastParsedSliceSegment();
    }

private:
    FrameSyntax* frame_syntax_;
    const IFrameSyntaxContext* frame_context_;
    FramePartitionManager* frame_partition_manager_;
    CABACContextStorage* cabac_context_storage_;

};

CodedVideoSequence::CodedVideoSequence(
    DecodeProcessorManager* decode_processor_manager,
    ParametersManager* parameters_manager, 
    FramePartitionManager* frame_partition_manager,
    CABACContextStorage* cabac_context_storage)
    : decode_processor_manager_(decode_processor_manager)
    , parameters_manager_(parameters_manager)
    , frame_partition_manager_(frame_partition_manager)
    , cabac_context_storage_(cabac_context_storage)
    , frame_syntax_()
    , pocs_info_()
{

}

CodedVideoSequence::~CodedVideoSequence()
{

}

uint8_t CodedVideoSequence::GetLayerID(uint32_t poc_value) const
{
    auto searchor =
        [poc_value](const pair<PictureOrderCount, uint8_t>& poc) -> bool
    {
        return poc.first.value == poc_value;
    };

    auto r = find_if(pocs_info_.begin(), pocs_info_.end(), searchor);
    return r != pocs_info_.end() ? r->second : 0;
}

bool CodedVideoSequence::GetPreviewPictureOrderCount(PictureOrderCount* poc) 
    const
{
    if (!poc || pocs_info_.empty())
        return false;

    *poc = pocs_info_[pocs_info_.size() - 1].first;
    return true;
}

void CodedVideoSequence::Flush()
{
    decode_processor_manager_->Flush();
    pocs_info_.clear();
}

bool CodedVideoSequence::PushNALOfSliceSegment(NalUnit* nal, bool is_idr_frame)
{
    if (!nal)
        return false;

    BitStream* bit_stream = nal->GetBitSteam();
    bool is_new_frame = bit_stream->ReadBool();
    if (is_new_frame)
    {
        bool success = decode_processor_manager_->Decode(frame_syntax_.get());
        if (!success)
            return false;

        if (is_idr_frame)
            Flush();

        frame_syntax_.reset(new FrameSyntax(this));
    }
    bit_stream->Seek(bit_stream->GetBytePosition(),
                     bit_stream->GetBitPosition() - 1);

    shared_ptr<SliceSegmentSyntax> slice_segment_syntax(
        new SliceSegmentSyntax(nal->GetNalUnitType(), nal->GetNuhLayerID(), 
                               parameters_manager_));
    SliceSegmentContext slice_segment_context(frame_syntax_.get(), this, 
                                              frame_partition_manager_,
                                              cabac_context_storage_);
    bool success = slice_segment_syntax->Parse(nal->GetBitSteam(),
                                               &slice_segment_context);
    if (!success)
        return false;

    success = frame_syntax_->AddSliceSegment(slice_segment_syntax);
    if (!success)
        return false;

    if (is_new_frame)
    {
        pocs_info_.push_back(
            make_pair(frame_syntax_->GetPictureOrderCount(), 
                      nal->GetNuhLayerID()));
    }
    return true;
}
