#include "hevc_decoder/syntax/transform_unit.h"

TransformUnit::TransformUnit()
{

}

TransformUnit::~TransformUnit()
{

}

bool TransformUnit::Parse(CABACReader* cabac_reader, 
                          ITransformUnitContext* context)
{
    return false;
}