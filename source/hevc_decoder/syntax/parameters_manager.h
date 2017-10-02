#ifndef _PARAMETES_MANAGER_H_
#define _PARAMETES_MANAGER_H_

#include <memory>
#include <map>

class VideoParameterSet;
class PictureParameterSet;
class SequenceParameterSet;

class ParametersManager
{
public:
    ParametersManager();
    ~ParametersManager();

    bool AddVideoParameterSet(std::unique_ptr<VideoParameterSet> vps);
    const VideoParameterSet* GetVideoParameterSet(uint8_t vps_id);

    bool AddPictureParameterSet(std::unique_ptr<PictureParameterSet> pps);
    const PictureParameterSet* GetPictureParameterSet(uint32_t pps_id);
            
    bool AddSequenceParameterSet(std::unique_ptr<SequenceParameterSet> sps);
    const SequenceParameterSet* GetSequenceParameterSet(uint32_t sps_id);

private:
    std::map<uint8_t, std::unique_ptr<VideoParameterSet>> vpss_;
    std::map<uint32_t, std::unique_ptr<PictureParameterSet>> ppss_;
    std::map<uint32_t, std::unique_ptr<SequenceParameterSet>> spss_;
};

#endif