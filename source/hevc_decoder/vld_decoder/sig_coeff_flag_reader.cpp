#include "hevc_decoder/vld_decoder/sig_coeff_flag_reader.h"

#include <cassert>

#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

SigCoeffFlagReader::SigCoeffFlagReader(CABACReader* cabac_reader, 
                                       CABACInitType init_type,
                                       const ISigCoeffFlagReaderContext* context)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(SIG_COEFF_FLAG, init_type))
    , context_(context)
{

}

SigCoeffFlagReader::~SigCoeffFlagReader()
{

}

bool SigCoeffFlagReader::Read()
{
    auto bit_reader = bind(&SigCoeffFlagReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(1);
}

uint32_t SigCoeffFlagReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    uint32_t sig_context = 0;
    const Coordinate& c = context_->GetCurrentRelativeCoordinate();
    if (context_->IsTransformSkipContextEnabled() && 
        (context_->IsTransformSkip() || context_->IsCUTransquantBypass()))
    {
        sig_context = (context_->GetColorIndex() == 0) ? 42 : 16;
    }
    else if (context_->GetLog2TransformSizeY() == 2)
    {
        const uint32_t context_index_map[16] = 
        {
            0, 1, 4, 5, 2, 3, 4, 5, 6, 6, 8, 8, 7, 7, 8, 8
        };
        sig_context = context_index_map[(c.GetY() << 2) + c.GetX()];
    }
    else if (c.GetX() + c.GetY() == 0)
    {
        sig_context = 0;
    }
    else
    {
        uint32_t preview_coded_sub_block = 
            context_->HasCodedSubBlockOnRight() ? 1 : 0;

        if (context_->HasCodedSubBlockOnBottom())
            preview_coded_sub_block += 2;

        Coordinate p(c.GetX() & 3, c.GetY() & 3);
        switch (preview_coded_sub_block)
        {
            case 0:
                sig_context = (p.GetX() + p.GetY() == 0) ? 2 : 
                    ((p.GetX() + p.GetY() < 3) ? 1 : 0);
                break;

            case 1:
                sig_context = (p.GetY() == 0) ? 2 : ((p.GetY() == 1) ? 1 : 0);
                break;

            case 2:
                sig_context = (p.GetX() == 0) ? 2 : ((p.GetX() == 1) ? 1 : 0);
                break;

            case 3:
                sig_context = 2;
                break;

            default:
                assert(false);
                return false;
        }
        if (context_->GetColorIndex() == 0)
        {
            if (c.GetX() + c.GetY() > 0)
                sig_context += 3;

            if (context_->GetLog2TransformSizeY() == 3)
                sig_context += ((context_->GetScanIndex() == 0) ? 9 : 15);
            else
                sig_context += 21;
        }
        else
        {
            sig_context += ((context_->GetLog2TransformSizeY()) == 3 ? 9 : 12);
        }
    }
    uint32_t context_inc = 
        ((context_->GetColorIndex() == 0) ? sig_context : (sig_context + 27));
    return lowest_context_index_ + context_inc;
}

SyntaxElementName SigCoeffFlagReader::GetSyntaxElementName()
{
    return SIG_COEFF_FLAG;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    SigCoeffFlagReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}

