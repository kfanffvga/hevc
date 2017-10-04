#include "hevc_decoder/decode_processor_manager.h"

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

bool DecodeProcessorManager::Decode(const FrameSyntax* frame_syntax)
{
    return true;
}

