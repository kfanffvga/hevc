#include "hevc_decoder/syntax/transform_tree.h"

TransformTree::TransformTree(const Coordinate& current, const Coordinate& base, 
                             uint32_t transform_unit_size_y, uint32_t layer, 
                             uint32_t block_index)
    : current_point_(current)
    , base_point_(base)
    , transform_unit_size_y_(transform_unit_size_y)
    , layer_(layer)
    , block_index_(block_index)
{

}

TransformTree::~TransformTree()
{

}

bool TransformTree::Parse(CABACReader* cabac_reader)
{
    return false;
}
