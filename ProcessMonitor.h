#ifndef PROCESSMONITOR_H
#define PROCESSMONITOR_H

#include "Event.h"

#include <string>
#include <unordered_set>

class ProcessMonitor
{
private:

    std::unordered_set<unsigned long>previousProcessIDs;
    bool initialized;

public:

    ProcessMonitor();

    bool checkForNewProcess(Event& event);
};

#endif