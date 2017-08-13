#ifndef _SCALING_LIST_DATA_H_
#define _SCALING_LIST_DATA_H_

#include <stdint.h>
#include <vector>

#include "hevc_decoder/syntax/base_syntax.h"

class ScalingListData : public BaseSyntax
{
public:
    ScalingListData();
    virtual ~ScalingListData();

    virtual bool Parse(BitStream* bit_stream) override;

private:
    std::vector<int32_t> GetDefaultScalingList(int size_id, int ref_matrix_id);
};
#endif