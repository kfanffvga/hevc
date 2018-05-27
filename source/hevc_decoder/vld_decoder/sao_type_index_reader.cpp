#include "hevc_decoder/vld_decoder/sao_type_index_reader.h"

#include <cassert>
#include <functional>

#include "hevc_decoder/vld_decoder/truncated_rice_value_reader.h"
#include "hevc_decoder/vld_decoder/cabac_context_storage.h"

using std::bind;

SAOTypeIndexReader::SAOTypeIndexReader(CABACReader* cabac_reader, 
                                       CABACInitType init_type)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(SAO_TYPE_IDX, init_type))
{

}

SAOTypeIndexReader::~SAOTypeIndexReader()
{

}

uint32_t SAOTypeIndexReader::Read()
{
    auto bit_reader = bind(&SAOTypeIndexReader::ReadBit, this);
    return TruncatedRiceValueReader(bit_reader).Read(2, 0);
}

uint32_t SAOTypeIndexReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return lowest_context_index_;
}

SyntaxElementName SAOTypeIndexReader::GetSyntaxElementName()
{
    return SAO_TYPE_IDX;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    SAOTypeIndexReader::GetFunctionIndex(uint16_t bin_idx)
{
    const CommonCABACSyntaxReader::ReadFunctionIndex function_indices[2] =
    {
        CommonCABACSyntaxReader::ARITHMETIC_READER,
        CommonCABACSyntaxReader::BYPASS_READER
    };
    assert(bin_idx < 2);
    return function_indices[bin_idx];
}
