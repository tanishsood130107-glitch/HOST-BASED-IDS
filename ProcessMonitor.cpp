#include "ProcessMonitor.h"
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <sstream>

ProcessMonitor::ProcessMonitor(): initialized(false)
{

}

bool ProcessMonitor::checkForNewProcess(Event& event)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot == INVALID_HANDLE_VALUE)
    {
        std::cout << "Failed to create process snapshot.\n";
        return false;
    }

    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32);

    std::unordered_set<unsigned long> currentProcessIDs;

    if (Process32First(snapshot, &processEntry))
    {
        do
        {
            unsigned long pid = processEntry.th32ProcessID;

            currentProcessIDs.insert(pid);

            if (initialized &&previousProcessIDs.find(pid) == previousProcessIDs.end())
            {
                std::string processName;

#ifdef UNICODE
                std::wstring wideName(processEntry.szExeFile);
                processName = std::string(wideName.begin(),wideName.end());
#else
                processName = processEntry.szExeFile;
#endif

                std::stringstream description;

                description<< "New process detected: "<< processName<< " (PID: "<< pid<< ")";

                event = Event(pid,"PROCESS_START",description.str(),"CURRENT_TIME");

                previousProcessIDs = currentProcessIDs;

                std::cout<< "Process detected: "<< processName<< " (PID: "<< pid<< ")\n";

                CloseHandle(snapshot);
                return true;
            }

        } while (Process32Next(snapshot, &processEntry));
    }

    CloseHandle(snapshot);

    if (!initialized)
    {
        previousProcessIDs = currentProcessIDs;
        initialized = true;

        std::cout<< "Initial process baseline created. "<< currentProcessIDs.size()<< " processes recorded.\n";

        return false;
    }

    previousProcessIDs = currentProcessIDs;

    return false;
}