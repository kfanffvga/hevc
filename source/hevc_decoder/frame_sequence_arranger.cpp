#include "hevc_decoder/frame_sequence_arranger.h"

#include "hevc_decoder/frame.h"

using std::unique_ptr;
using std::list;

FrameSequenceArranger::FrameSequenceArranger()
{

}

FrameSequenceArranger::~FrameSequenceArranger()
{

}

bool FrameSequenceArranger::GetFrames(list<unique_ptr<Frame>>* frames)
{
    return true;
}

void FrameSequenceArranger::Push(uint32_t frame_poc, unique_ptr<Frame> frame)
{

}
