#ifndef _FRAME_SEQUENCE_ARRANGER_H_
#define _FRAME_SEQUENCE_ARRANGER_H_

#include <memory>
#include <list>

#include "hevc_decoder/decode_processor_manager.h"

class FrameSyntax;
class Frame;

class FrameSequenceArranger : public IFrameSequenceArranger
{
public:
    FrameSequenceArranger();
    ~FrameSequenceArranger();

    virtual void Push(uint32_t frame_poc, std::unique_ptr<Frame> frame) override;

    bool GetFrames(std::list<std::unique_ptr<Frame>>* frames);

private:
    std::list<std::pair<uint32_t, std::unique_ptr<Frame>>> frames_;
};

#endif