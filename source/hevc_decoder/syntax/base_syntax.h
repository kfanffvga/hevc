#ifndef _BASE_SYNTAX_H_
#define _BASE_SYNTAX_H_

class BitStream;

class BaseSyntax
{
public:
    virtual ~BaseSyntax() { }

    virtual bool Parse(BitStream* bit_stream) = 0;
};

#endif