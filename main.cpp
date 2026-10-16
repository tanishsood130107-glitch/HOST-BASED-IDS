#include <iostream>
#include <string>
#include <windows.h>

#include "Event.h"
#include "EventCollector.h"
#include "EventFilter.h"
#include "Rule.h"
#include "RuleEngine.h"
#include "Alert.h"
#include "LoginMonitor.h"
#include "ProcessMonitor.h"
#include "FileMonitor.h"

using namespace std;

int main()
{
    // ========================================================
    // HIDS HEADER
    // ========================================================

    cout
        << "========================================"
        << endl;

    cout
        << "       HOST BASED INTRUSION DETECTION"
        << endl;

    cout
        << "========================================"
        << endl;

    // ========================================================
    // CREATE COMPONENTS
    // ========================================================

    EventCollector collector;

    EventFilter filter;

    RuleEngine ruleEngine;

    LoginMonitor loginMonitor;

    ProcessMonitor processMonitor;

    FileMonitor fileMonitor(
        "HIDS_Monitor"
    );

    // ========================================================
    // LOGIN RULE
    // ========================================================

    Rule failedLoginRule(
        1,
        "Multiple Failed Logins",
        "LOGIN_FAILURE",
        2,
        "HIGH"
    );

    // ========================================================
    // PROCESS RULE
    // ========================================================

    Rule suspiciousProcessRule(
        2,
        "Suspicious PowerShell Process",
        "PROCESS_START",
        1,
        "HIGH",
        "powershell.exe"
    );

    // ========================================================
    // FILE RULE
    // ========================================================

    Rule suspiciousFileRule(
        3,
        "Suspicious Script File",
        "FILE_CREATED",
        1,
        "HIGH",
        ".ps1"
    );

    // ========================================================
    // ADD RULES
    // ========================================================

    ruleEngine.addRule(
        failedLoginRule
    );

    ruleEngine.addRule(
        suspiciousProcessRule
    );

    ruleEngine.addRule(
        suspiciousFileRule
    );

    // ========================================================
    // START MONITORING
    // ========================================================

    cout
        << "\n[MAIN] Login monitoring started."
        << endl;

    cout
        << "[MAIN] Process monitoring started."
        << endl;

    cout
        << "[MAIN] File monitoring started."
        << endl;

    cout
        << "[MAIN] Monitoring directory: "
        << "HIDS_Monitor"
        << endl;

    cout
        << "\n----------------------------------------"
        << endl;

    // ========================================================
    // MAIN MONITORING LOOP
    // ========================================================

    while (true)
    {
        // ====================================================
        // LOGIN MONITORING
        // ====================================================

        Event loginEvent;

        if (loginMonitor.checkForLoginFailure(
                loginEvent))
        {
            collector.addEvent(
                loginEvent
            );
        }

        // ====================================================
        // PROCESS MONITORING
        // ====================================================

        Event processEvent;

        if (processMonitor.checkForNewProcess(
                processEvent))
        {
            collector.addEvent(
                processEvent
            );
        }

        // ====================================================
        // FILE MONITORING
        // ====================================================

        Event fileEvent;

        if (fileMonitor.checkForFileChange(
                fileEvent))
        {
            collector.addEvent(
                fileEvent
            );
        }

        // ====================================================
        // PROCESS EVENTS
        // ====================================================

        while (collector.hasEvents())
        {
            Event currentEvent =
                collector.getNextEvent();

            cout
                << "\n[MAIN] Processing event: "
                << currentEvent.getEventType()
                << " at "
                << currentEvent.getTimestamp()
                << endl;

            // =================================================
            // EVENT FILTER
            // =================================================

            if (!filter.isRelevant(
                    currentEvent))
            {
                continue;
            }

            // =================================================
            // RULE ENGINE
            // =================================================

            string matchedRule;

            string severity;

            bool ruleMatched =
                ruleEngine.checkEvent(
                    currentEvent,
                    matchedRule,
                    severity
                );

            // =================================================
            // ALERT
            // =================================================

            if (ruleMatched)
            {
                Alert alert(
                    1,
                    severity,
                    matchedRule,
                    "Suspicious activity detected."
                );

                alert.display();
            }
        }

        // ====================================================
        // WAIT 2 SECONDS
        // ====================================================

        Sleep(2000);
    }

    return 0;
}