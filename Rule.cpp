#include "Rule.h"

using namespace std;

Rule::Rule(
    int id,
    string name,
    string type,
    int thresholdValue,
    string severityLevel,
    string keyword,
    int windowSecs)
{
    ruleID = id;

    ruleName = name;

    eventType = type;

    threshold = thresholdValue;

    severity = severityLevel;

    matchKeyword = keyword;

    windowSeconds = windowSecs;
}

bool Rule::matches(
    const Event& event) const
{
    // Event type must match
    if (event.getEventType()
        != eventType)
    {
        return false;
    }

    // If there is no keyword,
    // event type is enough
    if (matchKeyword.empty())
    {
        return true;
    }

    // Keyword must exist
    // in event description
    return event.getDescription()
        .find(matchKeyword)
        != string::npos;
}

int Rule::getRuleID() const
{
    return ruleID;
}

string Rule::getRuleName() const
{
    return ruleName;
}

string Rule::getEventType() const
{
    return eventType;
}

int Rule::getThreshold() const
{
    return threshold;
}

string Rule::getSeverity() const
{
    return severity;
}

int Rule::getWindowSeconds() const
{
    return windowSeconds;
}