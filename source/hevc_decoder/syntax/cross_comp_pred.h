#ifndef _CROSS_COMP_PRED_H_
#define _CROSS_COMP_PRED_H_

class CABACReader;
class ICrossCompPredContext;

class CrossCompPred
{
public:
    CrossCompPred();
    ~CrossCompPred();

    bool Parse(CABACReader* cabac_reader, ICrossCompPredContext* context);
};

#endif
