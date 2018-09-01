#ifndef _COMMON_CABAC_SYNTAX_READER_H_
#define _COMMON_CABAC_SYNTAX_READER_H_

#include <array>
#include <functional>
#include <stdint.h>

#include "hevc_decoder/vld_decoder/cabac_context_types.h"

class CABACReader;
enum SliceType;

class CommonCABACSyntaxReader
{
public:
    CommonCABACSyntaxReader(CABACReader* reader);
    virtual ~CommonCABACSyntaxReader();

protected:
    enum ReadFunctionIndex
    {
        ARITHMETIC_READER = 0,
        TERMIRATE_READER = 1,
        BYPASS_READER = 2,
    };

    uint8_t ReadBit();
    CABACReader* GetCABACReader();

    // TODO: 将来要考虑此处改为得到inc，而把最小值的获取放在基类
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) = 0;
    virtual SyntaxElementName GetSyntaxElementName() = 0;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) = 0;
    
private:
    uint8_t ArithmeticRead();
    uint8_t BypassRead();
    uint8_t TerminateRead();

    uint16_t bin_idx_;
    CABACReader* reader_;
    std::array<std::function<uint8_t()>, 3> read_functions_;
};

#endif