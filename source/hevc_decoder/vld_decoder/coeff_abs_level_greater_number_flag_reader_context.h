#ifndef _COEFF_ABS_LEVEL_GREATER_NUMBER_FLAG_READER_CONTEXT_H_
#define _COEFF_ABS_LEVEL_GREATER_NUMBER_FLAG_READER_CONTEXT_H_

#include <stdint.h>

class CoeffAbsLevelGreaterNumberFlagReaderContext
{
public:
    CoeffAbsLevelGreaterNumberFlagReaderContext();
    ~CoeffAbsLevelGreaterNumberFlagReaderContext();

    void Reset();
    void Init(int32_t sub_block_index, uint32_t color_index);

    int32_t GetContextSet();
    int32_t GetGreaterNumberContext();

    void UpdateContext(bool is_greater_number);

private:
    // 有两个级别，当上一个block有值为真的时候，为一个档次，当上一个block无值为真的时候，为
    // 另外一个档次（值为真代表block里有某个像素变换后的残差值大于1）
    int32_t context_set_;
    // 有四个级别，0为本block有值为真，1，2，3 表示累积有多少值为假，真假的含义如上所述
    int32_t greater_number_context_;
    bool initialzed_;
};

#endif
