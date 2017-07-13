#ifndef _SYNTAX_DISPATCHER_H_
#define _SYNTAX_DISPATCHER_H_

#include <memory>

class NalUnit;

class SyntaxDispatcher
{
public:
    SyntaxDispatcher();
    ~SyntaxDispatcher();

    bool CreateSyntaxAndDispatch(std::unique_ptr<NalUnit> nal_unit);

private:

};

#endif