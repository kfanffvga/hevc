#include "hevc_decoder/vld_decoder/end_of_slice_segment_flag_reader.h"

#include <cassert>

EndOfSliceSegmentFlagReader::EndOfSliceSegmentFlagReader(CABACReader* reader)
    : CommonCABACSyntaxReader(reader)
{

}

EndOfSliceSegmentFlagReader::~EndOfSliceSegmentFlagReader()
{

}

bool EndOfSliceSegmentFlagReader::Read()
{ 
    return !!ReadBit();
}

int EndOfSliceSegmentFlagReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return 0;
}

SyntaxElementName EndOfSliceSegmentFlagReader::GetSyntaxElementName()
{
    return END_OF_SLICE_SEGMENT_FLAG;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    EndOfSliceSegmentFlagReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::TERMIRATE_READER;
}