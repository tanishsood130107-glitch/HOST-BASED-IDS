// #ifndef FILEMONITOR_H
// #define FILEMONITOR_H

// #include "Event.h"
// #include <string>
// #include <unordered_map>

// class FileMonitor
// {
// private:
//     std::string monitoredDirectory;

//     // Stores file path and its last modification time
//     std::unordered_map<std::string, long long> previousFiles;

//     bool initialized;

// public:
//     FileMonitor(const std::string& directory);

//     bool checkForFileChange(Event& event);
// };

// #endif

#ifndef FILEMONITOR_H
#define FILEMONITOR_H

#include "Event.h"

#include <string>
#include <unordered_map>

class FileMonitor
{
private:

    std::string monitoredDirectory;

    std::unordered_map<
        std::string,
        long long
    > previousFiles;

    bool initialized;

public:

    FileMonitor(
        const std::string& directory
    );

    bool checkForFileChange(
        Event& event
    );
};

#endif