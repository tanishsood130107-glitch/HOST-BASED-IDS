
#ifndef RULEENGINE_H
#define RULEENGINE_H

#include "Rule.h"

#include <string>
#include <vector>
#include <ctime>
#include <deque>

class RuleEngine
{
private:

    std::vector<Rule> rules;

    std::deque<std::time_t>
        failedLoginTimes;

public:

    RuleEngine();

    void addRule(
        const Rule& rule
    );

    bool checkEvent(
        const Event& event,
        std::string& matchedRule,
        std::string& severity
    );
};

#endif