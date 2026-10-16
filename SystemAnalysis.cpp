
#include "SystemAnalysis.h"

#include <algorithm>

namespace hids_analysis {

SystemAnalysisReport SystemAnalysis::analyze(
    const SystemResourceStats& resources,
    const std::vector<ProcessSnapshot>& processes) const
{
    SystemAnalysisReport report;
    report.resources = resources;
    report.processCount = processes.size();

    if (resources.cpuAvailable && resources.cpuPercent >= 90.0) {
        report.findings.push_back(
            "System CPU usage is very high.");
    }

    if (resources.memoryAvailable &&
        resources.memoryPercent >= 90.0) {
        report.findings.push_back(
            "System memory usage is very high.");
    }

    for (const auto& process : processes) {
        if (process.cpuPercent >= 80.0) {
            ++report.highCpuProcessCount;
        }

        if (process.memoryMB >= 1024.0) {
            ++report.highMemoryProcessCount;
        }
    }

    // Copy processes and order them by reported CPU usage.
    report.topCpuProcesses = processes;

    std::sort(
        report.topCpuProcesses.begin(),
        report.topCpuProcesses.end(),
        [](const ProcessSnapshot& a, const ProcessSnapshot& b) {
            return a.cpuPercent > b.cpuPercent;
        });

    if (report.highCpuProcessCount > 0) {
        report.findings.push_back(
            "One or more processes have high reported CPU usage.");
    }

    if (report.highMemoryProcessCount > 0) {
        report.findings.push_back(
            "One or more processes use at least 1 GB of memory.");
    }

    return report;
}

} // namespace hids_analysis
