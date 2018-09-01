#include <Windows.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>

#include <list>

#include <boost/multi_array.hpp>

#include "hevc_decoder/hevc_decoder.h"

int main(int argc, char** argv, char** env)
{
    const wchar_t h265_video_file[] = L"D:\\mv_test\\hevc_sequence\\BQTerrace_1920x1080_60_600f_420_8bit_qp24_wpp_io_main_Ejctvc-hm12.1.hevc";    
    // const wchar_t h265_video_file[] = L"D:\\3_2048_1080.265";    
    HANDLE h265_video_handle = ::CreateFileW(h265_video_file,
                                             GENERIC_READ, FILE_SHARE_READ,
                                             NULL, OPEN_EXISTING,
                                             NULL, NULL);
    if (h265_video_handle == INVALID_HANDLE_VALUE)
    {
        DWORD error = ::GetLastError();
        return 0;
    }
    const int buffer_size = 4096;
    FILE_ID_BOTH_DIR_INFO file_info = {};
    ::GetFileInformationByHandleEx(h265_video_handle, FileNameInfo,
                                   &file_info, sizeof(file_info));

    int length = 0;
    length = ::GetFileSize(h265_video_handle, nullptr);    
    HEVCDecoder decoder;
    decoder.Init(1);
    std::vector<int8_t> viode_buffer(buffer_size);
    DWORD read_length = 0;

    if (::ReadFile(h265_video_handle, &viode_buffer.front(), buffer_size,
        &read_length, NULL))
    {
        decoder.Decode(&viode_buffer.front(), read_length, nullptr);
    }

    length -= read_length;

    viode_buffer.resize(8192);
    while (length > 0)
    {
        if (::ReadFile(h265_video_handle, &viode_buffer.front(), 8192,
                       &read_length, NULL))
        {
            decoder.Decode(&viode_buffer.front(), read_length, nullptr);
        }

        length -= read_length;
    }

    getchar();
    ::CloseHandle(h265_video_handle);
    return 0;
}