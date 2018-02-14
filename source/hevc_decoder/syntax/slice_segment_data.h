﻿#ifndef _SLICE_SEGMENT_DATA_H_
#define _SLICE_SEGMENT_DATA_H_

#include <memory>
#include <vector>
#include <stdint.h>

class BitStream;
class CodedTreeUnit;
class ISliceSegmentDataContext;
class CABACReader;
class SliceSegmentHeader;
class FramePartition;

class SliceSegmentData
{
public:
    SliceSegmentData();
    ~SliceSegmentData();

    bool Parse(BitStream* bit_stream, ISliceSegmentDataContext* context);

    uint32_t GetCTUCount() const;
    uint32_t GetStartCTUIndexOfTileScan() const;
    bool GetCABACContextStorageIndexByTileScanIndex(
        uint32_t tile_scan_index, uint32_t* cabac_context_storage_index) const;

    uint32_t GetCABACContextStorageIndex() const;
    bool IsInnerCTUByTileScanIndex(uint32_t index) const;

private:
    bool Parse(BitStream* bit_stream, CABACReader* reader, 
               ISliceSegmentDataContext* context);
    bool IsNextCTUNeedCABACInit(
        uint32_t tile_scan_index_of_ctu, SliceSegmentHeader* header, 
        const std::shared_ptr<FramePartition>& frame_partition);

    uint32_t start_ctu_index_of_tile_scan_;
    uint32_t cabac_context_storage_index_;
    std::vector<std::shared_ptr<CodedTreeUnit>> ctus_;
};

#endif