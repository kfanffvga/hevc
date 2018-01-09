#ifndef _FRAME_PARTITION_MANAGER_H_
#define _FRAME_PARTITION_MANAGER_H_

#include <memory>
#include <list>
#include <utility>

class IFramePartitionCreatorInfoProvider;
class FramePartition;

class FramePartitionManager
{
public:
    FramePartitionManager();
    ~FramePartitionManager();

    std::shared_ptr<FramePartition> Get(
        const std::unique_ptr<IFramePartitionCreatorInfoProvider>& provider);

private:
    std::list<std::shared_ptr<FramePartition>> frame_partitions_;


};
#endif