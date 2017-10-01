#ifndef _SPS_3D_EXTENSION_H_
#define _SPS_3D_EXTENSION_H_

#include <stdint.h>

#include "hevc_decoder/syntax/base_syntax.h"

class SPS3DExtension : public BaseSyntax
{
public:
    SPS3DExtension(uint32_t ctb_log2_size_y);
    virtual ~SPS3DExtension();

    virtual bool Parse(BitStream* bit_stream) override;

private:
    struct SPS3DExtensionItem
    {
        bool is_iv_di_mc_enabled;
        bool is_iv_mv_scal_enabled;
        uint32_t log2_ivmc_sub_pb_size;
        bool is_iv_res_pred_enabled;
        bool is_depth_ref_enabled;
        bool is_vsp_mc_enabled;
        bool is_dbbp_enabled;
        bool is_tex_mc_enabled;
        uint32_t log2_texmc_sub_pb_size;
        bool is_intra_contour_enabled;
        bool is_intra_dc_only_wedge_enabled;
        bool is_cqt_cu_part_pred_enabled;
        bool is_inter_dc_only_enabled;
        bool is_skip_intra_enabled;
    };

    uint32_t ctb_log2_size_y_;
};

#endif