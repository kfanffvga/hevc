#include "hevc_decoder/syntax/profile_tier_level.h"
#include "hevc_decoder/base/stream/bit_stream.h"

#include <boost/multi_array.hpp>

#include <memory>
#include <vector>
#include <assert.h>

using std::unique_ptr;
using std::vector;
using boost::multi_array;

ProfileTierLevel::ProfileTierLevel(BitStream* bit_stream,
                                   bool profile_present_flag,
                                   int max_num_sub_layers)
    : bit_stream_(bit_stream)
    , profile_present_flag_(profile_present_flag)
    , max_num_sub_layers_(max_num_sub_layers)
{

}

ProfileTierLevel::~ProfileTierLevel()
{

}

bool ProfileTierLevel::Parser()
{
    if (!bit_stream_)
        return false;

    if (profile_present_flag_)
    {
        const uint32 allocate_booler_vector = 32;
        uint8 general_profile_space = bit_stream_->Read<uint8>(2);
        bool general_tier_flag = bit_stream_->ReadBool();
        uint8 general_profile_idc = bit_stream_->Read<uint8>(5);
        bool general_profile_compatibility_flag[allocate_booler_vector] = { };
        for (int i = 0; i < allocate_booler_vector; ++i)
        {
            general_profile_compatibility_flag[i] = bit_stream_->ReadBool();
        }

        bool general_progressive_source_flag = bit_stream_->ReadBool();
        bool general_interlaced_source_flag = bit_stream_->ReadBool();
        bool general_non_packed_constraint_flag = bit_stream_->ReadBool();
        bool general_frame_only_constraint_flag = bit_stream_->ReadBool();
        if ((general_profile_idc == 4) || general_profile_compatibility_flag[4] ||
            (general_profile_idc == 5) || general_profile_compatibility_flag[5] ||
            (general_profile_idc == 6) || general_profile_compatibility_flag[6] ||
            (general_profile_idc == 7) || general_profile_compatibility_flag[7])
        {
            bool general_max_12bit_constraint_flag = bit_stream_->ReadBool();
            bool general_max_10bit_constraint_flag = bit_stream_->ReadBool();
            bool general_max_8bit_constraint_flag = bit_stream_->ReadBool();
            bool general_max_422chroma_constraint_flag = bit_stream_->ReadBool();
            bool general_max_420chroma_constraint_flag = bit_stream_->ReadBool();
            bool general_max_monochrome_constraint_flag = bit_stream_->ReadBool();
            bool general_intra_constraint_flag = bit_stream_->ReadBool();
            bool general_one_picture_only_constraint_flag = bit_stream_->ReadBool();
            bool general_lower_bit_rate_constraint_flag = bit_stream_->ReadBool();
            // reserved 34 bits
            bit_stream_->Read<uint32>(32);
            bit_stream_->Read<uint8>(2);
        }
        else
        {
            // reserved 43 bits
            bit_stream_->Read<uint32>(32);
            bit_stream_->Read<uint16>(11);
            if (((general_profile_idc >= 1) && (general_profile_idc <= 5)) ||
                general_profile_compatibility_flag[1] || 
                general_profile_compatibility_flag[2] ||
                general_profile_compatibility_flag[3] ||
                general_profile_compatibility_flag[4] ||
                general_profile_compatibility_flag[5])
            { 
                bool general_inbld_flag = bit_stream_->ReadBool();
            }
            else
            {
                // reserved 1 bit
                bit_stream_->Read<uint8>(1);
            }
        }

        uint8 general_level_idc = bit_stream_->Read<uint8>(8);
        multi_array<bool, 1> sub_layer_profile_present(boost::extents[max_num_sub_layers_]);
        multi_array<bool, 1> sub_layer_level_present(boost::extents[max_num_sub_layers_]);
        for (int i = 0; i < max_num_sub_layers_; ++i)
        {
            sub_layer_profile_present[bit_stream_->ReadBool()];
            sub_layer_level_present[bit_stream_->ReadBool()];
        }

        if (max_num_sub_layers_ > 0)
        {           
            for (int i = max_num_sub_layers_; i < 8; ++i)
            { // reserved 2 bits
                bit_stream_->Read<uint8>(2);
            }

            vector<uint8> sub_layer_profile_space;            
            multi_array<bool, 1> sub_layer_tier(boost::extents[max_num_sub_layers_]);
            vector<uint8> sub_layer_profile_idc;
            multi_array<bool, 2> sub_layer_profile_compatibility(boost::extents[max_num_sub_layers_][allocate_booler_vector]);
            multi_array<bool, 1> sub_layer_progressive_source(boost::extents[max_num_sub_layers_]);
            multi_array<bool, 1> sub_layer_interlaced_source(boost::extents[max_num_sub_layers_]);
            multi_array<bool, 1> sub_layer_non_packed_constraint(boost::extents[max_num_sub_layers_]);
            multi_array<bool, 1> sub_layer_frame_only_constraint(boost::extents[max_num_sub_layers_]);
            multi_array<bool, 1> sub_layer_max_12bit_constraint(boost::extents[max_num_sub_layers_]);
            multi_array<bool, 1> sub_layer_max_10bit_constraint(boost::extents[max_num_sub_layers_]);
            multi_array<bool, 1> sub_layer_max_8bit_constraint(boost::extents[max_num_sub_layers_]);
            multi_array<bool, 1> sub_layer_max_422chroma_constraint(boost::extents[max_num_sub_layers_]);
            multi_array<bool, 1> sub_layer_max_420chroma_constraint(boost::extents[max_num_sub_layers_]);
            multi_array<bool, 1> sub_layer_max_monochrome_constraint(boost::extents[max_num_sub_layers_]);
            multi_array<bool, 1> sub_layer_intra_constraint(boost::extents[max_num_sub_layers_]);
            multi_array<bool, 1> sub_layer_one_picture_only_constraint(boost::extents[max_num_sub_layers_]);
            multi_array<bool, 1> sub_layer_lower_bit_rate_constraint(boost::extents[max_num_sub_layers_]);
            for (int i = 0; i < max_num_sub_layers_; ++i)
            {
                if (sub_layer_profile_present[i])
                {
                    sub_layer_profile_space.push_back(bit_stream_->Read<uint8>(2));
                    sub_layer_tier[bit_stream_->ReadBool()];
                    sub_layer_profile_idc.push_back(bit_stream_->Read<uint8>(5));
                    for (int j = 0; j < 32; ++j)
                    {
                        sub_layer_profile_compatibility[i][j] = bit_stream_->ReadBool();
                    }

                    sub_layer_progressive_source[bit_stream_->ReadBool()];
                    sub_layer_interlaced_source[bit_stream_->ReadBool()];
                    sub_layer_non_packed_constraint[bit_stream_->ReadBool()];
                    sub_layer_frame_only_constraint[bit_stream_->ReadBool()];                    
                    if (sub_layer_profile_idc[i] == 4 ||
                        sub_layer_profile_idc[i] == 5 ||
                        sub_layer_profile_idc[i] == 6 ||
                        sub_layer_profile_idc[i] == 7 ||
                        sub_layer_profile_compatibility[i][4] ||
                        sub_layer_profile_compatibility[i][5] ||
                        sub_layer_profile_compatibility[i][6] ||
                        sub_layer_profile_compatibility[i][7])
                    {
                        sub_layer_max_12bit_constraint[bit_stream_->ReadBool()];
                        sub_layer_max_10bit_constraint[bit_stream_->ReadBool()];
                        sub_layer_max_8bit_constraint[bit_stream_->ReadBool()];
                        sub_layer_max_422chroma_constraint[
                            bit_stream_->ReadBool()];
                        sub_layer_max_420chroma_constraint[
                            bit_stream_->ReadBool()];
                        sub_layer_max_monochrome_constraint[
                            bit_stream_->ReadBool()];
                        sub_layer_intra_constraint[bit_stream_->ReadBool()];
                        sub_layer_one_picture_only_constraint[
                            bit_stream_->ReadBool()];
                        sub_layer_lower_bit_rate_constraint[
                            bit_stream_->ReadBool()];
                        // reserved 34 bits
                        bit_stream_->Read<uint8>(32);
                        bit_stream_->Read<uint8>(2);
                    }
                    else
                    {
                        // reserved 43 bits
                        bit_stream_->Read<uint8>(32);
                        bit_stream_->Read<uint8>(11);
                        multi_array<bool, 1> sub_layer_inbld(boost::extents[max_num_sub_layers_]);
                        if (((sub_layer_profile_idc[i] >= 1) &&
                            (sub_layer_profile_idc[i] <= 5)) ||
                            sub_layer_profile_compatibility[0][1] ||
                            sub_layer_profile_compatibility[0][2] ||
                            sub_layer_profile_compatibility[0][3] ||
                            sub_layer_profile_compatibility[0][4] ||
                            sub_layer_profile_compatibility[0][5])
                        {
                            sub_layer_inbld[i] = bit_stream_->ReadBool();
                        }
                        else
                        {
                            // reserved zero
                            assert(bit_stream_->ReadBool() == false);
                        }
                    }
                }

                vector<uint8> sub_layer_level_idc;
                if (sub_layer_level_present[i])
                    sub_layer_level_idc.push_back(bit_stream_->Read<uint8>(8));
            }

        }
    }

    return true;
}