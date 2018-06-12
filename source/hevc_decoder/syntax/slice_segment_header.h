#ifndef _SLICE_SEGMENT_HEADER_H_
#define _SLICE_SEGMENT_HEADER_H_

#include <stdint.h>
#include <vector>
#include <memory>

#include "hevc_decoder/base/tile_info.h"

class ParametersManager;
class PictureParameterSet;
class SequenceParameterSet;
class ReferencePictureListsModification;
class ShortTermReferencePictureSet;
class ISliceSegmentHeaderContext;
class SPSScreenContentCodingExtension;
class PaletteTable;
enum NalUnitType;
enum SliceType;
enum ChromaFormatType;
class BitStream;
class TileInfo;

// 由于sps与pps在此处是parse的时候得到，如得不到，整个解码过程就结束了，
// 并且parse函数均是在一开始时候调用，因此，在以后函数的使用的时候，
// 为了代码的速度与阅读的流畅性，不进行相关的sps与pps的指针的判断，
// 如调用者在调用parse之前就去调用相关的有访问到sps与pps的函数产生的问题，
// 由调用者进行保证

// 虽然sps与pps是全局公用的参数集，但由于考虑到将来多线程的问题，每个slice应该可以独立解码
// 因此，此处认为，所有的解码所需的参数都应该在header里可以得到，保证每个线程或者每个线程组
// 可以在得到slice之后，就可以独立完成其他所有的运算

class SliceSegmentHeader
{
public:
    SliceSegmentHeader();
    virtual ~SliceSegmentHeader();
    
    bool Parse(BitStream* bit_stream, ISliceSegmentHeaderContext* context);

    bool IsFirstSliceSegmentInPic() const;
    uint32_t GetWidth() const;
    uint32_t GetHeight() const;
    const TileInfo& GetTileInfo() const;
    uint32_t GetCTBLog2SizeY() const;
    uint32_t GetCTBHeight() const;
    uint32_t GetMinTBLog2SizeY() const;
    uint32_t GetMaxTransformBlockSizeY() const;
    uint32_t GetMinTransformBlockSizeY() const;
    SliceType GetSliceType() const;
    uint32_t GetSliceSegmentAddress() const;
    bool IsDependentSliceSegment() const;
    uint32_t GetQuantizationParameter() const;
    bool IsEntropyCodingSyncEnabled() const;
    bool IsTilesEnabled() const;
    bool IsSliceSAOLuma() const;
    bool IsSliceSAOChroma() const;
    bool IsUsedCABACInit() const;
    bool IsCUQPDeltaEnabled() const;
    bool IsCUChromaQPOffsetEnabled() const;
    bool IsTransquantBypassEnabled() const;
    bool IsAMPEnabled() const;
    bool IsPCMEnabled() const;
    ChromaFormatType GetChromaFormatType() const;
    uint32_t GetBitDepthLuma() const;
    uint32_t GetBitDepthChroma() const;
    uint32_t GetMinCBLog2SizeY() const;
    uint32_t GetMinCBSizeY() const;
    uint32_t GetLog2MinCUQPDeltaSize() const;
    uint32_t GetLog2MinCUChromaQPOffsetSize() const;
    uint32_t GetMinPCMCodingBlockSizeY() const;
    uint32_t GetMaxPCMCodingBlockSizeY() const;
    uint32_t GetMaxTransformHierarchyDepthIntra() const;
    uint32_t GetMaxTransformHierarchyDepthInter() const;
    PaletteTable GetPalettePredictorInitializer() const;
    uint32_t GetPaletteMaxSize() const;
    uint32_t GetPredictorPaletteMaxSize() const;
    const std::vector<int32_t>& GetNegativeRefPOCList() const;
    const std::vector<int32_t>& GetPositiveRefPOCList() const;
    const SPSScreenContentCodingExtension& GetSPSScreenContentCodingExtension()
        const;

    uint32_t GetChromaQPOffsetListtLen() const;
    const std::vector<int32_t>& GetCbQPOffsetList() const;
    const std::vector<int32_t>& GetCrQPOffsetList() const;

private:
    struct LongTermReferencePictureOrderCountInfo
    {
        uint32_t picture_order_count_value; // 当前cvs里的长期参考帧的poc的值
        bool is_used_by_curr_pic_lt;
    };

    typedef std::vector<LongTermReferencePictureOrderCountInfo> 
        LongTermRefPOCInfoSet;

    bool ParseIndependentSyntax(BitStream* bit_stream,
                                ISliceSegmentHeaderContext* context);

    bool ParseReferencePictureSet(
        BitStream* bit_stream, ISliceSegmentHeaderContext* context,
        LongTermRefPOCInfoSet* long_term_ref_poc_infos, 
        ShortTermReferencePictureSet* short_term_ref_poc_infos);

    bool ParseReferenceDetailInfo(
        bool is_slice_temporal_mvp_enabled, 
        const ShortTermReferencePictureSet& short_term_ref_poc_infos,
        const LongTermRefPOCInfoSet& long_term_ref_poc_infos,
        BitStream* bit_stream, ISliceSegmentHeaderContext* context);

    bool ParseQuantizationParameterInfo(BitStream* bit_stream);
    bool ParseReconstructPictureInfo(BitStream* bit_stream);
    bool ParseTileInfo(BitStream* bit_stream);

    uint32_t GetCurrentAvailableReferencePictureCount(
        const ShortTermReferencePictureSet& short_term_ref_poc_infos,
        const LongTermRefPOCInfoSet& long_term_ref_poc_infos);

    bool ConstructReferencePOCList(
        ISliceSegmentHeaderContext* context, bool is_current_picture_ref_enabled,
        const ShortTermReferencePictureSet& short_term_ref_poc_infos,
        const LongTermRefPOCInfoSet& long_term_ref_poc_infos,
        const ReferencePictureListsModification& ref_pic_list_modification,
        std::vector<int32_t>* negative_ref_pic_list,
        std::vector<int32_t>* positive_ref_pic_list);

    std::shared_ptr<PictureParameterSet> pps_;
    std::shared_ptr<SequenceParameterSet> sps_;

    bool is_first_slice_segment_in_pic_;
    std::vector<int32_t> negative_ref_poc_list_;
    std::vector<int32_t> positive_ref_poc_list_;
    SliceType slice_type_;
    uint32_t slice_segment_address_;
    bool is_dependent_slice_segment_;
    uint32_t quantization_parameter_;
    bool is_slice_sao_luma_;
    bool is_slice_sao_chroma_;
    bool is_used_cabac_init_;
    bool is_cu_chroma_qp_offset_enabled_;
};

#endif