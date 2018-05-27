#ifndef _CHROMA_QP_OFFSET_CONTEXT_H_
#define _CHROMA_QP_OFFSET_CONTEXT_H_

#include <stdint.h>

enum CABACInitType;

class IChromaQPOffsetContext
{
public:
    virtual CABACInitType GetCABACInitType() const = 0;
    virtual bool IsCUChromaQPOffsetEnable() const = 0;
    virtual bool IsCUChromaQPOffsetCoded() const = 0;
    virtual uint32_t GetChromaQPOffsetListtLen() const = 0;
    virtual void SetCUChromaQPOffsetIndex(uint32_t cu_chroma_qp_offset_index) = 0;
};

#endif
