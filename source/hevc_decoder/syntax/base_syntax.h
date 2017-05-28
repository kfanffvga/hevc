#ifndef _BASE_SYNTAX_H_
#define _BASE_SYNTAX_H_

class BaseSyntax
{
public:
    virtual ~BaseSyntax() { }

    virtual bool Parser() = 0;
};

#endif