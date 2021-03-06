﻿#include "hevc_decoder/variant_length_data_decoder/cabac_reader.h"

#include <cassert>

#include "hevc_decoder/base/slice_segment_context.h"
#include "hevc_decoder/base/frame_syntax_context.h"
#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/coding_tree_block_context.h"
#include "hevc_decoder/variant_length_data_decoder/cabac_context_storage.h"

namespace
{
// pstateidx, qrangeidx
static const uint16_t range_of_lps[][4] = 
{ 
    {128, 176, 208, 240}, {128, 167, 197, 227}, {128, 158, 187, 216},
    {123, 150, 178, 205}, {116, 142, 169, 195}, {111, 135, 160, 185}, 
    {105, 128, 152, 175}, {100, 122, 144, 166}, {95, 116, 137, 158}, 
    {90, 110, 130, 150}, {85, 104, 123, 142}, {81, 99, 117, 135}, 
    {77, 94, 111, 128}, {73, 89, 105, 122}, {69, 85, 100, 116}, 
    {66, 80, 95, 110}, {62, 76, 90, 104}, {59, 72, 86, 99}, {56, 69, 81, 94}, 
    {53, 65, 77, 89}, {51, 62, 73, 85}, {48, 59, 69, 80}, {46, 56, 66, 76}, 
    {43, 53, 63, 72}, {41, 50, 59, 69}, {39, 48, 56, 65}, {37, 45, 54, 62}, 
    {35, 43, 51, 59}, {33, 41, 48, 56}, {32, 39, 46, 53}, {30, 37, 43, 50}, 
    {29, 35, 41, 48}, {27, 33, 39, 45}, {26, 31, 37, 43}, {24, 30, 35, 41}, 
    {23, 28, 33, 39}, {22, 27, 32, 37}, {21, 26, 30, 35}, {20, 24, 29, 33}, 
    {19, 23, 27, 31}, {18, 22, 26, 30}, {17, 21, 25, 28}, {16, 20, 23, 27}, 
    {15, 19, 22, 25}, {14, 18, 21, 24}, {14, 17, 20, 23}, {13, 16, 19, 22}, 
    {12, 15, 18, 21}, {12, 14, 17, 20}, {11, 14, 16, 19}, {11, 13, 15, 18}, 
    {10, 12, 15, 17}, {10, 12, 14, 16}, {9, 11, 13, 15}, {9, 11, 12, 14 }, 
    {8, 10, 12, 14 }, {8, 9, 11, 13}, {7, 9, 11, 12 }, {7, 9, 10, 12}, 
    {7, 8, 10, 11}, {6, 8, 9,11 }, {6, 7, 9 ,10 }, {6, 7, 8 ,9}, {2, 2, 2, 2}
    
};

static const uint8_t trans_idx_mps[] = 
{ 
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 
    22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 
    60, 61, 62, 62, 63
};

static const uint8_t trans_idx_lps[] = 
{
    0,  0, 1, 2, 2, 4, 4, 5, 6, 7, 8, 9, 9, 11, 11, 12, 13, 13, 15, 15, 16, 16, 
    18, 18, 19, 19, 21, 21, 22, 22, 23, 24, 24, 25, 26, 26, 27, 27, 28, 29, 29, 
    30, 30, 30, 31, 32, 32, 33, 33, 33, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 
    37, 38, 38, 63
};

}

CABACReader::CABACReader(CABACContextStorage* cabac_context_storage, 
                         BitStream* stream, 
                         const IFrameSyntaxContext* frame_context, 
                         const ISliceSegmentContext* slice_segment_context)
    : cabac_context_storage_(cabac_context_storage)
    , stream_(stream)
    , frame_context_(frame_context)
    , slice_segment_context_(slice_segment_context)
    , is_first_ctb_in_slice_segment_(true)
    , context_()
    , current_range_(0)
    , offset_(0)
{
    assert(cabac_context_storage);
    assert(stream);
    assert(frame_context);
    assert(slice_segment_context);
}

CABACReader::~CABACReader()
{

}

bool CABACReader::StartToReadWithNewCTB(const Coordinate& current_ctb)
{
    do {
        if (is_first_ctb_in_slice_segment_)
        {
            InitReader(current_ctb);
            is_first_ctb_in_slice_segment_ = false;
            break;
        }
    
        if (frame_context_->IsTheFirstBlockInTile(current_ctb) ||
            (frame_context_->IsTheFirstBlockInRowOfTile(current_ctb) && 
             slice_segment_context_->IsEntropyCodingSyncEnabled()))
        {
            InitReader(current_ctb);
            break;
        }
    } while (false);
    return !context_.syntax_context.empty();
}

bool CABACReader::FinishToReadInCTB(uint32_t* index_of_ctb_pool)
{
    if (!index_of_ctb_pool)
        return false;

    *index_of_ctb_pool = cabac_context_storage_->SaveCTBStorageContext(context_);
    return true;
}

void CABACReader::InitReader(const Coordinate& current_ctb)
{
    InitContext(current_ctb);
    current_range_ = 510;
    offset_ = stream_->Read<uint16_t>(9);
}

bool CABACReader::FinishToReadSliceSegment(uint32_t* index_of_slice_segment_pool)
{
    if (!index_of_slice_segment_pool)
        return false;

    *index_of_slice_segment_pool = 
        cabac_context_storage_->SaveSliceSegmentStorageContext(context_);
    return true;
}

void CABACReader::InitContext(const Coordinate& current_ctb)
{
    do {
        if (frame_context_->IsTheFirstBlockInTile(current_ctb))
            break;

        if (slice_segment_context_->IsEntropyCodingSyncEnabled() &&
            (frame_context_->IsTheFirstBlockInRowOfFrame(current_ctb) ||
             frame_context_->IsTheFirstBlockInRowOfTile(current_ctb)))
        {
            Coordinate neighbouring_ctb = 
            {
                current_ctb.x + frame_context_->GetCTBHeight(),
                current_ctb.y - frame_context_->GetCTBHeight()
            };
            bool is_available = 
                frame_context_->IsZScanOrderNeighbouringBlockAvailable(
                    current_ctb, neighbouring_ctb);

            if (!is_available)
                break;

            const ICodingTreeBlockContext* ctb_context = 
                frame_context_->GetCodingTreeBlockContext(neighbouring_ctb);

            uint32_t index = ctb_context->GetCABACStorageIndex();
            context_ = cabac_context_storage_->GetCTBStorageContext(index);
            return;
        }

        if (!is_first_ctb_in_slice_segment_)
            break;

        if (!slice_segment_context_->IsDependentSliceSegment())
            break;

        uint32_t address = slice_segment_context_->GetSliceSegmentAddress();
        const ISliceSegmentContext* independent_slice_segmengt_context = 
            frame_context_->GetIndependentSliceSegmentContext(address);

        if (!independent_slice_segmengt_context)
            break;

        uint32_t index = 
            independent_slice_segmengt_context->GetCABACStorageIndex();
        context_ = cabac_context_storage_->GetSliceSegmentStorageContext(index);
        return;

    } while (false);
    uint32_t qp = slice_segment_context_->GetQuantizationParameter();
    context_ = cabac_context_storage_->GetDefaultContext(qp);
}

uint8_t CABACReader::ReadBypassBit()
{
   offset_ = (offset_ << 1) | stream_->Read<uint16_t>(1);
   if (offset_ > current_range_)
   {
       offset_ -= current_range_;
       return 1;
   }
   return 0;
}

uint8_t CABACReader::ReadTerminateBit()
{
    current_range_ -= 2;
    if (offset_ > current_range_)
        return 1;

    Renormalize();
    return 0;
}

uint8_t CABACReader::ReadNormalBit(SyntaxElementName syntax_name, uint32_t ctxidx)
{
    uint32_t q = (current_range_ >> 6) & 3;
    ContextItem& context_item = context_.syntax_context[syntax_name][ctxidx];
    uint32_t lps_range = range_of_lps[context_item.state_idx][q];
    uint32_t mps_range = current_range_ - lps_range;
    uint8_t val = 0;
    if (offset_ >= mps_range)
    {
        val = !context_item.val_mps;
        offset_ -= mps_range;
        current_range_ = lps_range;
        if (0 == context_item.state_idx)
            context_item.val_mps = 1 - context_item.val_mps;

        context_item.state_idx = trans_idx_lps[context_item.state_idx];
    }
    else
    {
        val = context_item.val_mps;
        context_item.state_idx = trans_idx_mps[context_item.state_idx];
        current_range_ = mps_range;
    }
    if (current_range_ <= 256)
        Renormalize();

    return val;

}

void CABACReader::Renormalize()
{
    current_range_ <<= 1;
    offset_ = (offset_ << 1) | stream_->Read<uint16_t>(1);
}
