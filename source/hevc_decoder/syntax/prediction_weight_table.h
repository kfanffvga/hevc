#ifndef _PREDICTION_WEIGHT_TABLE_H_
#define _PREDICTION_WEIGHT_TABLE_H_

#include <stdint.h>
#include <array>
#include <vector>

class IPredictionWeightTableContext;
class BitStream;

class PredictionWeightTable
{
public:
    PredictionWeightTable();
    ~PredictionWeightTable();

    bool Parse(BitStream* bit_stream, 
               const IPredictionWeightTableContext* context);

private:
    bool Parse(BitStream* bit_stream, const std::vector<int32_t>& ref_poc_list,
               const IPredictionWeightTableContext* context,
               uint32_t luma_log2_weight_denom, 
               uint32_t chroma_log2_weight_demon,
               std::vector<int32_t>* luma_weight, 
               std::vector<int32_t>* luma_offset, 
               std::array<std::vector<int32_t>, 2>* chroma_weight, 
               std::array<std::vector<int32_t>, 2>* chroma_offset);

    void ParseChromaWeightInfo(
        BitStream* bit_stream, int32_t wp_offset_half_range,
        uint32_t luma_log2_weight_denom, 
        std::array<std::vector<int32_t>, 2>* chroma_weight,
        std::array<std::vector<int32_t>, 2>* chroma_offset);
};

#endif 