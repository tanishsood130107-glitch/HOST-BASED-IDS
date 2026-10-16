
#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <string>
#include <vector>
#include <unordered_map>

struct OngoingProcessInfo {
    DWORD pid = 0;
    DWORD parentPid = 0;

    std::string name;
    std::string executablePath;

    double memoryMB = 0.0;
    double cpuPercent = 0.0;
    double runningSeconds = 0.0;

    bool memoryAvailable = false;
    bool cpuAvailable = false;
    bool startTimeAvailable = false;
};

class OngoingProcessMonitor {
public:
    std::vector<OngoingProcessInfo> collectProcesses();

private:
    struct CpuSample {
        ULONGLONG creationTime = 0;
        ULONGLONG cpuTime = 0;
        ULONGLONG sampleTime = 0;
    };

    std::unordered_map<DWORD, CpuSample> previousSamples;

    static ULONGLONG fileTimeToUInt64(const FILETIME& time);

    std::vector<OngoingProcessInfo> collectSnapshot();
};
