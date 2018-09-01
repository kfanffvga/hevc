#ifndef _CABAC_CONTEXT_TYPES_H_
#define _CABAC_CONTEXT_TYPES_H_

#include <vector>
#include <array>

enum CABACInitType
{
    FIRST_TYPE = 0,
    SECOND_TYPE = 1,
    THIRD_TYPE = 2
};

struct ContextItem
{
    int val_mps;
    int state_idx;
};

// 此处的句法元素是需要用到概率表的句法元素，但由于某些句法元素是全部用bypass解码，
// termirate解码, 则那些句法元素不会出现在此处，
// 因此在CommonCABACSyntaxReader::GetSyntaxElementName的实现的时候，
// 可以返回SYNTAX_ELEMENT_NAME_COUNT作为该函数的返回值，当用
// SYNTAX_ELEMENT_NAME_COUNT返回的时候，则认为是不需要概率表的

enum SyntaxElementName
{
    SAO_MERGE_FLAG = 0,
    SAO_TYPE_IDX,
    SPLIT_CU_FLAG,
    CU_TRANSQUANT_BYPASS_FLAG,
    CU_SKIP_FLAG,
    PALETTE_MODE_FLAG,
    PRED_MODE_FLAG,
    PART_MODE,
    PREV_INTRA_LUMA_PRED_FLAG,
    INTRA_CHROMA_PRED_MODE,
    RQT_ROOT_CBF,
    MERGE_FLAG,
    MERGE_IDX,
    INTER_PRED_IDC,
    REF_IDX,
    MVP_FLAG,
    SPLIT_TRANSFORM_FLAG,
    CBF_LUMA,
    CBF_CHROMA, // cbf_cr, cbf_cb
    ABS_MVD_GREATER0_FLAG,  
    ABS_MVD_GREATER1_FLAG,
    TU_RESIDUAL_ACT_FLAG,
    LOG2_RES_SCALE_ABS,
    RES_SCALE_SIGN_FLAG,
    TRANSFORM_SKIP_FLAG_FOR_LUMA,
    TRANSFORM_SKIP_FLAG_FOR_CHROMA,
    EXPLICIT_RDPCM_FLAG_FOR_LUMA,
    EXPLICIT_RDPCM_FLAG_FOR_CHROMA,
    EXPLICIT_RDPCM_DIR_FLAG_FOR_LUMA,
    EXPLICIT_RDPCM_DIR_FLAG_FOR_CHROMA,
    LAST_SIG_COEFF_X_PREFIX,
    LAST_SIG_COEFF_Y_PREFIX,
    CODED_SUB_BLOCK_FLAG,
    SIG_COEFF_FLAG,
    COEFF_ABS_LEVEL_GREATER_1_FLAG, 
    COEFF_ABS_LEVEL_GREATER_2_FLAG,
    PALETTE_RUN_PREFIX,
    COPY_ABOVE_PALETTE_INDICES_FLAG,
    COPY_ABOVE_INDICES_FOR_FINAL_RUN_FLAG,
    PALETTE_TRANSPOSE_FLAG,
    CU_QP_DELTA_ABS,
    CU_CHROMA_QP_OFFSET_FLAG,
    CU_CHROMA_QP_OFFSET_IDX,
    END_OF_SLICE_SEGMENT_FLAG,
    SYNTAX_ELEMENT_NAME_COUNT
};

// SyntaxElementName, ctxidx
struct CABACContext
{
    CABACContext()
        : syntax_context()
        , state_coefficient()
    {

    }
    std::vector<std::vector<ContextItem>> syntax_context;
    std::array<uint32_t, 4> state_coefficient;
};

#endif