
#pragma once

#include "SystemResourceMonitor.h"

#include <string>
#include <vector>

namespace hids_analysis {

struct ProcessSnapshot {
    unsigned long pid = 0;
    std::string name;
    double cpuPercent = 0.0;
    double memoryMB = 0.0;
};

struct SystemAnalysisReport {
    SystemResourceStats resources;

    std::size_t processCount = 0;
    std::size_t highCpuProcessCount = 0;
    std::size_t highMemoryProcessCount = 0;

    std::vector<ProcessSnapshot> topCpuProcesses;
    std::vector<std::string> findings;
};

class SystemAnalysis {
public:
    SystemAnalysisReport analyze(
        const SystemResourceStats& resources,
        const std::vector<ProcessSnapshot>& processes) const;
};

} // namespace hids_analysis
