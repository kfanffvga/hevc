#ifndef _SYNTAX_DISPATCHER_H_
#define _SYNTAX_DISPATCHER_H_

#include <memory>

class NalUnit;
class ParametersManager;
class FrameSyntax;
class DecodeProcessorManager;

class SyntaxDispatcher
{
public:
    SyntaxDispatcher(ParametersManager* parameters_manager,
                     DecodeProcessorManager* processor_manager);
    ~SyntaxDispatcher();

    bool CreateSyntaxAndDispatch(std::unique_ptr<NalUnit> nal_unit);

private:
    bool CreateSliceSegmentSyntaxAndDispatch(NalUnit* nal_unit);

    ParametersManager* parameters_manager_;
    DecodeProcessorManager* decode_processor_manager_;
    std::unique_ptr<FrameSyntax> frame_syntax_;
};

#endif