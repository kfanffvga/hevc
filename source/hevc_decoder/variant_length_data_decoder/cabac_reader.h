#ifndef _CABAC_READER_H_
#define _CABAC_READER_H_

#include "hevc_decoder/base/basic_types.h"

#include "hevc_decoder/variant_length_data_decoder/cabac_context_types.h"

class BitStream;
class CABACContextStorage;
class IFrameSyntaxContext;
class ISliceSegmentContext;

class CABACReader
{
public:
    CABACReader(CABACContextStorage* cabac_context_storage, BitStream* stream, 
                const IFrameSyntaxContext* frame_context, 
                const ISliceSegmentContext* slice_segment_context);
    ~CABACReader();

    bool StartToReadWithNewCTB(const Coordinate& current_ctb);
    bool FinishToReadInCTB(uint32* index_of_ctb_pool);
    bool FinishToReadSliceSegment(uint32* index_of_slice_segment_pool);

    uint8 ReadBypassBit();
    uint8 ReadTerminateBit();
    uint8 ReadNormalBit(SyntaxElementName syntax_name, uint32 ctxidx);

private:
    void InitReader(const Coordinate& current_ctb);
    void InitContext(const Coordinate& current_ctb);

    inline void Renormalize();

    CABACContextStorage* cabac_context_storage_;
    BitStream* stream_;
    const IFrameSyntaxContext* frame_context_;
    const ISliceSegmentContext* slice_segment_context_;
    bool is_first_ctb_in_slice_segment_;
    CABACContext context_;
    uint16 current_range_;
    uint16 offset_;
};

#endif