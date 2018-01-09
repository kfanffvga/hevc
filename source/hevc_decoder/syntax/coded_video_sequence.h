#ifndef _CODED_VIDEO_SEQUENCE_H_
#define _CODED_VIDEO_SEQUENCE_H_

#include <memory>
#include <vector>

#include "hevc_decoder/syntax/frame_syntax_context.h"

class NalUnit;
class DecodeProcessorManager;
class FramePartitionManager;
class ParametersManager;
class FrameSyntax;

class CodedVideoSequence : public IFrameSyntaxContext
{
public:
    CodedVideoSequence(DecodeProcessorManager* decode_processor_manager,
                       ParametersManager* parameters_manager,
                       FramePartitionManager* frame_partition_manager);
    virtual ~CodedVideoSequence();

    void Flush();
    bool PushNALOfSliceSegment(NalUnit* nal, bool is_idr_frame);

private:
    virtual uint8_t GetLayerID(uint32_t poc_value) const override;
    virtual bool GetPreviewPictureOrderCount(PictureOrderCount* poc) const
        override;

    DecodeProcessorManager* decode_processor_manager_;
    ParametersManager* parameters_manager_;
    FramePartitionManager* frame_partition_manager_;
    std::unique_ptr<FrameSyntax> frame_syntax_;
    std::vector<std::pair<PictureOrderCount, uint8_t>> pocs_info_;
};

#endif