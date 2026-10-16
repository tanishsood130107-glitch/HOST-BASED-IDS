#ifndef RULE_H
#define RULE_H

#include "Event.h"

#include <string>

class Rule
{
private:
    int ruleID;

    std::string ruleName;

    std::string eventType;

    int threshold;

    std::string severity;

    std::string matchKeyword;

    int windowSeconds;

public:

    Rule(
        int id,
        std::string name,
        std::string type,
        int thresholdValue,
        std::string severityLevel,
        std::string keyword = "",
        int windowSecs = 60
    );

    bool matches(
        const Event& event
    ) const;

    int getRuleID() const;

    std::string getRuleName() const;

    std::string getEventType() const;

    int getThreshold() const;

    std::string getSeverity() const;

    int getWindowSeconds() const;
};

#endif