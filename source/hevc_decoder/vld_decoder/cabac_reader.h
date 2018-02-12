#ifndef _CABAC_READER_H_
#define _CABAC_READER_H_

#include <stdint.h>

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/vld_decoder/cabac_context_types.h"

class BitStream;
class CABACContextStorage;
class IFrameInfoProviderForCABAC;

class CABACReader
{
public:
    CABACReader(
        CABACContextStorage* cabac_context_storage, BitStream* stream, 
        const IFrameInfoProviderForCABAC* frame_info_provider);
    ~CABACReader();

    bool StartToReadWithNewCTU(const Coordinate& current_ctb);
    bool FinishToReadInCTU(uint32_t* index_of_ctb_pool);
    bool FinishToReadSliceSegment(uint32_t* index_of_slice_segment_pool);

    uint8_t ReadBypassBit();
    uint8_t ReadTerminateBit();
    uint8_t ReadNormalBit(SyntaxElementName syntax_name, uint32_t ctxidx);

private:
    void InitReader(const Coordinate& current_ctb);
    void InitContext(const Coordinate& current_ctb);

    inline void Renormalize();

    CABACContextStorage* cabac_context_storage_;
    BitStream* stream_;
    const IFrameInfoProviderForCABAC* frame_info_provider_;
    bool is_first_ctb_in_slice_segment_;
    CABACContext context_;
    uint16_t current_range_;
    uint16_t offset_;
};

#endif