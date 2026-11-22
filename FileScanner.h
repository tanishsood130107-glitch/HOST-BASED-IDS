#pragma once

#include <memory>
#include <string>
#include <vector>

enum class ScanType
{
    Quick  = 0,
    Full   = 1,
    Custom = 2
};

struct ScanFinding
{
    std::string path;
    std::string name;
    std::string reason;
    std::string level;
    std::string sha256;
    std::string time;
    unsigned long long size = 0;
    int score = 0;
};

struct ScanStatus
{
    bool running   = false;
    bool finished  = false;
    bool cancelled = false;
    ScanType type = ScanType::Quick;

    unsigned long long filesScanned = 0;
    unsigned long long filesSkipped = 0;
    unsigned long long bytesScanned = 0;

    int malicious  = 0;
    int suspicious = 0;

    double elapsedSeconds = 0.0;

    std::string currentFile;
    std::string target;
};

struct SignatureInfo
{
    std::string category;
    std::string name;
    std::string pattern;
    int weight = 0;
    bool instantMalicious = false;
};

class FileScanner
{
public:
    FileScanner();
    ~FileScanner();

    FileScanner(const FileScanner&) = delete;
    FileScanner& operator=(const FileScanner&) = delete;

    bool start(ScanType type, const std::string& customPath = "");

    void cancel();
    void shutdown();

    bool isRunning() const;

    ScanStatus getStatus() const;
    std::vector<ScanFinding> getFindings() const;

    bool exportReport(std::string& outPath) const;

    static std::vector<SignatureInfo> getSignatures();

    static int suspiciousThreshold();

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};