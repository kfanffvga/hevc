#ifndef _COMMON_CABAC_SYNTAX_READER_H_
#define _COMMON_CABAC_SYNTAX_READER_H_

#include <array>
#include <functional>
#include <stdint.h>

#include "hevc_decoder/variant_length_data_decoder/cabac_context_types.h"

class CABACReader;

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
    virtual int GetArithmeticContextIndex(uint16_t bin_idx) = 0;
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