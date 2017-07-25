#include "hevc_decoder/syntax/profile_tier_level.h"
#include "hevc_decoder/base/stream/bit_stream.h"

#include <boost/multi_array.hpp>

#include <memory>
#include <vector>
#include <assert.h>

using std::unique_ptr;
using std::vector;
using std::max;
using boost::multi_array;

namespace
{
const uint32 compatibility_flag_count = 32;
}

ProfileTierLevel::ProfileTierLevel(bool profile_present_flag,
                                   int max_num_sub_layers)
    : profile_present_flag_(profile_present_flag)
    , max_num_sub_layers_(max_num_sub_layers)
{

}

ProfileTierLevel::~ProfileTierLevel()
{

}

bool ProfileTierLevel::Parse(BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    ParseGeneralProfileInfo(bit_stream);

    uint8 general_level_idc = bit_stream->Read<uint8>(8);

    ParseSubLayerInfo(bit_stream);
    return true;
}

void ProfileTierLevel::ParseGeneralProfileInfo(BitStream* bit_stream)
{
    if (!profile_present_flag_)
        return;

    uint8 general_profile_space = bit_stream->Read<uint8>(2);
    bool is_general_tier = bit_stream->ReadBool();
    uint8 general_profile_idc = bit_stream->Read<uint8>(5);
    bool is_general_profile_compatibility[compatibility_flag_count] = { };
    for (int i = 0; i < compatibility_flag_count; ++i)
        is_general_profile_compatibility[i] = bit_stream->ReadBool();

    bool is_general_progressive_source = bit_stream->ReadBool();
    bool is_general_interlaced_source = bit_stream->ReadBool();
    bool is_general_non_packed_constraint = bit_stream->ReadBool();
    bool is_general_frame_only_constraint = bit_stream->ReadBool();

    if ((general_profile_idc == 4) || is_general_profile_compatibility[4] ||
        (general_profile_idc == 5) || is_general_profile_compatibility[5] ||
        (general_profile_idc == 6) || is_general_profile_compatibility[6] ||
        (general_profile_idc == 7) || is_general_profile_compatibility[7] ||
        (general_profile_idc == 8) || is_general_profile_compatibility[8] || 
        (general_profile_idc == 9) || is_general_profile_compatibility[9] || 
        (general_profile_idc == 10) || is_general_profile_compatibility[10])
    {
        bool has_general_max_12bit_constraint = bit_stream->ReadBool();
        bool has_general_max_10bit_constraint = bit_stream->ReadBool();
        bool has_general_max_8bit_constraint = bit_stream->ReadBool();
        bool has_general_max_422chroma_constraint = bit_stream->ReadBool();
        bool has_general_max_420chroma_constraint = bit_stream->ReadBool();
        bool has_general_max_monochrome_constraint = bit_stream->ReadBool();
        bool has_general_intra_constraint = bit_stream->ReadBool();
        bool has_general_one_picture_only_constraint = bit_stream->ReadBool();
        bool has_general_lower_bit_rate_constraint = bit_stream->ReadBool();
        if ((general_profile_idc == 5) || is_general_profile_compatibility[5] ||
            (general_profile_idc == 9) || is_general_profile_compatibility[9] ||
            (general_profile_idc == 10) || is_general_profile_compatibility[10])
        {
            bool has_general_max_14bit_constraint = bit_stream->ReadBool();
            bit_stream->SkipBits(33);
        }
        else
        {
            bit_stream->SkipBits(34);
        }
    }
    else
    {
        bit_stream->SkipBits(43);
        if (((general_profile_idc >= 1) && (general_profile_idc <= 5)) ||
            (general_profile_idc == 9) ||
            is_general_profile_compatibility[1] ||
            is_general_profile_compatibility[2] ||
            is_general_profile_compatibility[3] ||
            is_general_profile_compatibility[4] ||
            is_general_profile_compatibility[5] ||
            is_general_profile_compatibility[9])
        {
            bool general_inbld_flag = bit_stream->ReadBool();
        }
        else
        {
            bit_stream->SkipBits(1);
        }
    }
}

void ProfileTierLevel::ParseSubLayerInfo(BitStream* bit_stream)
{
    if (max_num_sub_layers_ <= 0)
    {
        assert(false);
        return;
    }

    int max_num_sub_layers_minus1 = max_num_sub_layers_ - 1;
    auto sub_layers_range = boost::extents[max_num_sub_layers_minus1];
    multi_array<bool, 1> has_sub_layer_profile_present(sub_layers_range);
    multi_array<bool, 1> has_sub_layer_level_present(sub_layers_range);
    for (int i = 0; i < max_num_sub_layers_minus1; ++i)
    {
        has_sub_layer_profile_present[i] = bit_stream->ReadBool();
        has_sub_layer_level_present[i] = bit_stream->ReadBool();
    }

    if (max_num_sub_layers_minus1 > 0)
        bit_stream->SkipBits(max(8 - max_num_sub_layers_minus1, 0) << 1);

    vector<uint8> sub_layer_profile_space;
    vector<uint8> sub_layer_profile_idc;
    multi_array<bool, 2> has_sub_layer_profile_compatibility(
        boost::extents[max_num_sub_layers_minus1][compatibility_flag_count]);

    typedef multi_array<bool, 1> BoolArray;
    BoolArray is_sub_layer_tier(sub_layers_range);
    BoolArray is_sub_layer_progressive_source(sub_layers_range);
    BoolArray is_sub_layer_interlaced_source(sub_layers_range);
    BoolArray has_sub_layer_non_packed_constraint(sub_layers_range);
    BoolArray has_sub_layer_frame_only_constraint(sub_layers_range);
    BoolArray has_sub_layer_max_12bit_constraint(sub_layers_range);
    BoolArray has_sub_layer_max_10bit_constraint(sub_layers_range);
    BoolArray has_sub_layer_max_8bit_constraint(sub_layers_range);
    BoolArray has_sub_layer_max_422chroma_constraint(sub_layers_range);
    BoolArray has_sub_layer_max_420chroma_constraint(sub_layers_range);
    BoolArray has_sub_layer_max_monochrome_constraint(sub_layers_range);
    BoolArray has_sub_layer_intra_constraint(sub_layers_range);
    BoolArray has_sub_layer_one_picture_only_constraint(sub_layers_range);
    BoolArray has_sub_layer_lower_bit_rate_constraint(sub_layers_range);

    for (int i = 0; i < max_num_sub_layers_minus1; ++i)
    {
        if (has_sub_layer_profile_present[i])
        {
            sub_layer_profile_space.push_back(bit_stream->Read<uint8>(2));
            is_sub_layer_tier[i] = bit_stream->ReadBool();
            sub_layer_profile_idc.push_back(bit_stream->Read<uint8>(5));
            for (int j = 0; j < compatibility_flag_count; ++j)
            {
                has_sub_layer_profile_compatibility[i][j] = 
                    bit_stream->ReadBool();
            }

            is_sub_layer_progressive_source[i] = bit_stream->ReadBool();
            is_sub_layer_interlaced_source[i] = bit_stream->ReadBool();
            has_sub_layer_non_packed_constraint[i] = bit_stream->ReadBool();
            has_sub_layer_frame_only_constraint[i] = bit_stream->ReadBool();
            if ((sub_layer_profile_idc[i] == 4) ||
                (sub_layer_profile_idc[i] == 5) ||
                (sub_layer_profile_idc[i] == 6) ||
                (sub_layer_profile_idc[i] == 7) ||
                (sub_layer_profile_idc[i] == 8) ||
                (sub_layer_profile_idc[i] == 9) ||
                (sub_layer_profile_idc[i] == 10) ||
                (has_sub_layer_profile_compatibility[i][4]) ||
                (has_sub_layer_profile_compatibility[i][5]) ||
                (has_sub_layer_profile_compatibility[i][6]) ||
                (has_sub_layer_profile_compatibility[i][7]) ||
                (has_sub_layer_profile_compatibility[i][8]) || 
                (has_sub_layer_profile_compatibility[i][9]) || 
                (has_sub_layer_profile_compatibility[i][10]))
            {
                has_sub_layer_max_12bit_constraint[i] = bit_stream->ReadBool();
                has_sub_layer_max_10bit_constraint[i] = bit_stream->ReadBool();
                has_sub_layer_max_8bit_constraint[i] = bit_stream->ReadBool();
                has_sub_layer_max_422chroma_constraint[i] = 
                    bit_stream->ReadBool();
                has_sub_layer_max_420chroma_constraint[i] = 
                    bit_stream->ReadBool();
                has_sub_layer_max_monochrome_constraint[i] = 
                    bit_stream->ReadBool();
                has_sub_layer_intra_constraint[i] = bit_stream->ReadBool();
                has_sub_layer_one_picture_only_constraint[i] = 
                    bit_stream->ReadBool();
                has_sub_layer_lower_bit_rate_constraint[i] = 
                    bit_stream->ReadBool();
                if ((sub_layer_profile_idc[0] == 5) ||
                    has_sub_layer_profile_compatibility[0][5])
                {
                    bool has_sub_layer_14bit_constraint = bit_stream->ReadBool();
                    bit_stream->SkipBits(33);
                }
                else
                {
                    bit_stream->SkipBits(34);
                }
            }
            else
            {
                bit_stream->SkipBits(43);
                BoolArray sub_layer_inbld(sub_layers_range);
                if (((sub_layer_profile_idc[i] >= 1) &&
                    (sub_layer_profile_idc[i] <= 5)) ||
                    (sub_layer_profile_idc[i] == 9) ||
                    has_sub_layer_profile_compatibility[0][1] ||
                    has_sub_layer_profile_compatibility[0][2] ||
                    has_sub_layer_profile_compatibility[0][3] ||
                    has_sub_layer_profile_compatibility[0][4] ||
                    has_sub_layer_profile_compatibility[0][5] ||
                    has_sub_layer_profile_compatibility[0][9])
                {
                    sub_layer_inbld[i] = bit_stream->ReadBool();
                }
                else
                {
                    // reserved zero
                    bit_stream->SkipBits(1);
                }
            }
        }

        vector<uint8> sub_layer_level_idc;
        if (has_sub_layer_level_present[i])
            sub_layer_level_idc.push_back(bit_stream->Read<uint8>(8));
    }
}
