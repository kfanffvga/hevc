#ifndef _PPS_RANGE_EXTENSION_H_
#define _PPS_RANGE_EXTENSION_H_

#include <stdint.h>
#include <vector>

class BitStream;

class PPSRangeExtension 
{
public:
    PPSRangeExtension();
    ~PPSRangeExtension();

    bool Parse(BitStream* bit_stream, bool is_transform_skip_enabled);

    bool IsChromaQPOffsetListEnabled() const;
    uint32_t GetDiffCUChromaQPOffsetDepth() const;
    uint32_t GetChromaQPOffsetListtLen() const;
    const std::vector<int32_t>& GetCbQPOffsetList() const;
    const std::vector<int32_t>& GetCrQPOffsetList() const;

private:
    bool is_chroma_qp_offset_list_enabled_;
    uint32_t diff_cu_chroma_qp_offset_depth_;
    std::vector<int32_t> cb_qp_offset_list_;
    std::vector<int32_t> cr_qp_offset_list_;
};

#endif