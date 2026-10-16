#include "LoginMonitor.h"

#include <iostream>
#include <string>
#include <cstdio>

// ======================================================
// Constructor
// ======================================================

LoginMonitor::LoginMonitor()
{
    lastRecordID = 0;

    initialized = false;
}

// ======================================================
// Check for a NEW failed login
// ======================================================

bool LoginMonitor::checkForLoginFailure(
    Event& event)
{
    std::string xml;

    unsigned long long recordID = 0;

    // Get latest Windows 4625 event
    if (!getLatestEvent(
            xml,
            recordID))
    {
        return false;
    }

    // ==================================================
    // First successful check
    // ==================================================

    if (!initialized)
    {
        lastRecordID = recordID;

        initialized = true;

        std::cout
            << "[LoginMonitor] Monitoring started."
            << std::endl;

        std::cout
            << "[LoginMonitor] Existing events ignored."
            << std::endl;

        std::cout
            << "[LoginMonitor] Starting Record ID: "
            << lastRecordID
            << std::endl;

        return false;
    }

    // ==================================================
    // Check whether event is new
    // ==================================================

    if (recordID <= lastRecordID)
    {
        return false;
    }

    // ==================================================
    // NEW failed login
    // ==================================================

    lastRecordID = recordID;

    std::cout
        << std::endl
        << "[LoginMonitor] NEW failed login detected!"
        << std::endl;

    std::cout
        << "[LoginMonitor] Windows Record ID: "
        << recordID
        << std::endl;

    // Create HIDS Event

    event = Event(
        static_cast<int>(recordID),
        "LOGIN_FAILURE",
        "Windows failed logon detected",
        "CURRENT_TIME"
    );

    return true;
}

// ======================================================
// Get latest Windows Event ID 4625
// ======================================================

bool LoginMonitor::getLatestEvent(
    std::string& xml,
    unsigned long long& recordID)
{
    xml.clear();

    recordID = 0;

    // ==================================================
    // Ask Windows for latest Event ID 4625
    // ==================================================

    const char* command =
        "wevtutil qe Security "
        "/q:\"*[System[(EventID=4625)]]\" "
        "/c:1 "
        "/rd:true "
        "/f:xml";

    FILE* pipe = _popen(
        command,
        "r"
    );

    if (pipe == nullptr)
    {
        std::cout
            << "[LoginMonitor] ERROR: Could not access "
            << "Windows Event Log."
            << std::endl;

        return false;
    }

    // ==================================================
    // Read command output
    // ==================================================

    char buffer[4096];

    while (fgets(
        buffer,
        sizeof(buffer),
        pipe))
    {
        xml += buffer;
    }

    int result =
        _pclose(pipe);

    if (result != 0 ||
        xml.empty())
    {
        std::cout
            << "[LoginMonitor] ERROR: Could not read "
            << "Security Event Log."
            << std::endl;

        return false;
    }

    // ==================================================
    // Find EventRecordID
    // ==================================================

    const std::string startTag =
        "<EventRecordID>";

    const std::string endTag =
        "</EventRecordID>";

    std::size_t start =
        xml.find(startTag);

    if (start ==
        std::string::npos)
    {
        std::cout
            << "[LoginMonitor] ERROR: EventRecordID "
            << "not found."
            << std::endl;

        return false;
    }

    start +=
        startTag.length();

    std::size_t end =
        xml.find(
            endTag,
            start
        );

    if (end ==
        std::string::npos)
    {
        std::cout
            << "[LoginMonitor] ERROR: Invalid event XML."
            << std::endl;

        return false;
    }

    std::string recordIDText =
        xml.substr(
            start,
            end - start
        );

    // ==================================================
    // Convert Record ID
    // ==================================================

    try
    {
        recordID =
            std::stoull(recordIDText);
    }
    catch (...)
    {
        std::cout
            << "[LoginMonitor] ERROR: Invalid Record ID."
            << std::endl;

        return false;
    }

    // ==================================================
    // Debug information
    // ==================================================

    std::cout
        << "[LoginMonitor] Latest 4625 Record ID: "
        << recordID
        << std::endl;

    return true;
}