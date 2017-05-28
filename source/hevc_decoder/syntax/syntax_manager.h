#ifndef _SYNTAX_MANAGER_H_
#define _SYNTAX_MANAGER_H_

#include <memory>

class NalUnit;
using std::unique_ptr;

class SyntaxManager
{
public:
    SyntaxManager();
    ~SyntaxManager();

    bool CreateSyntaxElement(unique_ptr<NalUnit> nal_unit);

private:

};

#endif