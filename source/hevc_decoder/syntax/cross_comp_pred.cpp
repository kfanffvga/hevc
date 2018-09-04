#include "hevc_decoder/syntax/cross_comp_pred.h"

#include "hevc_decoder/syntax/cross_comp_pred_context.h"
#include "hevc_decoder/vld_decoder/log2_res_scale_abs_reader.h"
#include "hevc_decoder/vld_decoder/res_scale_sign_flag_reader.h"

CrossCompPred::CrossCompPred()
{

}

CrossCompPred::~CrossCompPred()
{

}

bool CrossCompPred::Parse(CABACReader* cabac_reader, 
                          ICrossCompPredContext* context)
{
    Log2ResScaleAbsReader log2_res_scale_abs_reader(
        cabac_reader, context->GetCABACInitType(), context->GetColorIndex() - 1);
    int32_t log2_res_scale_abs = log2_res_scale_abs_reader.Read();
    if (log2_res_scale_abs != 0)
    {
        ResScaleSignFlagReader res_scale_sign_flag_reader(
            cabac_reader, context->GetCABACInitType(), 
            context->GetColorIndex() - 1);
        bool is_negative_res_scale = res_scale_sign_flag_reader.Read();
    }   
    return true;
}
