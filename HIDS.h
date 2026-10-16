#ifndef HIDS_H
#define HIDS_H

#include "EventCollector.h"
#include "EventFilter.h"
#include "RuleEngine.h"

class HIDS
{
private:

    EventCollector collector;

    EventFilter filter;

    RuleEngine ruleEngine;

public:

    HIDS();

    void addEvent(
        const Event& event
    );

    void processEvents();
};

#endif