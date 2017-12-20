#ifndef _SYNTAX_DISPATCHER_H_
#define _SYNTAX_DISPATCHER_H_

#include <memory>
#include <utility>
#include <vector>

#include "hevc_decoder/syntax/coded_video_sequence.h"

class NalUnit;
class ParametersManager;
class FrameSyntax;
class DecodeProcessorManager;

class SyntaxDispatcher : public ICodedVideoSequence
{
public:
    SyntaxDispatcher(ParametersManager* parameters_manager,
                     DecodeProcessorManager* processor_manager);
    virtual ~SyntaxDispatcher();

    bool CreateSyntaxAndDispatch(std::unique_ptr<NalUnit> nal_unit);

private:
    bool CreateSliceSegmentSyntaxAndDispatch(NalUnit* nal_unit, 
                                             bool is_idr_frame);

    virtual uint32_t GetLayerID(uint32_t poc_value) const override;
    virtual bool GetPreviewPictureOrderCount(PictureOrderCount* poc) const 
        override;

    ParametersManager* parameters_manager_;
    DecodeProcessorManager* decode_processor_manager_;

    std::unique_ptr<FrameSyntax> frame_syntax_;
    std::vector<std::pair<PictureOrderCount, uint32_t>> pocs_info_;
};

#endif