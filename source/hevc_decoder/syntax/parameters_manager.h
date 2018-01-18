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

    bool AddVideoParameterSet(std::shared_ptr<VideoParameterSet> vps);
    std::shared_ptr<VideoParameterSet> GetVideoParameterSet(uint8_t vps_id) 
        const;

    bool AddPictureParameterSet(std::shared_ptr<PictureParameterSet> pps);
    std::shared_ptr<PictureParameterSet> GetPictureParameterSet(uint32_t pps_id) 
        const;
            
    bool AddSequenceParameterSet(std::shared_ptr<SequenceParameterSet> sps);
    std::shared_ptr<SequenceParameterSet> GetSequenceParameterSet(
        uint32_t sps_id) const;

private:
    std::map<uint8_t, std::shared_ptr<VideoParameterSet>> vpss_;
    std::map<uint32_t, std::shared_ptr<PictureParameterSet>> ppss_;
    std::map<uint32_t, std::shared_ptr<SequenceParameterSet>> spss_;
};

#endif