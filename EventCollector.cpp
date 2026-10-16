// #include "EventCollector.h"
// #include <iostream>

// using namespace std;

// // Add event to queue
// void EventCollector::addEvent(const Event& event)
// {
//     eventQueue.push(event);

//     cout << "[EventCollector] Event received." << endl;
// }

// // Check whether queue contains events
// bool EventCollector::hasEvents() const
// {
//     return !eventQueue.empty();
// }

// // Get the next event
// Event EventCollector::getNextEvent()
// {
//     if (eventQueue.empty())
//     {
//         return Event();
//     }

//     Event event = eventQueue.front();
//     eventQueue.pop();

//     return event;
// }

// // Get number of events
// int EventCollector::getEventCount() const
// {
//     return static_cast<int>(eventQueue.size());
// }

#include "EventCollector.h"

void EventCollector::addEvent(
    const Event& event)
{
    eventQueue.push(event);
}

bool EventCollector::hasEvents() const
{
    return !eventQueue.empty();
}

Event EventCollector::getNextEvent()
{
    Event event =
        eventQueue.front();

    eventQueue.pop();

    return event;
}

int EventCollector::getEventCount() const
{
    return static_cast<int>(
        eventQueue.size()
    );
}