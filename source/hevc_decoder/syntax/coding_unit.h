#ifndef _CODING_UNIT_H_
#define _CODING_UNIT_H_

#include <memory>
#include <vector>
#include <boost/multi_array.hpp>

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/base/coordinate.h"

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
    PartModeType GetPartMode() const;
    bool IsCUTransquantBypass() const;
    bool IsPCM() const;
    const std::vector<uint32_t>& GetIntraChromaPredModeIdentification() const;
    IntraPredModeType GetIntraLumaPredMode(const Coordinate& c) const;
    IntraPredModeType GetIntraChromaPredMode(const Coordinate& c) const;
    const Coordinate& GetCoordinate() const;

private:
    bool ParseDetailInfo(CABACReader* cabac_reader, ICodingUnitContext* context, 
                         bool is_cu_skip);

    bool ParseIntraDetailInfo(CABACReader* cabac_reader,
                              ICodingUnitContext* context, 
                              PartModeType part_mode);

    bool ParseInterDetailInfo(CABACReader* cabac_reader, 
                              ICodingUnitContext* context, bool is_cu_skip,
                              PartModeType part_mode);

    bool ParseIntraLumaPredictedMode(CABACReader* cabac_reader, 
                                     ICodingUnitContext* context, 
                                     uint32_t block_count);

    bool DeriveIntraLumaPredictedModes(
        ICodingUnitContext* context, 
        const boost::multi_array<bool, 1>& is_prev_intra_luma_pred, 
        const std::vector<uint32_t>& mpm_idx,
        const std::vector<uint32_t>& rem_intra_luma_pred_mode);

    IntraPredModeType DeriveSingleIntraLumaPredictedMode(
        ICodingUnitContext* context, bool is_prev_intra_luma_pred, 
        uint32_t mpm_idx, uint32_t rem_intra_luma_pred_mode, 
        const Coordinate& left_pb_point, const Coordinate& up_pb_point);

    IntraPredModeType GetNeighbourBlockIntraPredModeType(
        ICodingUnitContext* context, const Coordinate& neighbour_point);

    std::array<IntraPredModeType, 3> GetCandidateIntraPredModes(
        IntraPredModeType left_pb_intra_luma_pred_mode, 
        IntraPredModeType up_pb_intra_luma_pred_mode);

    bool DeriveIntraChromaPredictedModes(
        ICodingUnitContext* context, 
        const std::vector<IntraPredModeType>& intra_luma_pred_modes,
        const std::vector<uint32_t>& intra_chroma_pred_mode_identification);

    Coordinate point_;
    uint32_t layer_;
    uint32_t cb_size_y_;
    PredModeType pred_mode_;
    PartModeType part_mode_;
    bool is_cu_transquant_bypass_;
    std::vector<uint32_t> intra_chroma_pred_mode_identification_;
    std::vector<IntraPredModeType> intra_luma_pred_modes_;
    std::vector<IntraPredModeType> intra_chroma_pred_modes_;
    bool is_pcm_;

    // maybe empty
    std::vector<std::shared_ptr<PredictionUnit>> prediction_units_;
};

#endif
