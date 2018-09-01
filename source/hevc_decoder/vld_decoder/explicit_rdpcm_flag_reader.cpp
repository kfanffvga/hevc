#include "hevc_decoder/vld_decoder/explicit_rdpcm_flag_reader.h"

#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;
using std::min;

namespace
{
const SyntaxElementName explicit_rdpcm_flag_names[2] = 
    {EXPLICIT_RDPCM_FLAG_FOR_LUMA, EXPLICIT_RDPCM_FLAG_FOR_CHROMA};
}

ExplicitRDPCMFlagReader::ExplicitRDPCMFlagReader(CABACReader* cabac_reader, 
                                                 CABACInitType init_type, 
                                                 uint32_t color_index)
    : CommonCABACSyntaxReader(cabac_reader)
    , color_index_(color_index)
    , explicit_rdpcm_flag_name_(
        explicit_rdpcm_flag_names[min(1ui32, color_index_)])

    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(explicit_rdpcm_flag_name_, 
                                                init_type))
{

}

ExplicitRDPCMFlagReader::~ExplicitRDPCMFlagReader()
{

}

bool ExplicitRDPCMFlagReader::Read()
{
    auto bit_reader = bind(&ExplicitRDPCMFlagReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(1);
}

uint32_t ExplicitRDPCMFlagReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return lowest_context_index_;
}

SyntaxElementName ExplicitRDPCMFlagReader::GetSyntaxElementName()
{
    return explicit_rdpcm_flag_name_;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    ExplicitRDPCMFlagReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}

