#ifndef _PREDICTION_WEIGHT_TABLE_H_
#define _PREDICTION_WEIGHT_TABLE_H_

#include <stdint.h>
#include <vector>
#include <array>

enum SliceType;
enum ChromaFormatType;
class ICodedVideoSequence;
struct PictureOrderCount;
class SequenceParameterSet;
class BitStream;

class PredictionWeightTable
{
public:
    PredictionWeightTable(ChromaFormatType chroma_format_type, 
                          const SequenceParameterSet* sps,
                          const PictureOrderCount& current_poc,
                          uint32_t nuh_layer_id, SliceType slice_type,
                          const std::vector<int32_t>& negative_ref_poc_list,
                          const std::vector<int32_t>& positive_ref_poc_list,
                          const ICodedVideoSequence* coded_video_sequence);
    ~PredictionWeightTable();

    bool Parse(BitStream* bit_stream);

private:
    bool Parse(BitStream* bit_stream, const std::vector<int32_t>& ref_poc_list,
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

    ChromaFormatType chroma_format_type_;
    const PictureOrderCount& current_poc_;
    uint32_t nuh_layer_id_;
    SliceType slice_type_;
    const std::vector<int32_t>& negative_ref_poc_list_;
    const std::vector<int32_t>& positive_ref_poc_list_;
    const ICodedVideoSequence* coded_video_sequence_;
    const SequenceParameterSet* sps_;
};

#endif 