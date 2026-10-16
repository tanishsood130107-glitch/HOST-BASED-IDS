
#pragma once

#include "SystemResourceMonitor.h"

#include <cstddef>
#include <deque>
#include <mutex>
#include <vector>

namespace hids_analysis {

class ResourceHistory {
public:
    explicit ResourceHistory(std::size_t maxSamples = 60);

    void addSample(const SystemResourceStats& stats);

    std::vector<SystemResourceStats> getSamples() const;

    void clear();

private:
    std::size_t maxSamples_;
    mutable std::mutex mutex_;
    std::deque<SystemResourceStats> samples_;
};

} // namespace hids_analysis
