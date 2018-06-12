#ifndef _TRANSFORM_TREE_CONTEXT_H_
#define _TRANSFORM_TREE_CONTEXT_H_

#include <array>

enum CABACInitType;
enum PredModeType;
enum ChromaFormatType;

class ITransformTreeContext
{
public:
    virtual uint32_t GetMaxTransformBlockSizeY() const = 0;
    virtual uint32_t GetMinTransformBlockSizeY() const = 0;
    virtual uint32_t GetMaxTransformTreeDepth() const = 0;
    virtual bool IsAllowIntraSplit() const = 0;
    virtual CABACInitType GetCABACInitType() const = 0;
    virtual uint32_t GetMaxTransformHierarchyDepthInter() const = 0;
    virtual PredModeType GetCUPredMode() const = 0;
    virtual PartModeType GetPartMode() const = 0;
    virtual ChromaFormatType GetChromaFormatType() const = 0;
    virtual const std::array<bool, 2>&  
        IsPreviousCBContainedTransformCoefficientOfColorBlue() const = 0;

    virtual const std::array<bool, 2>& 
        IsPreviousCBContainedTransformCoefficientOfColorRed() const = 0;

};

#endif
