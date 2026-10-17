


#pragma once
 
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "FileScanner.h"   // ScanType, ScanStatus, ScanFinding (no windows.h inside)
 
// Plain data structs: no <windows.h> here, so the UI file stays clean.
 
struct BackendStats {
    float         cpuPercent   = 0.0f;
    float         ramPercent   = 0.0f;
    int           processCount = 0;
    unsigned long sample       = 0;     // increases with every new sample
};
 
struct BackendProcess {
    int         pid            = 0;
    std::string name;
    float       cpuPercent     = 0.0f;
    float       memoryMB       = 0.0f;
    double      runningSeconds = 0.0;
    bool        isApp          = false;   // has a visible window (like Task Manager "Apps")
    std::string windowTitle;              // title of that window
};
 
struct BackendAlert {
    int         id = 0;
    std::string time;
    std::string severity;
    std::string source;
    std::string rule;
    std::string description;

    // Resource alerts: the app that is hogging the machine (End Task button)
    bool        canEndTask      = false;
    int         culpritPid      = 0;
    std::string culpritName;
    float       culpritCpu      = 0.0f;
    float       culpritMemoryMB = 0.0f;
};
 
struct BackendEvent {
    std::string time;
    std::string type;
    std::string description;
};
 
// Result of the System Analysis modules, refreshed every second.
struct BackendAnalysis {
    bool valid = false;
 
    double        cpuPercent        = 0.0;
    double        memoryPercent     = 0.0;
    std::uint64_t totalMemoryMB     = 0;
    std::uint64_t usedMemoryMB      = 0;
    std::uint64_t availableMemoryMB = 0;
    bool          cpuAvailable      = false;
    bool          memoryAvailable   = false;
 
    std::size_t processCount           = 0;
    std::size_t highCpuProcessCount    = 0;
    std::size_t highMemoryProcessCount = 0;
 
    std::vector<BackendProcess> topCpuProcesses;   // sorted, top 10
    std::vector<std::string>    findings;
 
    std::vector<float> cpuHistory;                 // oldest -> newest
    std::vector<float> ramHistory;
};
 
class HIDSBackend {
public:
    HIDSBackend();
    ~HIDSBackend();
 
    HIDSBackend(const HIDSBackend&) = delete;
    HIDSBackend& operator=(const HIDSBackend&) = delete;
 
    void start();
    void stop();
    void shutdown();
    bool isRunning() const;
 
    BackendStats                getStats() const;
    std::vector<BackendProcess> getProcesses() const;
    std::vector<BackendAlert>   getAlerts() const;
    std::vector<BackendEvent>   getEvents() const;
    BackendAnalysis             getAnalysis() const;

    // Ends an APP process (one with a visible window). System and
    // background processes are refused. message receives the result text.
    bool                        endTask(int pid, std::string& message,
                                const std::string& expectedName = "");

    // ---- Password lock (Logs & Reports, End Task) ----
    bool                        isLockConfigured() const;
    int                         lockoutSecondsLeft() const;

    // Wrong guesses are counted, rate-limited and raised as alerts.
    bool                        verifyPassword(const std::string& password,
                                               std::string& message);

    // Creates the password (oldPassword ignored) or changes it
    // (oldPassword must be correct).
    bool                        setPassword(const std::string& newPassword,
                                            const std::string& oldPassword,
                                            std::string& message);

    // ---- File scanner ----
    // startScan returns false if a scan is already running.
    bool                     startScan(ScanType type, const std::string& customPath = "");
    void                     cancelScan();
    bool                     isScanRunning() const;
    ScanStatus               getScanStatus() const;
    std::vector<ScanFinding> getScanFindings() const;
    bool                     exportScanReport(std::string& outPath) const;
 
private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
 