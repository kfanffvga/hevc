#ifndef _SYNTAX_DISPATCHER_H_
#define _SYNTAX_DISPATCHER_H_

#include <memory>
#include <utility>
#include <vector>

#include "hevc_decoder/syntax/coded_video_sequence.h"

class NalUnit;
class ParametersManager;
class CodedVideoSequence;
class CABACContextStorage;

class SyntaxDispatcher
{
public:
    SyntaxDispatcher(ParametersManager* parameters_manager,
                     CodedVideoSequence* coded_video_sequence);
    virtual ~SyntaxDispatcher();

    bool CreateSyntaxAndDispatch(std::unique_ptr<NalUnit> nal_unit);
    void Flush();

private:
    ParametersManager* parameters_manager_;
    CodedVideoSequence* coded_video_sequence_;
};

#endif