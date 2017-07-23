#ifndef _CODING_TREE_BLOCK_CONTEXT_H_
#define _CODING_TREE_BLOCK_CONTEXT_H_

#include "hevc_decoder/base/basic_types.h"

class ICodingTreeBlockContext
{
public:
    virtual uint32_t GetCABACStorageIndex() const = 0;
};

#endif