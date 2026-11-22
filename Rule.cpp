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
    
    if (event.getEventType()
        != eventType)
    {
        return false;
    }

    
    if (matchKeyword.empty())
    {
        return true;
    }

    
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
