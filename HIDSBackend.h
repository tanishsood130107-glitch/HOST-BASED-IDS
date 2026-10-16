
//helooo




// #pragma once
 
// #include <cstddef>
// #include <cstdint>
// #include <memory>
// #include <string>
// #include <vector>
 
// // Plain data structs: no <windows.h> here, so the UI file stays clean.
 
// struct BackendStats {
//     float         cpuPercent   = 0.0f;
//     float         ramPercent   = 0.0f;
//     int           processCount = 0;
//     unsigned long sample       = 0;     // increases with every new sample
// };
 
// struct BackendProcess {
//     int         pid            = 0;
//     std::string name;
//     float       cpuPercent     = 0.0f;
//     float       memoryMB       = 0.0f;
//     double      runningSeconds = 0.0;
// };
 
// struct BackendAlert {
//     int         id = 0;
//     std::string time;
//     std::string severity;
//     std::string source;
//     std::string rule;
//     std::string description;
// };
 
// struct BackendEvent {
//     std::string time;
//     std::string type;
//     std::string description;
// };
 
// // Result of the System Analysis modules, refreshed every second.
// struct BackendAnalysis {
//     bool valid = false;
 
//     double        cpuPercent        = 0.0;
//     double        memoryPercent     = 0.0;
//     std::uint64_t totalMemoryMB     = 0;
//     std::uint64_t usedMemoryMB      = 0;
//     std::uint64_t availableMemoryMB = 0;
//     bool          cpuAvailable      = false;
//     bool          memoryAvailable   = false;
 
//     std::size_t processCount           = 0;
//     std::size_t highCpuProcessCount    = 0;
//     std::size_t highMemoryProcessCount = 0;
 
//     std::vector<BackendProcess> topCpuProcesses;   // sorted, top 10
//     std::vector<std::string>    findings;
 
//     std::vector<float> cpuHistory;                 // oldest -> newest
//     std::vector<float> ramHistory;
// };
 
// class HIDSBackend {
// public:
//     HIDSBackend();
//     ~HIDSBackend();
 
//     HIDSBackend(const HIDSBackend&) = delete;
//     HIDSBackend& operator=(const HIDSBackend&) = delete;
 
//     void start();
//     void stop();
//     void shutdown();
//     bool isRunning() const;
 
//     BackendStats                getStats() const;
//     std::vector<BackendProcess> getProcesses() const;
//     std::vector<BackendAlert>   getAlerts() const;
//     std::vector<BackendEvent>   getEvents() const;
//     BackendAnalysis             getAnalysis() const;
 
// private:
//     struct Impl;
//     std::unique_ptr<Impl> impl;
// };
 

// #ifndef HIDSBACKEND_H
// #define HIDSBACKEND_H

// // ============================================================
// // HIDSBackend
// //
// // The only thing the dashboard needs to include.
// //
// // It owns every monitor (login, process, file), the filter and
// // the rule engine, runs them on background threads, and hands
// // the UI thread plain-data COPIES of the latest state.
// // The UI never touches a monitor directly and never blocks.
// //
// // Deliberately includes no Windows headers, so <windows.h> never
// // ends up in the same file as GLFW / ImGui.
// // ============================================================

// #include <memory>
// #include <string>
// #include <vector>

// struct BackendStats
// {
//     float cpuPercent = 0.0f;    // whole machine, 0-100
//     float ramPercent = 0.0f;    // physical memory in use, 0-100
//     int   processCount = 0;
//     unsigned long sample = 0;   // increases every time new stats arrive
// };

// struct BackendProcess
// {
//     int pid = 0;
//     std::string name;
//     float cpuPercent = 0.0f;       // share of ALL cores, 0-100
//     float memoryMB = 0.0f;         // working set
//     double runningSeconds = 0.0;
// };

// struct BackendAlert
// {
//     int id = 0;
//     std::string time;          // HH:MM:SS
//     std::string severity;      // Critical / High / Medium / Low
//     std::string source;        // Login Monitor / Process Monitor / File Monitor
//     std::string rule;
//     std::string description;
// };

// struct BackendEvent
// {
//     std::string time;          // YYYY-MM-DD HH:MM:SS
//     std::string type;          // LOGIN_FAILURE, FILE_CREATED, ...
//     std::string description;
// };

// class HIDSBackend
// {
// public:
//     HIDSBackend();
//     ~HIDSBackend();

//     HIDSBackend(const HIDSBackend&) = delete;
//     HIDSBackend& operator=(const HIDSBackend&) = delete;

//     // Start monitoring. Does nothing if already running.
//     // Every start() begins from a fresh baseline (processes, files,
//     // last login record), so nothing that happened while stopped
//     // is reported as new.
//     void start();

//     // Ask the monitors to stop. Returns immediately (never blocks the UI).
//     void stop();

//     // Stop and wait for the threads to finish. Call once at app exit.
//     void shutdown();

//     bool isRunning() const;

//     // Each call returns a copy that is safe to use on the UI thread.
//     BackendStats getStats() const;
//     std::vector<BackendProcess> getProcesses() const;
//     std::vector<BackendAlert> getAlerts() const;   // newest first
//     std::vector<BackendEvent> getEvents() const;   // login + file events, newest first

// private:
//     struct Impl;
//     std::unique_ptr<Impl> impl;
// };

// #endif


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
};
 
struct BackendAlert {
    int         id = 0;
    std::string time;
    std::string severity;
    std::string source;
    std::string rule;
    std::string description;
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
 