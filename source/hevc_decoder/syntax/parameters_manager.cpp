#include "hevc_decoder/syntax/parameters_manager.h"

#include "hevc_decoder/syntax/video_parameter_set.h"
#include "hevc_decoder/syntax/picture_parameter_set.h"

using std::unique_ptr;
using std::make_pair;

ParametersManager::ParametersManager()
    : vpss_()
    , ppss_()
{

}

ParametersManager::~ParametersManager()
{

}

bool ParametersManager::AddVideoParameterSet(unique_ptr<VideoParameterSet> vps)
{
    if (GetVideoParameterSet(vps->GetVideoParameterSetID()))
        return false;

    vpss_.insert(make_pair(vps->GetVideoParameterSetID(), move(vps)));
    return true;
}

const VideoParameterSet* ParametersManager::GetVideoParameterSet(uint8_t vps_id)
{
    auto r = vpss_.find(vps_id);
    return vpss_.end() == r ? nullptr : r->second.get();
}

bool ParametersManager::AddPictureParameterSet(
    unique_ptr<PictureParameterSet> pps)
{
    if (GetPictureParameterSet(pps->GetPictureParameterSetID()))
        return false;

    ppss_.insert(make_pair(pps->GetPictureParameterSetID(), move(pps)));
    return true;
}

const PictureParameterSet* ParametersManager::GetPictureParameterSet(
    uint32_t pps_id)
{
    auto r = ppss_.find(pps_id);
    return ppss_.end() == r ? nullptr : r->second.get();
}

