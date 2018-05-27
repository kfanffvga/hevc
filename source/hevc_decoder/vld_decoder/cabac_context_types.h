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

// �˴��ľ䷨Ԫ������Ҫ�õ����ʱ�ľ䷨Ԫ�أ�������ĳЩ�䷨Ԫ����ȫ����bypass���룬
// termirate����, ����Щ�䷨Ԫ�ز�������ڴ˴���
// �����CommonCABACSyntaxReader::GetSyntaxElementName��ʵ�ֵ�ʱ��
// ���Է���SYNTAX_ELEMENT_NAME_COUNT��Ϊ�ú����ķ���ֵ������
// SYNTAX_ELEMENT_NAME_COUNT���ص�ʱ������Ϊ�ǲ���Ҫ���ʱ��

enum SyntaxElementName
{
    SAO_MERGE_FLAG = 0,
    SAO_TYPE_IDX,
    SPLIT_CU_FLAG,
    CU_TRANSQUANT_BYPASS_FLAG,
    CU_SKIP_FLAG,
    PRED_MODE_FLAG,
    PALETTE_MODE_FLAG,
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
    ABS_MVD_GREATER0,
    ABS_MVD_GREATER1,
    CU_QP_DELTA_ABS,
    CU_CHROMA_QP_OFFSET_FLAG,
    CU_CHROMA_QP_OFFSET_IDX,
    LOG2_RES_SCALE_ABS_PLUS_1,
    RES_SCALE_SIGN_FLAG,
    TRANSFORM_SKIP_FLAG_0,
    TRANSFORM_SKIP_FLAG_1,
    TRANSFORM_SKIP_FLAG_2,
    EXPLICIT_RDPCM_FLAG_0,
    EXPLICIT_RDPCM_FLAG_1_AND_2,
    EXPLICIT_RDPCM_DIR_FLAG_0,
    EXPLICIT_RDPCM_DIR_FLAG_1_AND_2,
    LAST_SIG_COEFF_X_PREFIX,
    LAST_SIG_COEFF_Y_PREFIX,
    CODED_SUB_BLOCK_FLAG,
    SIG_COEFF_FLAG,
    COEFF_ABS_LEVEL_GREATER1_FLAG,
    COEFF_ABS_LEVEL_GREATER2_FLAG,
    PALETTE_RUN_PREFIX,
    COPY_ABOVE_PALETTE_INDICES_FLAG,
    COPY_ABOVE_INDICES_FOR_FINAL_RUN_FLAG,
    PALETTE_TRANSPOSE_FLAG,
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