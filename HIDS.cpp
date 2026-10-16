#include "HIDS.h"

#include <iostream>

HIDS::HIDS()
{
}

void HIDS::addEvent(
    const Event& event)
{
    collector.addEvent(event);
}

void HIDS::processEvents()
{
    while (collector.hasEvents())
    {
        Event event =
            collector.getNextEvent();

        std::cout
            << "[HIDS] Processing event: "
            << event.getEventType()
            << std::endl;

        if (filter.isRelevant(event))
        {
            std::string matchedRule;

            std::string severity;

            if (ruleEngine.checkEvent(
                    event,
                    matchedRule,
                    severity))
            {
                std::cout
                    << "[HIDS] Alert triggered."
                    << std::endl;
            }
        }
    }
}