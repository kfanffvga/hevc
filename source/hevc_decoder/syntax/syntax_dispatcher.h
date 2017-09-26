#ifndef _SYNTAX_DISPATCHER_H_
#define _SYNTAX_DISPATCHER_H_

#include <memory>

class NalUnit;
class ParametersManager;

class SyntaxDispatcher
{
public:
    SyntaxDispatcher(ParametersManager* parameters_manager);
    ~SyntaxDispatcher();

    bool CreateSyntaxAndDispatch(std::unique_ptr<NalUnit> nal_unit);

private:
    ParametersManager* parameters_manager_;
};

#endif