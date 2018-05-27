#include "hevc_decoder/vld_decoder/new_palette_entries_reader.h"

#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

NewPaletteEntriesReader::NewPaletteEntriesReader(CABACReader* reader)
    : CommonCABACSyntaxReader(reader)
{

}

NewPaletteEntriesReader::~NewPaletteEntriesReader()
{

}

uint32_t NewPaletteEntriesReader::Read(uint32_t bit_depth)
{
    auto bit_reader = bind(&NewPaletteEntriesReader::ReadBit, this);
    return FixedLengthValueReader(bit_reader).Read(bit_depth);
}

uint32_t NewPaletteEntriesReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return 0;
}

SyntaxElementName NewPaletteEntriesReader::GetSyntaxElementName()
{
    return SYNTAX_ELEMENT_NAME_COUNT;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    NewPaletteEntriesReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::BYPASS_READER;
}

