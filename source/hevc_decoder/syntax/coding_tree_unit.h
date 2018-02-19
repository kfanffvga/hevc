#ifndef _CODING_TREE_UNIT_H_
#define _CODING_TREE_UNIT_H_

#include <stdint.h>
#include <memory>

// 考虑到将来的硬解上是以ctu作为线程处理单元，以线程组作为tile处理单元，
// 因此除slice_segment_header之外，所有的其他数据，应该是ctu自备
// 也就是说，当进行到预测解码和转换解码的时候，不应该存在着本ctu依赖另外的ctu的情况的出现

class CABACReader;
class ICodingTreeUnitContext;
class SampleAdaptiveOffset;

class CodingTreeUnit
{
public:
    CodingTreeUnit(uint32_t tile_scan_index);
    ~CodingTreeUnit();

    bool Parse(CABACReader* reader, ICodingTreeUnitContext* context);

    uint32_t GetCABACContextStorageIndex() const;
    const SampleAdaptiveOffset* GetSampleAdaptiveOffset() const;

private:
    uint32_t tile_scan_index_;
    uint32_t cabac_context_storage_index_;

    // maybe null
    std::unique_ptr<SampleAdaptiveOffset> sample_adaptive_offset_;
};

#endif
