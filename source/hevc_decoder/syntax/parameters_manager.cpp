#include "hevc_decoder/syntax/parameters_manager.h"

#include "hevc_decoder/syntax/video_parameter_set.h"
#include "hevc_decoder/syntax/picture_parameter_set.h"
#include "hevc_decoder/syntax/sequence_parameter_set.h"

using std::shared_ptr;
using std::make_pair;

ParametersManager::ParametersManager()
    : vpss_()
    , ppss_()
{

}

ParametersManager::~ParametersManager()
{

}

bool ParametersManager::AddVideoParameterSet(shared_ptr<VideoParameterSet> vps)
{
    if (GetVideoParameterSet(vps->GetVideoParameterSetID()))
        return false;

    vpss_.insert(make_pair(vps->GetVideoParameterSetID(), move(vps)));
    return true;
}

shared_ptr<VideoParameterSet> ParametersManager::GetVideoParameterSet(
    uint8_t vps_id) const
{
    auto r = vpss_.find(vps_id);
    return vpss_.end() == r ? shared_ptr<VideoParameterSet>() : r->second;
}

bool ParametersManager::AddPictureParameterSet(
    shared_ptr<PictureParameterSet> pps)
{
    if (GetPictureParameterSet(pps->GetPictureParameterSetID()))
        return false;

    ppss_.insert(make_pair(pps->GetPictureParameterSetID(), move(pps)));
    return true;
}

shared_ptr<PictureParameterSet> ParametersManager::GetPictureParameterSet(
    uint32_t pps_id) const
{
    auto r = ppss_.find(pps_id);
    return ppss_.end() == r ? shared_ptr<PictureParameterSet>() : r->second;
}

bool ParametersManager::AddSequenceParameterSet(
    shared_ptr<SequenceParameterSet> sps)
{
    if (GetSequenceParameterSet(sps->GetSequenceParameterSetID()))
        return false;

    spss_.insert(make_pair(sps->GetSequenceParameterSetID(), move(sps)));
    return true;
}

shared_ptr<SequenceParameterSet> ParametersManager::GetSequenceParameterSet(
    uint32_t sps_id) const
{
    auto r = spss_.find(sps_id);
    return spss_.end() == r ? shared_ptr<SequenceParameterSet>() : r->second;
}

