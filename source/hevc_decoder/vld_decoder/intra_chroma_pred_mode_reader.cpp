#include "hevc_decoder/vld_decoder/intra_chroma_pred_mode_reader.h"

#include "hevc_decoder/vld_decoder/cabac_context_storage.h"

using std::bind;

IntraChromaPredModeReader::IntraChromaPredModeReader(CABACReader* cabac_reader,
                                                     CABACInitType init_type)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(INTRA_CHROMA_PRED_MODE, 
                                                init_type))
{

}

IntraChromaPredModeReader::~IntraChromaPredModeReader()
{

}

uint32_t IntraChromaPredModeReader::Read()
{
    uint32_t r = ReadBit();
    if (0 == r)
        return r;

    for (uint32_t i = 0; i < 2; ++i)
        r = (r << 1) | ReadBit();

    return r & 0x3;
}

uint32_t IntraChromaPredModeReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    if (0 == bin_idx)
        return lowest_context_index_;

    return 0;
}

SyntaxElementName IntraChromaPredModeReader::GetSyntaxElementName()
{
    return INTRA_CHROMA_PRED_MODE;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    IntraChromaPredModeReader::GetFunctionIndex(uint16_t bin_idx)
{
    if (0 == bin_idx)
        return CommonCABACSyntaxReader::ARITHMETIC_READER;

    return CommonCABACSyntaxReader::BYPASS_READER;
}
