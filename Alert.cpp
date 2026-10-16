#include "Alert.h"

#include <ctime>
#include <iostream>

static std::string CurrentTimestamp()
{
    const std::time_t now = std::time(nullptr);

    char buffer[32];

    std::strftime(
        buffer,
        sizeof(buffer),
        "%Y-%m-%d %H:%M:%S",
        std::localtime(&now)
    );

    return std::string(buffer);
}

Alert::Alert(
    int id,
    std::string severityLevel,
    std::string rule,
    std::string msg,
    std::string alertSource,
    std::string alertTimestamp)
{
    alertID   = id;
    severity  = severityLevel;
    ruleName  = rule;
    message   = msg;
    source    = alertSource;

    timestamp = alertTimestamp.empty()
        ? CurrentTimestamp()
        : alertTimestamp;
}

int Alert::getID() const                { return alertID; }
std::string Alert::getSeverity() const  { return severity; }
std::string Alert::getRuleName() const  { return ruleName; }
std::string Alert::getMessage() const   { return message; }
std::string Alert::getSource() const    { return source; }
std::string Alert::getTimestamp() const { return timestamp; }

void Alert::display() const
{
    std::cout << "\n================================\n"
              << "          SECURITY ALERT\n"
              << "================================\n"
              << "Alert ID : " << alertID   << "\n"
              << "Time     : " << timestamp << "\n"
              << "Severity : " << severity  << "\n"
              << "Source   : " << source    << "\n"
              << "Rule     : " << ruleName  << "\n"
              << "Message  : " << message   << "\n"
              << "================================"
              << std::endl;
}
