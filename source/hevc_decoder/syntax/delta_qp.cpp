#include "hevc_decoder/syntax/delta_qp.h"

#include "hevc_decoder/syntax/delta_qp_context.h"
#include "hevc_decoder/vld_decoder/cabac_reader.h"
#include "hevc_decoder/vld_decoder/cu_qp_delta_abs_reader.h"
#include "hevc_decoder/vld_decoder/cu_qp_delta_sign_flag_reader.h"

DeltaQP::DeltaQP()
{

}

DeltaQP::~DeltaQP()
{

}

bool DeltaQP::Parse(CABACReader* cabac_reader, IDeltaQPContext* context)
{
    if (context->IsCUQPDeltaEnabled() && !context->IsCUQPDeltaCoded())
    {
        int32_t cu_qp_delta_abs = 
            CUQPDeltaAbsReader(cabac_reader, context->GetCABACInitType()).Read();
        bool is_negative_cu_qp_delta_sign = false;
        if (cu_qp_delta_abs != 0)
        {
            is_negative_cu_qp_delta_sign = 
                CUQPDeltaSignFlagReader(cabac_reader).Read();
        }
        context->SetCUQPDeltaVal(
            is_negative_cu_qp_delta_sign ? -cu_qp_delta_abs : cu_qp_delta_abs);
    }
    return true;
}

