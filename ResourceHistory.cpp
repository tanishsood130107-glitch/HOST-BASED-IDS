
#include "ResourceHistory.h"

#include <algorithm>
#include <mutex>

namespace hids_analysis {

ResourceHistory::ResourceHistory(std::size_t maxSamples)
    : maxSamples_(std::max<std::size_t>(1, maxSamples))
{
}

void ResourceHistory::addSample(
    const SystemResourceStats& stats)
{
    std::lock_guard<std::mutex> lock(mutex_);

    samples_.push_back(stats);

    while (samples_.size() > maxSamples_) {
        samples_.pop_front();
    }
}

std::vector<SystemResourceStats>
ResourceHistory::getSamples() const
{
    std::lock_guard<std::mutex> lock(mutex_);

    return std::vector<SystemResourceStats>(
        samples_.begin(),
        samples_.end());
}

void ResourceHistory::clear()
{
    std::lock_guard<std::mutex> lock(mutex_);

    samples_.clear();
}

} // namespace hids_analysis
