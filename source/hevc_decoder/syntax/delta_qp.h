#ifndef _DELTA_QP_H_
#define _DELTA_QP_H_

class IDeltaQPContext;
class CABACReader;

class DeltaQP
{
public:
    DeltaQP();
    ~DeltaQP();

    bool Parse(CABACReader* cabac_reader, IDeltaQPContext* context);

};

#endif
