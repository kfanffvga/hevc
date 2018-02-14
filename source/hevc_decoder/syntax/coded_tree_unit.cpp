#include "hevc_decoder/syntax/coded_tree_unit.h"

CodedTreeUnit::CodedTreeUnit(uint32_t tile_scan_index)
    : tile_scan_index_(tile_scan_index)
    , cabac_context_storage_index_(0)
{

}

CodedTreeUnit::~CodedTreeUnit()
{

}

bool CodedTreeUnit::Parse(CABACReader* reader, ICodedTreeUnitContext* context)
{
    return false;
}

uint32_t CodedTreeUnit::GetCABACContextStorageIndex() const
{
    return cabac_context_storage_index_;
}

