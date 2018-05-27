#include "hevc_decoder/vld_decoder/palette_run_suffix_reader.h"

#include "hevc_decoder/vld_decoder/truncated_binary_reader.h"

using std::bind;

PaletteRunSuffixReader::PaletteRunSuffixReader(CABACReader* reader)
    : CommonCABACSyntaxReader(reader)
{

}

PaletteRunSuffixReader::~PaletteRunSuffixReader()
{

}

uint32_t PaletteRunSuffixReader::Read(uint32_t prefix_offset, 
                                      uint32_t palette_max_run)
{
    uint32_t max_value = (prefix_offset << 1) > palette_max_run ? 
        palette_max_run - prefix_offset : prefix_offset - 1;

    auto bit_reader = bind(&PaletteRunSuffixReader::ReadBit, this);
    return TruncatedBinaryReader(bit_reader).Read(max_value);
}

uint32_t PaletteRunSuffixReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return 0;
}

SyntaxElementName PaletteRunSuffixReader::GetSyntaxElementName()
{
    return SYNTAX_ELEMENT_NAME_COUNT; 
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    PaletteRunSuffixReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::BYPASS_READER;
}
