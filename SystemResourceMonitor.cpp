
#include "SystemResourceMonitor.h"

#include <windows.h>
#include <cstdint>

namespace hids_analysis {

SystemResourceStats SystemResourceMonitor::collect()
{
    SystemResourceStats result;

    // --------------------------------
    // 1. Collect overall CPU usage
    // --------------------------------

    FILETIME idleTime{};
    FILETIME kernelTime{};
    FILETIME userTime{};

    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        ULARGE_INTEGER idle{};
        ULARGE_INTEGER kernel{};
        ULARGE_INTEGER user{};

        idle.LowPart = idleTime.dwLowDateTime;
        idle.HighPart = idleTime.dwHighDateTime;

        kernel.LowPart = kernelTime.dwLowDateTime;
        kernel.HighPart = kernelTime.dwHighDateTime;

        user.LowPart = userTime.dwLowDateTime;
        user.HighPart = userTime.dwHighDateTime;

        const ULONGLONG currentIdle = idle.QuadPart;
        const ULONGLONG currentKernel = kernel.QuadPart;
        const ULONGLONG currentUser = user.QuadPart;

        if (hasPreviousCpuSample_) {
            const ULONGLONG idleDelta =
                currentIdle - previousIdle_;

            const ULONGLONG kernelDelta =
                currentKernel - previousKernel_;

            const ULONGLONG userDelta =
                currentUser - previousUser_;

            // Windows kernel time includes idle time.
            const ULONGLONG totalDelta =
                kernelDelta + userDelta;

            if (totalDelta > 0 && idleDelta <= totalDelta) {
                result.cpuPercent =
                    100.0 *
                    (1.0 -
                     static_cast<double>(idleDelta) /
                     static_cast<double>(totalDelta));

                if (result.cpuPercent < 0.0) {
                    result.cpuPercent = 0.0;
                }

                if (result.cpuPercent > 100.0) {
                    result.cpuPercent = 100.0;
                }

                result.cpuAvailable = true;
            }
        }

        previousIdle_ = currentIdle;
        previousKernel_ = currentKernel;
        previousUser_ = currentUser;

        hasPreviousCpuSample_ = true;
    }

    // --------------------------------
    // 2. Collect overall RAM usage
    // --------------------------------

    MEMORYSTATUSEX memory{};
    memory.dwLength = sizeof(memory);

    if (GlobalMemoryStatusEx(&memory)) {
        constexpr std::uint64_t BYTES_PER_MB =
            1024ULL * 1024ULL;

        result.totalMemoryMB =
            memory.ullTotalPhys / BYTES_PER_MB;

        result.availableMemoryMB =
            memory.ullAvailPhys / BYTES_PER_MB;

        result.usedMemoryMB =
            result.totalMemoryMB > result.availableMemoryMB
                ? result.totalMemoryMB - result.availableMemoryMB
                : 0;

        if (memory.ullTotalPhys > 0) {
            result.memoryPercent =
                100.0 *
                static_cast<double>(
                    memory.ullTotalPhys - memory.ullAvailPhys) /
                static_cast<double>(memory.ullTotalPhys);

            result.memoryAvailable = true;
        }
    }

    return result;
}

} // namespace hids_analysis
