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
    int32_t context_set_;
    int32_t greater_number_context_;
};

#endif
