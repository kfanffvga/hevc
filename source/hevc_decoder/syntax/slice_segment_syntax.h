#ifndef _SLICE_SEGMENT_SYNTAX_H_
#define _SLICE_SEGMENT_SYNTAX_H_

#include <stdint.h>
#include <memory>

class ISliceSegmentContext;
class ParametersManager;
class BitStream;
class SliceSegmentHeader;
class SliceSegmentData;
enum NalUnitType;

class SliceSegmentSyntax
{
public:
    SliceSegmentSyntax(NalUnitType nal_unit_type, uint8_t nuh_layer_id,
                       const ParametersManager* parameters_manager);
    ~SliceSegmentSyntax();

    bool Parse(BitStream* bit_stream, ISliceSegmentContext* context);

    NalUnitType GetNalUnitType() const;
    uint8_t GetNuhLayerID() const;

    const SliceSegmentHeader& GetSliceSegmentHeader() const;
    const SliceSegmentData& GetSliceSegmentData() const;

private:
    NalUnitType nal_unit_type_;
    uint8_t nuh_layer_id_;
    std::unique_ptr<SliceSegmentHeader> header_;
    std::unique_ptr<SliceSegmentData> data_;
};

#endif