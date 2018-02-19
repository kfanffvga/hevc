#ifndef _CABAC_CONTEXT_STORAGE_H_
#define _CABAC_CONTEXT_STORAGE_H_

#include <vector>
#include <memory>
#include <array>
#include <map>
#include <stdint.h>

#include "hevc_decoder/vld_decoder/cabac_context_types.h"

enum SliceType;

class CABACContextStorage
{
public:
    CABACContextStorage();
    ~CABACContextStorage();

    static CABACInitType GetInitType(SliceType slice_type, bool is_cabac_init);
    static uint32_t GetLowestContextID(SyntaxElementName name, 
                                       CABACInitType init_type);

    void Init();

    CABACContext GetDefaultContext(uint32_t qp);
    CABACContext GetCTBStorageContext(uint32_t ctb_storage_id);
    CABACContext GetSliceSegmentStorageContext(uint32_t slice_segment_storage_id);

    uint32_t SaveCTUStorageContext(const CABACContext& context);
    uint32_t SaveSliceSegmentStorageContext(const CABACContext& context);

    void ClearStorageContext();

private:
    void InitByQuantizationParameter(uint32_t qp);

    bool initialzed_;

    // index is qp
    std::vector<CABACContext> default_contexts_;

    // index is value that returned from SaveXXXStorageContext
    std::vector<CABACContext> tile_contexts_;
    std::vector<CABACContext> slice_segment_contexts_;

};

#endif