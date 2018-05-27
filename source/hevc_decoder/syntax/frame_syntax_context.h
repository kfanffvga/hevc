#ifndef _FRAME_SYNTAX_CONTEXT_H_
#define _FRAME_SYNTAX_CONTEXT_H_

#include <stdint.h>

struct PictureOrderCount;
class ParametersManager;
class FramePartitionManager;
class CABACContextStorage;

class IFrameSyntaxContext
{
public:
    virtual const ParametersManager& GetParametersManager() const = 0;
    virtual uint8_t GetLayerID(uint32_t poc_value) const = 0;
    virtual bool GetPreviewPictureOrderCount(PictureOrderCount* poc) const = 0;
    virtual FramePartitionManager* GetFramePartitionManager() const = 0;
    virtual CABACContextStorage* GetCABACContextStorage() const = 0;
};

#endif