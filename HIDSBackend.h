#pragma once
 
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "FileScanner.h"   //used for scanning the files
 

struct BackendStats {float         cpuPercent   = 0.0f;float         ramPercent   = 0.0f;int           processCount = 0;unsigned long sample       = 0;     };
 
struct BackendProcess {
    int         pid            = 0;
    std::string name;
    float       cpuPercent     = 0.0f;
    float       memoryMB       = 0.0f;
    double      runningSeconds = 0.0;
    bool        isApp          = false;  
    std::string windowTitle;              
};
 
struct BackendAlert {
    int         id = 0;
    std::string time;
    std::string severity;
    std::string source;
    std::string rule;
    std::string description;

   
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
 
    std::vector<BackendProcess> topCpuProcesses;   
    std::vector<std::string>    findings;
 
    std::vector<float> cpuHistory;                 
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

    //git 
    bool                        endTask(int pid, std::string& message,
                                const std::string& expectedName = "");

    
    bool                        isLockConfigured() const;
    int                         lockoutSecondsLeft() const;

    
    bool                        verifyPassword(const std::string& password,
                                               std::string& message);

    
    bool                        setPassword(const std::string& newPassword,
                                            const std::string& oldPassword,
                                            std::string& message);

   
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
 
