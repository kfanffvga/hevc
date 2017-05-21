#ifndef _FRAME_SYNTAX_CONTEXT_H_
#define _FRAME_SYNTAX_CONTEXT_H_

class IFrameSyntaxContext
{
public:
    virtual uint32 GetSliceAddressByRasterScanBlockIndex(uint32 index) const = 0;
};

#endif