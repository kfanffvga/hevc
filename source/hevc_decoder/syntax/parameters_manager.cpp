#include "hevc_decoder/syntax/parameters_manager.h"

#include "hevc_decoder/syntax/video_parameter_set.h"

using std::unique_ptr;

ParametersManager::ParametersManager()
{

}

ParametersManager::~ParametersManager()
{

}

bool ParametersManager::AddVideoParameterSet(unique_ptr<VideoParameterSet> vps)
{
    return true;
}

const VideoParameterSet* ParametersManager::GetVideoParameterSet()
{
    return nullptr;
}

