#include "hevc_decoder/syntax/chroma_qp_offset.h"

#include "hevc_decoder/syntax/chroma_qp_offset_context.h"
#include "hevc_decoder/vld_decoder/cu_chroma_qp_offset_flag_reader.h"
#include "hevc_decoder/vld_decoder/cu_chroma_qp_offset_idx_reader.h"

ChromaQPOffset::ChromaQPOffset()
{

}

ChromaQPOffset::~ChromaQPOffset()
{

}

bool ChromaQPOffset::Parse(CABACReader* cabac_reader, 
                           IChromaQPOffsetContext* context)
{
    if (context->IsCUChromaQPOffsetEnable() &&
        !context->IsCUChromaQPOffsetCoded())
    {
        CUChromaQPOffsetFlagReader cu_chroma_qp_offset_flag_reader(
            cabac_reader, context->GetCABACInitType());
        bool has_cu_chroma_qp_offset = cu_chroma_qp_offset_flag_reader.Read();
        if (has_cu_chroma_qp_offset &&
            (context->GetChromaQPOffsetListtLen() > 0))
        {
            CUChromaQPOffsetIDXReader cu_chroma_qp_offset_idx_reader(
                cabac_reader, context->GetCABACInitType());
            uint32_t cu_chroma_qp_offset_idx = 
                cu_chroma_qp_offset_idx_reader.Read(
                    context->GetChromaQPOffsetListtLen());

            context->SetCUChromaQPOffsetIndex(cu_chroma_qp_offset_idx);
        }
    }
    return true;
}
