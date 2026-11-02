// #pragma once

// #include <windows.h>
// #include <shellapi.h>      // NIIF_* flags (not included by WIN32_LEAN_AND_MEAN)
// #include <string>

// class DesktopNotifier {
// private:
//     HWND hwnd;
//     bool iconRegistered = false;

// public:
//     explicit DesktopNotifier(HWND windowHandle);

//     bool initialize();
//     void sendAlert(const std::wstring& title,
//                    const std::wstring& message,
//                    DWORD severity = NIIF_WARNING);
//     void shutdown();

//     ~DesktopNotifier();
// };

// #pragma once
 
// #include <windows.h>
// #include <shellapi.h>      // NIIF_* flags (not pulled in by WIN32_LEAN_AND_MEAN)
// #include <string>
 
// class DesktopNotifier {
// private:
//     HWND hwnd;
//     bool iconRegistered = false;
 
// public:
//     explicit DesktopNotifier(HWND windowHandle);
 
//     bool initialize();
//     void sendAlert(
//         const std::wstring& title,
//         const std::wstring& message,
//         DWORD severity = NIIF_WARNING
//     );
 
//     void shutdown();
 
//     ~DesktopNotifier();
// };
 
#pragma once

#ifndef DESKTOP_NOTIFIER_H
#define DESKTOP_NOTIFIER_H

#include <windows.h>
#include <shellapi.h>
#include <string>

class DesktopNotifier
{
private:
    HWND hwnd;
    bool iconRegistered = false;

public:
    explicit DesktopNotifier(HWND windowHandle);

    bool initialize();

    void sendAlert(
        const std::wstring& title,
        const std::wstring& message,
        DWORD severity = NIIF_WARNING
    );

    void shutdown();

    ~DesktopNotifier();
};

#endif