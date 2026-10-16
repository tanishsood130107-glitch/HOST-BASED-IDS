
#include "AnomalyDetector.h"

#include <algorithm>

namespace hids_analysis {

AnomalyDetector::AnomalyDetector(const AnomalyConfig& config)
    : config_(config)
{
    config_.consecutiveSamplesRequired =
        std::max(1U, config_.consecutiveSamplesRequired);
}

AnomalyResult AnomalyDetector::evaluate(
    const SystemResourceStats& stats)
{
    AnomalyResult result;

    if (stats.cpuAvailable &&
        stats.cpuPercent >= config_.cpuThresholdPercent) {
        ++highCpuSamples_;
    } else {
        highCpuSamples_ = 0;
        cpuAlertReported_ = false;
    }

    if (stats.memoryAvailable &&
        stats.memoryPercent >= config_.memoryThresholdPercent) {
        ++highMemorySamples_;
    } else {
        highMemorySamples_ = 0;
        memoryAlertReported_ = false;
    }

    if (!cpuAlertReported_ &&
        highCpuSamples_ >= config_.consecutiveSamplesRequired) {
        result.detected = true;
        result.message =
            "Sustained high CPU usage detected; investigate the cause.";
        cpuAlertReported_ = true;
    }

    if (!memoryAlertReported_ &&
        highMemorySamples_ >= config_.consecutiveSamplesRequired) {
        if (result.detected) {
            result.message +=
                " Sustained high memory usage was also detected.";
        } else {
            result.detected = true;
            result.message =
                "Sustained high memory usage detected; investigate the cause.";
        }

        memoryAlertReported_ = true;
    }

    return result;
}

void AnomalyDetector::reset()
{
    highCpuSamples_ = 0;
    highMemorySamples_ = 0;

    cpuAlertReported_ = false;
    memoryAlertReported_ = false;
}

} // namespace hids_analysis
