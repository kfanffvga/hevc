#include "hevc_decoder/vld_decoder/palette_predictor_run_reader.h"

#include "hevc_decoder/vld_decoder/exponential_golomb_reader.h"

using std::bind;

PalettePredictorRunReader::PalettePredictorRunReader(CABACReader* cabac_reader)
    : CommonCABACSyntaxReader(cabac_reader)
{

}

PalettePredictorRunReader::~PalettePredictorRunReader()
{

}

uint32_t PalettePredictorRunReader::Read()
{
    auto bit_reader = bind(&PalettePredictorRunReader::ReadBit, this);
    return ExponentialGolombReader(bit_reader).Read(0);
}

uint32_t PalettePredictorRunReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return 0;
}

SyntaxElementName PalettePredictorRunReader::GetSyntaxElementName()
{
    return  SYNTAX_ELEMENT_NAME_COUNT;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    PalettePredictorRunReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::BYPASS_READER;
}

