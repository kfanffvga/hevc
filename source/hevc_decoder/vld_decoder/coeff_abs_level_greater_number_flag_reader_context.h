#ifndef _COEFF_ABS_LEVEL_GREATER_NUMBER_FLAG_READER_CONTEXT_H_
#define _COEFF_ABS_LEVEL_GREATER_NUMBER_FLAG_READER_CONTEXT_H_

#include <stdint.h>

class CoeffAbsLevelGreaterNumberFlagReaderContext
{
public:
    CoeffAbsLevelGreaterNumberFlagReaderContext();
    ~CoeffAbsLevelGreaterNumberFlagReaderContext();

    void Init(int32_t sub_block_index, uint32_t color_index);

    int32_t GetContextSet();
    int32_t GetGreaterNumberContext();

    void UpdateContext(bool is_greater_number);

private:
    // 有多少个block是有大于1级别的残差值的
    int32_t context_set_;
    // 在开始的情况下，从一开始读取到第一个有大于1级别的残差值的像素，一共有多少个
    int32_t greater_number_context_;
};

#endif
