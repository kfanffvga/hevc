#ifndef _DELTA_QP_CONTEXT_H_
#define _DELTA_QP_CONTEXT_H_

#include <stdint.h>

enum CABACInitType;

class IDeltaQPContext
{
public:
    virtual bool IsCUQPDeltaEnabled() const = 0;
    virtual bool IsCUQPDeltaCoded() const = 0;
    virtual void SetCUQPDeltaVal(int32_t cu_qp_delta_val) = 0;
    virtual CABACInitType GetCABACInitType() const = 0;
};

#endif
