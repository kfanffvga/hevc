#ifndef _PREDICTION_UNIT_CONTEXT_H_
#define _PREDICTION_UNIT_CONTEXT_H_

class IPredictionUnitContext
{
public:
    virtual bool IsCUSkip() const = 0;
};

#endif
