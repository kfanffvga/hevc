#ifndef _PARAMETES_MANAGER_H_
#define _PARAMETES_MANAGER_H_

#include <memory>
#include <vector>

class VideoParameterSet;

class ParametersManager
{
public:
    ParametersManager();
    ~ParametersManager();

    bool AddVideoParameterSet(std::unique_ptr<VideoParameterSet> vps);
    const VideoParameterSet* GetVideoParameterSet();

private:
    std::vector<std::unique_ptr<VideoParameterSet>> vps_;

};

#endif