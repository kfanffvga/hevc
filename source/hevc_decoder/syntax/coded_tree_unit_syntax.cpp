#include "hevc_decoder/syntax/coded_tree_unit_syntax.h"

CodedTreeUnitSyntax::CodedTreeUnitSyntax(uint32_t tile_scan_index)
    : tile_scan_index_(tile_scan_index)
    , cabac_context_storage_index_(0)
{

}

CodedTreeUnitSyntax::~CodedTreeUnitSyntax()
{

}

bool CodedTreeUnitSyntax::Parse(CABACReader* reader, 
                                ICodedTreeUnitContext* context)
{
    return false;
}

uint32_t CodedTreeUnitSyntax::GetCABACContextStorageIndex() const
{
    return cabac_context_storage_index_;
}

