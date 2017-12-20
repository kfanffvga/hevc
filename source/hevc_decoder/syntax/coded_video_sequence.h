#ifndef _CODED_VIDEO_SEQUENCE_H_
#define _CODED_VIDEO_SEQUENCE_H_

#include <stdint.h>

struct PictureOrderCount;

class ICodedVideoSequence
{
public:
    virtual uint32_t GetLayerID(uint32_t poc_value) const = 0;
    virtual bool GetPreviewPictureOrderCount(PictureOrderCount* poc) const = 0;
};

#endif