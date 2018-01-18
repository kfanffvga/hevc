#ifndef _SHORT_TERM_REFERENCE_PICTURE_SET_CONTEXT_IMPL_H_
#define _SHORT_TERM_REFERENCE_PICTURE_SET_CONTEXT_IMPL_H_

#include "hevc_decoder/syntax/short_term_reference_picture_set_context.h"
#include "hevc_decoder/syntax/sequence_parameter_set.h"

class ShortTermReferencePictureSetContext :
    public IShortTermReferencePictureSetContext
{
public:
    ShortTermReferencePictureSetContext(
        const std::shared_ptr<SequenceParameterSet>& sps)
        : sps_(sps)
    {

    }

    virtual ~ShortTermReferencePictureSetContext()
    {

    }

    virtual uint32_t GetShortTermReferencePictureSetCount() const override
    {
        return sps_->GetShortTermReferencePictureSetCount();
    }

    virtual const ShortTermReferencePictureSet*
        GetShortTermReferencePictureSet(uint32_t index) const override
    {
        return sps_->GetShortTermReferencePictureSet(index);
    }

private:
    const std::shared_ptr<SequenceParameterSet> sps_;
};

#endif