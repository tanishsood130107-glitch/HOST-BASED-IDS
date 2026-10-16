
#pragma once

#include "SystemResourceMonitor.h"

#include <string>

namespace hids_analysis {

struct AnomalyConfig {
    double cpuThresholdPercent = 90.0;
    double memoryThresholdPercent = 90.0;
    unsigned int consecutiveSamplesRequired = 3;
};

struct AnomalyResult {
    bool detected = false;
    std::string message;
};

class AnomalyDetector {
public:
    explicit AnomalyDetector(
        const AnomalyConfig& config = AnomalyConfig{});

    AnomalyResult evaluate(const SystemResourceStats& stats);

    void reset();

private:
    AnomalyConfig config_;

    unsigned int highCpuSamples_ = 0;
    unsigned int highMemorySamples_ = 0;

    bool cpuAlertReported_ = false;
    bool memoryAlertReported_ = false;
};

} // namespace hids_analysis
