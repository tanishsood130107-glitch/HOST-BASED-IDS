#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <cstdint>

namespace hids_analysis {

struct SystemResourceStats {
    double cpuPercent = 0.0;
    double memoryPercent = 0.0;

    std::uint64_t totalMemoryMB = 0;
    std::uint64_t usedMemoryMB = 0;
    std::uint64_t availableMemoryMB = 0;

    bool cpuAvailable = false;
    bool memoryAvailable = false;
};

class SystemResourceMonitor
{
 public:
    SystemResourceStats collect();

 private:
    ULONGLONG previousIdle_ = 0;
    ULONGLONG previousKernel_ = 0;
    ULONGLONG previousUser_ = 0;

    bool hasPreviousCpuSample_ = false;
};

}
