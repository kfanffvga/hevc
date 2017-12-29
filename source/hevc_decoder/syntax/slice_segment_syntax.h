#ifndef _SLICE_SEGMENT_SYNTAX_H_
#define _SLICE_SEGMENT_SYNTAX_H_

#include "hevc_decoder/variant_length_data_decoder/slice_segment_info_provider_for_cabac.h"

class ISliceSegmentContext;
class ParametersManager;
class BitStream;
enum NalUnitType;

class SliceSegmentSyntax : public ISliceSegmentInfoProviderForCABAC
{
public:
    SliceSegmentSyntax(NalUnitType nal_unit_type, uint8_t nuh_layer_id,
                       const ParametersManager* parameters_manager);
    ~SliceSegmentSyntax();

    bool Parse(BitStream* bit_stream, ISliceSegmentContext* context);

    NalUnitType GetNalUnitType() const;
    uint8_t GetNuhLayerID() const;

private:
    virtual uint32_t GetQuantizationParameter() const override;
    virtual bool IsEntropyCodingSyncEnabled() const override;
    virtual bool IsDependentSliceSegment() const override;
    virtual uint32_t GetSliceSegmentAddress() const override;
    virtual uint32_t GetCABACStorageIndex() const override;

    const ParametersManager* parameters_manager_;
    NalUnitType nal_unit_type_;
    uint8_t nuh_layer_id_;
};

#endif