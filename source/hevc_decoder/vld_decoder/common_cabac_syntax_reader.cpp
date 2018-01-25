#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

#include "hevc_decoder/vld_decoder/cabac_reader.h"

using std::bind;

CommonCABACSyntaxReader::CommonCABACSyntaxReader(CABACReader* reader)
    : bin_idx_(0)
    , read_functions_()
    , reader_(reader_)
{
    read_functions_[ARITHMETIC_READER] = 
        bind(&CommonCABACSyntaxReader::ArithmeticRead, this);
    read_functions_[TERMIRATE_READER] = 
        bind(&CommonCABACSyntaxReader::TerminateRead, this);
    read_functions_[BYPASS_READER] =
        bind(&CommonCABACSyntaxReader::BypassRead, this);

}

CommonCABACSyntaxReader::~CommonCABACSyntaxReader()
{

}

uint8_t CommonCABACSyntaxReader::ReadBit()
{
    uint8_t value = read_functions_[GetFunctionIndex(bin_idx_)]();
    ++bin_idx_;
    return value;
}

uint8_t CommonCABACSyntaxReader::ArithmeticRead()
{
    return reader_->ReadNormalBit(GetSyntaxElementName(), 
                                  GetArithmeticContextIndex(bin_idx_));
}

uint8_t CommonCABACSyntaxReader::BypassRead()
{
    return reader_->ReadBypassBit();
}

uint8_t CommonCABACSyntaxReader::TerminateRead()
{
    return reader_->ReadTerminateBit();
}
