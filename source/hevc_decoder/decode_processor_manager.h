#ifndef _DECODE_PROCESSOR_MANAGER_H_
#define _DECODE_PROCESSOR_MANAGER_H_

#include <memory>
#include <list>

class ParametersManager;
class FrameSyntax;
class Frame;

class IFrameSequenceArranger
{
public:
    virtual void Push(uint32_t frame_poc, std::unique_ptr<Frame> frames) = 0;
};

class DecodeProcessorManager
{
public:
    DecodeProcessorManager(ParametersManager* parameters_manager,
                           IFrameSequenceArranger* frame_sequence_arranger);
    ~DecodeProcessorManager();

    bool Flush();
    bool Decode(const FrameSyntax* frame_syntax);

private:
    ParametersManager* parameters_manager_;
    IFrameSequenceArranger* frame_sequence_arranger_;
};
#endif