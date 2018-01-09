#ifndef _HEVC_DECODER_H_
#define _HEVC_DECODER_H_

#include <memory>
#include <list>
#include <stdint.h>

#include "hevc_decoder/hevc_decoder.h"

class Frame;
class NALOrganizer;
class ParametersManager;
class DecodeProcessorManager;
class FramePartitionManager;
class CodedVideoSequence;
class FrameSequenceArranger;
class SyntaxDispatcher;
enum HEVC_ERROR;

class HEVCDecoder
{
public:
    HEVCDecoder();
    ~HEVCDecoder();

    bool Init(uint32_t thread_count);

    HEVC_ERROR Decode(const int8_t* buffer, uint32_t len, 
                      std::list<std::unique_ptr<Frame>>* frames);

    HEVC_ERROR Flush();

    void Close();

private:
    std::unique_ptr<ParametersManager> parameters_manager_;
    std::unique_ptr<FrameSequenceArranger> frame_sequence_arranger_;
    std::unique_ptr<FramePartitionManager> frame_partition_manager_;
    std::unique_ptr<DecodeProcessorManager> decode_processor_manager_;
    std::unique_ptr<CodedVideoSequence> coded_video_sequence_;
    std::unique_ptr<SyntaxDispatcher> syntax_dispatcher_;
    std::unique_ptr<NALOrganizer> organizer_;
};
#endif