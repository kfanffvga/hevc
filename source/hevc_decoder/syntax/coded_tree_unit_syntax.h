#ifndef _CODED_TREE_UNIT_SYNTAX_H_
#define _CODED_TREE_UNIT_SYNTAX_H_

#include <stdint.h>

class CABACReader;
class ICodedTreeUnitContext;

class CodedTreeUnitSyntax
{
public:
    CodedTreeUnitSyntax(uint32_t tile_scan_index);
    ~CodedTreeUnitSyntax();

    bool Parse(CABACReader* reader, ICodedTreeUnitContext* context);

    uint32_t GetCABACContextStorageIndex() const;

private:
    uint32_t tile_scan_index_;
    uint32_t cabac_context_storage_index_;
};

#endif
