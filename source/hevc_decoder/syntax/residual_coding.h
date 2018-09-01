#ifndef _RESIDUAL_CODING_H_
#define _RESIDUAL_CODING_H_

#include <stdint.h>

#include <boost/multi_array.hpp>

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/base/coordinate.h"
#include "hevc_decoder/partitions/block_scan_order_provider.h"

class CABACReader;
class IResidualCodingContext;
class CoeffAbsLevelGreaterNumberFlagReaderContext;

class ResidualCoding
{
public:
    ResidualCoding(const Coordinate& current, uint32_t log2_transform_size_y, 
                   uint32_t color_index);
    ~ResidualCoding();

    bool Parse(CABACReader* cabac_reader, IResidualCodingContext* context);

    uint32_t GetColorIndex() const;
    bool IsTransformSkip() const;
    uint32_t GetLog2TransformSizeY() const;
    
private:
    bool ParseLastSigCoeffPositionValue(CABACReader* cabac_reader, 
                                        IResidualCodingContext* context,
                                        uint32_t* last_sig_coeff_x, 
                                        uint32_t* last_sig_coeff_y);

	bool ParseSingleBlockTransformCoeffLevel(
        CABACReader* cabac_reader, IResidualCodingContext* context,
        CoeffAbsLevelGreaterNumberFlagReaderContext* greater_1_reader_context,
        CoeffAbsLevelGreaterNumberFlagReaderContext* greater_2_reader_context,
        int32_t sub_block_index, const Coordinate& last_sig_coeff_c,
        int32_t last_sub_block_pos, int32_t last_scan_pos,
        BlockScanOrderProvider::BlockSize transform_block_size,
        boost::multi_array<bool, 1>* has_coded_sub_blocks);

    bool ParseAndDerivedTransformCoeffLevel(
        CABACReader* cabac_reader, IResidualCodingContext* context,
        const Coordinate& sub_block_begin_c,
        CoeffAbsLevelGreaterNumberFlagReaderContext* greater_1_reader_context,
        CoeffAbsLevelGreaterNumberFlagReaderContext* greater_2_reader_context,
        bool has_sig_coeff[16]);

    bool CombineTransformCoeffLevel(
        CABACReader* cabac_reader, IResidualCodingContext* context, 
        int32_t last_greater_1_scan_pos, int32_t first_sig_scan_pos, 
        bool is_sign_hidden, const Coordinate& sub_block_begin_c, 
        bool has_sig_coeff[16], 
        const std::array<bool, 16>& is_coeff_abs_level_greater_1,
        const std::array<bool, 16>& is_coeff_abs_level_greater_2,
        const std::array<bool, 16>& is_negative_sign);

    BlockScanOrderProvider::ScanType DeriveScanType(
        IResidualCodingContext* context);

    bool LocateLastSubBlockAndLastScanPos(
        uint32_t last_sig_coeff_x, uint32_t last_sig_coeff_y, 
        BlockScanOrderProvider::ScanType scan_type, int32_t* last_sub_block_pos, 
        int32_t* last_scan_pos);

    uint32_t color_index_;
    Coordinate current_;
    uint32_t transform_size_y_;
    uint32_t log2_transform_size_y_;
    BlockScanOrderProvider::ScanType scan_type_;
    bool is_transform_skip_;
    bool is_explicit_rdpcm_;
    boost::multi_array<int32_t, 2> transform_coeff_level_;

};
#endif
