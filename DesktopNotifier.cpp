
// #include "DesktopNotifier.h"
// #include <shellapi.h>

// DesktopNotifier::DesktopNotifier(HWND windowHandle)
//     : hwnd(windowHandle)
// {
// }

// bool DesktopNotifier::initialize()
// {
//     NOTIFYICONDATAW data = {};

//     data.cbSize = sizeof(data);
//     data.hWnd = hwnd;
//     data.uID = 1;
//     data.uFlags = NIF_ICON | NIF_TIP;
//     data.hIcon = LoadIcon(nullptr, IDI_APPLICATION);

//     wcscpy_s(data.szTip, L"HIDS Security Monitor");

//     iconRegistered =
//         Shell_NotifyIconW(NIM_ADD, &data) == TRUE;

//     return iconRegistered;
// }

// void DesktopNotifier::sendAlert(
//     const std::wstring& title,
//     const std::wstring& message,
//     DWORD severity)
// {
//     if (!iconRegistered)
//         return;

//     NOTIFYICONDATAW data = {};

//     data.cbSize = sizeof(data);
//     data.hWnd = hwnd;
//     data.uID = 1;
//     data.uFlags = NIF_INFO;
//     data.dwInfoFlags = severity;
//     data.uTimeout = 10000;

//     wcscpy_s(data.szInfoTitle, title.c_str());
//     wcscpy_s(data.szInfo, message.c_str());

//     Shell_NotifyIconW(NIM_MODIFY, &data);
// }

// void DesktopNotifier::shutdown()
// {
//     if (iconRegistered)
//     {
//         NOTIFYICONDATAW data = {};

//         data.cbSize = sizeof(data);
//         data.hWnd = hwnd;
//         data.uID = 1;

//         Shell_NotifyIconW(NIM_DELETE, &data);

//         iconRegistered = false;
//     }
// }

// DesktopNotifier::~DesktopNotifier()
// {
//     shutdown();
// // }


// #include "DesktopNotifier.h"
 
// #include <shellapi.h>
// #include <wchar.h>
 
// DesktopNotifier::DesktopNotifier(HWND windowHandle)
//     : hwnd(windowHandle)
// {
// }
 
// bool DesktopNotifier::initialize()
// {
//     NOTIFYICONDATAW data = {};
 
//     data.cbSize = sizeof(data);
//     data.hWnd = hwnd;
//     data.uID = 1;
//     data.uFlags = NIF_ICON | NIF_TIP;
//     data.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
 
//     wcsncpy_s(data.szTip, ARRAYSIZE(data.szTip),
//               L"HIDS Security Monitor", _TRUNCATE);
 
//     iconRegistered =
//         Shell_NotifyIconW(NIM_ADD, &data) == TRUE;
 
//     return iconRegistered;
// }
 
// void DesktopNotifier::sendAlert(
//     const std::wstring& title,
//     const std::wstring& message,
//     DWORD severity)
// {
//     if (!iconRegistered)
//         return;
 
//     NOTIFYICONDATAW data = {};
 
//     data.cbSize = sizeof(data);
//     data.hWnd = hwnd;
//     data.uID = 1;
//     data.uFlags = NIF_INFO;
//     data.dwInfoFlags = severity;
//     data.uTimeout = 10000;
 
//     // _TRUNCATE: long alert text is cut instead of crashing the app
//     wcsncpy_s(data.szInfoTitle, ARRAYSIZE(data.szInfoTitle),
//               title.c_str(), _TRUNCATE);
//     wcsncpy_s(data.szInfo, ARRAYSIZE(data.szInfo),
//               message.c_str(), _TRUNCATE);
 
//     Shell_NotifyIconW(NIM_MODIFY, &data);
// }
 
// void DesktopNotifier::shutdown()
// {
//     if (iconRegistered)
//     {
//         NOTIFYICONDATAW data = {};
 
//         data.cbSize = sizeof(data);
//         data.hWnd = hwnd;
//         data.uID = 1;
 
//         Shell_NotifyIconW(NIM_DELETE, &data);
 
//         iconRegistered = false;
//     }
// }
 
// DesktopNotifier::~DesktopNotifier()
// {
//     shutdown();
// }


#include "DesktopNotifier.h"

#include <shellapi.h>
#include <wchar.h>

DesktopNotifier::DesktopNotifier(HWND windowHandle)
    : hwnd(windowHandle)
{
}

bool DesktopNotifier::initialize()
{
    if (hwnd == nullptr)
        return false;

    NOTIFYICONDATAW data = {};

    data.cbSize = sizeof(data);
    data.hWnd = hwnd;
    data.uID = 1;

    data.uFlags = NIF_ICON | NIF_TIP;

    data.hIcon = LoadIcon(nullptr, IDI_APPLICATION);

    wcsncpy_s(
        data.szTip,
        ARRAYSIZE(data.szTip),
        L"HIDS Security Monitor",
        _TRUNCATE
    );

    iconRegistered =
        Shell_NotifyIconW(NIM_ADD, &data) == TRUE;

    return iconRegistered;
}

void DesktopNotifier::sendAlert(
    const std::wstring& title,
    const std::wstring& message,
    DWORD severity
)
{
    if (!iconRegistered)
        return;

    NOTIFYICONDATAW data = {};

    data.cbSize = sizeof(data);
    data.hWnd = hwnd;
    data.uID = 1;

    data.uFlags = NIF_INFO;

    data.dwInfoFlags = severity;
    data.uTimeout = 10000;

    wcsncpy_s(
        data.szInfoTitle,
        ARRAYSIZE(data.szInfoTitle),
        title.c_str(),
        _TRUNCATE
    );

    wcsncpy_s(
        data.szInfo,
        ARRAYSIZE(data.szInfo),
        message.c_str(),
        _TRUNCATE
    );

    Shell_NotifyIconW(NIM_MODIFY, &data);
}

void DesktopNotifier::shutdown()
{
    if (iconRegistered)
    {
        NOTIFYICONDATAW data = {};

        data.cbSize = sizeof(data);
        data.hWnd = hwnd;
        data.uID = 1;

        Shell_NotifyIconW(NIM_DELETE, &data);

        iconRegistered = false;
    }
}

DesktopNotifier::~DesktopNotifier()
{
    shutdown();
}