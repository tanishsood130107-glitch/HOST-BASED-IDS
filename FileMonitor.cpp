


#include "FileMonitor.h"

#include <filesystem>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;

FileMonitor::FileMonitor(
    const std::string& directory)
{
    monitoredDirectory =
        directory;

    initialized = false;
}

bool FileMonitor::checkForFileChange(
    Event& event)
{
    try
    {
       

        if (!fs::exists(
                monitoredDirectory))
        {
            fs::create_directories(
                monitoredDirectory
            );

            std::cout
                << "[FileMonitor] Directory created: "
                << monitoredDirectory
                << std::endl;

            return false;
        }

        std::unordered_map<
            std::string,
            long long
        > currentFiles;

        
        for (const auto& entry :
             fs::directory_iterator(
                 monitoredDirectory))
        {
            if (!entry.is_regular_file())
            {
                continue;
            }

            std::string filePath =
                entry.path().string();

            auto lastWriteTime =
                fs::last_write_time(
                    entry
                );

            long long timeValue =
                lastWriteTime
                    .time_since_epoch()
                    .count();

            currentFiles[filePath] =
                timeValue;
        }


        if (!initialized)
        {
            previousFiles =
                currentFiles;

            initialized = true;

            std::cout
                << "[FileMonitor] Initial file scan completed."
                << std::endl;

            return false;
        }

        
        for (const auto& currentFile :
             currentFiles)
        {
            const std::string& filePath =
                currentFile.first;

            long long currentTime =
                currentFile.second;

            

            if (previousFiles.find(
                    filePath)
                ==
                previousFiles.end())
            {
                std::stringstream description;

                description
                    << "New file created: "
                    << filePath;

                event = Event(
                    1,
                    "FILE_CREATED",
                    description.str(),
                    "CURRENT_TIME"
                );

                previousFiles =
                    currentFiles;

                std::cout
                    << "\n[FileMonitor] NEW FILE DETECTED!"
                    << std::endl;

                std::cout
                    << "File: "
                    << filePath
                    << std::endl;

                return true;
            }

           
            if (previousFiles[filePath]
                != currentTime)
            {
                std::stringstream description;

                description
                    << "File modified: "
                    << filePath;

                event = Event(
                    2,
                    "FILE_MODIFIED",
                    description.str(),
                    "CURRENT_TIME"
                );

                previousFiles =
                    currentFiles;

                std::cout
                    << "\n[FileMonitor] FILE MODIFIED!"
                    << std::endl;

                std::cout
                    << "File: "
                    << filePath
                    << std::endl;

                return true;
            }
        }

       

        for (const auto& oldFile :
             previousFiles)
        {
            if (currentFiles.find(
                    oldFile.first)
                ==
                currentFiles.end())
            {
                std::stringstream description;

                description
                    << "File deleted: "
                    << oldFile.first;

                event = Event(
                    3,
                    "FILE_DELETED",
                    description.str(),
                    "CURRENT_TIME"
                );

                previousFiles =
                    currentFiles;

                std::cout
                    << "\n[FileMonitor] FILE DELETED!"
                    << std::endl;

                std::cout
                    << "File: "
                    << oldFile.first
                    << std::endl;

                return true;
            }
        }

       
        previousFiles =
            currentFiles;
    }
    catch (const std::exception& e)
    {
        std::cerr
            << "[FileMonitor] Error: "
            << e.what()
            << std::endl;
    }

    return false;
}