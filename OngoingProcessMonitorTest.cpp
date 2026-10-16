
#include "OngoingProcessMonitor.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

static std::string csvEscape(const std::string& value) {
    std::string result = "\"";

    for (char c : value) {
        if (c == '"') {
            result += "\"\"";
        } else {
            result += c;
        }
    }

    result += '"';
    return result;
}

static bool saveCSV(
    const std::string& filename,
    const std::vector<OngoingProcessInfo>& processes) {

    std::ofstream file(filename);

    if (!file.is_open()) {
        return false;
    }

    file << "Process Name,PID,RAM MB,CPU Percent,"
            "Running Minutes,Executable Path\n";

    for (const auto& p : processes) {
        file << csvEscape(p.name) << ','
             << p.pid << ',';

        if (p.memoryAvailable) {
            file << p.memoryMB;
        }

        file << ',';

        if (p.cpuAvailable) {
            file << p.cpuPercent;
        }

        file << ',';

        if (p.startTimeAvailable) {
            file << p.runningSeconds / 60.0;
        }

        file << ','
             << csvEscape(p.executablePath)
             << '\n';
    }

    return file.good();
}

int main() {
    OngoingProcessMonitor monitor;

    while (true) {
        auto processes = monitor.collectProcesses();

        const bool saved =
            saveCSV("processes_report.csv", processes);

        auto displayList = processes;

        std::sort(
            displayList.begin(),
            displayList.end(),
            [](const OngoingProcessInfo& a,
               const OngoingProcessInfo& b) {
                if (a.memoryAvailable != b.memoryAvailable) {
                    return a.memoryAvailable;
                }

                return a.memoryMB > b.memoryMB;
            });

        std::system("cls");

        std::cout << "============================================================\n";
        std::cout << "              HIDS ONGOING PROCESS MONITOR\n";
        std::cout << "============================================================\n";

        std::cout << "Processes found: "
                  << processes.size() << '\n';

        std::cout << "Showing top 20 by RAM usage\n\n";

        std::cout << std::left
                  << std::setw(28) << "Process"
                  << std::setw(10) << "PID"
                  << std::setw(12) << "RAM MB"
                  << std::setw(12) << "CPU %"
                  << "Running\n";

        std::cout << std::string(75, '-') << '\n';

        const size_t limit =
            std::min<size_t>(20, displayList.size());

        for (size_t i = 0; i < limit; ++i) {
            const auto& p = displayList[i];

            std::cout << std::left
                      << std::setw(28) << p.name
                      << std::setw(10) << p.pid;

            if (p.memoryAvailable) {
                std::cout << std::setw(12)
                          << std::fixed << std::setprecision(1)
                          << p.memoryMB;
            } else {
                std::cout << std::setw(12) << "N/A";
            }

            if (p.cpuAvailable) {
                std::cout << std::setw(12)
                          << std::fixed << std::setprecision(1)
                          << p.cpuPercent;
            } else {
                std::cout << std::setw(12) << "Sampling";
            }

            if (p.startTimeAvailable) {
                const auto totalMinutes =
                    static_cast<unsigned long long>(
                        p.runningSeconds / 60.0);

                std::cout << totalMinutes / 60 << "h "
                          << totalMinutes % 60 << "m";
            } else {
                std::cout << "N/A";
            }

            std::cout << '\n';
        }

        std::cout << "\nFull report: processes_report.csv";
        std::cout << "\nCSV status: "
                  << (saved ? "Saved" : "Could not save");
        std::cout << "\nRefresh interval: 2 seconds";
        std::cout << "\nPress Ctrl+C to stop.\n";

        Sleep(2000);
    }

    return 0;
}
