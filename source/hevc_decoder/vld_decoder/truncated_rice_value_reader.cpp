#include "hevc_decoder/vld_decoder/truncated_rice_value_reader.h"

#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

TruncatedRiceValueReader::TruncatedRiceValueReader(const BitReader& bit_reader)
    : bit_reader_(bit_reader)
{

}

TruncatedRiceValueReader::~TruncatedRiceValueReader()
{

}

// 其实可以在输入时候传入前缀最大多少位，后缀多少位，然后解码过程可以简单地变成，读取前缀部分
// 如前缀部分读完了前缀最大位数，则判断后缀是否有位，有的话，则读取后缀，相加就行，如果前缀部
// 分是遇到0结束，则向左移动后缀那么多位就可以结束了

uint32_t TruncatedRiceValueReader::Read(uint32_t max_value, uint32_t rice_param)
{
    uint32_t max_prefix_value = (max_value >> rice_param);
    uint32_t prefix_value = 0;
    while (bit_reader_() != 0)
    {
        ++prefix_value;
        if (prefix_value >= max_prefix_value)
            break;
    }
    if ((0 == rice_param) || (prefix_value < max_prefix_value) || 
        ((prefix_value << rice_param) >= max_value))
        return prefix_value;

    FixedLengthValueReader fixed_length_value_reader(bit_reader_);
    uint32_t suffix_value = fixed_length_value_reader.Read(rice_param);
    return (prefix_value << rice_param) + suffix_value;

}