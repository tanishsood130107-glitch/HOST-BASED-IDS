#ifndef EVENTCOLLECTOR_H
#define EVENTCOLLECTOR_H

#include "Event.h"
#include <queue>

class EventCollector
{
private:
    std::queue<Event> eventQueue;

public:
    void addEvent(
        const Event& event
    );

    bool hasEvents() const;

    Event getNextEvent();

    int getEventCount() const;
};

#endif