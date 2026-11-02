#include "EventCollector.h"

void EventCollector::addEvent(const Event& event)
{
    eventQueue.push(event);
}

bool EventCollector::hasEvents() const
{
    return !eventQueue.empty();
}

Event EventCollector::getNextEvent()
{
    Event event = eventQueue.front();

    eventQueue.pop();

    return event;
}

int EventCollector::getEventCount() const
{
    return static_cast<int>(eventQueue.size());
}