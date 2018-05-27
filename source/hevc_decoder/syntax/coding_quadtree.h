#ifndef _CODING_QUANTREE_H_
#define _CODING_QUANTREE_H_

#include <stdint.h>
#include <memory>
#include <array>

#include "hevc_decoder/base/basic_types.h"

class CABACReader;
class ICodingQuadtreeContext;

class CodingQuadtree
{
public: 
    CodingQuadtree(const Coordinate& point, uint32_t cb_log2_size_y, 
                   uint32_t layer);
    ~CodingQuadtree();

    bool Parse(CABACReader* cabac_reader, ICodingQuadtreeContext* context);

    uint32_t GetNearestCULayerByCoordinate(const Coordinate& point) const;
    uint32_t GetCurrentLayer() const;
    uint32_t GetCUQPDeltaVal() const;
    uint32_t GetLog2SizeY() const;
    void SetCUQPDeltaVal(int32_t cu_qp_delta_val);
    void SetCUChromaQPOffsetCb(int32_t cu_chroma_qp_offset_cb);
    void SetCUChromaQPOffsetCr(int32_t cu_chroma_qp_offset_cr);

private:
    Coordinate point_;
    uint32_t cb_log2_size_y_;
    uint32_t cb_size_y_;
    uint32_t layer_;
    int32_t cu_qp_delta_val_;
    int32_t cu_chroma_qp_offset_cb_;
    int32_t cu_chroma_qp_offset_cr_;
    // maybe null 假如是null，代表是有cu或者有越界
    std::array<std::unique_ptr<CodingQuadtree>, 4> sub_coding_quadtrees_;
    
};

#endif
