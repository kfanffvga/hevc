#include "hevc_decoder/vld_decoder/coded_sub_block_flag_reader.h"

#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;
using std::min;

CodedSubBlockFlagReader::CodedSubBlockFlagReader(
    CABACReader* cabac_reader, CABACInitType init_type, 
    bool has_coded_sub_block_on_right, bool has_coded_sub_block_on_bottom,
    uint32_t color_index)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(CODED_SUB_BLOCK_FLAG, 
                                                init_type))

    , has_coded_sub_block_on_right_(has_coded_sub_block_on_right)
    , has_coded_sub_block_on_bottom_(has_coded_sub_block_on_bottom)
    , color_index_(color_index)
{

}

CodedSubBlockFlagReader::~CodedSubBlockFlagReader()
{

}

bool CodedSubBlockFlagReader::Read()
{
    auto bit_reader = bind(&CodedSubBlockFlagReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(1);
}

uint32_t CodedSubBlockFlagReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
     uint32_t context = 0;
     context += has_coded_sub_block_on_right_ ? 1 : 0;
     context += has_coded_sub_block_on_bottom_ ? 1 : 0;
     return lowest_context_index_ + 
         (0 == color_index_ ? min(context, 1ui32) : (2 + min(context, 1ui32)));
}

SyntaxElementName CodedSubBlockFlagReader::GetSyntaxElementName()
{
    return CODED_SUB_BLOCK_FLAG;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    CodedSubBlockFlagReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}

