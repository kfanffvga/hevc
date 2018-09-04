#ifndef _CROSS_COMP_PRED_CONTEXT_H_
#define _CROSS_COMP_PRED_CONTEXT_H_

#include <stdint.h>

enum CABACInitType;

class ICrossCompPredContext
{
public:
    virtual uint32_t GetColorIndex() const = 0;
    virtual CABACInitType GetCABACInitType() const = 0;
};

#endif
