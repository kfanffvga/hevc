#ifndef _SLICE_SYNTAX_H_
#define _SLICE_SYNTAX_H_

#include <memory>
#include <list>

class SliceSegmentSyntax;
class IFrameSyntaxContext;

class SliceSyntax
{
public: 
    SliceSyntax(IFrameSyntaxContext* context);
    ~SliceSyntax();

    bool AddSliceSegment(std::shared_ptr<SliceSegmentSyntax> slice_segment);
    bool GetSliceSegmentAddressByCTUTileScanIndex(uint32_t index, 
                                                  uint32_t* address);

    uint32_t GetCABACContextIndexInLastParsedSliceSegment();

private:
    std::list<std::shared_ptr<SliceSegmentSyntax>> slice_segments_;
};
#endif