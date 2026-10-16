#include "EventFilter.h"
#include "Event.h"
#include <iostream>
#include <string>

using namespace std;

bool EventFilter::isRelevant(const Event& event) const


{
    string type = event.getEventType();

    if (type == "LOGIN_FAILURE")
    {
        cout<< "[EventFilter] Relevant login event."<< endl; 
        return true;
    }

    if (type == "PROCESS_START")
    {
        cout<< "[EventFilter] Relevant process event."<< endl;

        return true;
    }

    if (type == "FILE_CREATED")
    {
        cout<< "[EventFilter] Relevant file creation event."<< endl;

        return true;
    }

    if (type == "FILE_MODIFIED")
    {
        cout<< "[EventFilter] Relevant file modification event."<< endl;

        return true;
    }

    if (type == "FILE_DELETED")
    {
        cout<< "[EventFilter] Relevant file deletion event."<< endl;

        return true;
    }

    cout<< "[EventFilter] Event ignored."<< endl;

    return false;
}