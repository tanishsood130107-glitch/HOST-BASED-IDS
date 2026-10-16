#include "HIDSBackend.h"
 
#include "Alert.h"
#include "Event.h"
#include "EventCollector.h"
#include "EventFilter.h"
#include "FileMonitor.h"
#include "LoginMonitor.h"
#include "OngoingProcessMonitor.h"   // also pulls in <windows.h> (with NOMINMAX)
#include "ProcessMonitor.h"
#include "Rule.h"
#include "RuleEngine.h"
 
// System Analysis modules
#include "AnomalyDetector.h"
#include "ResourceHistory.h"
#include "SystemAnalysis.h"
#include "SystemResourceMonitor.h"
 
#include <windows.h>
 
#include <algorithm>
#include <atomic>
#include <cctype>
#include <chrono>
#include <cstdio>
#include <deque>
#include <mutex>
#include <thread>
 
// The System Analysis modules live in namespace hids_analysis
using hids_analysis::AnomalyDetector;
using hids_analysis::AnomalyResult;
using hids_analysis::ProcessSnapshot;
using hids_analysis::ResourceHistory;
using hids_analysis::SystemAnalysis;
using hids_analysis::SystemAnalysisReport;
using hids_analysis::SystemResourceMonitor;
using hids_analysis::SystemResourceStats;
 
// ------------------------------------------------------------
// Settings
// ------------------------------------------------------------
 
namespace
{
    const char* const WATCHED_DIRECTORY = "HIDS_Monitor";
 
    const std::size_t MAX_ALERTS = 200;
    const std::size_t MAX_EVENTS = 500;
 
    const int EVENT_POLL_MS = 2000;   // login / process / file checks
    const int STATS_POLL_MS = 1000;   // CPU, RAM, process list
 
    // "HIGH" -> "High" (the dashboard filter buttons use this form)
    std::string TitleCase(std::string text)
    {
        for (std::size_t i = 0; i < text.size(); ++i)
        {
            const unsigned char c = static_cast<unsigned char>(text[i]);
            text[i] = static_cast<char>(i == 0 ? std::toupper(c) : std::tolower(c));
        }
 
        return text;
    }
 
    std::string SourceForEventType(const std::string& type)
    {
        if (type == "LOGIN_FAILURE")        return "Login Monitor";
        if (type == "PROCESS_START")        return "Process Monitor";
        if (type.rfind("FILE_", 0) == 0)    return "File Monitor";
 
        return "HIDS";
    }
 
    // "2026-10-13 10:24:05" -> "10:24:05"
    std::string TimeOfDay(const std::string& timestamp)
    {
        return timestamp.size() >= 19 ? timestamp.substr(11, 8) : timestamp;
    }
 
    // Current local time as "HH:MM:SS" (used for System Analysis alerts)
    std::string NowTimeOfDay()
    {
        SYSTEMTIME st{};
        GetLocalTime(&st);
 
        char text[16];
        std::snprintf(text, sizeof(text), "%02d:%02d:%02d",
                      static_cast<int>(st.wHour),
                      static_cast<int>(st.wMinute),
                      static_cast<int>(st.wSecond));
 
        return text;
    }
}
 
// ------------------------------------------------------------
// Impl: everything private lives here
// ------------------------------------------------------------
 
struct HIDSBackend::Impl
{
    // ---- Detection pipeline: touched ONLY by the event thread ----
    EventCollector collector;
    EventFilter    filter;
    RuleEngine     ruleEngine;
    LoginMonitor   loginMonitor;
    ProcessMonitor processMonitor;
    FileMonitor    fileMonitor;
    int            nextAlertID;
 
    // ---- System sampling: touched ONLY by the stats thread ----
    OngoingProcessMonitor processSampler;
    SystemResourceMonitor resourceMonitor;
    ResourceHistory       resourceHistory;
    SystemAnalysis        analysisEngine;
    AnomalyDetector       anomalyDetector;
    int                   anomalyAlertID;
    unsigned long         sampleCounter;
 
    // ---- Threads ----
    std::atomic<bool> running;
    std::thread eventThread;
    std::thread statsThread;
 
    // ---- Shared with the UI: always accessed under dataMutex ----
    mutable std::mutex dataMutex;
    BackendStats stats;
    BackendAnalysis analysis;
    std::vector<BackendProcess> processes;
    std::deque<BackendAlert> alerts;
    std::deque<BackendEvent> events;
 
    Impl()
        : fileMonitor(WATCHED_DIRECTORY),
          nextAlertID(1),
          resourceHistory(60),
          anomalyAlertID(100000),
          sampleCounter(0),
          running(false)
    {
        addRules();
    }
 
    // Same rules that used to live in the old console main.cpp
    void addRules()
    {
        ruleEngine.addRule(Rule(
            1, "Multiple Failed Logins", "LOGIN_FAILURE", 2, "HIGH"));
 
        ruleEngine.addRule(Rule(
            2, "Suspicious PowerShell Process", "PROCESS_START", 1, "HIGH",
            "powershell.exe"));
 
        ruleEngine.addRule(Rule(
            3, "Suspicious Script File", "FILE_CREATED", 1, "HIGH",
            ".ps1"));
    }
 
    // Fresh baselines for every start()
    void resetPipeline()
    {
        collector      = EventCollector();
        ruleEngine     = RuleEngine();
        loginMonitor   = LoginMonitor();
        processMonitor = ProcessMonitor();
        fileMonitor    = FileMonitor(WATCHED_DIRECTORY);
        processSampler = OngoingProcessMonitor();
 
        resourceMonitor = SystemResourceMonitor();
        resourceHistory.clear();
        anomalyDetector.reset();
 
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            analysis = BackendAnalysis();
        }
 
        addRules();
    }
 
    void joinThreads()
    {
        if (eventThread.joinable()) eventThread.join();
        if (statsThread.joinable()) statsThread.join();
    }
 
    // Sleep in small slices so stop() is noticed quickly
    void sleepWhileRunning(int milliseconds)
    {
        for (int waited = 0; waited < milliseconds && running; waited += 100)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
 
    // --------------------------------------------------------
    // Event thread: the old main.cpp loop
    // --------------------------------------------------------
 
    void eventLoop()
    {
        while (running)
        {
            pollMonitors();
            drainEvents();
 
            sleepWhileRunning(EVENT_POLL_MS);
        }
    }
 
    void pollMonitors()
    {
        Event loginEvent;
        if (loginMonitor.checkForLoginFailure(loginEvent))
            collector.addEvent(loginEvent);
 
        Event processEvent;
        if (processMonitor.checkForNewProcess(processEvent))
            collector.addEvent(processEvent);
 
        Event fileEvent;
        if (fileMonitor.checkForFileChange(fileEvent))
            collector.addEvent(fileEvent);
    }
 
    void drainEvents()
    {
        while (collector.hasEvents())
        {
            const Event event = collector.getNextEvent();
 
            if (!filter.isRelevant(event))
                continue;
 
            recordEvent(event);
 
            std::string matchedRule;
            std::string severity;
 
            if (ruleEngine.checkEvent(event, matchedRule, severity))
                recordAlert(event, matchedRule, severity);
        }
    }
 
    // Keeps login and file activity for the Login / File pages.
    // Process starts are skipped: there are far too many of them.
    void recordEvent(const Event& event)
    {
        const std::string type = event.getEventType();
 
        if (type.rfind("LOGIN_", 0) != 0 && type.rfind("FILE_", 0) != 0)
            return;
 
        BackendEvent record;
        record.time        = event.getTimestamp();
        record.type        = type;
        record.description = event.getDescription();
 
        std::lock_guard<std::mutex> lock(dataMutex);
 
        events.push_front(record);
 
        if (events.size() > MAX_EVENTS)
            events.pop_back();
    }
 
    void recordAlert(
        const Event& event,
        const std::string& matchedRule,
        const std::string& severity)
    {
        const Alert alert(
            nextAlertID++,
            TitleCase(severity),
            matchedRule,
            event.getDescription(),
            SourceForEventType(event.getEventType()),
            event.getTimestamp()
        );
 
        BackendAlert record;
        record.id          = alert.getID();
        record.time        = TimeOfDay(alert.getTimestamp());
        record.severity    = alert.getSeverity();
        record.source      = alert.getSource();
        record.rule        = alert.getRuleName();
        record.description = alert.getRuleName() + " - " + alert.getMessage();
 
        std::lock_guard<std::mutex> lock(dataMutex);
 
        alerts.push_front(record);
 
        if (alerts.size() > MAX_ALERTS)
            alerts.pop_back();
    }
 
    // --------------------------------------------------------
    // Stats thread: CPU, RAM, process list and System Analysis
    // --------------------------------------------------------
 
    void statsLoop()
    {
        while (running)
        {
            sampleSystem();
 
            sleepWhileRunning(STATS_POLL_MS);
        }
    }
 
    void sampleSystem()
    {
        // Process list (can take a moment, so done outside the lock)
        const std::vector<OngoingProcessInfo> raw =
            processSampler.collectProcesses();
 
        std::vector<BackendProcess> list;
        list.reserve(raw.size());
 
        for (const OngoingProcessInfo& p : raw)
        {
            BackendProcess row;
 
            row.pid  = static_cast<int>(p.pid);
            row.name = p.name.empty() ? "Unknown" : p.name;
 
            row.cpuPercent = p.cpuAvailable
                ? static_cast<float>(p.cpuPercent) : 0.0f;
 
            row.memoryMB = p.memoryAvailable
                ? static_cast<float>(p.memoryMB) : 0.0f;
 
            row.runningSeconds = p.startTimeAvailable
                ? p.runningSeconds : 0.0;
 
            list.push_back(std::move(row));
        }
 
        // Whole-machine CPU / RAM via the System Analysis monitor
        const SystemResourceStats res = resourceMonitor.collect();
        resourceHistory.addSample(res);
 
        // Sustained high CPU / RAM check
        const AnomalyResult anomaly = anomalyDetector.evaluate(res);
 
        // Run the analysis on the current process list
        std::vector<ProcessSnapshot> snapshots;
        snapshots.reserve(list.size());
 
        for (const BackendProcess& p : list)
        {
            ProcessSnapshot s;
            s.pid        = static_cast<unsigned long>(p.pid);
            s.name       = p.name;
            s.cpuPercent = p.cpuPercent;
            s.memoryMB   = p.memoryMB;
            snapshots.push_back(std::move(s));
        }
 
        const SystemAnalysisReport report =
            analysisEngine.analyze(res, snapshots);
 
        BackendAnalysis out;
        out.valid                  = true;
        out.cpuPercent             = res.cpuPercent;
        out.memoryPercent          = res.memoryPercent;
        out.totalMemoryMB          = res.totalMemoryMB;
        out.usedMemoryMB           = res.usedMemoryMB;
        out.availableMemoryMB      = res.availableMemoryMB;
        out.cpuAvailable           = res.cpuAvailable;
        out.memoryAvailable        = res.memoryAvailable;
        out.processCount           = report.processCount;
        out.highCpuProcessCount    = report.highCpuProcessCount;
        out.highMemoryProcessCount = report.highMemoryProcessCount;
        out.findings               = report.findings;
 
        const std::size_t topN =
            std::min<std::size_t>(10, report.topCpuProcesses.size());
 
        for (std::size_t i = 0; i < topN; ++i)
        {
            const ProcessSnapshot& s = report.topCpuProcesses[i];
 
            BackendProcess row;
            row.pid            = static_cast<int>(s.pid);
            row.name           = s.name;
            row.cpuPercent     = static_cast<float>(s.cpuPercent);
            row.memoryMB       = static_cast<float>(s.memoryMB);
            row.runningSeconds = 0.0;
 
            out.topCpuProcesses.push_back(std::move(row));
        }
 
        for (const SystemResourceStats& h : resourceHistory.getSamples())
        {
            out.cpuHistory.push_back(static_cast<float>(h.cpuPercent));
            out.ramHistory.push_back(static_cast<float>(h.memoryPercent));
        }
 
        // Publish
        std::lock_guard<std::mutex> lock(dataMutex);
 
        stats.cpuPercent   = static_cast<float>(res.cpuPercent);
        stats.ramPercent   = static_cast<float>(res.memoryPercent);
        stats.processCount = static_cast<int>(list.size());
        stats.sample       = ++sampleCounter;
 
        processes.swap(list);
        analysis = std::move(out);
 
        // High resource usage is a diagnostic signal, not proof of an
        // intrusion, so the alert is only Medium severity.
        if (anomaly.detected)
        {
            BackendAlert a;
            a.id          = anomalyAlertID++;
            a.time        = NowTimeOfDay();
            a.severity    = "Medium";
            a.source      = "System Analysis";
            a.rule        = "Resource Anomaly";
            a.description = "Resource Anomaly - " + anomaly.message;
 
            alerts.push_front(a);
 
            if (alerts.size() > MAX_ALERTS)
                alerts.pop_back();
        }
    }
};
 
// ------------------------------------------------------------
// Public API
// ------------------------------------------------------------
 
HIDSBackend::HIDSBackend()
    : impl(new Impl())
{
}
 
HIDSBackend::~HIDSBackend()
{
    shutdown();
}
 
void HIDSBackend::start()
{
    if (impl->running)
        return;
 
    // A previous run may still be finishing its last cycle
    impl->joinThreads();
    impl->resetPipeline();
 
    impl->running = true;
 
    impl->eventThread = std::thread(&Impl::eventLoop, impl.get());
    impl->statsThread = std::thread(&Impl::statsLoop, impl.get());
}
 
void HIDSBackend::stop()
{
    impl->running = false;
}
 
void HIDSBackend::shutdown()
{
    impl->running = false;
    impl->joinThreads();
}
 
bool HIDSBackend::isRunning() const
{
    return impl->running;
}
 
BackendStats HIDSBackend::getStats() const
{
    std::lock_guard<std::mutex> lock(impl->dataMutex);
    return impl->stats;
}
 
std::vector<BackendProcess> HIDSBackend::getProcesses() const
{
    std::lock_guard<std::mutex> lock(impl->dataMutex);
    return impl->processes;
}
 
std::vector<BackendAlert> HIDSBackend::getAlerts() const
{
    std::lock_guard<std::mutex> lock(impl->dataMutex);
    return std::vector<BackendAlert>(impl->alerts.begin(), impl->alerts.end());
}
 
std::vector<BackendEvent> HIDSBackend::getEvents() const
{
    std::lock_guard<std::mutex> lock(impl->dataMutex);
    return std::vector<BackendEvent>(impl->events.begin(), impl->events.end());
}
 
BackendAnalysis HIDSBackend::getAnalysis() const
{
    std::lock_guard<std::mutex> lock(impl->dataMutex);
    return impl->analysis;
}
 