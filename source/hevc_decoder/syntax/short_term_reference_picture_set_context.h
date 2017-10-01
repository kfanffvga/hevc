#ifndef _SHORT_TERM_REFERENCE_PICTURE_SET_CONTEXT_H_
#define _SHORT_TERM_REFERENCE_PICTURE_SET_CONTEXT_H_

#include <stdint.h>

class ShortTermReferencePictureSet;

class ShortTermReferencePictureSetContext
{
public: 
    virtual ~ShortTermReferencePictureSetContext() { }
    virtual uint32_t GetShortTermReferencePictureSetCount() = 0;
    virtual const ShortTermReferencePictureSet*
        GetShortTermReferencePictureSet(uint32_t index) = 0;

};
#endif