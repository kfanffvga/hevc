#ifndef _SAMPLE_ADAPTIVE_OFFSET_H_
#define _SAMPLE_ADAPTIVE_OFFSET_H_

#include <stdint.h>
#include <array>
#include <vector>

class CABACReader;
class ISampleAdaptiveOffsetContext;

enum SampleAdaptiveOffsetType
{
    NOT_APPLIED = 0,
    BAND_OFFSET = 1,
    EDGE_OFFSET = 2
};

class SampleAdaptiveOffset
{
public:
    SampleAdaptiveOffset();
    ~SampleAdaptiveOffset();

    bool Parse(CABACReader* cabac_reader, ISampleAdaptiveOffsetContext* context);
    
    const std::vector<SampleAdaptiveOffsetType>& GetSAOTypeIndices() const;
    const std::vector<std::array<int, 4>>& GetSAOOffsets() const;
    const std::vector<uint32_t> GetSAOBandPositions() const;
    uint32_t GetSAOEoClassLuma() const;
    uint32_t GetSAOEoClassChroma() const;

private:
    bool ParseIndependentSAOInfo(CABACReader* cabac_reader, 
                                 ISampleAdaptiveOffsetContext* context);

    bool ParseSingleColorSAODetailInfo(
        CABACReader* cabac_reader, uint32_t sample_bit_depth, 
        uint32_t color_index, SampleAdaptiveOffsetType sao_type, 
        std::array<int, 4>* sao_offset, uint32_t* sao_band_position);

    bool CopyDependentSAOInfo(ISampleAdaptiveOffsetContext* context, 
                              bool is_allow_sao_merge_left, 
                              bool is_allow_sao_merge_up);

    std::vector<SampleAdaptiveOffsetType> sao_type_indices_;
    std::vector<std::array<int, 4>> sao_offsets_;
    std::vector<uint32_t> sao_band_positions_;
    uint32_t sao_eo_class_luma_;
    uint32_t sao_eo_class_chroma_;
};

#endif