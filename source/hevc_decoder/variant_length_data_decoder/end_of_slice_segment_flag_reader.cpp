#include "hevc_decoder/variant_length_data_decoder/end_of_slice_segment_flag_reader.h"

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

int EndOfSliceSegmentFlagReader::GetArithmeticContextIndex(uint16 bin_idx)
{
    return 0;
}

SyntaxElementName EndOfSliceSegmentFlagReader::GetSyntaxElementName()
{
    return END_OF_SLICE_SEGMENT_FLAG;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    EndOfSliceSegmentFlagReader::GetFunctionIndex(uint16 bin_idx)
{
    return CommonCABACSyntaxReader::TERMIRATE_READER;
}