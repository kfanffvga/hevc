#include "hevc_decoder/partitions/frame_partition_manager.h"

#include "hevc_decoder/partitions/frame_partition.h"
#include "hevc_decoder/partitions/frame_partition_creator_info_provider.h"
#include "hevc_decoder/partitions/frame_partition_creator_info_provider_impl.h"

using std::shared_ptr;

FramePartitionManager::FramePartitionManager()
{

}

FramePartitionManager::~FramePartitionManager()
{

}

shared_ptr<FramePartition> FramePartitionManager::Get(
    const shared_ptr<IFramePartitionCreatorInfoProvider>& provider)
{
    if (!provider)
        return shared_ptr<FramePartition>();

    for (const auto& i : frame_partitions_)
    {
        auto exist_provider = i->GetCreationInfoProvider();
        if (exist_provider && (*exist_provider == *provider))
            return i;
    }
    shared_ptr<FramePartition> partition(new FramePartition());
    if (!partition->Init(provider))
        return shared_ptr<FramePartition>();

    frame_partitions_.push_back(partition);
    return partition;
}
