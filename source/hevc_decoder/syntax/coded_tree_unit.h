#ifndef _CODED_TREE_UNIT_H_
#define _CODED_TREE_UNIT_H_

#include <stdint.h>

class CABACReader;
class ICodedTreeUnitContext;

class CodedTreeUnit
{
public:
    CodedTreeUnit(uint32_t tile_scan_index);
    ~CodedTreeUnit();

    bool Parse(CABACReader* reader, ICodedTreeUnitContext* context);

    uint32_t GetCABACContextStorageIndex() const;

private:
    uint32_t tile_scan_index_;
    uint32_t cabac_context_storage_index_;
};

#endif
