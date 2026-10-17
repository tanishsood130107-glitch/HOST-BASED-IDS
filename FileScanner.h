// #pragma once
 
// // FileScanner: signature + heuristic file scanner for the HIDS project.
// // Runs on its own background thread; the UI polls getStatus()/getFindings().
// // Requires C++17 (std::filesystem). Windows only.
 
// #include <memory>
// #include <string>
// #include <vector>
 
// enum class ScanType
// {
//     Quick  = 0,   // user folders, temp, startup, AppData
//     Full   = 1,   // every fixed drive
//     Custom = 2    // one folder (or file) chosen by the user
// };
 
// struct ScanFinding
// {
//     std::string path;
//     std::string name;      // e.g. "EICAR-Test-File" or "Disguised executable"
//     std::string reason;    // all reasons that contributed
//     std::string level;     // "Malicious" or "Suspicious"
//     std::string sha256;    // empty if the file was not hashed
//     std::string time;      // HH:MM:SS
//     unsigned long long size = 0;
//     int score = 0;
// };
 
// struct ScanStatus
// {
//     bool running   = false;
//     bool finished  = false;   // a scan completed (or was stopped) at least once
//     bool cancelled = false;
//     ScanType type  = ScanType::Quick;
 
//     unsigned long long filesScanned = 0;
//     unsigned long long filesSkipped = 0;   // unreadable / too large
//     unsigned long long bytesScanned = 0;
 
//     int malicious  = 0;
//     int suspicious = 0;
 
//     double elapsedSeconds = 0.0;
 
//     std::string currentFile;
//     std::string target;       // human readable description of what is scanned
// };
 
// class FileScanner
// {
// public:
//     FileScanner();
//     ~FileScanner();
 
//     FileScanner(const FileScanner&) = delete;
//     FileScanner& operator=(const FileScanner&) = delete;
 
//     // customPath (UTF-8) is only used for ScanType::Custom.
//     // Returns false if a scan is already running.
//     bool start(ScanType type, const std::string& customPath = "");
 
//     void cancel();      // ask the scan to stop (returns immediately)
//     void shutdown();    // cancel + join (call before exiting)
 
//     bool isRunning() const;
 
//     ScanStatus               getStatus() const;
//     std::vector<ScanFinding> getFindings() const;
 
//     // Writes HIDS_Scan_Report_<timestamp>.txt next to the executable.
//     // On success outPath receives the file name.
//     bool exportReport(std::string& outPath) const;
 
// private:
//     struct Impl;
//     std::unique_ptr<Impl> impl;
// };
 

#pragma once
 
// FileScanner: signature + heuristic file scanner for the HIDS project.
// Runs on its own background thread; the UI polls getStatus()/getFindings().
// Requires C++17 (std::filesystem). Windows only.
 
#include <memory>
#include <string>
#include <vector>
 
enum class ScanType
{
    Quick  = 0,   // user folders, temp, startup, AppData
    Full   = 1,   // every fixed drive
    Custom = 2    // one folder (or file) chosen by the user
};
 
struct ScanFinding
{
    std::string path;
    std::string name;      // e.g. "EICAR-Test-File" or "Disguised executable"
    std::string reason;    // all reasons that contributed
    std::string level;     // "Malicious" or "Suspicious"
    std::string sha256;    // empty if the file was not hashed
    std::string time;      // HH:MM:SS
    unsigned long long size = 0;
    int score = 0;
};
 
struct ScanStatus
{
    bool running   = false;
    bool finished  = false;   // a scan completed (or was stopped) at least once
    bool cancelled = false;
    ScanType type  = ScanType::Quick;
 
    unsigned long long filesScanned = 0;
    unsigned long long filesSkipped = 0;   // unreadable / too large
    unsigned long long bytesScanned = 0;
 
    int malicious  = 0;
    int suspicious = 0;
 
    double elapsedSeconds = 0.0;
 
    std::string currentFile;
    std::string target;       // human readable description of what is scanned
};
 
// One detection rule the scanner knows about (shown on the Logs & Reports page).
struct SignatureInfo
{
    std::string category;          // "Known hash", "File content", "File name / location"
    std::string name;              // human readable label
    std::string pattern;           // what is matched / the condition
    int         weight = 0;        // score added when it matches
    bool        instantMalicious = false;   // true = file is reported Malicious at once
};

class FileScanner
{
public:
    FileScanner();
    ~FileScanner();
 
    FileScanner(const FileScanner&) = delete;
    FileScanner& operator=(const FileScanner&) = delete;
 
    // customPath (UTF-8) is only used for ScanType::Custom.
    // Returns false if a scan is already running.
    bool start(ScanType type, const std::string& customPath = "");
 
    void cancel();      // ask the scan to stop (returns immediately)
    void shutdown();    // cancel + join (call before exiting)
 
    bool isRunning() const;
 
    ScanStatus               getStatus() const;
    std::vector<ScanFinding> getFindings() const;
 
    // Writes HIDS_Scan_Report_<timestamp>.txt next to the executable.
    // On success outPath receives the file name.
    bool exportReport(std::string& outPath) const;

    // Every rule the scanner checks (static: no scan needed to list them).
    static std::vector<SignatureInfo> getSignatures();

    // Heuristic score needed before a file is reported as Suspicious.
    static int suspiciousThreshold();
 
private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};