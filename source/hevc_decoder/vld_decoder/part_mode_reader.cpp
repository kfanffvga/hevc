#include "hevc_decoder/vld_decoder/part_mode_reader.h"

#include <cassert>

#include "hevc_decoder/vld_decoder/cabac_context_storage.h"

PartModeReader::PartModeReader(CABACReader* cabac_reader, 
                               CABACInitType init_type, 
                               IPartModeReaderContext* context)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(PART_MODE, init_type))
    , context_(context)
{

}

PartModeReader::~PartModeReader()
{

}

PartModeType PartModeReader::Read()
{
    if (context_->GetCUPredMode() == MODE_INTRA)
        return ReadBit() == 1 ? PART_2Nx2N : PART_NxN;
    
    uint32_t r = ReadBit();
    if (1 == r)
        return PART_2Nx2N;
    else if (context_->GetCUPredMode() == MODE_INTRA)
        return PART_NxN;

    r = (r << 1) | ReadBit();
    if (context_->GetCBSizeY() > context_->GetMinCBSizeY())
    {
        if (context_->IsAsymmetricMotionPartitionsEnabled())
        {
            r = (r << 1) | ReadBit();
            if (3 == r)
                return PART_2NxN;
            
            if (1 == r)
                return PART_Nx2N;

            r = (r << 1) | ReadBit();
            switch (r)
            {
                case 4: 
                    return PART_2NxnU;

                case 5:
                    return PART_2NxnD;

                case 0: 
                    return PART_nLx2N;

                case 1:
                    return PART_nRx2N;

                default: 
                    assert(false);
                    return PART_2Nx2N;
            }
        }
        else
        {
            return 1 == r ? PART_2NxN : PART_Nx2N;
        }
    }
    
    if (context_->GetCBSizeY() == 8)
        return 1 == r ? PART_2NxN : PART_Nx2N;

    if (1 == r)
        return PART_2NxN;

    r = (r << 1) | ReadBit();
    return 1 == r ? PART_Nx2N : PART_NxN;
}

uint32_t PartModeReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    uint32_t context_index = lowest_context_index_;
    switch (bin_idx)
    {
        case 0:
        case 1:
            context_index += bin_idx;
            break;

        case 2:
            if (context_->GetCBSizeY() == context_->GetMinCBSizeY())
                context_index += bin_idx;
            else
                context_index += (bin_idx + 1);

            break;

        case 3:
            context_index = 0;
            break;

        default: 
            assert(false);
            context_index = 0;
    }
    return context_index;
}

SyntaxElementName PartModeReader::GetSyntaxElementName()
{
    return PART_MODE;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    PartModeReader::GetFunctionIndex(uint16_t bin_idx)
{
    return bin_idx > 2 ? CommonCABACSyntaxReader::BYPASS_READER : 
        CommonCABACSyntaxReader::ARITHMETIC_READER;
}
