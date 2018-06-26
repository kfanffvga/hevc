#ifndef _RESIDUAL_CODING_H_
#define _RESIDUAL_CODING_H_

class CABACReader;
class IResidualCodingContext;

class ResidualCoding
{
public:
    ResidualCoding();
    ~ResidualCoding();

    bool Parse(CABACReader* cabac_reader, IResidualCodingContext* context);

private:

};
#endif
