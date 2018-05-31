#ifndef _CODING_UNIT_H_
#define _CODING_UNIT_H_

#include <memory>
#include <vector>

#include "hevc_decoder/base/basic_types.h"

class CABACReader;
class ICodingUnitContext;
class PredictionUnit;

class CodingUnit
{
public:
    CodingUnit(const Coordinate& point, uint32_t layer, uint32_t cb_size_y);
    virtual ~CodingUnit();

    bool Parse(CABACReader* cabac_reader, ICodingUnitContext* context);

    uint32_t GetCurrentLayer() const;
    uint32_t GetCBSizeY() const;
    PredModeType GetPredMode() const;
    bool IsCUTransquantBypass() const;

private:
    bool ParseDetailInfo(CABACReader* cabac_reader, ICodingUnitContext* context, 
                         bool is_cu_skip);

    bool ParseIntraDetailInfo(CABACReader* cabac_reader,
                              ICodingUnitContext* context, 
                              PartModeType part_mode, bool* is_pcm);

    bool ParseInterDetailInfo(CABACReader* cabac_reader, 
                              ICodingUnitContext* context, bool is_cu_skip,
                              PartModeType part_mode);

    Coordinate point_;
    uint32_t layer_;
    uint32_t cb_size_y_;
    PredModeType pred_mode_;
    bool is_cu_transquant_bypass_;

    // maybe empty
    std::vector<std::shared_ptr<PredictionUnit>> prediction_units_;
};

#endif
