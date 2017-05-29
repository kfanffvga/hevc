#ifndef _FRAME_SYNTAX_H_
#define _FRAME_SYNTAX_H_

#include <memory>
#include <vector>

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/base/frame_syntax_context.h"

class SliceSyntax;
class SliceSegmentSyntax;

class FrameSyntax : public IFrameSyntaxContext
{
public:
    FrameSyntax();
    virtual ~FrameSyntax();

    bool AddSliceSegment(std::unique_ptr<SliceSegmentSyntax> slice_segment);

private:
    virtual uint32 GetSliceAddressByRasterScanBlockIndex(uint32 index) const
        override;
    
    std::vector<std::unique_ptr<SliceSyntax>> slices_;
};
#endif