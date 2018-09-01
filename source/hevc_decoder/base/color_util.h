#ifndef _COLOR_UTIL_H_
#define _COLOR_UTIL_H_

#include <stdint.h>

enum ChromaFormatType;

uint32_t GetColorCompomentCount(ChromaFormatType chroma_format_type);
bool IsSingleChromaPerPixel(ChromaFormatType chroma_format_type);

#endif