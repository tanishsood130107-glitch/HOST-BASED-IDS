#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <ctime>

class Event
{
private:
    int eventID;
    std::string eventType;
    std::string description;
    std::string timestamp;
    std::time_t epochSeconds;

public:
    Event();

    Event(
        int id,
        std::string type,
        std::string desc,
        std::string time
    );

    int getEventID() const;
    std::string getEventType() const;
    std::string getDescription() const;
    std::string getTimestamp() const;
    std::time_t getEpochSeconds() const;

    void display() const;
};

#endif
