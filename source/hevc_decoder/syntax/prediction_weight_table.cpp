#include "hevc_decoder/syntax/prediction_weight_table.h"

#include <boost/multi_array.hpp>

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"
#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/base/math.h"
#include "hevc_decoder/syntax/prediction_weight_table_context.h"

using boost::multi_array;
using std::vector;
using std::array;

PredictionWeightTable::PredictionWeightTable()
{

}

PredictionWeightTable::~PredictionWeightTable()
{

}

bool PredictionWeightTable::Parse(BitStream* bit_stream, 
                                  const IPredictionWeightTableContext* context)
{
    if (!bit_stream)
        return false;

    GolombReader golomb_reader(bit_stream);
    uint32_t luma_log2_weight_denom = golomb_reader.ReadUnsignedValue();
    int32_t chroma_log2_weight_denom = luma_log2_weight_denom;
    if ((context->GetChromaFormatType() != MONO_CHROME) &&
        (context->GetChromaFormatType() != YUV_MONO_CHROME))
    {
        int32_t delta_chroma_log2_weight_denom = golomb_reader.ReadSignedValue();
        chroma_log2_weight_denom += delta_chroma_log2_weight_denom;
    }
    vector<int32_t> luma_weight_for_negative_ref_list;
    vector<int32_t> luma_offset_for_negative_ref_list;
    array<vector<int32_t>, 2> chroma_weight_for_negative_ref_list;
    array<vector<int32_t>, 2> chroma_offset_for_negative_ref_list;
    bool success = Parse(bit_stream, context->GetNegativeRefPOCList(), context,
                         luma_log2_weight_denom, chroma_log2_weight_denom,
                         &luma_weight_for_negative_ref_list,
                         &luma_offset_for_negative_ref_list, 
                         &chroma_weight_for_negative_ref_list,
                         &chroma_offset_for_negative_ref_list);

    if (!success)
        return false;
    
    if (context->GetSliceType() == B_SLICE)
    {
        vector<int32_t> luma_weight_for_positive_ref_list;
        vector<int32_t> luma_offset_for_positive_ref_list;
        array<vector<int32_t>, 2> chroma_weight_for_positive_ref_list;
        array<vector<int32_t>, 2> chroma_offset_for_positive_ref_list;
        success = Parse(bit_stream, context->GetPositiveRefPOCList(), context,
                        luma_log2_weight_denom, chroma_log2_weight_denom,
                        &luma_weight_for_positive_ref_list,
                        &luma_offset_for_positive_ref_list,
                        &chroma_weight_for_positive_ref_list,
                        &chroma_offset_for_positive_ref_list);
    }
    return success;
}

bool PredictionWeightTable::Parse(BitStream* bit_stream, 
                                  const vector<int32_t>& ref_poc_list, 
                                  const IPredictionWeightTableContext* context,
                                  uint32_t luma_log2_weight_denom, 
                                  uint32_t chroma_log2_weight_demon,
                                  vector<int32_t>* luma_weight, 
                                  vector<int32_t>* luma_offset, 
                                  array<vector<int32_t>, 2>* chroma_weight, 
                                  array<vector<int32_t>, 2>* chroma_offset)
{
    if (!bit_stream)
        return false;

    if (!luma_weight || !luma_offset || !chroma_weight || !chroma_offset)
        return false;

    if (ref_poc_list.empty())
        return true;

    multi_array<bool, 1> has_luma_weight(boost::extents[ref_poc_list.size()]);
    for (uint32_t i = 0; i < ref_poc_list.size(); ++i)
    {
        uint32_t ref_nuh_layer_id = 
            context->GetNuhLayerIDByPOCValue(ref_poc_list[i]);
        if ((ref_nuh_layer_id != context->GetCurrentNuhLayerID()) ||
            (ref_poc_list[i] != context->GetCurrentPictureOrderCount().value))
        {
            has_luma_weight[i] = bit_stream->ReadBool();
        }
        else
        {
            has_luma_weight[i] = false;
        }
    }

    multi_array<bool, 1> has_chroma_weight(boost::extents[ref_poc_list.size()]);
    if ((context->GetChromaFormatType() != MONO_CHROME) &&
        (context->GetChromaFormatType() != YUV_MONO_CHROME))
    {
        for (uint32_t i = 0; i < ref_poc_list.size(); ++i)
        {
            uint32_t ref_nuh_layer_id =
                context->GetNuhLayerIDByPOCValue(ref_poc_list[i]);
            if ((ref_nuh_layer_id != context->GetCurrentNuhLayerID()) ||
                (ref_poc_list[i] != context->GetCurrentPictureOrderCount().value))
            {
                has_chroma_weight[i] = bit_stream->ReadBool();
            }
            else
            {
                has_chroma_weight[i] = false;
            }
        }
    }

    GolombReader golomb_reader(bit_stream);
    int32_t luma_weight_denom = 1 << luma_log2_weight_denom;
    int32_t chroma_weight_demon = 1 << chroma_log2_weight_demon;
    int32_t wp_offset_half_range = context->GetWPOffsetHalfRange();
    for (uint32_t i = 0; i < ref_poc_list.size(); ++i)
    {
        int32_t luma_weight_value = luma_weight_denom;
        int32_t luma_offset_value = 0;
        if (has_luma_weight[i])
        {
            int32_t delta_luma_weight = golomb_reader.ReadSignedValue();
            luma_weight_value += delta_luma_weight;

            luma_offset_value = golomb_reader.ReadSignedValue();
        }
        luma_weight->push_back(luma_weight_value);
        luma_offset->push_back(luma_offset_value);

        if (has_chroma_weight[i])
        {
            ParseChromaWeightInfo(bit_stream, wp_offset_half_range, 
                                  chroma_log2_weight_demon, chroma_weight, 
                                  chroma_offset);
        }
        else
        {
            for (uint32_t j = 0; j < 2; ++j)
            {
                (*chroma_weight)[j].push_back(chroma_weight_demon);
                (*chroma_offset)[j].push_back(0);
            }   
        }
    }
    return true;
}

void PredictionWeightTable::ParseChromaWeightInfo(
    BitStream* bit_stream, int32_t wp_offset_half_range, 
    uint32_t chroma_log2_weight_demon, 
    array<vector<int32_t>, 2>* chroma_weight, 
    array<vector<int32_t>, 2>* chroma_offset)
{
    assert(bit_stream);
    GolombReader golomb_reader(bit_stream);
    int32_t chroma_weight_demon = 1 << chroma_log2_weight_demon;
    for (int j = 0; j < 2; ++j)
    {
        int32_t chroma_weight_value = 
            chroma_weight_demon + golomb_reader.ReadSignedValue();
        (*chroma_weight)[j].push_back(chroma_weight_value);

        int32_t delta_chroma_offset_value = golomb_reader.ReadSignedValue();

        int32_t chroma_offset_value = wp_offset_half_range -
            ((wp_offset_half_range * chroma_weight_value) >>
            chroma_log2_weight_demon) + delta_chroma_offset_value;

        chroma_offset_value = Clip3(-wp_offset_half_range, wp_offset_half_range,
                                    chroma_offset_value);

        (*chroma_offset)[j].push_back(chroma_offset_value);
    }
}
