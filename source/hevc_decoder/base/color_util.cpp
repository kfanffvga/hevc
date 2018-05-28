#include "hevc_decoder/base/color_util.h"

#include "hevc_decoder/base/basic_types.h"

uint32_t GetColorCompomentCount(ChromaFormatType chroma_format_type)
{
    uint32_t color_component_count = 1;
    if ((chroma_format_type != MONO_CHROME) && 
        (chroma_format_type != YUV_MONO_CHROME))
        color_component_count = 3;

    return color_component_count;
}
