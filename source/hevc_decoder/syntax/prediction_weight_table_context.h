#ifndef _PREDICTION_WEIGHT_TABLE_CONTEXT_H_
#define _PREDICTION_WEIGHT_TABLE_CONTEXT_H_

#include <stdint.h>
#include <vector>

enum ChromaFormatType;
enum SliceType;
struct PictureOrderCount;

class IPredictionWeightTableContext
{
public:
    virtual ChromaFormatType GetChromaFormatType() const = 0;
    virtual PictureOrderCount GetCurrentPictureOrderCount() const = 0;
    virtual uint8_t GetCurrentNuhLayerID() const = 0;
    virtual uint8_t GetNuhLayerIDByPOCValue(uint32_t poc_value) const = 0;
    virtual SliceType GetSliceType() const = 0;
    virtual const std::vector<int32_t>& GetNegativeRefPOCList() const = 0;
    virtual const std::vector<int32_t>& GetPositiveRefPOCList() const = 0;
    virtual uint32_t GetWPOffsetHalfRange() const = 0;
};

#endif