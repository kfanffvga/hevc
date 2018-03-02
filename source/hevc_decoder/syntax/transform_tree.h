#ifndef _TRANSFORM_TREE_H_
#define _TRANSFORM_TREE_H_

#include "hevc_decoder/base/basic_types.h"

class CABACReader;

class TransformTree
{
public:
    TransformTree(const Coordinate& current, const Coordinate& base, 
                  uint32_t transform_unit_size_y, uint32_t layer, 
                  uint32_t block_index);
    ~TransformTree();

    bool Parse(CABACReader* cabac_reader);

private:
    Coordinate current_point_;
    Coordinate base_point_;
    uint32_t transform_unit_size_y_;
    uint32_t layer_;
    uint32_t block_index_;
};

#endif
