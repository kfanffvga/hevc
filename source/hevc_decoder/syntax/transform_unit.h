#ifndef _TRANSFORM_UNIT_H_
#define _TRANSFORM_UNIT_H_

class CABACReader;
class ITransformUnitContext;

class TransformUnit
{
public:
    TransformUnit();
    ~TransformUnit();

    bool Parse(CABACReader* cabac_reader, ITransformUnitContext* context);
};

#endif
