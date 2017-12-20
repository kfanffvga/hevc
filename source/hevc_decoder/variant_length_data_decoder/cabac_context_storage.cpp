#include "hevc_decoder/variant_length_data_decoder/cabac_context_storage.h"

#include "hevc_decoder/base/math.h"
#include "hevc_decoder/base/basic_types.h"

using std::vector;
using std::make_pair;

const static vector<int> init_type_to_ctxidx[SYNTAX_ELEMENT_NAME_COUNT - 1][3] = 
{
    {vector<int>{0}, vector<int>{1}, vector<int>{2}},
    {vector<int>{0}, vector<int>{1}, vector<int>{2}},
    {vector<int>{0,1,2}, vector<int>{3,4,5}, vector<int>{6,7,8}},
    {vector<int>{0}, vector<int>{1}, vector<int>{2}},
    {vector<int>{}, vector<int>{0,1,2}, vector<int>{3,4,5}},
    {vector<int>{}, vector<int>{0}, vector<int>{1}},
    {vector<int>{0}, vector<int>{1,2,3,4}, vector<int>{5,6,7,8}},
    {vector<int>{0}, vector<int>{1}, vector<int>{2}},
    {vector<int>{0}, vector<int>{1}, vector<int>{2}},
    {vector<int>{}, vector<int>{0}, vector<int>{1}},
    {vector<int>{}, vector<int>{0}, vector<int>{1}},
    {vector<int>{}, vector<int>{0}, vector<int>{1}},
    {vector<int>{}, vector<int>{0,1,2,3,4}, vector<int>{5,6,7,8,9}},
    {vector<int>{}, vector<int>{0,1}, vector<int>{2,3}},
    {vector<int>{}, vector<int>{0}, vector<int>{1}},
    {vector<int>{0,1,2}, vector<int>{3,4,5}, vector<int>{6,7,8}},
    {vector<int>{0,1}, vector<int>{2,3}, vector<int>{4,5}},
    {vector<int>{0,1,2,3,12}, vector<int>{4,5,6,7,13}, 
     vector<int>{8,9,10,11,14}},

    {vector<int>{}, vector<int>{0}, vector<int>{2}},
    {vector<int>{}, vector<int>{1}, vector<int>{3}},
    {vector<int>{0,1}, vector<int>{2,3}, vector<int>{4,5}},
    {vector<int>{0}, vector<int>{1}, vector<int>{2}},
    {vector<int>{0}, vector<int>{1}, vector<int>{2}},
    {vector<int>{0,1,2,3,4,5,6,7}, vector<int>{8,9,10,11,12,13,14,15}, 
     vector<int>{16,17,18,19,20,21,22,23}},

    {vector<int>{0,1}, vector<int>{2,3}, vector<int>{4,5}},
    {vector<int>{0}, vector<int>{1}, vector<int>{2}},
    {vector<int>{3}, vector<int>{4}, vector<int>{5}},
    {vector<int>{}, vector<int>{0}, vector<int>{1}},
    {vector<int>{}, vector<int>{2}, vector<int>{3}},
    {vector<int>{}, vector<int>{0}, vector<int>{1}},
    {vector<int>{}, vector<int>{2}, vector<int>{3}},
    {vector<int>{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17}, 
     vector<int>{18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35}, 
     vector<int>{36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53}},

    {vector<int>{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17}, 
     vector<int>{18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35}, 
     vector<int>{36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53}},

    {vector<int>{0,1,2,3}, vector<int>{4,5,6,7}, vector<int>{8,9,10,11}},
    {vector<int>{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,
                 24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,126,127}, 
     vector<int>{42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,
                 63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,
                 128,129}, 
     vector<int>{84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,
                 103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,
                 118,119,120,121,122,123,124,125,130,131}},
    
    {vector<int>{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23}, 
     vector<int>{24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,
                 45,46,47}, 
     vector<int>{48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,
                 69,70,71}},

    {vector<int>{0,1,2,3,4,5}, vector<int>{6,7,8,9,10,11}, 
     vector<int>{12,13,14,15,16,17}}
};

// 第一个索引是SyntaxElementName 第二个索引是ctx_idx
const static vector<vector<int>> init_values = 
{ 
    vector<int>{153,153,153}, 
    vector<int>{200,185,160},
    vector<int>{139,141,157,107,139,126,107,139,126},
    vector<int>{154,154,154},
    vector<int>{197,185,201,197,185,201},
    vector<int>{149,134},
    vector<int>{184,154,139,154,154,154,139,154,154},
    vector<int>{184,154,153},
    vector<int>{63,152,152},
    vector<int>{79,79},
    vector<int>{110,154},
    vector<int>{122,137},
    vector<int>{95,79,63,31,31,95,79,63,31,31},
    vector<int>{153,153,153,153},
    vector<int>{168,168},
    vector<int>{153,138,138,124,138,94,224,167,122},
    vector<int>{111,141,153,111,153,111},
    vector<int>{94,138,182,154,149,107,167,154,149,92,167,154,154,154,154},
    vector<int>{140,198,169,198},
    vector<int>{154,154,154,154,154,154},
    vector<int>{139,139,139,139,139,139},
    vector<int>{110,110,124,125,140,153,125,127,140,109,111,143,127,111,79,108,
                123,63,125,110,94,110,95,79,125,111,110,78,110,111,111,95,94,
                108,123,108,125,110,124,110,95,94,125,111,111,79,125,126,111,
                111,79,108,123,93},

    vector<int>{110,110,124,125,140,153,125,127,140,109,111,143,127,111,79,108,
                123,63,125,110,94,110,95,79,125,111,110,78,110,111,111,95,94,
                108,123,108,125,110,124,110,95,94,125,111,111,79,125,126,111,
                111,79,108,123,93},

    vector<int>{91,171,134,141,121,140,61,154,121,140,61,154},
    vector<int>{111,111,125,110,110,94,124,108,124,107,125,141,179,153,125,107,
                125,141,179,153,125,107,125,141,179,153,125,140,139,182,182,152,
                136,152,136,153,136,139,111,136,139,111,155,154,139,154,139,123,
                123,63,153,166,183,140,136,153,154,166,183,140,136,153,154,166,
                183,140,136,153,154,170,153,123,123,107,121,107,121,167,151,183,
                140,151,183,140,170,154,139,153,139,123,123,63,124,166,183,140,
                136,153,154,166,183,140,136,153,154,166,183,140,136,153,154,170,
                153,138,138,122,121,122,121,167,151,183,140,151,183,140,141,111,
                140,140,140,140},

    vector<int>{140,92,137,138,140,152,138,139,153,74,149,92,139,107,122,152,
                140,179,166,182,140,227,122,197,154,196,196,167,154,152,167,182,
                182,134,149,136,153,121,136,137,169,194,166,167,154,167,137,182,
                154,196,167,167,154,152,167,182,182,134,149,136,153,121,136,122,
                169,208,166,167,154,152,167,182},

    vector<int>{138,153,136,167,152,152,107,167,91,122,107,167,107,167,91,107,
                107,167},

    vector<int>{139,139,139,139},
    vector<int>{139,139,139,139},
    vector<int>{154,154,154},
    vector<int>{154,154,154},
    vector<int>{154,154,154,154,154,154,154,154,154,154,154,154,154,154,154,154,
                154,154,154,154,154,154,154,154},

    vector<int>{154,154,154,154,154,154}

};

CABACContextStorage::CABACContextStorage()
    : default_contexts_()
    , tile_contexts_()
    , slice_segment_contexts_()
{

}

CABACContextStorage::~CABACContextStorage()
{

}

void CABACContextStorage::Init()
{
    for (uint32_t qp = 0; qp < 52; ++qp)
        InitByQuantizationParameter(qp);
}

int CABACContextStorage::GetInitType(SliceType slice_type, bool is_cabac_init)
{
    if (I_SLICE == slice_type)
        return 0;

    if (P_SLICE == slice_type)
        return is_cabac_init ? 2 : 1;

    return is_cabac_init ? 1 : 2;
}

CABACContext CABACContextStorage::GetDefaultContext(uint32_t qp)
{
    assert(qp < 52);
    return qp >= 52 ? CABACContext() : default_contexts_[qp];
}

CABACContext CABACContextStorage::GetCTBStorageContext(uint32_t ctb_storage_id)
{
    return ctb_storage_id >= tile_contexts_.size() ? CABACContext() : 
        tile_contexts_[ctb_storage_id];
}

CABACContext CABACContextStorage::GetSliceSegmentStorageContext(
    uint32_t slice_segment_storage_id)
{
    return slice_segment_storage_id >= slice_segment_contexts_.size() ? 
        CABACContext() : slice_segment_contexts_[slice_segment_storage_id];
}

uint32_t CABACContextStorage::SaveCTBStorageContext(const CABACContext& context)
{
    uint32_t index = tile_contexts_.size();
    tile_contexts_.push_back(context);
    return index;
}

uint32_t CABACContextStorage::SaveSliceSegmentStorageContext(
    const CABACContext& context)
{
    uint32_t index = slice_segment_contexts_.size();
    slice_segment_contexts_.push_back(context);
    return index;
}

void CABACContextStorage::ClearStorageContext()
{
    slice_segment_contexts_.clear();
    tile_contexts_.clear();
}

void CABACContextStorage::InitByQuantizationParameter(uint32_t qp)
{
    CABACContext context_of_qp;
    for (const auto& values_of_syntax_element : init_values)
    {
        vector<ContextItem> context_of_syntax_element;
        for (const auto& v : values_of_syntax_element)
        {
            int slope_idx = v >> 4;
            int offset_idx = v & 0xf;
            int m = slope_idx * 5 - 45;
            int n = (offset_idx << 3) - 16;
            int prepare_context_state = 
                Clip3(1, 126, static_cast<int>(((m * qp) >> 4) + n));
            ContextItem item = { };
            item.val_mps = prepare_context_state <= 63 ? 0 : 1;
            item.state_idx = item.val_mps ? 
                prepare_context_state - 64 : 63 - prepare_context_state;
            context_of_syntax_element.push_back(item);
        }
        
        context_of_qp.syntax_context.push_back(context_of_syntax_element);
    }
    // end_of_slice_segment_flag 这个句法元素的概率不受量化参数影响
    context_of_qp.syntax_context.push_back({{63, 0}});
    default_contexts_.push_back(context_of_qp);
}

int CABACContextStorage::GetLowestContextID(SyntaxElementName name, 
                                            int init_type) const
{
    assert(!init_type_to_ctxidx[name][init_type].empty());
    return init_type_to_ctxidx[name][init_type][0];
}
