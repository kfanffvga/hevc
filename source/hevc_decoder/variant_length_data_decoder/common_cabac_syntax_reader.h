#ifndef _COMMON_CABAC_SYNTAX_READER_H_
#define _COMMON_CABAC_SYNTAX_READER_H_

#include <array>
#include <functional>

#include "hevc_decoder/base/basic_types.h"
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

    uint8 ReadBit();
    virtual int GetArithmeticContextIndex(uint16 bin_idx) = 0;
    virtual SyntaxElementName GetSyntaxElementName() = 0;
    virtual ReadFunctionIndex GetFunctionIndex(uint16 bin_idx) = 0;
    
private:
    uint8 ArithmeticRead();
    uint8 BypassRead();
    uint8 TerminateRead();

    uint16 bin_idx_;
    CABACReader* reader_;
    std::array<std::function<uint8()>, 3> read_functions_;
};

#endif