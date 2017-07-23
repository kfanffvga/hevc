#ifndef _CABAC_CONTEXT_STORAGE_H_
#define _CABAC_CONTEXT_STORAGE_H_

#include <vector>
#include <memory>
#include <array>
#include <map>
#include <stdint.h>

#include "hevc_decoder/variant_length_data_decoder/cabac_context_types.h"

enum SliceType
{
    I_SLICE = 0,
    P_SLICE = 1,
    B_SLICE = 2 
};

class CABACContextStorage
{
public:
    CABACContextStorage();
    ~CABACContextStorage();

    void Init();
    int GetInitType(SliceType slice_type, bool is_cabac_init);
    inline int GetLowestContextID(SyntaxElementName name, int init_type) const;

    CABACContext GetDefaultContext(uint32_t qp);
    CABACContext GetCTBStorageContext(uint32_t ctb_storage_id);
    CABACContext GetSliceSegmentStorageContext(uint32_t slice_segment_storage_id);

    uint32_t SaveCTBStorageContext(const CABACContext& context);
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