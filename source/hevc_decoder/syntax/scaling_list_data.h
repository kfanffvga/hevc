#ifndef _SCALING_LIST_DATA_H_
#define _SCALING_LIST_DATA_H_

#include <stdint.h>
#include <vector>

class BitStream;

class ScalingListData
{
public:
    ScalingListData();
    virtual ~ScalingListData();

    bool Parse(BitStream* bit_stream);

private:
    std::vector<int32_t> GetDefaultScalingList(int size_id, int ref_matrix_id);
};
#endif