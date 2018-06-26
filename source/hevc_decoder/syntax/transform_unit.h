#ifndef _TRANSFORM_UNIT_H_
#define _TRANSFORM_UNIT_H_

#include <memory>
#include <vector>
#include <functional>

class CABACReader;
class ITransformUnitContext;
class CrossCompPred;
class ResidualCoding;

class TransformUnit
{
public:
    TransformUnit(uint32_t block_index);
    ~TransformUnit();

    bool Parse(CABACReader* cabac_reader, ITransformUnitContext* context);

private:
    typedef std::function<bool (uint32_t index, uint32_t depth)> CBFChromaGetter;

    bool ParseResidualOfTransformCoefficient(CABACReader* cabac_reader, 
                                             ITransformUnitContext* context,
                                             bool has_coef_for_chroma, 
                                             bool has_coef_for_luma);

    bool ParseResidualDetailInfo(CABACReader* cabac_reader, 
                                 ITransformUnitContext* context, 
                                 bool has_coef_for_luma);

    bool ParseSingleChromaResidualInfoWithCrossCompomentPrediction(
        CABACReader* cabac_reader, ITransformUnitContext* context, 
        bool has_coef_for_luma, const CBFChromaGetter& get_cbf_chroma_value,
        CrossCompPred* cross_comp_pred, 
        std::vector<std::unique_ptr<ResidualCoding>>* residual_codings);

    uint32_t block_index_;
};

#endif
