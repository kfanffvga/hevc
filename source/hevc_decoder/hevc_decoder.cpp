#include "hevc_decoder/hevc_decoder.h"

#include <cassert>

#include "hevc_decoder/hevc_decoder_def.h"
#include "hevc_decoder/syntax/nal_organizer.h"
#include "hevc_decoder/syntax/parameters_manager.h"
#include "hevc_decoder/syntax/syntax_dispatcher.h"
#include "hevc_decoder/syntax/coded_video_sequence.h"
#include "hevc_decoder/decode_processor_manager.h"
#include "hevc_decoder/frame_sequence_arranger.h"
#include "hevc_decoder/partitions/frame_partition_manager.h"
#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/partitions/block_scan_order_provider.h"

using std::unique_ptr;
using std::list;

HEVCDecoder::HEVCDecoder()
    : parameters_manager_(new ParametersManager())
    , frame_sequence_arranger_(new FrameSequenceArranger())
    , frame_partition_manager_(new FramePartitionManager())
    , cabac_context_storage_(new CABACContextStorage())
    , decode_processor_manager_(
        new DecodeProcessorManager(parameters_manager_.get(), 
                                   frame_sequence_arranger_.get()))
    , coded_video_sequence_(
        new CodedVideoSequence(decode_processor_manager_.get(), 
                               parameters_manager_.get(),
                               frame_partition_manager_.get(),
                               cabac_context_storage_.get()))
    , syntax_dispatcher_(
        new SyntaxDispatcher(parameters_manager_.get(), 
                             coded_video_sequence_.get()))
    , organizer_(new NALOrganizer(syntax_dispatcher_.get()))
{
    BlockScanOrderProvider::GetInstance();
    cabac_context_storage_->Init();
}

HEVCDecoder::~HEVCDecoder()
{

}

bool HEVCDecoder::Init(uint32_t thread_count)
{
    return false;
}

HEVC_ERROR HEVCDecoder::Decode(const int8_t* buffer, uint32_t len, 
                               list<unique_ptr<Frame>>* frames)
{    
    bool success = organizer_->Decode(buffer, len);
    assert(success);
    if (success)
        success = frame_sequence_arranger_->GetFrames(frames);

    return DECODE_SUCCESS;
}

HEVC_ERROR HEVCDecoder::Flush()
{
    return DECODE_SUCCESS;
}

void HEVCDecoder::Close()
{

}

