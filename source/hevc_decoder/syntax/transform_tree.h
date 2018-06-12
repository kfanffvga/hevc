#ifndef _TRANSFORM_TREE_H_
#define _TRANSFORM_TREE_H_

#include <array>
#include <memory>

#include "hevc_decoder/base/basic_types.h"

class CABACReader;
class ITransformTreeContext;

class TransformTree
{
public:
    TransformTree(const Coordinate& current, const Coordinate& base, 
                  uint32_t transform_unit_size_y, uint32_t depth, 
                  uint32_t block_index);
    ~TransformTree();

    bool Parse(CABACReader* cabac_reader, ITransformTreeContext* context);

    const std::array<bool, 2>& IsCBContainedTransformCoefficientOfColorBlue() 
        const;

    const std::array<bool, 2>& IsCBContainedTransformCoefficientOfColorRed() 
        const;

private:
    bool ParseCBFCBValues(CABACReader* cabac_reader, 
                          ITransformTreeContext* context, 
                          bool is_split_transform);

    bool ParseSubTransformTree(CABACReader* cabac_reader, 
                               ITransformTreeContext* context);

    bool ParseTransformUnit(CABACReader* cabac_reader, 
                            ITransformTreeContext* context);

    Coordinate current_point_;
    Coordinate base_point_;
    uint32_t transform_unit_size_y_;
    uint32_t log2_transform_unit_size_y_;
    uint32_t depth_;
    uint32_t block_index_;

    std::array<bool, 2> cbf_cb_;
    std::array<bool, 2> cbf_cr_;
    std::array<std::unique_ptr<TransformTree>, 4> sub_transform_trees_;
};

#endif
