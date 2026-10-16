#include "Event.h"
#include <iostream>
#include <ctime>
#include <string>

static std::string formatTime(std::time_t t)
{
    char buffer[32];

    std::tm* local =
        std::localtime(&t);

    std::strftime(
        buffer,
        sizeof(buffer),
        "%Y-%m-%d %H:%M:%S",
        local
    );

    return std::string(buffer);
}

Event::Event()   // default constroctor of the class event 
{
    eventID = 0;
    eventType = "";
    description = "";
    timestamp = "";
    epochSeconds = 0;
}

Event::Event(int id,std::string type,std::string desc,std::string time) // parameterised constructor of class event 
{
    eventID = id;
    eventType = type;
    description = desc;

    epochSeconds =
        std::time(nullptr);

    if (time == "CURRENT_TIME")
    {
        timestamp =
            formatTime(epochSeconds);
    }
    else
    {
        timestamp = time;
    }
}

int Event::getEventID() const
{
    return eventID;
}

std::string Event::getEventType() const
{
    return eventType;
}

std::string Event::getDescription() const
{
    return description;
}

std::string Event::getTimestamp() const
{
    return timestamp;
}

std::time_t Event::getEpochSeconds() const
{
    return epochSeconds;
}

void Event::display() const
{
    std::cout<< "Event ID   : "<< eventID<< std::endl;

    std::cout<< "Event Type : "<< eventType<< std::endl;

    std::cout<< "Description: "<< description<< std::endl;

    std::cout<< "Timestamp  : "<< timestamp<< std::endl;
}