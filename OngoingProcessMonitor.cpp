
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <psapi.h>

#include "OngoingProcessMonitor.h"

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

ULONGLONG OngoingProcessMonitor::fileTimeToUInt64(
    const FILETIME& time) {

    ULARGE_INTEGER value{};
    value.LowPart = time.dwLowDateTime;
    value.HighPart = time.dwHighDateTime;
    return value.QuadPart;
}

std::vector<OngoingProcessInfo>
OngoingProcessMonitor::collectSnapshot() {

    std::vector<OngoingProcessInfo> results;

    // Get process IDs using PSAPI.
    std::vector<DWORD> pids(2048);
    DWORD bytesReturned = 0;

    while (true) {
        if (!EnumProcesses(
                pids.data(),
                static_cast<DWORD>(
                    pids.size() * sizeof(DWORD)),
                &bytesReturned)) {
            return results;
        }

        if (bytesReturned <
            pids.size() * sizeof(DWORD)) {
            break;
        }

        pids.resize(pids.size() * 2);
    }

    pids.resize(bytesReturned / sizeof(DWORD));

    FILETIME currentTime{};
    GetSystemTimeAsFileTime(&currentTime);

    const ULONGLONG now =
        fileTimeToUInt64(currentTime);

    SYSTEM_INFO systemInfo{};
    GetSystemInfo(&systemInfo);

    DWORD cpuCount = systemInfo.dwNumberOfProcessors;
    if (cpuCount == 0) {
        cpuCount = 1;
    }

    std::unordered_map<DWORD, CpuSample> currentSamples;

    for (DWORD pid : pids) {
        if (pid == 0) {
            continue;
        }

        OngoingProcessInfo info{};
        info.pid = pid;

        // Query path and process timing independently.
        HANDLE queryHandle = OpenProcess(
            PROCESS_QUERY_INFORMATION,
            FALSE,
            pid);

        if (queryHandle == nullptr) {
            queryHandle = OpenProcess(
                PROCESS_QUERY_LIMITED_INFORMATION,
                FALSE,
                pid);
        }

        if (queryHandle != nullptr) {
            char pathBuffer[32768]{};

            // Use PSAPI instead of QueryFullProcessImageNameA.
            DWORD pathLength = GetModuleFileNameExA(
                queryHandle,
                nullptr,
                pathBuffer,
                static_cast<DWORD>(sizeof(pathBuffer)));

            if (pathLength > 0) {
                info.executablePath =
                    std::string(pathBuffer, pathLength);

                const size_t separator =
                    info.executablePath.find_last_of("\\/");

                info.name = (separator == std::string::npos)
                    ? info.executablePath
                    : info.executablePath.substr(separator + 1);
            } else {
                info.name = "Name unavailable";
                info.executablePath = "Unavailable";
            }

            FILETIME creation{}, exit{}, kernel{}, user{};

            if (GetProcessTimes(
                    queryHandle,
                    &creation,
                    &exit,
                    &kernel,
                    &user)) {

                const ULONGLONG created =
                    fileTimeToUInt64(creation);

                const ULONGLONG cpuTime =
                    fileTimeToUInt64(kernel) +
                    fileTimeToUInt64(user);

                info.startTimeAvailable = true;

                if (now >= created) {
                    info.runningSeconds =
                        static_cast<double>(now - created) /
                        10000000.0;
                }

                auto previous = previousSamples.find(pid);

                if (previous != previousSamples.end() &&
                    previous->second.creationTime == created &&
                    now > previous->second.sampleTime &&
                    cpuTime >= previous->second.cpuTime) {

                    const ULONGLONG cpuDelta =
                        cpuTime - previous->second.cpuTime;

                    const ULONGLONG elapsed =
                        now - previous->second.sampleTime;

                    const double elapsedSeconds =
                        static_cast<double>(elapsed) /
                        10000000.0;

                    const double cpuSeconds =
                        static_cast<double>(cpuDelta) /
                        10000000.0;

                    if (elapsedSeconds > 0.0) {
                        double percent =
                            cpuSeconds /
                            (elapsedSeconds * cpuCount) * 100.0;

                        if (percent < 0.0) percent = 0.0;
                        if (percent > 100.0) percent = 100.0;

                        info.cpuPercent = percent;
                        info.cpuAvailable = true;
                    }
                }

                currentSamples[pid] = {
                    created, cpuTime, now
                };
            }

            CloseHandle(queryHandle);
        } else {
            info.name = "Access restricted";
            info.executablePath = "Unavailable";
        }

        // Memory query is separate so its failure does not
        // erase process path or timing information.
        HANDLE memoryHandle = OpenProcess(
            PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
            FALSE,
            pid);

        if (memoryHandle != nullptr) {
            PROCESS_MEMORY_COUNTERS_EX memory{};
            memory.cb = sizeof(memory);

            if (GetProcessMemoryInfo(
                    memoryHandle,
                    reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(
                        &memory),
                    sizeof(memory))) {

                info.memoryMB =
                    static_cast<double>(memory.WorkingSetSize) /
                    (1024.0 * 1024.0);

                info.memoryAvailable = true;
            }

            CloseHandle(memoryHandle);
        }

        results.push_back(std::move(info));
    }

    // Keep CPU samples for the next refresh.
    previousSamples = std::move(currentSamples);

    return results;
}

std::vector<OngoingProcessInfo>
OngoingProcessMonitor::collectProcesses() {
    return collectSnapshot();
}
