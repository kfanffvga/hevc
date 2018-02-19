#include "hevc_decoder/vld_decoder/sao_merge_flag_reader.h"

#include "hevc_decoder/vld_decoder/cabac_reader.h"
#include "hevc_decoder/vld_decoder/cabac_context_storage.h"

SAOMergeFlagReader::SAOMergeFlagReader(CABACReader* cabac_reader, 
                                       CABACInitType init_type)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(SAO_TYPE_IDX, init_type)
    )
{
    
}

SAOMergeFlagReader::~SAOMergeFlagReader()
{

}

bool SAOMergeFlagReader::Read()
{
    return !!ReadBit();
}

uint32_t SAOMergeFlagReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return lowest_context_index_;
}

SyntaxElementName SAOMergeFlagReader::GetSyntaxElementName()
{
    return SAO_MERGE_FLAG;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    SAOMergeFlagReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}
