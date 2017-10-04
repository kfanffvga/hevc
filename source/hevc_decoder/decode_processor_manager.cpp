#include "hevc_decoder/decode_processor_manager.h"

using std::shared_ptr;

DecodeProcessorManager::DecodeProcessorManager(
    ParametersManager* parameters_manager, 
    IFrameSequenceArranger* frame_sequence_arranger)
    : parameters_manager_(parameters_manager)
    , frame_sequence_arranger_(frame_sequence_arranger)
{

}

DecodeProcessorManager::~DecodeProcessorManager()
{

}

bool DecodeProcessorManager::Flush()
{
    return true;
}

bool DecodeProcessorManager::Decode(const shared_ptr<FrameSyntax>& frame_syntax)
{
    return true;
}

