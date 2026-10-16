#ifndef EVENTFILTER_H
#define EVENTFILTER_H

class Event;

class EventFilter
{
public:

    bool isRelevant(const Event &event) const;

};

#endif