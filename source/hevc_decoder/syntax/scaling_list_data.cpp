#include "hevc_decoder/syntax/scaling_list_data.h"

#include <algorithm>
#include <boost/multi_array.hpp>

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"

using std::min;
using std::vector;
using std::move;
using boost::multi_array;

ScalingListData::ScalingListData()
{

}

ScalingListData::~ScalingListData()
{

}

bool ScalingListData::Parse(BitStream* bit_stream)
{
    multi_array<vector<int32_t>, 2> scaling_list(boost::extents[4][6]);
    GolombReader golomb_reader(bit_stream);
    for (int size_id = 0; size_id < 4; ++size_id)
    {
        int32_t coef_num = min(64, 1 << (4 + (size_id << 1)));
        for (int matrix_id = 0; matrix_id < 6;)
        {
            bool has_scaling_list_pred_mode = bit_stream->ReadBool();
            // 如果是预设模式的话,就去取预设的,如果没有预设的话,那就需要文件记录了,
            // 文件记录为了减少字节的开销,因此文件记录的是与上一个的差值
            if (!has_scaling_list_pred_mode)
            {
                uint32_t scaling_list_pred_matrix_id_delta = 
                    golomb_reader.ReadUnsignedValue();
                int ref_matrix_id = matrix_id - 
                    scaling_list_pred_matrix_id_delta * (3 == size_id ? 3 : 1);
                scaling_list[size_id][matrix_id] = 
                    GetDefaultScalingList(size_id, ref_matrix_id);
            }
            else
            {
                scaling_list[size_id][matrix_id] = vector<int32_t>(coef_num);
                int32_t next_coef = 8;
                if (size_id > 1)
                    next_coef = golomb_reader.ReadSignedValue() + 8;

                for (int i = 0; i < coef_num; ++i)
                {
                    int32_t scaling_list_delta_coef = 
                        golomb_reader.ReadSignedValue();

                    next_coef = 
                        (next_coef + scaling_list_delta_coef + 256) & 0xff;
                    scaling_list[size_id][matrix_id][i] = next_coef;
                }
            }
            // 只有亮度才有32x32的块大小
            matrix_id += (3 == size_id) ? 3 : 1;
        }
    }
    return true;
}

vector<int32_t> ScalingListData::GetDefaultScalingList(int size_id, 
                                                       int ref_matrix_id)
{
    if (0 == size_id)
        return vector<int32_t>(16, 16);

    const uint32_t scaling_element_count = 64;
    const int32_t default_scaling_list[2][scaling_element_count] =
    {
        {
            16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 16, 17, 16, 17, 18, 
            17, 18, 18, 17, 18, 21, 19, 20, 21, 20, 19, 21, 24, 22, 22, 24, 
            24, 22, 22, 24, 25, 25, 27, 30, 27, 25, 25, 29, 31, 35, 35, 31, 
            29, 36, 41, 44, 41, 36, 47, 54, 54, 47, 65, 70, 65, 88, 88, 115
        }, 
        {
            16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18, 
            18, 18, 18, 18, 18, 20, 20, 20, 20, 20, 20, 20, 24, 24, 24, 24, 
            24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 28, 28, 28, 28, 28, 
            28, 33, 33, 33, 33, 33, 41, 41, 41, 41, 54, 54, 54, 71, 71, 91
        }
    };
    vector<int32_t> result(scaling_element_count);
    int matrix_id_of_predict_mode = ref_matrix_id % 3;
    memcpy(&result[0], default_scaling_list[matrix_id_of_predict_mode], 
           scaling_element_count * sizeof(int32_t));
    return move(result);
}

