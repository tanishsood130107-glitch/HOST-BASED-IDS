#ifndef LOGINMONITOR_H
#define LOGINMONITOR_H

#include "Event.h"

#include <string>

class LoginMonitor
{
private:

    unsigned long long lastRecordID;

    bool initialized;

    bool getLatestEvent(std::string& xml,unsigned long long& recordID);

public:

    LoginMonitor();

    bool checkForLoginFailure(Event& event);
};

#endif