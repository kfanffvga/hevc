﻿#include "hevc_decoder/vld_decoder/coeff_abs_level_greater_number_flag_reader_context.h"

CoeffAbsLevelGreaterNumberFlagReaderContext::CoeffAbsLevelGreaterNumberFlagReaderContext()
    : context_set_(-1)
    , greater_number_context_(1)
{

}

CoeffAbsLevelGreaterNumberFlagReaderContext::~CoeffAbsLevelGreaterNumberFlagReaderContext()
{

}

void CoeffAbsLevelGreaterNumberFlagReaderContext::Init(int32_t sub_block_index, 
                                                       uint32_t color_index)
{
    context_set_ = ((0 == sub_block_index) || (color_index > 0)) ? 0 : 2;
}

int32_t CoeffAbsLevelGreaterNumberFlagReaderContext::GetContextSet()
{
    return context_set_;
}

int32_t CoeffAbsLevelGreaterNumberFlagReaderContext::GetGreaterNumberContext()
{
    return greater_number_context_;
}

void CoeffAbsLevelGreaterNumberFlagReaderContext::UpdateContext(
    bool is_greater_number)
{
    // 这里的greater_number_context_，context_set_ 用来控制真正的概率，
    // 可以理解为：一开始是由greater_number_context_来控制概率的选择，
    // 直到当有值是大于1（is_greater_number == true）的情况，
    // 则进入另外的概率选择情况（context_set_）
    if (greater_number_context_ > 0)
    {
        if (is_greater_number)
            greater_number_context_ = 0;
        else
            ++greater_number_context_;
    }
    if (0 == greater_number_context_)
        ++context_set_;
}