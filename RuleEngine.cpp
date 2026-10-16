#include "RuleEngine.h"

#include <iostream>

using namespace std;

RuleEngine::RuleEngine()
{
}

void RuleEngine::addRule(
    const Rule& rule)
{
    rules.push_back(rule);
}

bool RuleEngine::checkEvent(
    const Event& event,
    string& matchedRule,
    string& severity)
{
    // ========================================================
    // LOGIN FAILURE
    // ========================================================

    if (event.getEventType() == "LOGIN_FAILURE")
    {
        for (const Rule& rule : rules)
        {
            if (rule.matches(event))
            {
                std::time_t now =
                    event.getEpochSeconds();

                failedLoginTimes.push_back(now);

                // cross out entries older than the window
                while (
                    !failedLoginTimes.empty() &&
                    std::difftime(
                        now,
                        failedLoginTimes.front()
                    ) > rule.getWindowSeconds()
                )
                {
                    failedLoginTimes.pop_front();
                }

                cout
                    << "[RuleEngine] Failed logins in last "
                    << rule.getWindowSeconds()
                    << "s: "
                    << failedLoginTimes.size()
                    << "/"
                    << rule.getThreshold()
                    << endl;

                if (
                    static_cast<int>(
                        failedLoginTimes.size()
                    )
                    >= rule.getThreshold()
                )
                {
                    matchedRule =
                        rule.getRuleName();

                    severity =
                        rule.getSeverity();

                    failedLoginTimes.clear();

                    cout
                        << "[RuleEngine] Login rule matched!"
                        << endl;

                    return true;
                }

                return false;
            }
        }

        cout
            << "[RuleEngine] No matching login rule."
            << endl;

        return false;
    }

    // ========================================================
    // PROCESS START
    // ========================================================

    if (event.getEventType()
        == "PROCESS_START")
    {
        for (const Rule& rule : rules)
        {
            if (rule.matches(event))
            {
                matchedRule =
                    rule.getRuleName();

                severity =
                    rule.getSeverity();

                cout
                    << "[RuleEngine] Process rule matched!"
                    << endl;

                return true;
            }
        }

        cout
            << "[RuleEngine] No matching process rule."
            << endl;

        return false;
    }

    // ========================================================
    // FILE ACTIVITY
    // ========================================================

    if (
        event.getEventType()
            == "FILE_CREATED"
        ||
        event.getEventType()
            == "FILE_MODIFIED"
        ||
        event.getEventType()
            == "FILE_DELETED"
    )
    {
        for (const Rule& rule : rules)
        {
            if (rule.matches(event))
            {
                matchedRule =
                    rule.getRuleName();

                severity =
                    rule.getSeverity();

                cout
                    << "[RuleEngine] File rule matched!"
                    << endl;

                return true;
            }
        }

        cout
            << "[RuleEngine] No matching file rule."
            << endl;

        return false;
    }

    return false;
}