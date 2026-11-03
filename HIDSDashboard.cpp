

// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)
// #include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"
// #include <GLFW/glfw3.h>
// <<<<<<< HEAD
// =======
 
// // Native window handle (needed for the tray icon / desktop notifications)
// #ifdef _WIN32
// #include <windows.h>
// #define GLFW_EXPOSE_NATIVE_WIN32
// #include <GLFW/glfw3native.h>
// #endif
 
// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)
// #include <algorithm>
// #include <cctype>
// #include <cfloat>
// #include <cmath>
// #include <cstdio>
// #include <cstdlib>
// #include <cstring>
// #include <set>
// #include <string>
// #include <vector>
// #include "HIDSBackend.h"
// <<<<<<< HEAD

// =======
// #include "DashboardTheme.h"   // themes, colours, icons, Settings page
 
// // ------------------------------------------------------------
// // DATA STRUCTURES
// // ------------------------------------------------------------
 
// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)
// struct ProcessInfo {
//     int pid;
//     std::string name;
//     float cpu;
//     float memory;
//     double runningSeconds;
//     std::string runningTime;   // formatted for display
//     bool isApp = false;        // can be ended from the Processes page
//     std::string title;         // window title (apps only)
// };
 
// struct AlertInfo {
//     int id = 0;
//     std::string time;
//     std::string severity;
//     std::string source;
//     std::string description;
 
//     bool        canEndTask = false;   // resource alert with a culprit app
//     int         culpritPid = 0;
//     std::string culpritName;
//     float       culpritMem = 0.0f;
// };
 
// struct EventInfo {
//     std::string time;
//     std::string type;
//     std::string description;
// };

// static HIDSBackend backend;
 
// static std::vector<ProcessInfo> processes;
// static std::vector<AlertInfo>   alerts;
// static std::vector<EventInfo>   loginEvents;
// static std::vector<EventInfo>   fileEvents;
// static BackendAnalysis          analysisData;
// <<<<<<< HEAD

// =======
 
// // File scanner page state
// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)
// static ScanStatus               scanStatus;
// static std::vector<ScanFinding> scanFindings;
// static int                      scanTypeSel = 0;      // 0 Quick, 1 Full, 2 Custom
// static char                     scanPath[512] = "";
// static std::string              scanMessage;
// static int processCount = 0;

// static int  selectedPage = 0;
// static char processSearch[128] = "";
// static bool monitoringActive = true;
 
// static int  severityFilter = 0;   // 0 = All, 1..4 = Critical..Low
// static const char* severityFilterLabels[] =
//     { "All", "Critical", "High", "Medium", "Low" };
// static char alertSearch[128] = "";
 
// static const char* pages[] = {
//     "Dashboard",
//     "Processes",
//     "Login Monitoring",
//     "File Monitoring",
//     "Alerts",
//     "Logs & Reports",
//     "System Analysis",
//     "File Scanner",
//     "Settings"
// };
 
// static const char* pageSubtitles[] = {
//     "Overview of this machine's security status",
//     "Everything currently running on this host",
//     "Sign-in activity on this host",
//     "Changes to monitored files and folders",
//     "Events raised by the HIDS monitors",
//     "Detection signatures and scan reports",
//     "Deeper inspection of system behaviour",
//     "Scan files for known malware and suspicious traits",
//     "Appearance, interface size, notifications and data source"
// };
 
// // Live metrics (ring buffers feed the sparklines)
// static const int HISTORY_SIZE = 60;
// static float cpuHistory[HISTORY_SIZE] = {};
// static float ramHistory[HISTORY_SIZE] = {};
// static int   historyOffset = 0;
// static float cpuNow = 0.0f;
// static float ramNow = 0.0f;
// <<<<<<< HEAD

// static const ImVec4 COLOR_BACKGROUND = ImVec4(0.025f, 0.045f, 0.065f, 1.0f);
// static const ImVec4 COLOR_SIDEBAR    = ImVec4(0.020f, 0.036f, 0.054f, 1.0f);
// static const ImVec4 COLOR_PANEL      = ImVec4(0.035f, 0.065f, 0.090f, 1.0f);
// static const ImVec4 COLOR_BORDER     = ImVec4(0.09f, 0.20f, 0.28f, 1.0f);
// static const ImVec4 COLOR_BLUE       = ImVec4(0.10f, 0.55f, 0.95f, 1.0f);
// static const ImVec4 COLOR_CYAN       = ImVec4(0.0f, 0.75f, 1.0f, 1.0f);
// static const ImVec4 COLOR_GREEN      = ImVec4(0.0f, 0.85f, 0.55f, 1.0f);
// static const ImVec4 COLOR_RED        = ImVec4(1.0f, 0.25f, 0.30f, 1.0f);
// static const ImVec4 COLOR_ORANGE     = ImVec4(1.0f, 0.50f, 0.15f, 1.0f);
// static const ImVec4 COLOR_YELLOW     = ImVec4(1.0f, 0.78f, 0.15f, 1.0f);
// static const ImVec4 COLOR_TEXT       = ImVec4(0.88f, 0.93f, 0.97f, 1.0f);
// static const ImVec4 COLOR_MUTED      = ImVec4(0.50f, 0.59f, 0.66f, 1.0f);
// =======
 
// // Threat activity (feeds the dashboard histogram)
// static float threatHistory[HISTORY_SIZE] = {};
// static float threatAccum = 0.0f;
// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)
 
// static float ClampFloat(float v, float lo, float hi)
// {
//     return v < lo ? lo : (v > hi ? hi : v);
// }
 
// <<<<<<< HEAD
// static ImVec4 WithAlpha(const ImVec4& c, float a)
// {
//     return ImVec4(c.x, c.y, c.z, a);
// }
 
// static void SetupStyle()
// {
//     ImGui::StyleColorsDark();
 
//     ImGuiStyle& style = ImGui::GetStyle();
 
//     style.WindowPadding    = ImVec2(16.0f, 16.0f);
//     style.FramePadding     = ImVec2(10.0f, 7.0f);
//     style.ItemSpacing      = ImVec2(10.0f, 10.0f);
//     style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);
//     style.CellPadding      = ImVec2(10.0f, 8.0f);
//     style.ScrollbarSize    = 12.0f;
 
//     style.WindowRounding    = 0.0f;
//     style.ChildRounding     = 10.0f;
//     style.FrameRounding     = 6.0f;
//     style.PopupRounding     = 6.0f;
//     style.ScrollbarRounding = 8.0f;
//     style.GrabRounding      = 6.0f;
//     style.TabRounding       = 6.0f;
 
//     style.WindowBorderSize = 0.0f;
//     style.ChildBorderSize  = 1.0f;
//     style.FrameBorderSize  = 0.0f;
 
//     ImVec4* colors = style.Colors;
 
//     colors[ImGuiCol_WindowBg] = COLOR_BACKGROUND;
//     colors[ImGuiCol_ChildBg]  = COLOR_PANEL;
//     colors[ImGuiCol_PopupBg]  = COLOR_PANEL;
//     colors[ImGuiCol_Border]   = COLOR_BORDER;
 
//     colors[ImGuiCol_Text]         = COLOR_TEXT;
//     colors[ImGuiCol_TextDisabled] = COLOR_MUTED;
 
//     colors[ImGuiCol_FrameBg]        = ImVec4(0.07f, 0.12f, 0.17f, 1.0f);
//     colors[ImGuiCol_FrameBgHovered] = ImVec4(0.10f, 0.22f, 0.31f, 1.0f);
//     colors[ImGuiCol_FrameBgActive]  = ImVec4(0.10f, 0.28f, 0.40f, 1.0f);
 
//     colors[ImGuiCol_Button]        = ImVec4(0.06f, 0.28f, 0.43f, 1.0f);
//     colors[ImGuiCol_ButtonHovered] = ImVec4(0.08f, 0.42f, 0.62f, 1.0f);
//     colors[ImGuiCol_ButtonActive]  = COLOR_BLUE;
 
//     colors[ImGuiCol_Header]        = ImVec4(0.05f, 0.23f, 0.35f, 0.70f);
//     colors[ImGuiCol_HeaderHovered] = ImVec4(0.08f, 0.34f, 0.48f, 0.80f);
//     colors[ImGuiCol_HeaderActive]  = COLOR_BLUE;
 
//     colors[ImGuiCol_TableHeaderBg]      = ImVec4(0.06f, 0.12f, 0.18f, 1.0f);
//     colors[ImGuiCol_TableBorderStrong]  = COLOR_BORDER;
//     colors[ImGuiCol_TableBorderLight]   = ImVec4(0.08f, 0.14f, 0.19f, 1.0f);
//     colors[ImGuiCol_TableRowBg]         = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
//     colors[ImGuiCol_TableRowBgAlt]      = ImVec4(1.0f, 1.0f, 1.0f, 0.025f);
 
//     colors[ImGuiCol_Separator]       = COLOR_BORDER;
//     colors[ImGuiCol_ScrollbarBg]     = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
//     colors[ImGuiCol_ScrollbarGrab]   = ImVec4(0.15f, 0.28f, 0.38f, 1.0f);
//     colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.20f, 0.38f, 0.50f, 1.0f);
 
//     colors[ImGuiCol_CheckMark]        = COLOR_GREEN;
//     colors[ImGuiCol_SliderGrab]       = COLOR_BLUE;
//     colors[ImGuiCol_SliderGrabActive] = COLOR_CYAN;
 
//     colors[ImGuiCol_PlotLines]     = COLOR_CYAN;
//     colors[ImGuiCol_PlotHistogram] = COLOR_CYAN;
// }
// =======
// // ------------------------------------------------------------
// // FONT LOADING
// // Uses a real TTF if one is found; otherwise scales the default
// // bitmap font so the dashboard is still readable.
// // ------------------------------------------------------------
// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)
 
// static bool FileExists(const char* path)
// {
//     FILE* f = std::fopen(path, "rb");
//     if (f == nullptr)
//         return false;
 
//     std::fclose(f);
//     return true;
// }
 
// static void LoadFonts(ImGuiIO& io)
// {
//     const char* candidates[] = {
//         "C:/Windows/Fonts/segoeui.ttf",
//         "C:/Windows/Fonts/arial.ttf",
//         "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
//         "/usr/share/fonts/TTF/DejaVuSans.ttf",
//         "/System/Library/Fonts/Supplemental/Arial.ttf"
//     };
 
//     for (const char* path : candidates)
//     {
//         if (!FileExists(path))
//             continue;
 
//         ImFont* font = io.Fonts->AddFontFromFileTTF(path, 18.0f);
 
//         if (font != nullptr)
//         {
//             io.FontDefault = font;
//             io.FontGlobalScale = 1.0f;
//             return;
//         }
//     }
 
//     io.Fonts->AddFontDefault();
//     io.FontGlobalScale = 1.25f;
// }

// static bool ContainsNoCase(const char* text, const char* search)
// {
//     if (search[0] == '\0')
//         return true;
 
//     const size_t textLen = std::strlen(text);
//     const size_t searchLen = std::strlen(search);
 
//     if (searchLen > textLen)
//         return false;
 
//     for (size_t i = 0; i + searchLen <= textLen; ++i)
//     {
//         size_t j = 0;
 
//         while (j < searchLen &&
//                std::tolower((unsigned char)text[i + j]) ==
//                std::tolower((unsigned char)search[j]))
//         {
//             ++j;
//         }
 
//         if (j == searchLen)
//             return true;
//     }
 
//     return false;
// }
 
// static ImVec4 SeverityColor(const char* severity)
// {
//     if (std::strcmp(severity, "Critical") == 0) return COLOR_RED;
//     if (std::strcmp(severity, "High") == 0)     return COLOR_ORANGE;
//     if (std::strcmp(severity, "Medium") == 0)   return COLOR_YELLOW;
 
//     return COLOR_CYAN;
// }
 
// static int CountSeverity(const char* severity)
// {
//     int count = 0;
 
//     for (const auto& alert : alerts)
//         if (alert.severity == severity)
//             ++count;
 
//     return count;
// }
 
// // Usage colour: green -> yellow -> red as load rises
// static ImVec4 LoadColor(float percent)
// {
//     if (percent >= 85.0f) return COLOR_RED;
//     if (percent >= 65.0f) return COLOR_YELLOW;
 
//     return COLOR_GREEN;
// }
 
// static void SectionHeading(const char* title, const char* hint = nullptr)
// {
//     {
//         const ImVec2 p = ImGui::GetCursorScreenPos();
//         const float  h = ImGui::GetTextLineHeight() * 1.1f;
 
//         ImGui::GetWindowDrawList()->AddRectFilled(
//             ImVec2(p.x, p.y + 1.0f), ImVec2(p.x + 4.0f, p.y + h),
//             ImGui::GetColorU32(COLOR_CYAN), 2.0f);
 
//         ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 12.0f);
//     }
 
//     ImGui::PushStyleColor(ImGuiCol_Text, COLOR_CYAN);
//     ImGui::SetWindowFontScale(1.1f);
//     ImGui::TextUnformatted(title);
//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::PopStyleColor();
 
//     if (hint != nullptr)
//     {
//         ImGui::SameLine();
//         ImGui::TextDisabled("  %s", hint);
//     }
 
//     ImGui::Spacing();
// }
 
// // Coloured pill, e.g. for alert severity
// static void SeverityBadge(const char* severity)
// {
//     const ImVec4 color = SeverityColor(severity);
//     const ImVec2 textSize = ImGui::CalcTextSize(severity);
//     const ImVec2 pad(10.0f, 2.0f);
 
//     const ImVec2 pos = ImGui::GetCursorScreenPos();
//     const ImVec2 size(textSize.x + pad.x * 2.0f,textSize.y + pad.y * 2.0f);
 
//     ImDrawList* draw = ImGui::GetWindowDrawList();
 
//     draw->AddRectFilled(
//         pos,
//         ImVec2(pos.x + size.x, pos.y + size.y),
//         ImGui::GetColorU32(WithAlpha(color, 0.16f)),
//         size.y * 0.5f
//     );
 
//     draw->AddRect(
//         pos,
//         ImVec2(pos.x + size.x, pos.y + size.y),
//         ImGui::GetColorU32(WithAlpha(color, 0.65f)),
//         size.y * 0.5f
//     );
 
//     draw->AddText(
//         ImVec2(pos.x + pad.x, pos.y + pad.y),
//         ImGui::GetColorU32(color),
//         severity
//     );
 
//     ImGui::Dummy(size);
// }
 
// // Pulsing dot used by the monitoring status
// static void StatusDot(const ImVec4& color, bool pulse)
// {
//     const float radius = 5.0f;
//     const ImVec2 pos = ImGui::GetCursorScreenPos();
//     const ImVec2 center(pos.x + radius + 2.0f, pos.y + ImGui::GetFrameHeight() * 0.5f);
 
//     ImDrawList* draw = ImGui::GetWindowDrawList();
 
//     if (pulse)
//     {
//         const float t = (float)ImGui::GetTime();
//         const float wave = 0.5f + 0.5f * std::sin(t * 3.0f);
 
//         draw->AddCircleFilled(
//             center,
//             radius + 3.0f + wave * 3.0f,
//             ImGui::GetColorU32(WithAlpha(color, 0.18f * (1.0f - wave) + 0.05f))
//         );
//     }
 
//     draw->AddCircleFilled(center, radius, ImGui::GetColorU32(color));
 
//     ImGui::Dummy(ImVec2(radius * 2.0f + 6.0f, ImGui::GetFrameHeight()));
// }
 
// static std::string FormatDuration(double seconds)
// {
//     const unsigned long long total =
//         seconds > 0.0 ? (unsigned long long)seconds : 0ULL;
//         char text[32];
 
//     std::snprintf(
//         text, sizeof(text), "%02llu:%02llu:%02llu",
//         total / 3600ULL, (total / 60ULL) % 60ULL, total % 60ULL
//     );
//     return text;
// }
 
// static float SeverityWeight(const std::string& s)
// {
//     if (s == "Critical") return 4.0f;
//     if (s == "High")     return 3.0f;
//     if (s == "Medium")   return 2.0f;
//     return 1.0f;
// }
 
// // Sample rows. Always labelled so they can't be mistaken for real ones:
// // source "DEMO", "[DEMO]" prefix, negative IDs/PIDs, no End Task button.
// static void AppendDemoData()
// {
//     struct DemoAlert { const char* sev; const char* text; };
 
//     static const DemoAlert demoAlerts[] = {
//         { "Critical", "[DEMO] Known-bad hash match - C:\\Demo\\sample.exe" },
//         { "High",     "[DEMO] Multiple Failed Logins - 5 attempts in 60 s" },
//         { "Medium",   "[DEMO] Suspicious PowerShell Process - powershell.exe" },
//         { "Low",      "[DEMO] Password Changed - sample entry" }
//     };
 
//     int id = -1;
 
//     for (const DemoAlert& d : demoAlerts)
//     {
//         AlertInfo a;
//         a.id          = id--;
//         a.time        = "--:--:--";
//         a.severity    = d.sev;
//         a.source      = "DEMO";
//         a.description = d.text;
//         alerts.push_back(a);              // after the real ones
//     }
 
//     ProcessInfo p;
//     p.pid            = -1;
//     p.name           = "demo_service.exe [DEMO]";
//     p.cpu            = 12.5f;
//     p.memory         = 640.0f;
//     p.runningSeconds = 4000.0;
//     p.runningTime    = FormatDuration(p.runningSeconds);
//     p.isApp          = false;             // can never be ended
//     processes.push_back(p);
 
//     loginEvents.push_back({ "--:--:--", "LOGIN_FAILURE",
//         "[DEMO] Failed sign-in for user 'demo'" });
//     fileEvents.push_back({ "--:--:--", "FILE_CREATED",
//         "[DEMO] HIDS_Monitor\\sample_script.ps1" });
// }
 
// static void RefreshFromBackend()
// {
//     static double lastRefresh = -1.0;
//     static unsigned long lastSample = 0;
 
//     const double now = ImGui::GetTime();
 
//     if (lastRefresh >= 0.0 && now - lastRefresh < 0.5)
//         return;
 
//     lastRefresh = now;
 
//     // CPU, RAM and process count
//     const BackendStats stats = backend.getStats();
 
//     cpuNow = stats.cpuPercent;
//     ramNow = stats.ramPercent;
//     processCount = stats.processCount;
 
//     if (stats.sample != lastSample)
//     {
//         lastSample = stats.sample;
 
//         cpuHistory[historyOffset]    = cpuNow;
//         ramHistory[historyOffset]    = ramNow;
//         threatHistory[historyOffset] = threatAccum;
//         threatAccum = 0.0f;
//         historyOffset = (historyOffset + 1) % HISTORY_SIZE;
//     }
 
//     analysisData = backend.getAnalysis();
//     processes.clear();
 
//     for (const BackendProcess& p : backend.getProcesses())
//     {
//         ProcessInfo row;
 
//         row.pid            = p.pid;
//         row.name           = p.name;
//         row.cpu            = p.cpuPercent;
//         row.memory         = p.memoryMB;
//         row.runningSeconds = p.runningSeconds;
//         row.runningTime    = FormatDuration(p.runningSeconds);
//         row.isApp          = p.isApp;
//         row.title          = p.windowTitle;
 
//         processes.push_back(row);
//     }
//     alerts.clear();
 
//     for (const BackendAlert& a : backend.getAlerts())
//     {
//         AlertInfo row;
 
//         row.time        = a.time;
//         row.severity    = a.severity;
//         row.source      = a.source;
//         row.description = a.description;
//         row.id          = a.id;
//         row.canEndTask  = a.canEndTask;
//         row.culpritPid  = a.culpritPid;
//         row.culpritName = a.culpritName;
//         row.culpritMem  = a.culpritMemoryMB;
 
//         alerts.push_back(row);
//     }
 
// <<<<<<< HEAD
// =======
//     // Weigh alerts that appeared since the last refresh
//     {
//         static std::set<int> seenAlerts;
//         static bool seeded = false;
 
//         for (const AlertInfo& a : alerts)
//             if (seenAlerts.insert(a.id).second && seeded)
//                 threatAccum += SeverityWeight(a.severity);
 
//         seeded = true;   // alerts already present at start-up aren't a "spike"
//     }
 
//     // Login and file activity (already newest first)
// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)
//     loginEvents.clear();
//     fileEvents.clear();
 
//     for (const BackendEvent& e : backend.getEvents())
//     {
//         EventInfo row;
 
//         row.time        = e.time;
//         row.type        = e.type;
//         row.description = e.description;
 
//         if (e.type.rfind("LOGIN_", 0) == 0)
//             loginEvents.push_back(row);
//         else if (e.type.rfind("FILE_", 0) == 0)
//             fileEvents.push_back(row);
//     }
 
//     if (demoMode)
//         AppendDemoData();
// }
// <<<<<<< HEAD

// =======
 
// // ------------------------------------------------------------
// // METRIC CARD
// // Accent stripe on the left, big value, optional sparkline.
// // linkPage >= 0 makes the card clickable.
// // ------------------------------------------------------------
 
// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)
// static void MetricCard(
//     const char* title,
//     const char* value,
//     const char* subtitle,
//     const ImVec4& accent,
//     float width,
//     const float* history = nullptr,
//     int linkPage = -1,
//     float gauge = -1.0f)          // 0..100 draws a ring gauge
// {
//     ImGui::BeginChild(
//         title,
//         ImVec2(width, 142.0f),
//         true,
//         ImGuiWindowFlags_NoScrollbar
//     );
 
//     // Accent stripe
//     {
//         const ImVec2 pos = ImGui::GetWindowPos();
//         const ImVec2 size = ImGui::GetWindowSize();
 
//         ImGui::GetWindowDrawList()->AddRectFilled(pos,ImVec2(pos.x + 4.0f, pos.y + size.y),
//             ImGui::GetColorU32(accent),
//             ImGui::GetStyle().ChildRounding,
//             ImDrawFlags_RoundCornersLeft
//         );
//     }
 
//     // Soft glow + ring gauge (or page icon) on the right side
//     {
//         const ImVec2 wp = ImGui::GetWindowPos();
//         const ImVec2 ws = ImGui::GetWindowSize();
//         ImDrawList*  d  = ImGui::GetWindowDrawList();
//         const ImVec2 gc(wp.x + ws.x - 54.0f, wp.y + 62.0f);
 
//         DrawGlow(d, gc, 46.0f, accent);
 
//         if (gauge >= 0.0f)
//             DrawGauge(d, gc, 24.0f, 6.0f, gauge / 100.0f, accent);
//         else if (linkPage >= 0)
//             DrawPageIcon(d, gc, 14.0f, linkPage,
//                 ImGui::GetColorU32(WithAlpha(accent, 0.85f)));
//     }
 
//     ImGui::TextDisabled("%s", title);
//     ImGui::PushStyleColor(ImGuiCol_Text, accent);
//     ImGui::SetWindowFontScale(1.55f);
//     ImGui::TextUnformatted(value);
//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::PopStyleColor();
 
//     if (history != nullptr)
//     {
// <<<<<<< HEAD
//         ImGui::PushStyleColor(ImGuiCol_PlotLines, accent);
//         ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
 
//         ImGui::PlotLines(
//             "##spark",
//             history,
//             HISTORY_SIZE,
//             historyOffset,
//             nullptr,
//             0.0f,
//             100.0f,
//             ImVec2(ImGui::GetContentRegionAvail().x, 26.0f)
//         );
//         ImGui::PopStyleColor(2);
// =======
//         DrawAreaChart("##spark", history, HISTORY_SIZE, historyOffset,
//             100.0f, ImVec2(ImGui::GetContentRegionAvail().x, 32.0f),
//             accent, "%", false);
// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)
//     }
//     else
//     {
//         ImGui::Spacing();
//         ImGui::TextDisabled("%s", subtitle);
//     }
 
//     // Hover highlight + click to open the related page
//     if (linkPage >= 0 && ImGui::IsWindowHovered())
//     {
//         const ImVec2 pos  = ImGui::GetWindowPos();
//         const ImVec2 size = ImGui::GetWindowSize();
 
//         ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
 
//         ImGui::GetWindowDrawList()->AddRect(
//             pos, ImVec2(pos.x + size.x, pos.y + size.y),
//             ImGui::GetColorU32(WithAlpha(accent, 0.85f)),
//             ImGui::GetStyle().ChildRounding, 0, 1.5f);
 
//         ImGui::SetTooltip("Open %s", pages[linkPage]);
 
//         if (ImGui::IsMouseClicked(0))
//             selectedPage = linkPage;
//     }
 
//     ImGui::EndChild();
// }

// static void DrawSidebar()
// {
//     ImGui::PushStyleColor(ImGuiCol_ChildBg, COLOR_SIDEBAR);
 
//     ImGui::BeginChild(
//         "Sidebar",
//         ImVec2(220.0f, 0.0f),
//         true
//     );
 
//     ImGui::PushStyleColor(ImGuiCol_Text, COLOR_CYAN);
//     ImGui::SetWindowFontScale(1.45f);
//     ImGui::TextUnformatted("HIDS");
//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::PopStyleColor();
 
//     ImGui::TextDisabled("Host Intrusion Detection");
 
//     ImGui::Spacing();
//     ImGui::Separator();
//     ImGui::Spacing();
 
//     const int alertCount = (int)alerts.size();
 
//     for (int i = 0; i < IM_ARRAYSIZE(pages); ++i)
//     {
//         ImGui::PushID(i);
 
//         const bool selected = (selectedPage == i);
 
//         if (!selected)
//             ImGui::PushStyleColor(ImGuiCol_Text, MixColor(COLOR_MUTED, COLOR_TEXT, 0.45f));
 
//         // Leading spaces leave room for the icon
//         char label[64];
//         std::snprintf(label, sizeof(label), "        %s", pages[i]);
 
//         if (ImGui::Selectable(label, selected, 0, ImVec2(0.0f, 38.0f)))
//             selectedPage = i;
 
//         if (!selected)
//             ImGui::PopStyleColor();
 
//         const ImVec2 itemMin = ImGui::GetItemRectMin();
//         const ImVec2 itemMax = ImGui::GetItemRectMax();
//         ImDrawList* draw = ImGui::GetWindowDrawList();
 
//         const bool hovered = ImGui::IsItemHovered();
 
//         if (hovered)
//             ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
 
//         DrawPageIcon(draw,
//             ImVec2(itemMin.x + 24.0f, (itemMin.y + itemMax.y) * 0.5f),
//             8.0f, i,
//             ImGui::GetColorU32(selected ? COLOR_CYAN
//                                : hovered ? COLOR_TEXT : COLOR_MUTED));
 
//         if (selected)
//         {
//             draw->AddRectFilled(
//                 itemMin,
//                 ImVec2(itemMin.x + 4.0f, itemMax.y),
//                 ImGui::GetColorU32(COLOR_CYAN),
//                 2.0f
//             );
//         }
 
//         // Alert count badge next to "Alerts"
//         if (std::strcmp(pages[i], "Alerts") == 0 && alertCount > 0)
//         {
//             char count[16];
//             std::snprintf(count, sizeof(count), "%d", alertCount);
 
//             const ImVec2 ts = ImGui::CalcTextSize(count);
//             const float w = ts.x + 14.0f;
//             const float h = ts.y + 2.0f;
//             const float x = itemMax.x - w - 8.0f;
//             const float y = itemMin.y + (itemMax.y - itemMin.y - h) * 0.5f;
 
//             draw->AddRectFilled(
//                 ImVec2(x, y), ImVec2(x + w, y + h),
//                 ImGui::GetColorU32(WithAlpha(COLOR_RED, 0.9f)),
//                 h * 0.5f
//             );
 
//             draw->AddText(
//                 ImVec2(x + 7.0f, y + 1.0f),
//                 IM_COL32(255, 255, 255, 255),
//                 count
//             );
//         }
 
//         ImGui::PopID();
//     }
 
//     ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 84.0f);
 
//     ImGui::Separator();
//     DataSourceBadge(monitoringActive);
//     ImGui::TextDisabled("HIDS Prototype v1.1");
 
//     ImGui::EndChild();
 
//     ImGui::PopStyleColor();
// }
 
// static void DrawTopBar()
// {
//     ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
 
//     ImGui::BeginChild(
//         "TopBar",
//         ImVec2(0.0f, 58.0f),
//         false,
//         ImGuiWindowFlags_NoScrollbar
//     );
 
//     ImGui::BeginGroup();
 
//     ImGui::SetWindowFontScale(1.30f);
//     ImGui::TextUnformatted(pages[selectedPage]);
//     ImGui::SetWindowFontScale(1.0f);
 
//     ImGui::TextDisabled("%s", pageSubtitles[selectedPage]);
 
//     ImGui::EndGroup();
 
//     // Right-aligned status + button
//     const char* statusText =
//         monitoringActive ? "Monitoring active" : "Monitoring stopped";
 
//     const ImVec4 statusColor =
//         monitoringActive ? COLOR_GREEN : COLOR_RED;
 
//     const float statusWidth =
//         ImGui::CalcTextSize(statusText).x + 24.0f;
 
//     const float buttonWidth = 120.0f;
 
//     ImGui::SameLine(
//         ImGui::GetWindowWidth() - statusWidth - buttonWidth - 28.0f
//     );
 
//     ImGui::AlignTextToFramePadding();
//     StatusDot(statusColor, monitoringActive);
 
//     ImGui::SameLine(0.0f, 4.0f);
 
//     ImGui::PushStyleColor(ImGuiCol_Text, statusColor);
//     ImGui::AlignTextToFramePadding();
//     ImGui::TextUnformatted(statusText);
//     ImGui::PopStyleColor();
 
//     ImGui::SameLine();
 
//     if (monitoringActive)
//     {
//         ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.70f, 0.14f, 0.18f, 1.0f));
//         ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COLOR_RED);
 
//         if (ImGui::Button("Stop HIDS", ImVec2(buttonWidth, 34.0f)))
//         {
//             backend.stop();
//             monitoringActive = false;
//         }
 
//         ImGui::PopStyleColor(2);
//     }
//     else
//     {
//         ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.55f, 0.36f, 1.0f));
//         ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COLOR_GREEN);
 
//         if (ImGui::Button("Start HIDS", ImVec2(buttonWidth, 34.0f)))
//         {
//             backend.start();
//             monitoringActive = true;
//         }
 
//         ImGui::PopStyleColor(2);
//     }
 
//     ImGui::EndChild();
 
//     ImGui::PopStyleColor();
// }
 
// static void DrawMetrics()
// {
//     const float spacing = ImGui::GetStyle().ItemSpacing.x;
 
//     const float cardWidth =
//         (ImGui::GetContentRegionAvail().x - spacing * 3.0f) / 4.0f;
 
//     char cpuText[16];
//     char ramText[16];
//     char alertText[16];
 
//     // Displayed values glide toward the live ones instead of jumping
//     static float cpuShown = 0.0f;
//     static float ramShown = 0.0f;
 
//     const float ease = ClampFloat(ImGui::GetIO().DeltaTime * 6.0f, 0.0f, 1.0f);
//     cpuShown += (cpuNow - cpuShown) * ease;
//     ramShown += (ramNow - ramShown) * ease;
 
//     std::snprintf(cpuText, sizeof(cpuText), "%.0f%%", cpuShown);
//     std::snprintf(ramText, sizeof(ramText), "%.0f%%", ramShown);
//     std::snprintf(alertText, sizeof(alertText), "%d", (int)alerts.size());
 
//     const ImVec4 alertColor = CountSeverity("Critical") > 0 ? COLOR_RED : COLOR_YELLOW;
 
//     char alertSubtitle[64];
//     std::snprintf(
//         alertSubtitle, sizeof(alertSubtitle),
//         "%d critical, %d high",
//         CountSeverity("Critical"), CountSeverity("High")
//     );
 
//     MetricCard("CPU usage", cpuText, "", LoadColor(cpuNow), cardWidth, cpuHistory, 6, cpuShown);
//     ImGui::SameLine();
 
//     MetricCard("RAM usage", ramText, "", LoadColor(ramNow), cardWidth, ramHistory, 6, ramShown);
//     ImGui::SameLine();
 
//     char processText[16];
//     std::snprintf(processText, sizeof(processText), "%d", processCount);
 
//     MetricCard("Running processes", processText, "On this host", COLOR_BLUE, cardWidth, nullptr, 1);
//     ImGui::SameLine();
 
//     MetricCard("Total alerts", alertText, alertSubtitle, alertColor, cardWidth, nullptr, 4);
// }
 
// // ------------------------------------------------------------
// // THREAT ACTIVITY PANEL (dashboard)
// // Histogram of new alerts per second + clickable severity bars.
// // ------------------------------------------------------------
 
// static void DrawThreatPanel()
// {
//     ImGui::BeginChild("ThreatPanel", ImVec2(0.0f, 196.0f), true,
//                       ImGuiWindowFlags_NoScrollbar);
 
//     SectionHeading("Threat activity",
//         "new alerts per second, weighted by severity (last 60 s)");
 
//     float peak = 6.0f;
//     for (float v : threatHistory)
//         peak = std::max(peak, v);
 
//     const float leftW = ImGui::GetContentRegionAvail().x * 0.62f;
 
//     DrawBarChart("##threat", threatHistory, HISTORY_SIZE, historyOffset,
//         peak, ImVec2(leftW, 122.0f));
 
//     ImGui::SameLine();
//     ImGui::BeginGroup();
 
//     static const char* names[] = { "Critical", "High", "Medium", "Low" };
//     const int total = std::max(1, (int)alerts.size());
 
//     for (int i = 0; i < 4; ++i)
//     {
//         const int count = CountSeverity(names[i]);
 
//         char label[40];
//         std::snprintf(label, sizeof(label), "%s  %d", names[i], count);
 
//         ImGui::PushID(i);
//         ImGui::PushStyleColor(ImGuiCol_PlotHistogram,
//             WithAlpha(SeverityColor(names[i]), 0.85f));
//         ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_FRAME);
 
//         ImGui::ProgressBar((float)count / (float)total,
//             ImVec2(ImGui::GetContentRegionAvail().x, 20.0f), label);
 
//         ImGui::PopStyleColor(2);
 
//         if (ImGui::IsItemHovered())
//         {
//             ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
//             ImGui::SetTooltip("Show %s alerts", names[i]);
//         }
 
//         if (ImGui::IsItemClicked())
//         {
//             selectedPage   = 4;        // Alerts
//             severityFilter = i + 1;
//         }
 
//         ImGui::PopID();
//     }
 
//     ImGui::EndGroup();
//     ImGui::EndChild();
// }
// <<<<<<< HEAD

// =======
 
// // ------------------------------------------------------------
// // PASSWORD LOCK (Logs & Reports page and End Task)
// // ------------------------------------------------------------
 
// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)
// static const double LOCK_IDLE_SECONDS = 300.0;     // auto-lock after 5 min idle
// static const int    LOGS_PAGE_INDEX   = 5;
 
// static bool        logsUnlocked   = false;
// static double      logsLastActive = 0.0;
// static char        lockOld[64]     = "";
// static char        lockNew[64]     = "";
// static char        lockConfirm[64] = "";
// static char        lockEntry[64]   = "";
// static std::string lockMessage;
// static bool        lockMessageOk   = false;
 
// static void Wipe(char* buffer, std::size_t size)
// {
//     std::memset(buffer, 0, size);
// }
 
// // "Create a password" form. Returns true on the frame it is created.
// static bool DrawSetPasswordForm()
// {
//     bool created = false;
 
//     ImGui::TextDisabled("Create a password (at least 6 characters)");
 
//     ImGui::SetNextItemWidth(260.0f);
//     ImGui::InputTextWithHint("##newpw", "New password",
//         lockNew, sizeof(lockNew), ImGuiInputTextFlags_Password);
 
//     ImGui::SetNextItemWidth(260.0f);
//     ImGui::InputTextWithHint("##confpw", "Confirm password",
//         lockConfirm, sizeof(lockConfirm), ImGuiInputTextFlags_Password);
 
//     if (ImGui::Button("Set Password"))
//     {
//         if (std::strcmp(lockNew, lockConfirm) != 0)
//         {
//             lockMessage   = "Passwords do not match.";
//             lockMessageOk = false;
//         }
//         else
//         {
//             std::string message;
 
//             if (backend.setPassword(lockNew, "", message))
//             {
//                 created = true;
//                 Wipe(lockNew, sizeof(lockNew));
//                 Wipe(lockConfirm, sizeof(lockConfirm));
//             }
 
//             lockMessage   = message;
//             lockMessageOk = created;
//         }
//     }
 
//     return created;
// }
 
// static void DrawLockScreen()
// {
//     ImGui::BeginChild("LockScreen", ImVec2(0.0f, 0.0f), true);
 
//     SectionHeading("Logs & Reports are locked",
//         "enter the password to see the detection signatures");
 
//     ImGui::Spacing();
 
//     if (!backend.isLockConfigured())
//     {
//         if (DrawSetPasswordForm())
//             logsUnlocked = true;          // whoever creates it is let in
//     }
//     else
//     {
//         const int wait = backend.lockoutSecondsLeft();
 
//         ImGui::BeginDisabled(wait > 0);
 
//         ImGui::SetNextItemWidth(260.0f);
//         const bool enter = ImGui::InputTextWithHint("##unlockpw", "Password",
//             lockEntry, sizeof(lockEntry),
//             ImGuiInputTextFlags_Password | ImGuiInputTextFlags_EnterReturnsTrue);
 
//         ImGui::SameLine();
//         const bool click = ImGui::Button("Unlock");
 
//         ImGui::EndDisabled();
 
//         if (wait > 0)
//             ImGui::TextColored(COLOR_YELLOW, "Locked. Try again in %d s.", wait);
 
//         if ((enter || click) && wait == 0)
//         {
//             std::string message;
 
//             if (backend.verifyPassword(lockEntry, message))
//             {
//                 logsUnlocked = true;
//                 lockMessage.clear();
//             }
//             else
//             {
//                 lockMessage   = message;
//                 lockMessageOk = false;
//             }
 
//             Wipe(lockEntry, sizeof(lockEntry));
//         }
//     }
 
//     if (!lockMessage.empty())
//         ImGui::TextColored(lockMessageOk ? COLOR_GREEN : COLOR_RED,
//             "%s", lockMessage.c_str());
 
//     ImGui::EndChild();
// }
 
// // true = unlocked, the page may draw its content
// static bool LogsGate()
// {
//     const double now = ImGui::GetTime();
//     ImGuiIO& io = ImGui::GetIO();
 
//     const bool active =
//         io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f ||
//         io.MouseWheel != 0.0f || ImGui::IsMouseClicked(0) ||
//         ImGui::IsAnyItemActive();
 
//     if (active || !logsUnlocked)
//         logsLastActive = now;
 
//     if (logsUnlocked && now - logsLastActive > LOCK_IDLE_SECONDS)
//     {
//         logsUnlocked  = false;
//         lockMessage   = "Locked after 5 minutes of inactivity.";
//         lockMessageOk = false;
//     }
 
//     if (logsUnlocked)
//         return true;
 
//     DrawLockScreen();
//     return false;
// }
// <<<<<<< HEAD

// =======
 
// // ------------------------------------------------------------
// // END TASK (shared by the Processes page and resource alerts)
// // ------------------------------------------------------------
 
// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)
// static const float HIGH_MEMORY_MB = 500.0f;
 
// static bool        procAppsOnly     = false;
// static bool        procHighMemOnly  = false;
// static std::string procMessage;                 // result of the last End Task
// static bool        procMessageOk    = true;
// static int         pendingPid       = 0;
// static std::string pendingName;
// static float       pendingMem       = 0.0f;
// static int         pendingAlertID   = 0;        // 0 = started from the Processes page
// static bool        openEndTaskPopup = false;
// static std::set<int> handledAlertIDs;           // alerts whose app was ended
 
// static char        endPassword[64] = "";
// static std::string endPopupMessage;
// <<<<<<< HEAD


// =======
 
// // Call once inside the page that may open the confirmation dialog.
// // Ending a task needs the access password.
// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)
// static void DrawEndTaskPopup()
// {
//     if (openEndTaskPopup)
//     {
//         ImGui::OpenPopup("Confirm End Task");
//         openEndTaskPopup = false;
 
//         Wipe(endPassword, sizeof(endPassword));
//         endPopupMessage.clear();
//         lockMessage.clear();
//     }
 
//     const ImVec2 screen = ImGui::GetIO().DisplaySize;
//     ImGui::SetNextWindowPos(
//         ImVec2(screen.x * 0.5f, screen.y * 0.5f),
//         ImGuiCond_Appearing,
//         ImVec2(0.5f, 0.5f));
 
//     if (ImGui::BeginPopupModal("Confirm End Task", nullptr,
//             ImGuiWindowFlags_AlwaysAutoResize))
//     {
//         ImGui::Text("End %s (PID %d)?", pendingName.c_str(), pendingPid);
//         ImGui::TextDisabled("It is using %.0f MB. Unsaved work in this app will be lost.",
//             pendingMem);
//         ImGui::Spacing();
 
//         if (!backend.isLockConfigured())
//         {
//             ImGui::TextColored(COLOR_YELLOW,
//                 "Ending tasks is password protected. Create a password first.");
//             ImGui::Spacing();
 
//             DrawSetPasswordForm();
 
//             if (!lockMessage.empty())
//                 ImGui::TextColored(lockMessageOk ? COLOR_GREEN : COLOR_RED,"%s", lockMessage.c_str());
//         }
//         else
//         {
//             const int wait = backend.lockoutSecondsLeft();
 
//             ImGui::BeginDisabled(wait > 0);
 
//             ImGui::SetNextItemWidth(260.0f);
//             const bool enter = ImGui::InputTextWithHint("##endpw", "Password",
//                 endPassword, sizeof(endPassword),
//                 ImGuiInputTextFlags_Password | ImGuiInputTextFlags_EnterReturnsTrue);
 
//             ImGui::EndDisabled();
 
//             if (wait > 0)
//                 ImGui::TextColored(COLOR_YELLOW, "Locked. Try again in %d s.", wait);
 
//             if (!endPopupMessage.empty())
//                 ImGui::TextColored(COLOR_RED, "%s", endPopupMessage.c_str());
 
//             ImGui::Spacing();
 
//             ImGui::BeginDisabled(wait > 0);
//             const bool click = ImGui::Button("End Task", ImVec2(120.0f, 0.0f));
//             ImGui::EndDisabled();
 
//             if ((enter || click) && wait == 0)
//             {
//                 std::string authMessage;
//                 const bool authorised = backend.verifyPassword(endPassword, authMessage);
 
//                 Wipe(endPassword, sizeof(endPassword));
 
//                 if (!authorised)
//                 {
//                     endPopupMessage = authMessage;
//                 }
//                 else
//                 {
//                     std::string result;
 
//                     procMessageOk = backend.endTask(pendingPid, result, pendingName);
//                     procMessage   = result;
 
//                     if (procMessageOk && pendingAlertID != 0)
//                         handledAlertIDs.insert(pendingAlertID);
 
//                     ImGui::CloseCurrentPopup();
//                 }
//             }
 
//             ImGui::SameLine();
//         }
 
//         if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
//         {
//             Wipe(endPassword, sizeof(endPassword));
//             Wipe(lockNew, sizeof(lockNew));
//             Wipe(lockConfirm, sizeof(lockConfirm));
//             ImGui::CloseCurrentPopup();
//         }
 
//         ImGui::EndPopup();
//     }
// }
// <<<<<<< HEAD
// =======
 
// // ------------------------------------------------------------
// // ALERT TABLE
// // compact = true: dashboard preview. false: full Alerts page.
// // ------------------------------------------------------------
// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)
 
// static void DrawAlerts(bool compact)
// {
//     ImGui::BeginChild(
//         "AlertsPanel",
//         ImVec2(0.0f, compact ? 270.0f : 0.0f),
//         true
//     );
 
//     SectionHeading(
//         compact ? "Recent security alerts" : "All security alerts"
//     );
 
//     // Severity filter + search (full page only)
//     if (!compact)
//     {
//         for (int i = 0; i < IM_ARRAYSIZE(severityFilterLabels); ++i)
//         {
//             const bool active = (severityFilter == i);
 
//             if (active)
//             {
//                 ImGui::PushStyleColor(ImGuiCol_Button, COLOR_BLUE);
//             }
//             else
//             {
//                 ImGui::PushStyleColor(ImGuiCol_Button, COLOR_FRAME);
//             }
 
//             if (ImGui::Button(severityFilterLabels[i], ImVec2(96.0f, 0.0f)))
//                 severityFilter = i;
 
//             ImGui::PopStyleColor();
 
//             if (i + 1 < IM_ARRAYSIZE(severityFilterLabels))
//                 ImGui::SameLine();
//         }
 
//         ImGui::SameLine();
//         ImGui::SetNextItemWidth(260.0f);
//         ImGui::InputTextWithHint("##alertsearch", "Search alerts",
//             alertSearch, IM_ARRAYSIZE(alertSearch));
 
//         ImGui::Spacing();
//     }
 
//     if (!procMessage.empty())
//         ImGui::TextColored(procMessageOk ? COLOR_GREEN : COLOR_RED,
//             "%s", procMessage.c_str());
 
//     DrawEndTaskPopup();
 
//     if (ImGui::BeginTable(
//             "AlertsTable",
//             5,
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);
//         ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 110.0f);
//         ImGui::TableSetupColumn("Severity", ImGuiTableColumnFlags_WidthFixed, 130.0f);
//         ImGui::TableSetupColumn("Source", ImGuiTableColumnFlags_WidthFixed, 200.0f);
//         ImGui::TableSetupColumn("Description");
//         ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 100.0f);
//         ImGui::TableHeadersRow();
 
//         int shown = 0;
 
//         for (const auto& alert : alerts)
//         {
//             if (!compact && severityFilter != 0 &&
//                 alert.severity != severityFilterLabels[severityFilter])
//             {
//                 continue;
//             }
 
//             if (!compact &&
//                 !ContainsNoCase(alert.description.c_str(), alertSearch) &&
//                 !ContainsNoCase(alert.source.c_str(), alertSearch))
//             {
//                 continue;
//             }
 
//             ++shown;
 
//             ImGui::TableNextRow();
 
//             ImGui::TableSetColumnIndex(0);
//             {
//                 const ImVec2 p = ImGui::GetCursorScreenPos();
 
//                 ImGui::GetWindowDrawList()->AddCircleFilled(
//                     ImVec2(p.x + 4.0f, p.y + ImGui::GetTextLineHeight() * 0.5f),
//                     4.0f,
//                     ImGui::GetColorU32(SeverityColor(alert.severity.c_str())));
 
//                 ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 16.0f);
//             }
//             ImGui::TextDisabled("%s", alert.time.c_str());
 
//             ImGui::TableSetColumnIndex(1);
//             SeverityBadge(alert.severity.c_str());
 
//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextUnformatted(alert.source.c_str());
 
//             ImGui::TableSetColumnIndex(3);
//             ImGui::TextUnformatted(alert.description.c_str());
 
//             // Resource alerts can offer End Task for the app causing them
//             ImGui::TableSetColumnIndex(4);
 
//             if (alert.canEndTask)
//             {
//                 if (handledAlertIDs.count(alert.id))
//                 {
//                     ImGui::TextColored(COLOR_GREEN, "Ended");
//                 }
//                 else
//                 {
//                     ImGui::PushID(alert.id);
 
//                     ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.55f, 0.12f, 0.15f, 1.0f));
//                     ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.18f, 0.22f, 1.0f));
//                     ImGui::PushStyleColor(ImGuiCol_ButtonActive,  COLOR_RED);
 
//                     if (ImGui::SmallButton("End Task"))
//                     {
//                         pendingAlertID   = alert.id;
//                         pendingPid       = alert.culpritPid;
//                         pendingName      = alert.culpritName;
//                         pendingMem       = alert.culpritMem;
//                         openEndTaskPopup = true;
//                     }
 
//                     ImGui::PopStyleColor(3);
//                     ImGui::PopID();
//                 }
//             }
//             else
//             {
//                 ImGui::TextDisabled("-");
//             }
//         }
 
//         if (shown == 0)
//         {
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(3);
//             ImGui::TextDisabled("No alerts match the current filters.");
//         }
 
//         ImGui::EndTable();
//     }
 
//     ImGui::EndChild();
// }
// <<<<<<< HEAD

// =======
 
// // ------------------------------------------------------------
// // PROCESS TABLE
// // Sortable columns, case-insensitive search, CPU usage bars.
// // ------------------------------------------------------------
 
// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)
// static void DrawProcesses()
// {
//     ImGui::BeginChild(
//         "ProcessesPanel",
//         ImVec2(0.0f, 0.0f),
//         true
//     );
 
//     SectionHeading("Running processes",
//         "apps can be ended here; system and background processes are protected");
 
//     ImGui::SetNextItemWidth(300.0f);
 
//     ImGui::InputTextWithHint(
//         "##ProcessSearch",
//         "Search by name or PID",
//         processSearch,
//         IM_ARRAYSIZE(processSearch)
//     );
 
//     ImGui::SameLine();
//     ImGui::Checkbox("Apps only", &procAppsOnly);
 
//     ImGui::SameLine();
 
//     char highLabel[64];
//     std::snprintf(highLabel, sizeof(highLabel),
//         "High memory only (%.0f MB+)", HIGH_MEMORY_MB);
//     ImGui::Checkbox(highLabel, &procHighMemOnly);
 
//     if (!procMessage.empty())
//         ImGui::TextColored(procMessageOk ? COLOR_GREEN : COLOR_RED,
//             "%s", procMessage.c_str());
 
//     ImGui::Spacing();
 
//     if (ImGui::BeginTable(
//             "ProcessTable",
//             6,
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_Sortable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);
 
//         ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed, 85.0f, 0);
//         ImGui::TableSetupColumn("Process name", ImGuiTableColumnFlags_WidthStretch, 0.0f, 1);
//         ImGui::TableSetupColumn(
//             "CPU %",
//             ImGuiTableColumnFlags_WidthFixed |
//             ImGuiTableColumnFlags_PreferSortDescending,
//             180.0f, 2);
//         ImGui::TableSetupColumn(
//             "Memory MB",
//             ImGuiTableColumnFlags_WidthFixed |
//             ImGuiTableColumnFlags_DefaultSort |
//             ImGuiTableColumnFlags_PreferSortDescending,
//             125.0f, 3);
//         ImGui::TableSetupColumn("Running time", ImGuiTableColumnFlags_WidthFixed, 135.0f, 4);
//         ImGui::TableSetupColumn("Action",
//             ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoSort,
//             100.0f, 5);
 
//         ImGui::TableHeadersRow();
 
//         // Build a sorted list of indices
//         std::vector<int> order(processes.size());
 
//         for (size_t i = 0; i < order.size(); ++i)
//             order[i] = (int)i;
 
//         if (ImGuiTableSortSpecs* specs = ImGui::TableGetSortSpecs())
//         {
//             if (specs->SpecsCount > 0)
//             {
//                 const ImGuiTableColumnSortSpecs& spec = specs->Specs[0];
//                 const bool asc =
//                     spec.SortDirection == ImGuiSortDirection_Ascending;
 
//                 std::sort(order.begin(), order.end(),
//                     [&](int a, int b)
//                     {
//                         const ProcessInfo& pa = processes[a];
//                         const ProcessInfo& pb = processes[b];
 
//                         int cmp = 0;
 
//                         switch (spec.ColumnUserID)
//                         {
//                         case 0: cmp = (pa.pid > pb.pid) - (pa.pid < pb.pid); break;
//                         case 1: cmp = pa.name.compare(pb.name); break;
//                         case 2: cmp = (pa.cpu > pb.cpu) - (pa.cpu < pb.cpu); break;
//                         case 3: cmp = (pa.memory > pb.memory) - (pa.memory < pb.memory); break;
//                         case 4: cmp = (pa.runningSeconds > pb.runningSeconds) - (pa.runningSeconds < pb.runningSeconds); break;
//                         }
 
//                         return asc ? (cmp < 0) : (cmp > 0);
//                     });
//             }
//         }
 
//         int shown = 0;
 
//         for (int index : order)
//         {
//             const ProcessInfo& process = processes[index];
 
//             if (procAppsOnly && !process.isApp)
//                 continue;
 
//             if (procHighMemOnly && process.memory < HIGH_MEMORY_MB)
//                 continue;
 
//             char pidText[32];
//             std::snprintf(pidText, sizeof(pidText), "%d", process.pid);
 
//             const bool matches =
//                 ContainsNoCase(process.name.c_str(), processSearch) ||
//                 ContainsNoCase(pidText, processSearch);
 
//             if (!matches)
//                 continue;
 
//             ++shown;
 
//             ImGui::TableNextRow();
 
//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextDisabled("%d", process.pid);
 
//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextUnformatted(process.name.c_str());
 
//             if (process.isApp)
//             {
//                 ImGui::SameLine();
//                 ImGui::TextDisabled("- %s", process.title.c_str());
//             }
 
//             // CPU bar, scaled so the busiest realistic process fills it
//             ImGui::TableSetColumnIndex(2);
 
//             char cpuLabel[16];
//             std::snprintf(cpuLabel, sizeof(cpuLabel), "%.1f", process.cpu);
 
//             ImGui::PushStyleColor(ImGuiCol_PlotHistogram, WithAlpha(LoadColor(process.cpu * 4.0f), 0.85f));
//             ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_FRAME);
 
//             ImGui::ProgressBar(
//                 std::min(process.cpu / 25.0f, 1.0f),
//                 ImVec2(-FLT_MIN, 18.0f),
//                 cpuLabel
//             );
 
//             ImGui::PopStyleColor(2);
 
//             // Memory: yellow when high, red when very high
//             ImGui::TableSetColumnIndex(3);
 
//             if (process.memory >= HIGH_MEMORY_MB * 2.0f)
//                 ImGui::TextColored(COLOR_RED, "%.1f", process.memory);
//             else if (process.memory >= HIGH_MEMORY_MB)
//                 ImGui::TextColored(COLOR_YELLOW, "%.1f", process.memory);
//             else
//                 ImGui::Text("%.1f", process.memory);
 
//             ImGui::TableSetColumnIndex(4);
//             ImGui::TextDisabled("%s", process.runningTime.c_str());
 
//             // Only apps (windowed programs) get an End Task button
//             ImGui::TableSetColumnIndex(5);
 
//             if (process.isApp)
//             {
//                 ImGui::PushID(process.pid);
 
//                 ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.55f, 0.12f, 0.15f, 1.0f));
//                 ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.18f, 0.22f, 1.0f));
//                 ImGui::PushStyleColor(ImGuiCol_ButtonActive,  COLOR_RED);
 
//                 if (ImGui::SmallButton("End Task"))
//                 {
//                     pendingAlertID   = 0;
//                     pendingPid       = process.pid;
//                     pendingName      = process.name;
//                     pendingMem       = process.memory;
//                     openEndTaskPopup = true;
//                 }
 
//                 ImGui::PopStyleColor(3);
//                 ImGui::PopID();
//             }
//             else
//             {
//                 ImGui::TextDisabled("-");
//             }
//         }
 
//         if (shown == 0)
//         {
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextDisabled("No process matches the current filters.");
//         }
 
//         ImGui::EndTable();
//     }
 
//     DrawEndTaskPopup();
 
//     ImGui::EndChild();
// }
// <<<<<<< HEAD
// =======
 
// // ------------------------------------------------------------
// // EVENT PAGE
// // Newest-first activity list for Login and File monitoring.
// // ------------------------------------------------------------
// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)
 
// static void DrawEventPage(
//     const char* title,
//     const std::vector<EventInfo>& events,
//     const char* emptyText,
//     const char* hint)
// {
//     ImGui::BeginChild(
//         title,
//         ImVec2(0.0f, 0.0f),
//         true
//     );
 
//     SectionHeading(title, hint);
 
//     if (ImGui::BeginTable(
//             "EventTable",
//             3,
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);
//         ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 190.0f);
//         ImGui::TableSetupColumn("Event", ImGuiTableColumnFlags_WidthFixed, 170.0f);
//         ImGui::TableSetupColumn("Description");
//         ImGui::TableHeadersRow();
 
//         for (const EventInfo& event : events)
//         {
//             ImGui::TableNextRow();
 
//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextDisabled("%s", event.time.c_str());
 
//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextUnformatted(event.type.c_str());
 
//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextUnformatted(event.description.c_str());
//         }
 
//         if (events.empty())
//         {
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextDisabled("%s", emptyText);
//         }
 
//         ImGui::EndTable();
//     }
 
//     ImGui::EndChild();
// }

// static void DrawSystemAnalysis()
// {
//     ImGui::BeginChild("AnalysisPanel", ImVec2(0.0f, 0.0f), true);
 
//     if (!analysisData.valid)
//     {
//         ImGui::TextDisabled("Collecting the first samples...");
//         ImGui::EndChild();
//         return;
//     }
 
//     const BackendAnalysis& a = analysisData;
 
//     // ---- Resource usage ----
//     SectionHeading("Resource usage");
 
//     char cpuText[16];
//     char ramText[16];
 
//     if (a.cpuAvailable)
//         std::snprintf(cpuText, sizeof(cpuText), "%.1f%%", a.cpuPercent);
//     else
//         std::snprintf(cpuText, sizeof(cpuText), "n/a");
 
//     if (a.memoryAvailable)
//         std::snprintf(ramText, sizeof(ramText), "%.1f%%", a.memoryPercent);
//     else
//         std::snprintf(ramText, sizeof(ramText), "n/a");
 
//     ImGui::Text("CPU:");
//     ImGui::SameLine();
//     ImGui::TextColored(LoadColor((float)a.cpuPercent), "%s", cpuText);
 
//     ImGui::Text("Memory:");
//     ImGui::SameLine();
//     ImGui::TextColored(LoadColor((float)a.memoryPercent), "%s", ramText);
//     ImGui::SameLine();
//     ImGui::TextDisabled("(%llu MB used of %llu MB, %llu MB free)",
//         (unsigned long long)a.usedMemoryMB,
//         (unsigned long long)a.totalMemoryMB,
//         (unsigned long long)a.availableMemoryMB);
 
//     ImGui::Spacing();
 
//     const float half =
//         (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;
 
//     if (!a.cpuHistory.empty())
//     {
//         ImGui::BeginGroup();
//         ImGui::TextDisabled("CPU usage (last 60 s)");
//         DrawAreaChart("##cpuh",
//             a.cpuHistory.data(), (int)a.cpuHistory.size(), 0,
//             100.0f, ImVec2(half, 170.0f),
//             LoadColor((float)a.cpuPercent));
//         ImGui::EndGroup();
//     }
 
//     ImGui::SameLine();
 
//     if (!a.ramHistory.empty())
//     {
//         ImGui::BeginGroup();
//         ImGui::TextDisabled("Memory usage (last 60 s)");
//         DrawAreaChart("##ramh",
//             a.ramHistory.data(), (int)a.ramHistory.size(), 0,
//             100.0f, ImVec2(half, 170.0f),
//             LoadColor((float)a.memoryPercent));
//         ImGui::EndGroup();
//     }
 
//     ImGui::Spacing();
//     ImGui::Separator();
//     ImGui::Spacing();
 
//     // ---- Findings ----
//     SectionHeading("Findings", "A diagnostic signal, not proof of an intrusion");
 
//     ImGui::Text(
//         "Processes: %llu    High CPU (>=80%%): %llu    High memory (>=1 GB): %llu",
//         (unsigned long long)a.processCount,
//         (unsigned long long)a.highCpuProcessCount,
//         (unsigned long long)a.highMemoryProcessCount);
 
//     if (a.findings.empty())
//     {
//         ImGui::TextColored(COLOR_GREEN, "No resource concerns detected.");
//     }
//     else
//     {
//         for (const std::string& finding : a.findings){
//             ImGui::TextColored(COLOR_YELLOW, "- %s", finding.c_str());
//         }
//     }
 
//     ImGui::Spacing();
//     ImGui::Separator();
//     ImGui::Spacing();
 
//     // ---- Top CPU processes ----
//     SectionHeading("Top processes by CPU");
 
//     if (ImGui::BeginTable(
//          "TopCpuTable",4,ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH |ImGuiTableFlags_SizingStretchProp))
//     {
//         ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed, 85.0f);
//         ImGui::TableSetupColumn("Process name");
//         ImGui::TableSetupColumn("CPU %", ImGuiTableColumnFlags_WidthFixed, 100.0f);
//         ImGui::TableSetupColumn("Memory MB", ImGuiTableColumnFlags_WidthFixed, 125.0f);
//         ImGui::TableHeadersRow();
 
//         for (const BackendProcess& p : a.topCpuProcesses)
//         {
//             ImGui::TableNextRow();
 
//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextDisabled("%d", p.pid);
 
//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextUnformatted(p.name.c_str());
 
//             ImGui::TableSetColumnIndex(2);
//             ImGui::Text("%.1f", p.cpuPercent);
 
//             ImGui::TableSetColumnIndex(3);
//             ImGui::Text("%.1f", p.memoryMB);
//         }
 
//         ImGui::EndTable();
//     }
 
//     ImGui::EndChild();
// }
// <<<<<<< HEAD

// =======
 
// // ------------------------------------------------------------
// // FILE SCANNER PAGE
// // ------------------------------------------------------------
 
// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)
// static std::string FormatBytes(unsigned long long b)
// {
//     char t[32];
 
//     if (b >= (1ULL << 30))      std::snprintf(t, sizeof(t), "%.2f GB", b / 1073741824.0);
//     else if (b >= (1ULL << 20)) std::snprintf(t, sizeof(t), "%.1f MB", b / 1048576.0);
//     else if (b >= 1024ULL)      std::snprintf(t, sizeof(t), "%.1f KB", b / 1024.0);
//     else                        std::snprintf(t, sizeof(t), "%llu B", b);
 
//     return t;
// }
// <<<<<<< HEAD

// =======
 
// // Rotating-arc spinner. Reserves a (2*radius) square at the cursor.
// // active = false draws a plain ring instead (idle / finished).
// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)
// static void DrawSpinner(float radius, float thickness, const ImVec4& color, bool active)
// {
//     const ImVec2 pos = ImGui::GetCursorScreenPos();
//     const ImVec2 center(pos.x + radius, pos.y + radius);
//     ImDrawList* draw = ImGui::GetWindowDrawList();
//     const float t = (float)ImGui::GetTime();
 
//     if (!active)
//     {
//         draw->AddCircle(center, radius, ImGui::GetColorU32(color), 48, thickness);
//     }
//     else
//     {
//         // faint full ring + a rotating arc on top
//         draw->AddCircle(center, radius,
//             ImGui::GetColorU32(WithAlpha(color, 0.18f)), 48, thickness);
 
//         const float start = t * 5.0f;
 
//         draw->PathClear();
//         draw->PathArcTo(center, radius, start, start + 2.4f, 32);
//         draw->PathStroke(ImGui::GetColorU32(color), 0, thickness);
 
//         // pulsing dot in the middle
//         const float pulse = 0.75f + 0.25f * std::sin(t * 4.0f);
//         draw->AddCircleFilled(center, radius * 0.30f * pulse,
//             ImGui::GetColorU32(WithAlpha(color, 0.85f)), 24);
//     }
 
//     ImGui::Dummy(ImVec2(radius * 2.0f, radius * 2.0f));
// }
 
// // "C:\a\b\file.txt" -> dir "C:\a\b", name "file.txt"
// static void SplitPath(const std::string& full, std::string& dir, std::string& name)
// {
//     const std::size_t pos = full.find_last_of("\\/");
 
//     if (pos == std::string::npos)
//     {
//         dir.clear();
//         name = full;
//     }
//     else
//     {
//         dir  = full.substr(0, pos);
//         name = full.substr(pos + 1);
//     }
// }
 
// static void DrawFileScanner()
// {
//     // Refresh a few times per second, not every frame
//     static double lastRefresh = -1.0;
//     const double now = ImGui::GetTime();
 
//     if (lastRefresh < 0.0 || now - lastRefresh >= 0.25)
//     {
//         lastRefresh  = now;
//         scanStatus   = backend.getScanStatus();
//         scanFindings = backend.getScanFindings();
//     }
 
//     const bool running = scanStatus.running;
 
//     ImGui::BeginChild("ScannerPanel", ImVec2(0.0f, 0.0f), true);
 
//     SectionHeading("File Scanner", "Signature + heuristic scan");
 
//     // ---- Scan type ----
//     ImGui::BeginDisabled(running);
 
//     ImGui::RadioButton("Quick (user folders, temp, startup)", &scanTypeSel, 0);
//     ImGui::SameLine();
//     ImGui::RadioButton("Full (all fixed drives)", &scanTypeSel, 1);
//     ImGui::SameLine();
//     ImGui::RadioButton("Custom", &scanTypeSel, 2);
 
//     if (scanTypeSel == 2)
//     {
//         ImGui::SetNextItemWidth(-1.0f);
//         ImGui::InputTextWithHint("##scanpath",
//             "Folder or file, e.g. C:\\Users\\you\\Downloads",
//             scanPath, sizeof(scanPath));
//     }
 
//     ImGui::EndDisabled();
 
//     // ---- Buttons ----
//     if (!running)
//     {
//         if (ImGui::Button("Start Scan"))
//         {
//             if (scanTypeSel == 2 && scanPath[0] == '\0')
//                 scanMessage = "Enter a folder or file path first.";
//             else if (!backend.startScan((ScanType)scanTypeSel, scanPath))
//                 scanMessage = "A scan is already running.";
//             else
//                 scanMessage.clear();
//         }
//     }
//     else
//     {
//         if (ImGui::Button("Stop Scan"))
//             backend.cancelScan();
//     }
 
//     if (!running && scanStatus.finished)
//     {
//         ImGui::SameLine();
 
//         if (ImGui::Button("Export Report"))
//         {
//             std::string out;
 
//             scanMessage = backend.exportScanReport(out)
//                 ? "Report saved: " + out
//                 : "Could not write the report file.";
//         }
//     }
 
//     if (!scanMessage.empty())
//     {
//         ImGui::SameLine();
//         ImGui::TextDisabled("%s", scanMessage.c_str());
//     }
 
//     ImGui::Separator();
//     ImGui::Spacing();
// <<<<<<< HEAD
// =======
 
//     // ========================================================
//     // Two panels side by side: live scan (left), threats (right)
//     // ========================================================
// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)
//     const float gap    = ImGui::GetStyle().ItemSpacing.x;
//     const float leftW  = (ImGui::GetContentRegionAvail().x - gap) * 0.58f;
//     const float panelH = 230.0f;
 
//     ImGui::PushStyleColor(ImGuiCol_ChildBg, COLOR_SIDEBAR);
 
//     // ---------------- LEFT: live scan ----------------
//     ImGui::BeginChild("ScanLive", ImVec2(leftW, panelH), true);
 
//     {
//         ImVec4 stateColor = COLOR_MUTED;
//         const char* stateText = "Ready to scan";
 
//         if (running)
//         {
//             stateColor = COLOR_CYAN;
//             stateText  = "Scanning";
//         }
//         else if (scanStatus.finished && scanStatus.cancelled)
//         {
//             stateColor = COLOR_YELLOW;
//             stateText  = "Scan stopped";
//         }
//         else if (scanStatus.finished)
//         {
//             stateColor = (scanStatus.malicious + scanStatus.suspicious > 0)
//                 ? COLOR_ORANGE : COLOR_GREEN;
//             stateText  = "Scan completed";
//         }
 
//         // Spinner (or a plain ring when idle)
//         DrawSpinner(26.0f, 4.0f, stateColor, running);
//         ImGui::SameLine(0.0f, 16.0f);
 
//         ImGui::BeginGroup();
 
//         ImGui::SetWindowFontScale(1.3f);
//         ImGui::PushStyleColor(ImGuiCol_Text, stateColor);
 
//         if (running)
//         {
//             const int dots = (int)(ImGui::GetTime() * 2.0) % 4;
//             ImGui::Text("%s%.*s", stateText, dots, "...");
//         }
//         else
//         {
//             ImGui::TextUnformatted(stateText);
//         }
 
//         ImGui::PopStyleColor();
//         ImGui::SetWindowFontScale(1.0f);
 
//         ImGui::TextDisabled("%s",
// <<<<<<< HEAD
//             scanStatus.target.empty() ? "Choose a scan type and press Start" : scanStatus.target.c_str());

// =======
//             scanStatus.target.empty() ? "Choose a scan type and press Start"
//                                       : scanStatus.target.c_str());
 
// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)
//         ImGui::EndGroup();
 
//         ImGui::Spacing();
//         ImGui::Separator();
//         ImGui::Spacing();
 
//         // Which file is being scanned right now
//         ImGui::TextDisabled("%s", running ? "NOW SCANNING" : "LAST FILE SCANNED");
 
//         std::string dir, name;
//         SplitPath(scanStatus.currentFile, dir, name);
 
//         if (name.empty())
//         {
//             ImGui::TextDisabled("-");
//         }
//         else
//         {
//             ImGui::SetWindowFontScale(1.15f);
//             ImGui::PushStyleColor(ImGuiCol_Text, COLOR_TEXT);
//             ImGui::TextUnformatted(name.c_str());
//             ImGui::PopStyleColor();
//             ImGui::SetWindowFontScale(1.0f);
 
//             ImGui::PushStyleColor(ImGuiCol_Text, COLOR_MUTED);
//             ImGui::TextWrapped("%s", dir.c_str());
//             ImGui::PopStyleColor();
//         }
 
//         ImGui::Spacing();
 
//         // Live counters
//         ImGui::Text("Files: %llu", scanStatus.filesScanned);
//         ImGui::SameLine(0.0f, 18.0f);
//         ImGui::Text("Skipped: %llu", scanStatus.filesSkipped);
//         ImGui::SameLine(0.0f, 18.0f);
//         ImGui::Text("Data: %s", FormatBytes(scanStatus.bytesScanned).c_str());
//         ImGui::SameLine(0.0f, 18.0f);
//         ImGui::Text("Time: %s", FormatDuration(scanStatus.elapsedSeconds).c_str());
//     }
 
//     ImGui::EndChild();
 
//     ImGui::SameLine();
 
//     // ---------------- RIGHT: threats found ----------------
//     ImGui::BeginChild("ScanThreats", ImVec2(0.0f, panelH), true);
 
//     {
//         ImGui::PushStyleColor(ImGuiCol_Text, COLOR_CYAN);
//         ImGui::SetWindowFontScale(1.1f);
//         ImGui::TextUnformatted("Suspicious & malicious files");
//         ImGui::SetWindowFontScale(1.0f);
//         ImGui::PopStyleColor();
 
//         ImGui::TextColored(COLOR_RED, "Malicious: %d", scanStatus.malicious);
//         ImGui::SameLine(0.0f, 18.0f);
//         ImGui::TextColored(COLOR_YELLOW, "Suspicious: %d", scanStatus.suspicious);
 
//         ImGui::Separator();
 
//         if (scanFindings.empty())
//         {
//             ImGui::Spacing();
 
//             if (running)
//                 ImGui::TextDisabled("Nothing suspicious so far...");
//             else if (scanStatus.finished)
//                 ImGui::TextColored(COLOR_GREEN, "No threats found.");
//             else
//                 ImGui::TextDisabled("Findings will appear here.");
//         }
 
//         // Newest first
//         for (std::size_t i = scanFindings.size(); i-- > 0; )
//         {
//             const ScanFinding& f = scanFindings[i];
//             const ImVec4 c = (f.level == "Malicious") ? COLOR_RED : COLOR_YELLOW;
 
//             std::string dir, name;
//             SplitPath(f.path, dir, name);
 
//             ImGui::PushID((int)i);
 
//             ImGui::TextColored(c, "[%s]", f.level.c_str());
//             ImGui::SameLine();
//             ImGui::TextUnformatted(f.name.c_str());
 
//             ImGui::TextDisabled("%s", name.c_str());
 
//             if (ImGui::IsItemHovered())
//             {
//                 ImGui::BeginTooltip();
//                 ImGui::Text("%s", f.path.c_str());
//                 ImGui::Text("Reason: %s", f.reason.c_str());
//                 ImGui::Text("Score: %d", f.score);
//                 ImGui::EndTooltip();
//             }
 
//             ImGui::Separator();
//             ImGui::PopID();
//         }
//     }
 
//     ImGui::EndChild();
 
//     ImGui::PopStyleColor();   // ChildBg
 
//     ImGui::Spacing();
// <<<<<<< HEAD

//     SectionHeading("All findings", "hover a name for score and SHA-256");

//     if (ImGui::BeginTable("ScanFindings",5,
// =======
 
//     // ========================================================
//     // Full findings table (all details)
//     // ========================================================
//     SectionHeading("All findings", "hover a name for score and SHA-256");
 
//     if (ImGui::BeginTable(
//             "ScanFindings",
//             5,
// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);
//         ImGui::TableSetupColumn("Time",   ImGuiTableColumnFlags_WidthFixed, 80.0f);
//         ImGui::TableSetupColumn("Level",  ImGuiTableColumnFlags_WidthFixed, 95.0f);
//         ImGui::TableSetupColumn("Name",   ImGuiTableColumnFlags_WidthFixed, 190.0f);
//         ImGui::TableSetupColumn("File");
//         ImGui::TableSetupColumn("Reason");
//         ImGui::TableHeadersRow();
 
//         for (const ScanFinding& f : scanFindings)
//         {
//             ImGui::TableNextRow();
 
//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextDisabled("%s", f.time.c_str());
 
//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextColored(
// <<<<<<< HEAD
//                 f.level == "Malicious" ? COLOR_RED : COLOR_YELLOW,"%s", f.level.c_str());
// =======
//                 f.level == "Malicious" ? COLOR_RED : COLOR_YELLOW,
//                 "%s", f.level.c_str());
 
// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)
//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextUnformatted(f.name.c_str());
 
//             if (ImGui::IsItemHovered())
//             {
//                 ImGui::BeginTooltip();
//                 ImGui::Text("Score: %d", f.score);
//                 ImGui::Text("Size:  %s", FormatBytes(f.size).c_str());
 
//                 if (!f.sha256.empty())
//                     ImGui::Text("SHA-256: %s", f.sha256.c_str());
 
//                 ImGui::EndTooltip();
//             }
 
//             ImGui::TableSetColumnIndex(3);
//             ImGui::TextUnformatted(f.path.c_str());
 
//             ImGui::TableSetColumnIndex(4);
//             ImGui::TextUnformatted(f.reason.c_str());
//         }
 
//         ImGui::EndTable();
//     }
 
//     ImGui::EndChild();
// }
// <<<<<<< HEAD

// =======
 
// // ------------------------------------------------------------
// // LOGS & REPORTS PAGE
// // Lists every signature / rule the File Scanner checks for.
// // ------------------------------------------------------------
 
// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)
// static void DrawLogsReports()
// {
//     if (!LogsGate())
//         return;
 
//     static std::vector<SignatureInfo> signatures;
//     static bool loaded = false;
//     static char search[128] = "";
 
//     if (!loaded)
//     {
//         signatures = FileScanner::getSignatures();
//         loaded = true;
//     }
 
//     int hashCount = 0, contentCount = 0, nameCount = 0;
 
//     for (const SignatureInfo& s : signatures)
//     {
//         if (s.category == "Known hash")        ++hashCount;
//         else if (s.category == "File content") ++contentCount;
//         else                                   ++nameCount;
//     }
 
//     ImGui::BeginChild("LogsPanel", ImVec2(0.0f, 0.0f), true);
 
//     SectionHeading("Detection signatures", "what the File Scanner checks for");
 
//     if (ImGui::Button("Lock now"))
//     {
//         logsUnlocked = false;
//         lockMessage.clear();
//     }
 
//     ImGui::SameLine();
//     ImGui::TextDisabled("Locks again after 5 minutes idle or when you leave this page.");
 
//     if (ImGui::CollapsingHeader("Change password"))
//     {
//         static std::string changeMessage;
//         static bool        changeOk = false;
 
//         ImGui::SetNextItemWidth(260.0f);
//         ImGui::InputTextWithHint("##oldpw", "Current password",
//             lockOld, sizeof(lockOld), ImGuiInputTextFlags_Password);
 
//         ImGui::SetNextItemWidth(260.0f);
//         ImGui::InputTextWithHint("##chnew", "New password",
//             lockNew, sizeof(lockNew), ImGuiInputTextFlags_Password);
 
//         ImGui::SetNextItemWidth(260.0f);
//         ImGui::InputTextWithHint("##chconf", "Confirm new password",
//             lockConfirm, sizeof(lockConfirm), ImGuiInputTextFlags_Password);
 
//         if (ImGui::Button("Update password"))
//         {
//             if (std::strcmp(lockNew, lockConfirm) != 0)
//             {
//                 changeMessage = "New passwords do not match.";
//                 changeOk      = false;
//             }
//             else
//             {
//                 changeOk = backend.setPassword(lockNew, lockOld, changeMessage);
 
//                 if (changeOk)
//                 {
//                     Wipe(lockOld, sizeof(lockOld));
//                     Wipe(lockNew, sizeof(lockNew));
//                     Wipe(lockConfirm, sizeof(lockConfirm));
//                 }
//             }
//         }
 
//         if (!changeMessage.empty())
//             ImGui::TextColored(changeOk ? COLOR_GREEN : COLOR_RED,
//                 "%s", changeMessage.c_str());
//     }
 
//     ImGui::Spacing();
 
//     ImGui::PushTextWrapPos(0.0f);
//     ImGui::TextColored(COLOR_MUTED,
//         "A file is reported Malicious on an instant match (EICAR test string or a "
//         "known-bad SHA-256). Otherwise every matching rule adds its weight to a "
//         "score, and a file scoring %d or more is reported Suspicious.",
//         FileScanner::suspiciousThreshold());
//     ImGui::PopTextWrapPos();
 
//     ImGui::Spacing();
 
//     ImGui::Text("%d rules", (int)signatures.size());
//     ImGui::SameLine(0.0f, 18.0f);
//     ImGui::TextColored(COLOR_RED,    "Known hashes: %d", hashCount);
//     ImGui::SameLine(0.0f, 18.0f);
//     ImGui::TextColored(COLOR_CYAN,   "File content: %d", contentCount);
//     ImGui::SameLine(0.0f, 18.0f);
//     ImGui::TextColored(COLOR_YELLOW, "Name / location: %d", nameCount);
 
//     ImGui::Spacing();
 
//     ImGui::SetNextItemWidth(300.0f);
//     ImGui::InputTextWithHint("##sigsearch", "Search signatures", search, IM_ARRAYSIZE(search));
 
//     ImGui::SameLine();
 
//     if (ImGui::Button("Reload"))
//         signatures = FileScanner::getSignatures();   // re-reads hids_hashes.txt
 
//     ImGui::Spacing();
// <<<<<<< HEAD

//     if (ImGui::BeginTable("SignatureTable",4,
// =======
 
//     if (ImGui::BeginTable(
//             "SignatureTable",
//             4,
// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);
//         ImGui::TableSetupColumn("Type",      ImGuiTableColumnFlags_WidthFixed, 160.0f);
//         ImGui::TableSetupColumn("Name",      ImGuiTableColumnFlags_WidthStretch, 1.0f);
//         ImGui::TableSetupColumn("Pattern / condition", ImGuiTableColumnFlags_WidthStretch, 1.4f);
//         ImGui::TableSetupColumn("Result",    ImGuiTableColumnFlags_WidthFixed, 100.0f);
//         ImGui::TableHeadersRow();
 
//         int shown = 0;
 
//         for (const SignatureInfo& s : signatures)
//         {
//             if (!ContainsNoCase(s.name.c_str(), search) &&
//                 !ContainsNoCase(s.pattern.c_str(), search) &&
//                 !ContainsNoCase(s.category.c_str(), search))
//                 continue;
 
//             ++shown;
 
//             ImGui::TableNextRow();
 
//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextColored(
//                 s.category == "Known hash"   ? COLOR_RED :
//                 s.category == "File content" ? COLOR_CYAN : COLOR_YELLOW,
//                 "%s", s.category.c_str());
 
//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextUnformatted(s.name.c_str());
 
//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextDisabled("%s", s.pattern.c_str());
 
//             ImGui::TableSetColumnIndex(3);
 
//             if (s.instantMalicious)
//                 ImGui::TextColored(COLOR_RED, "Malicious");
//             else
//                 ImGui::TextColored(
//                     s.weight >= 4 ? COLOR_ORANGE :
//                     s.weight >= 2 ? COLOR_YELLOW : COLOR_MUTED,
//                     "+%d score", s.weight);
//         }
 
//         if (shown == 0)
//         {
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextDisabled("No signature matches \"%s\".", search);
//         }
 
//         ImGui::EndTable();
//     }
 
//     ImGui::EndChild();
// }
// <<<<<<< HEAD
// =======
 
// // ------------------------------------------------------------
// // NOTIFICATION SETTINGS (shown on the Settings page)
// // ------------------------------------------------------------
// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)
 
// static void DrawNotificationSettings()
// {
//     ImGui::BeginChild("NotificationSettings", ImVec2(0.0f, 150.0f), true,
//                       ImGuiWindowFlags_NoScrollbar);
 
//     SectionHeading("Desktop notifications",
//         "pop-up toasts when a new alert is raised");
 
//     bool enabled = backend.notificationsEnabled();
 
//     if (ImGui::Checkbox("Show desktop notifications", &enabled))
//         backend.setNotificationsEnabled(enabled);
 
//     ImGui::BeginDisabled(!enabled);
 
//     static const char* levels[] = {
//         "Low and above", "Medium and above",
//         "High and above", "Critical only"
//     };
 
//     int level = backend.notificationMinSeverity();
 
//     ImGui::SetNextItemWidth(220.0f);
 
//     if (ImGui::Combo("Notify for", &level, levels, IM_ARRAYSIZE(levels)))
//         backend.setNotificationMinSeverity(level);
 
//     if (ImGui::Button("Send test notification"))
//         backend.sendTestNotification();
 
//     ImGui::EndDisabled();
 
//     ImGui::EndChild();
// }
 
// static void DrawMainContent()
// {
//     RefreshFromBackend();
 
//     DrawTopBar();
//     ImGui::Separator();
//     ImGui::Spacing();
 
//     // Leaving Logs & Reports locks it again; page changes restart the fade
//     static int    lastPage      = -1;
//     static double pageChangedAt = 0.0;
 
//     if (selectedPage != lastPage)
//     {
//         if (lastPage == LOGS_PAGE_INDEX)
//         {
//             logsUnlocked = false;
//             lockMessage.clear();
//             Wipe(lockOld, sizeof(lockOld));
//             Wipe(lockNew, sizeof(lockNew));
//             Wipe(lockConfirm, sizeof(lockConfirm));
//         }
 
//         lastPage      = selectedPage;
//         pageChangedAt = ImGui::GetTime();
//     }
 
//     // Demo banner: impossible to miss, so sample rows are never mistaken for real ones
//     if (demoMode)
//     {
//         DrawPill("DEMO DATA", COLOR_YELLOW);
//         ImGui::SameLine();
//         ImGui::TextColored(COLOR_YELLOW,
//             "Rows tagged [DEMO] are samples, not from this machine. Turn off in Settings.");
//         ImGui::Spacing();
//     }
 
//     // Quick fade-in when switching pages
//     const float fade = ClampFloat(
//         (float)((ImGui::GetTime() - pageChangedAt) / 0.18), 0.0f, 1.0f);
 
//     ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.25f + 0.75f * fade);
 
//     switch (selectedPage)
//     {
//     case 0:
//         DrawMetrics();
//         ImGui::Spacing();
//         DrawThreatPanel();
//         ImGui::Spacing();
//         DrawAlerts(true);
//         ImGui::Spacing();
//         DrawProcesses();
//         break;
 
//     case 1:
//         DrawProcesses();
//         break;
 
//     case 2:
//         DrawEventPage("Failed sign-in attempts", loginEvents,
//             "No failed sign-ins recorded since monitoring started.",
//             "Reads the Windows Security log - run as Administrator");
//         break;
 
//     case 3:
//         DrawEventPage("File activity", fileEvents,
//             "No changes detected in the watched folder yet.",
//             "Watching the HIDS_Monitor folder");
//         break;
 
//     case 4:
//         DrawAlerts(false);
//         break;
 
//     case 5:
//         DrawLogsReports();
//         break;
 
//     case 6:
//         DrawSystemAnalysis();
//         break;
 
//     case 7:
//         DrawFileScanner();
//         break;
 
//     case 8:
//         DrawNotificationSettings();
//         ImGui::Spacing();
//         DrawSettingsPage(monitoringActive);
//         break;
//     }
 
//     ImGui::PopStyleVar();
// }
 
// int main()
// {
//     if (!glfwInit())
//     {
//         std::fprintf(stderr, "ERROR: Failed to initialize GLFW.\n");
//         return 1;
//     }
 
//     // Request OpenGL 3.0
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
 
//     GLFWwindow* window = glfwCreateWindow(
//         1400,
//         900,
//         "HIDS - Host Intrusion Detection System",
//         nullptr,
//         nullptr
//     );
 
//     if (window == nullptr)
//     {
//         std::fprintf(stderr, "ERROR: Failed to create dashboard window.\n");
//         glfwTerminate();
//         return 1;
//     }
 
//     glfwMakeContextCurrent(window);
//     glfwSwapInterval(1);
 
//     // Native window handle for the tray icon / desktop notifications
// #ifdef _WIN32
//     backend.attachWindow(glfwGetWin32Window(window));
// #endif
 
//     IMGUI_CHECKVERSION();
//     ImGui::CreateContext();
 
//     ImGuiIO& io = ImGui::GetIO();
 
//     io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
 
//     // Don't write imgui.ini next to the executable
//     io.IniFilename = nullptr;
 
//     LoadFonts(io);
//     LoadUiConfig();           // saved theme / interface size / demo flag
//     ApplyTheme(themeIndex);   // also builds the ImGui style
//     InitUiScale(io);
 
//     // Seed sparkline history so the cards aren't empty on first frame
//     for (int i = 0; i < HISTORY_SIZE; ++i)
//     {
//         cpuHistory[i] = cpuNow;
//         ramHistory[i] = ramNow;
//     }
 
//     ImGui_ImplGlfw_InitForOpenGL(window, true);
//     ImGui_ImplOpenGL3_Init("#version 130");
 
//     // Start the monitors (login, process, file) on background threads
//     backend.start();
 
//     // Main application loop
//     while (!glfwWindowShouldClose(window))
//     {
//         glfwPollEvents();
 
//         // Send queued desktop notifications (must run on the UI thread)
//         backend.flushNotifications();
 
//         // Theme clicks are queued and applied here, never mid-frame
//         ApplyPendingTheme();
 
//         ImGui_ImplOpenGL3_NewFrame();
//         ImGui_ImplGlfw_NewFrame();
//         ImGui::NewFrame();
 
//         // Gradient backdrop; the root window is transparent so it shows through
//         DrawBackdrop();
//         ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
 
//         // Full-window root
//         ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
//         ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
 
//         ImGui::Begin(
//             "HIDSRoot",
//             nullptr,
//             ImGuiWindowFlags_NoTitleBar |
//             ImGuiWindowFlags_NoResize |
//             ImGuiWindowFlags_NoMove |
//             ImGuiWindowFlags_NoCollapse |
//             ImGuiWindowFlags_NoScrollbar |
//             ImGuiWindowFlags_NoBringToFrontOnFocus
//         );
 
//         DrawSidebar();
//         ImGui::SameLine();
 
//         ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
 
//         ImGui::BeginChild(
//             "MainContent",
//             ImVec2(0.0f, 0.0f),
//             false
//         );
 
//         DrawMainContent();
 
//         ImGui::EndChild();
//         ImGui::PopStyleColor();
 
//         ImGui::End();
//         ImGui::PopStyleColor();   // WindowBg
 
//         ImGui::Render();
 
//         int displayWidth = 0;
//         int displayHeight = 0;
 
//         glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
//         glViewport(0, 0, displayWidth, displayHeight);
 
//         glClearColor(
//             HIDS_COLOR_BACKGROUND.x,
//             HIDS_COLOR_BACKGROUND.y,
//             HIDS_COLOR_BACKGROUND.z,
//             HIDS_COLOR_BACKGROUND.w
//         );
 
//         glClear(GL_COLOR_BUFFER_BIT);
 
//         ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
 
//         glfwSwapBuffers(window);
//     }
 
//     backend.shutdown();
 
//     ImGui_ImplOpenGL3_Shutdown();
//     ImGui_ImplGlfw_Shutdown();
//     ImGui::DestroyContext();
 
//     glfwDestroyWindow(window);
//     glfwTerminate();
 
//     return 0;
// }
// <<<<<<< HEAD
// =======
 
// >>>>>>> 84ad615 (Update HIDS monitoring, backend and dashboard)




// #include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"
// #include <GLFW/glfw3.h>
 
// // Native window handle (needed for the tray icon / desktop notifications)
// #ifdef _WIN32
// #include <windows.h>
// #define GLFW_EXPOSE_NATIVE_WIN32
// #include <GLFW/glfw3native.h>
// #endif
 
// #include <algorithm>
// #include <cctype>
// #include <cfloat>
// #include <cmath>
// #include <cstdio>
// #include <cstdlib>
// #include <cstring>
// #include <set>
// #include <string>
// #include <vector>
// #include "HIDSBackend.h"
// #include "DashboardTheme.h"   // themes, colours, icons, Settings page
 
// // ------------------------------------------------------------
// // DATA STRUCTURES
// // ------------------------------------------------------------
 
// struct ProcessInfo {
//     int pid;
//     std::string name;
//     float cpu;
//     float memory;
//     double runningSeconds;
//     std::string runningTime;   // formatted for display
//     bool isApp = false;        // can be ended from the Processes page
//     std::string title;         // window title (apps only)
// };
 
// struct AlertInfo {
//     int id = 0;
//     std::string time;
//     std::string severity;
//     std::string source;
//     std::string description;
 
//     bool        canEndTask = false;   // resource alert with a culprit app
//     int         culpritPid = 0;
//     std::string culpritName;
//     float       culpritMem = 0.0f;
// };
 
// struct EventInfo {
//     std::string time;
//     std::string type;
//     std::string description;
// };

// static HIDSBackend backend;
 
// static std::vector<ProcessInfo> processes;
// static std::vector<AlertInfo>   alerts;
// static std::vector<EventInfo>   loginEvents;
// static std::vector<EventInfo>   fileEvents;
// static BackendAnalysis          analysisData;
 
// // File scanner page state
// static ScanStatus               scanStatus;
// static std::vector<ScanFinding> scanFindings;
// static int                      scanTypeSel = 0;      // 0 Quick, 1 Full, 2 Custom
// static char                     scanPath[512] = "";
// static std::string              scanMessage;
// static int processCount = 0;

// static int  selectedPage = 0;
// static char processSearch[128] = "";
// static bool monitoringActive = true;
 
// static int  severityFilter = 0;   // 0 = All, 1..4 = Critical..Low
// static const char* severityFilterLabels[] =
//     { "All", "Critical", "High", "Medium", "Low" };
// static char alertSearch[128] = "";
 
// static const char* pages[] = {
//     "Dashboard",
//     "Processes",
//     "Login Monitoring",
//     "File Monitoring",
//     "Alerts",
//     "Logs & Reports",
//     "System Analysis",
//     "File Scanner",
//     "Settings"
// };
 
// static const char* pageSubtitles[] = {
//     "Overview of this machine's security status",
//     "Everything currently running on this host",
//     "Sign-in activity on this host",
//     "Changes to monitored files and folders",
//     "Events raised by the HIDS monitors",
//     "Detection signatures and scan reports",
//     "Deeper inspection of system behaviour",
//     "Scan files for known malware and suspicious traits",
//     "Appearance, interface size, notifications and data source"
// };
 
// // Live metrics (ring buffers feed the sparklines)
// static const int HISTORY_SIZE = 60;
// static float cpuHistory[HISTORY_SIZE] = {};
// static float ramHistory[HISTORY_SIZE] = {};
// static int   historyOffset = 0;
// static float cpuNow = 0.0f;
// static float ramNow = 0.0f;
 
// // Threat activity (feeds the dashboard histogram)
// static float threatHistory[HISTORY_SIZE] = {};
// static float threatAccum = 0.0f;
 
// static float ClampFloat(float v, float lo, float hi)
// {
//     return v < lo ? lo : (v > hi ? hi : v);
// }
 
// // ------------------------------------------------------------
// // FONT LOADING
// // Uses a real TTF if one is found; otherwise scales the default
// // bitmap font so the dashboard is still readable.
// // ------------------------------------------------------------
 
// static bool FileExists(const char* path)
// {
//     FILE* f = std::fopen(path, "rb");
//     if (f == nullptr)
//         return false;
 
//     std::fclose(f);
//     return true;
// }
 
// static void LoadFonts(ImGuiIO& io)
// {
//     const char* candidates[] = {
//         "C:/Windows/Fonts/segoeui.ttf",
//         "C:/Windows/Fonts/arial.ttf",
//         "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
//         "/usr/share/fonts/TTF/DejaVuSans.ttf",
//         "/System/Library/Fonts/Supplemental/Arial.ttf"
//     };
 
//     for (const char* path : candidates)
//     {
//         if (!FileExists(path))
//             continue;
 
//         ImFont* font = io.Fonts->AddFontFromFileTTF(path, 18.0f);
 
//         if (font != nullptr)
//         {
//             io.FontDefault = font;
//             io.FontGlobalScale = 1.0f;
//             return;
//         }
//     }
 
//     io.Fonts->AddFontDefault();
//     io.FontGlobalScale = 1.25f;
// }

// static bool ContainsNoCase(const char* text, const char* search)
// {
//     if (search[0] == '\0')
//         return true;
 
//     const size_t textLen = std::strlen(text);
//     const size_t searchLen = std::strlen(search);
 
//     if (searchLen > textLen)
//         return false;
 
//     for (size_t i = 0; i + searchLen <= textLen; ++i)
//     {
//         size_t j = 0;
 
//         while (j < searchLen &&
//                std::tolower((unsigned char)text[i + j]) ==
//                std::tolower((unsigned char)search[j]))
//         {
//             ++j;
//         }
 
//         if (j == searchLen)
//             return true;
//     }
 
//     return false;
// }
 
// static ImVec4 SeverityColor(const char* severity)
// {
//     if (std::strcmp(severity, "Critical") == 0) return COLOR_RED;
//     if (std::strcmp(severity, "High") == 0)     return COLOR_ORANGE;
//     if (std::strcmp(severity, "Medium") == 0)   return COLOR_YELLOW;
 
//     return COLOR_CYAN;
// }
 
// static int CountSeverity(const char* severity)
// {
//     int count = 0;
 
//     for (const auto& alert : alerts)
//         if (alert.severity == severity)
//             ++count;
 
//     return count;
// }
 
// // Usage colour: green -> yellow -> red as load rises
// static ImVec4 LoadColor(float percent)
// {
//     if (percent >= 85.0f) return COLOR_RED;
//     if (percent >= 65.0f) return COLOR_YELLOW;
 
//     return COLOR_GREEN;
// }
 
// static void SectionHeading(const char* title, const char* hint = nullptr)
// {
//     {
//         const ImVec2 p = ImGui::GetCursorScreenPos();
//         const float  h = ImGui::GetTextLineHeight() * 1.1f;
 
//         ImGui::GetWindowDrawList()->AddRectFilled(
//             ImVec2(p.x, p.y + 1.0f), ImVec2(p.x + 4.0f, p.y + h),
//             ImGui::GetColorU32(COLOR_CYAN), 2.0f);
 
//         ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 12.0f);
//     }
 
//     ImGui::PushStyleColor(ImGuiCol_Text, COLOR_CYAN);
//     ImGui::SetWindowFontScale(1.1f);
//     ImGui::TextUnformatted(title);
//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::PopStyleColor();
 
//     if (hint != nullptr)
//     {
//         ImGui::SameLine();
//         ImGui::TextDisabled("  %s", hint);
//     }
 
//     ImGui::Spacing();
// }
 
// // Coloured pill, e.g. for alert severity
// static void SeverityBadge(const char* severity)
// {
//     const ImVec4 color = SeverityColor(severity);
//     const ImVec2 textSize = ImGui::CalcTextSize(severity);
//     const ImVec2 pad(10.0f, 2.0f);
 
//     const ImVec2 pos = ImGui::GetCursorScreenPos();
//     const ImVec2 size(textSize.x + pad.x * 2.0f,textSize.y + pad.y * 2.0f);
 
//     ImDrawList* draw = ImGui::GetWindowDrawList();
 
//     draw->AddRectFilled(
//         pos,
//         ImVec2(pos.x + size.x, pos.y + size.y),
//         ImGui::GetColorU32(WithAlpha(color, 0.16f)),
//         size.y * 0.5f
//     );
 
//     draw->AddRect(
//         pos,
//         ImVec2(pos.x + size.x, pos.y + size.y),
//         ImGui::GetColorU32(WithAlpha(color, 0.65f)),
//         size.y * 0.5f
//     );
 
//     draw->AddText(
//         ImVec2(pos.x + pad.x, pos.y + pad.y),
//         ImGui::GetColorU32(color),
//         severity
//     );
 
//     ImGui::Dummy(size);
// }
 
// // Pulsing dot used by the monitoring status
// static void StatusDot(const ImVec4& color, bool pulse)
// {
//     const float radius = 5.0f;
//     const ImVec2 pos = ImGui::GetCursorScreenPos();
//     const ImVec2 center(pos.x + radius + 2.0f, pos.y + ImGui::GetFrameHeight() * 0.5f);
 
//     ImDrawList* draw = ImGui::GetWindowDrawList();
 
//     if (pulse)
//     {
//         const float t = (float)ImGui::GetTime();
//         const float wave = 0.5f + 0.5f * std::sin(t * 3.0f);
 
//         draw->AddCircleFilled(
//             center,
//             radius + 3.0f + wave * 3.0f,
//             ImGui::GetColorU32(WithAlpha(color, 0.18f * (1.0f - wave) + 0.05f))
//         );
//     }
 
//     draw->AddCircleFilled(center, radius, ImGui::GetColorU32(color));
 
//     ImGui::Dummy(ImVec2(radius * 2.0f + 6.0f, ImGui::GetFrameHeight()));
// }
 
// static std::string FormatDuration(double seconds)
// {
//     const unsigned long long total =
//         seconds > 0.0 ? (unsigned long long)seconds : 0ULL;
//         char text[32];
 
//     std::snprintf(
//         text, sizeof(text), "%02llu:%02llu:%02llu",
//         total / 3600ULL, (total / 60ULL) % 60ULL, total % 60ULL
//     );
//     return text;
// }
 
// static float SeverityWeight(const std::string& s)
// {
//     if (s == "Critical") return 4.0f;
//     if (s == "High")     return 3.0f;
//     if (s == "Medium")   return 2.0f;
//     return 1.0f;
// }
 
// // Sample rows. Always labelled so they can't be mistaken for real ones:
// // source "DEMO", "[DEMO]" prefix, negative IDs/PIDs, no End Task button.
// static void AppendDemoData()
// {
//     struct DemoAlert { const char* sev; const char* text; };
 
//     static const DemoAlert demoAlerts[] = {
//         { "Critical", "[DEMO] Known-bad hash match - C:\\Demo\\sample.exe" },
//         { "High",     "[DEMO] Multiple Failed Logins - 5 attempts in 60 s" },
//         { "Medium",   "[DEMO] Suspicious PowerShell Process - powershell.exe" },
//         { "Low",      "[DEMO] Password Changed - sample entry" }
//     };
 
//     int id = -1;
 
//     for (const DemoAlert& d : demoAlerts)
//     {
//         AlertInfo a;
//         a.id          = id--;
//         a.time        = "--:--:--";
//         a.severity    = d.sev;
//         a.source      = "DEMO";
//         a.description = d.text;
//         alerts.push_back(a);              // after the real ones
//     }
 
//     ProcessInfo p;
//     p.pid            = -1;
//     p.name           = "demo_service.exe [DEMO]";
//     p.cpu            = 12.5f;
//     p.memory         = 640.0f;
//     p.runningSeconds = 4000.0;
//     p.runningTime    = FormatDuration(p.runningSeconds);
//     p.isApp          = false;             // can never be ended
//     processes.push_back(p);
 
//     loginEvents.push_back({ "--:--:--", "LOGIN_FAILURE",
//         "[DEMO] Failed sign-in for user 'demo'" });
//     fileEvents.push_back({ "--:--:--", "FILE_CREATED",
//         "[DEMO] HIDS_Monitor\\sample_script.ps1" });
// }
 
// static void RefreshFromBackend()
// {
//     static double lastRefresh = -1.0;
//     static unsigned long lastSample = 0;
 
//     const double now = ImGui::GetTime();
 
//     if (lastRefresh >= 0.0 && now - lastRefresh < 0.5)
//         return;
 
//     lastRefresh = now;
 
//     // CPU, RAM and process count
//     const BackendStats stats = backend.getStats();
 
//     cpuNow = stats.cpuPercent;
//     ramNow = stats.ramPercent;
//     processCount = stats.processCount;
 
//     if (stats.sample != lastSample)
//     {
//         lastSample = stats.sample;
 
//         cpuHistory[historyOffset]    = cpuNow;
//         ramHistory[historyOffset]    = ramNow;
//         threatHistory[historyOffset] = threatAccum;
//         threatAccum = 0.0f;
//         historyOffset = (historyOffset + 1) % HISTORY_SIZE;
//     }
 
//     analysisData = backend.getAnalysis();
//     processes.clear();
 
//     for (const BackendProcess& p : backend.getProcesses())
//     {
//         ProcessInfo row;
 
//         row.pid            = p.pid;
//         row.name           = p.name;
//         row.cpu            = p.cpuPercent;
//         row.memory         = p.memoryMB;
//         row.runningSeconds = p.runningSeconds;
//         row.runningTime    = FormatDuration(p.runningSeconds);
//         row.isApp          = p.isApp;
//         row.title          = p.windowTitle;
 
//         processes.push_back(row);
//     }
//     alerts.clear();
 
//     for (const BackendAlert& a : backend.getAlerts())
//     {
//         AlertInfo row;
 
//         row.time        = a.time;
//         row.severity    = a.severity;
//         row.source      = a.source;
//         row.description = a.description;
//         row.id          = a.id;
//         row.canEndTask  = a.canEndTask;
//         row.culpritPid  = a.culpritPid;
//         row.culpritName = a.culpritName;
//         row.culpritMem  = a.culpritMemoryMB;
 
//         alerts.push_back(row);
//     }
 
//     // Weigh alerts that appeared since the last refresh
//     {
//         static std::set<int> seenAlerts;
//         static bool seeded = false;
 
//         for (const AlertInfo& a : alerts)
//             if (seenAlerts.insert(a.id).second && seeded)
//                 threatAccum += SeverityWeight(a.severity);
 
//         seeded = true;   // alerts already present at start-up aren't a "spike"
//     }
 
//     // Login and file activity (already newest first)
//     loginEvents.clear();
//     fileEvents.clear();
 
//     for (const BackendEvent& e : backend.getEvents())
//     {
//         EventInfo row;
 
//         row.time        = e.time;
//         row.type        = e.type;
//         row.description = e.description;
 
//         if (e.type.rfind("LOGIN_", 0) == 0)
//             loginEvents.push_back(row);
//         else if (e.type.rfind("FILE_", 0) == 0)
//             fileEvents.push_back(row);
//     }
 
//     if (demoMode)
//         AppendDemoData();
// }
 
// // ------------------------------------------------------------
// // METRIC CARD
// // Accent stripe on the left, big value, optional sparkline.
// // linkPage >= 0 makes the card clickable.
// // ------------------------------------------------------------
 
// static void MetricCard(
//     const char* title,
//     const char* value,
//     const char* subtitle,
//     const ImVec4& accent,
//     float width,
//     const float* history = nullptr,
//     int linkPage = -1,
//     float gauge = -1.0f)          // 0..100 draws a ring gauge
// {
//     ImGui::BeginChild(
//         title,
//         ImVec2(width, 142.0f),
//         true,
//         ImGuiWindowFlags_NoScrollbar
//     );
 
//     // Accent stripe
//     {
//         const ImVec2 pos = ImGui::GetWindowPos();
//         const ImVec2 size = ImGui::GetWindowSize();
 
//         ImGui::GetWindowDrawList()->AddRectFilled(pos,ImVec2(pos.x + 4.0f, pos.y + size.y),
//             ImGui::GetColorU32(accent),
//             ImGui::GetStyle().ChildRounding,
//             ImDrawFlags_RoundCornersLeft
//         );
//     }
 
//     // Soft glow + ring gauge (or page icon) on the right side
//     {
//         const ImVec2 wp = ImGui::GetWindowPos();
//         const ImVec2 ws = ImGui::GetWindowSize();
//         ImDrawList*  d  = ImGui::GetWindowDrawList();
//         const ImVec2 gc(wp.x + ws.x - 54.0f, wp.y + 62.0f);
 
//         DrawGlow(d, gc, 46.0f, accent);
 
//         if (gauge >= 0.0f)
//             DrawGauge(d, gc, 24.0f, 6.0f, gauge / 100.0f, accent);
//         else if (linkPage >= 0)
//             DrawPageIcon(d, gc, 14.0f, linkPage,
//                 ImGui::GetColorU32(WithAlpha(accent, 0.85f)));
//     }
 
//     ImGui::TextDisabled("%s", title);
//     ImGui::PushStyleColor(ImGuiCol_Text, accent);
//     ImGui::SetWindowFontScale(1.55f);
//     ImGui::TextUnformatted(value);
//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::PopStyleColor();
 
//     if (history != nullptr)
//     {
//         DrawAreaChart("##spark", history, HISTORY_SIZE, historyOffset,
//             100.0f, ImVec2(ImGui::GetContentRegionAvail().x, 32.0f),
//             accent, "%", false);
//     }
//     else
//     {
//         ImGui::Spacing();
//         ImGui::TextDisabled("%s", subtitle);
//     }
 
//     // Hover highlight + click to open the related page
//     if (linkPage >= 0 && ImGui::IsWindowHovered())
//     {
//         const ImVec2 pos  = ImGui::GetWindowPos();
//         const ImVec2 size = ImGui::GetWindowSize();
 
//         ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
 
//         ImGui::GetWindowDrawList()->AddRect(
//             pos, ImVec2(pos.x + size.x, pos.y + size.y),
//             ImGui::GetColorU32(WithAlpha(accent, 0.85f)),
//             ImGui::GetStyle().ChildRounding, 0, 1.5f);
 
//         ImGui::SetTooltip("Open %s", pages[linkPage]);
 
//         if (ImGui::IsMouseClicked(0))
//             selectedPage = linkPage;
//     }
 
//     ImGui::EndChild();
// }

// static void DrawSidebar()
// {
//     ImGui::PushStyleColor(ImGuiCol_ChildBg, COLOR_SIDEBAR);
 
//     ImGui::BeginChild(
//         "Sidebar",
//         ImVec2(220.0f, 0.0f),
//         true
//     );
 
//     ImGui::PushStyleColor(ImGuiCol_Text, COLOR_CYAN);
//     ImGui::SetWindowFontScale(1.45f);
//     ImGui::TextUnformatted("HIDS");
//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::PopStyleColor();
 
//     ImGui::TextDisabled("Host Intrusion Detection");
 
//     ImGui::Spacing();
//     ImGui::Separator();
//     ImGui::Spacing();
 
//     const int alertCount = (int)alerts.size();
 
//     for (int i = 0; i < IM_ARRAYSIZE(pages); ++i)
//     {
//         ImGui::PushID(i);
 
//         const bool selected = (selectedPage == i);
 
//         if (!selected)
//             ImGui::PushStyleColor(ImGuiCol_Text, MixColor(COLOR_MUTED, COLOR_TEXT, 0.45f));
 
//         // Leading spaces leave room for the icon
//         char label[64];
//         std::snprintf(label, sizeof(label), "        %s", pages[i]);
 
//         if (ImGui::Selectable(label, selected, 0, ImVec2(0.0f, 38.0f)))
//             selectedPage = i;
 
//         if (!selected)
//             ImGui::PopStyleColor();
 
//         const ImVec2 itemMin = ImGui::GetItemRectMin();
//         const ImVec2 itemMax = ImGui::GetItemRectMax();
//         ImDrawList* draw = ImGui::GetWindowDrawList();
 
//         const bool hovered = ImGui::IsItemHovered();
 
//         if (hovered)
//             ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
 
//         DrawPageIcon(draw,
//             ImVec2(itemMin.x + 24.0f, (itemMin.y + itemMax.y) * 0.5f),
//             8.0f, i,
//             ImGui::GetColorU32(selected ? COLOR_CYAN
//                                : hovered ? COLOR_TEXT : COLOR_MUTED));
 
//         if (selected)
//         {
//             draw->AddRectFilled(
//                 itemMin,
//                 ImVec2(itemMin.x + 4.0f, itemMax.y),
//                 ImGui::GetColorU32(COLOR_CYAN),
//                 2.0f
//             );
//         }
 
//         // Alert count badge next to "Alerts"
//         if (std::strcmp(pages[i], "Alerts") == 0 && alertCount > 0)
//         {
//             char count[16];
//             std::snprintf(count, sizeof(count), "%d", alertCount);
 
//             const ImVec2 ts = ImGui::CalcTextSize(count);
//             const float w = ts.x + 14.0f;
//             const float h = ts.y + 2.0f;
//             const float x = itemMax.x - w - 8.0f;
//             const float y = itemMin.y + (itemMax.y - itemMin.y - h) * 0.5f;
 
//             draw->AddRectFilled(
//                 ImVec2(x, y), ImVec2(x + w, y + h),
//                 ImGui::GetColorU32(WithAlpha(COLOR_RED, 0.9f)),
//                 h * 0.5f
//             );
 
//             draw->AddText(
//                 ImVec2(x + 7.0f, y + 1.0f),
//                 IM_COL32(255, 255, 255, 255),
//                 count
//             );
//         }
 
//         ImGui::PopID();
//     }
 
//     ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 84.0f);
 
//     ImGui::Separator();
//     DataSourceBadge(monitoringActive);
//     ImGui::TextDisabled("HIDS Prototype v1.1");
 
//     ImGui::EndChild();
 
//     ImGui::PopStyleColor();
// }
 
// static void DrawTopBar()
// {
//     ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
 
//     ImGui::BeginChild(
//         "TopBar",
//         ImVec2(0.0f, 58.0f),
//         false,
//         ImGuiWindowFlags_NoScrollbar
//     );
 
//     ImGui::BeginGroup();
 
//     ImGui::SetWindowFontScale(1.30f);
//     ImGui::TextUnformatted(pages[selectedPage]);
//     ImGui::SetWindowFontScale(1.0f);
 
//     ImGui::TextDisabled("%s", pageSubtitles[selectedPage]);
 
//     ImGui::EndGroup();
 
//     // Right-aligned status + button
//     const char* statusText =
//         monitoringActive ? "Monitoring active" : "Monitoring stopped";
 
//     const ImVec4 statusColor =
//         monitoringActive ? COLOR_GREEN : COLOR_RED;
 
//     const float statusWidth =
//         ImGui::CalcTextSize(statusText).x + 24.0f;
 
//     const float buttonWidth = 120.0f;
 
//     ImGui::SameLine(
//         ImGui::GetWindowWidth() - statusWidth - buttonWidth - 28.0f
//     );
 
//     ImGui::AlignTextToFramePadding();
//     StatusDot(statusColor, monitoringActive);
 
//     ImGui::SameLine(0.0f, 4.0f);
 
//     ImGui::PushStyleColor(ImGuiCol_Text, statusColor);
//     ImGui::AlignTextToFramePadding();
//     ImGui::TextUnformatted(statusText);
//     ImGui::PopStyleColor();
 
//     ImGui::SameLine();
 
//     if (monitoringActive)
//     {
//         ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.70f, 0.14f, 0.18f, 1.0f));
//         ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COLOR_RED);
 
//         if (ImGui::Button("Stop HIDS", ImVec2(buttonWidth, 34.0f)))
//         {
//             backend.stop();
//             monitoringActive = false;
//         }
 
//         ImGui::PopStyleColor(2);
//     }
//     else
//     {
//         ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.55f, 0.36f, 1.0f));
//         ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COLOR_GREEN);
 
//         if (ImGui::Button("Start HIDS", ImVec2(buttonWidth, 34.0f)))
//         {
//             backend.start();
//             monitoringActive = true;
//         }
 
//         ImGui::PopStyleColor(2);
//     }
 
//     ImGui::EndChild();
 
//     ImGui::PopStyleColor();
// }
 
// static void DrawMetrics()
// {
//     const float spacing = ImGui::GetStyle().ItemSpacing.x;
 
//     const float cardWidth =
//         (ImGui::GetContentRegionAvail().x - spacing * 3.0f) / 4.0f;
 
//     char cpuText[16];
//     char ramText[16];
//     char alertText[16];
 
//     // Displayed values glide toward the live ones instead of jumping
//     static float cpuShown = 0.0f;
//     static float ramShown = 0.0f;
 
//     const float ease = ClampFloat(ImGui::GetIO().DeltaTime * 6.0f, 0.0f, 1.0f);
//     cpuShown += (cpuNow - cpuShown) * ease;
//     ramShown += (ramNow - ramShown) * ease;
 
//     std::snprintf(cpuText, sizeof(cpuText), "%.0f%%", cpuShown);
//     std::snprintf(ramText, sizeof(ramText), "%.0f%%", ramShown);
//     std::snprintf(alertText, sizeof(alertText), "%d", (int)alerts.size());
 
//     const ImVec4 alertColor = CountSeverity("Critical") > 0 ? COLOR_RED : COLOR_YELLOW;
 
//     char alertSubtitle[64];
//     std::snprintf(
//         alertSubtitle, sizeof(alertSubtitle),
//         "%d critical, %d high",
//         CountSeverity("Critical"), CountSeverity("High")
//     );
 
//     MetricCard("CPU usage", cpuText, "", LoadColor(cpuNow), cardWidth, cpuHistory, 6, cpuShown);
//     ImGui::SameLine();
 
//     MetricCard("RAM usage", ramText, "", LoadColor(ramNow), cardWidth, ramHistory, 6, ramShown);
//     ImGui::SameLine();
 
//     char processText[16];
//     std::snprintf(processText, sizeof(processText), "%d", processCount);
 
//     MetricCard("Running processes", processText, "On this host", COLOR_BLUE, cardWidth, nullptr, 1);
//     ImGui::SameLine();
 
//     MetricCard("Total alerts", alertText, alertSubtitle, alertColor, cardWidth, nullptr, 4);
// }
 
// // ------------------------------------------------------------
// // THREAT ACTIVITY PANEL (dashboard)
// // Histogram of new alerts per second + clickable severity bars.
// // ------------------------------------------------------------
 
// static void DrawThreatPanel()
// {
//     ImGui::BeginChild("ThreatPanel", ImVec2(0.0f, 196.0f), true,
//                       ImGuiWindowFlags_NoScrollbar);
 
//     SectionHeading("Threat activity",
//         "new alerts per second, weighted by severity (last 60 s)");
 
//     float peak = 6.0f;
//     for (float v : threatHistory)
//         peak = std::max(peak, v);
 
//     const float leftW = ImGui::GetContentRegionAvail().x * 0.62f;
 
//     DrawBarChart("##threat", threatHistory, HISTORY_SIZE, historyOffset,
//         peak, ImVec2(leftW, 122.0f));
 
//     ImGui::SameLine();
//     ImGui::BeginGroup();
 
//     static const char* names[] = { "Critical", "High", "Medium", "Low" };
//     const int total = std::max(1, (int)alerts.size());
 
//     for (int i = 0; i < 4; ++i)
//     {
//         const int count = CountSeverity(names[i]);
 
//         char label[40];
//         std::snprintf(label, sizeof(label), "%s  %d", names[i], count);
 
//         ImGui::PushID(i);
//         ImGui::PushStyleColor(ImGuiCol_PlotHistogram,
//             WithAlpha(SeverityColor(names[i]), 0.85f));
//         ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_FRAME);
 
//         ImGui::ProgressBar((float)count / (float)total,
//             ImVec2(ImGui::GetContentRegionAvail().x, 20.0f), label);
 
//         ImGui::PopStyleColor(2);
 
//         if (ImGui::IsItemHovered())
//         {
//             ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
//             ImGui::SetTooltip("Show %s alerts", names[i]);
//         }
 
//         if (ImGui::IsItemClicked())
//         {
//             selectedPage   = 4;        // Alerts
//             severityFilter = i + 1;
//         }
 
//         ImGui::PopID();
//     }
 
//     ImGui::EndGroup();
//     ImGui::EndChild();
// }
 
// // ------------------------------------------------------------
// // PASSWORD LOCK (Logs & Reports page and End Task)
// // ------------------------------------------------------------
 
// static const double LOCK_IDLE_SECONDS = 300.0;     // auto-lock after 5 min idle
// static const int    LOGS_PAGE_INDEX   = 5;
 
// static bool        logsUnlocked   = false;
// static double      logsLastActive = 0.0;
// static char        lockOld[64]     = "";
// static char        lockNew[64]     = "";
// static char        lockConfirm[64] = "";
// static char        lockEntry[64]   = "";
// static std::string lockMessage;
// static bool        lockMessageOk   = false;
 
// static void Wipe(char* buffer, std::size_t size)
// {
//     std::memset(buffer, 0, size);
// }
 
// // "Create a password" form. Returns true on the frame it is created.
// static bool DrawSetPasswordForm()
// {
//     bool created = false;
 
//     ImGui::TextDisabled("Create a password (at least 6 characters)");
 
//     ImGui::SetNextItemWidth(260.0f);
//     ImGui::InputTextWithHint("##newpw", "New password",
//         lockNew, sizeof(lockNew), ImGuiInputTextFlags_Password);
 
//     ImGui::SetNextItemWidth(260.0f);
//     ImGui::InputTextWithHint("##confpw", "Confirm password",
//         lockConfirm, sizeof(lockConfirm), ImGuiInputTextFlags_Password);
 
//     if (ImGui::Button("Set Password"))
//     {
//         if (std::strcmp(lockNew, lockConfirm) != 0)
//         {
//             lockMessage   = "Passwords do not match.";
//             lockMessageOk = false;
//         }
//         else
//         {
//             std::string message;
 
//             if (backend.setPassword(lockNew, "", message))
//             {
//                 created = true;
//                 Wipe(lockNew, sizeof(lockNew));
//                 Wipe(lockConfirm, sizeof(lockConfirm));
//             }
 
//             lockMessage   = message;
//             lockMessageOk = created;
//         }
//     }
 
//     return created;
// }
 
// static void DrawLockScreen()
// {
//     ImGui::BeginChild("LockScreen", ImVec2(0.0f, 0.0f), true);
 
//     SectionHeading("Logs & Reports are locked",
//         "enter the password to see the detection signatures");
 
//     ImGui::Spacing();
 
//     if (!backend.isLockConfigured())
//     {
//         if (DrawSetPasswordForm())
//             logsUnlocked = true;          // whoever creates it is let in
//     }
//     else
//     {
//         const int wait = backend.lockoutSecondsLeft();
 
//         ImGui::BeginDisabled(wait > 0);
 
//         ImGui::SetNextItemWidth(260.0f);
//         const bool enter = ImGui::InputTextWithHint("##unlockpw", "Password",
//             lockEntry, sizeof(lockEntry),
//             ImGuiInputTextFlags_Password | ImGuiInputTextFlags_EnterReturnsTrue);
 
//         ImGui::SameLine();
//         const bool click = ImGui::Button("Unlock");
 
//         ImGui::EndDisabled();
 
//         if (wait > 0)
//             ImGui::TextColored(COLOR_YELLOW, "Locked. Try again in %d s.", wait);
 
//         if ((enter || click) && wait == 0)
//         {
//             std::string message;
 
//             if (backend.verifyPassword(lockEntry, message))
//             {
//                 logsUnlocked = true;
//                 lockMessage.clear();
//             }
//             else
//             {
//                 lockMessage   = message;
//                 lockMessageOk = false;
//             }
 
//             Wipe(lockEntry, sizeof(lockEntry));
//         }
//     }
 
//     if (!lockMessage.empty())
//         ImGui::TextColored(lockMessageOk ? COLOR_GREEN : COLOR_RED,
//             "%s", lockMessage.c_str());
 
//     ImGui::EndChild();
// }
 
// // true = unlocked, the page may draw its content
// static bool LogsGate()
// {
//     const double now = ImGui::GetTime();
//     ImGuiIO& io = ImGui::GetIO();
 
//     const bool active =
//         io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f ||
//         io.MouseWheel != 0.0f || ImGui::IsMouseClicked(0) ||
//         ImGui::IsAnyItemActive();
 
//     if (active || !logsUnlocked)
//         logsLastActive = now;
 
//     if (logsUnlocked && now - logsLastActive > LOCK_IDLE_SECONDS)
//     {
//         logsUnlocked  = false;
//         lockMessage   = "Locked after 5 minutes of inactivity.";
//         lockMessageOk = false;
//     }
 
//     if (logsUnlocked)
//         return true;
 
//     DrawLockScreen();
//     return false;
// }
 
// // ------------------------------------------------------------
// // END TASK (shared by the Processes page and resource alerts)
// // ------------------------------------------------------------
 
// static const float HIGH_MEMORY_MB = 500.0f;
 
// static bool        procAppsOnly     = false;
// static bool        procHighMemOnly  = false;
// static std::string procMessage;                 // result of the last End Task
// static bool        procMessageOk    = true;
// static int         pendingPid       = 0;
// static std::string pendingName;
// static float       pendingMem       = 0.0f;
// static int         pendingAlertID   = 0;        // 0 = started from the Processes page
// static bool        openEndTaskPopup = false;
// static std::set<int> handledAlertIDs;           // alerts whose app was ended
 
// static char        endPassword[64] = "";
// static std::string endPopupMessage;
 
// // Call once inside the page that may open the confirmation dialog.
// // Ending a task needs the access password.
// static void DrawEndTaskPopup()
// {
//     if (openEndTaskPopup)
//     {
//         ImGui::OpenPopup("Confirm End Task");
//         openEndTaskPopup = false;
 
//         Wipe(endPassword, sizeof(endPassword));
//         endPopupMessage.clear();
//         lockMessage.clear();
//     }
 
//     const ImVec2 screen = ImGui::GetIO().DisplaySize;
//     ImGui::SetNextWindowPos(
//         ImVec2(screen.x * 0.5f, screen.y * 0.5f),
//         ImGuiCond_Appearing,
//         ImVec2(0.5f, 0.5f));
 
//     if (ImGui::BeginPopupModal("Confirm End Task", nullptr,
//             ImGuiWindowFlags_AlwaysAutoResize))
//     {
//         ImGui::Text("End %s (PID %d)?", pendingName.c_str(), pendingPid);
//         ImGui::TextDisabled("It is using %.0f MB. Unsaved work in this app will be lost.",
//             pendingMem);
//         ImGui::Spacing();
 
//         if (!backend.isLockConfigured())
//         {
//             ImGui::TextColored(COLOR_YELLOW,
//                 "Ending tasks is password protected. Create a password first.");
//             ImGui::Spacing();
 
//             DrawSetPasswordForm();
 
//             if (!lockMessage.empty())
//                 ImGui::TextColored(lockMessageOk ? COLOR_GREEN : COLOR_RED,"%s", lockMessage.c_str());
//         }
//         else
//         {
//             const int wait = backend.lockoutSecondsLeft();
 
//             ImGui::BeginDisabled(wait > 0);
 
//             ImGui::SetNextItemWidth(260.0f);
//             const bool enter = ImGui::InputTextWithHint("##endpw", "Password",
//                 endPassword, sizeof(endPassword),
//                 ImGuiInputTextFlags_Password | ImGuiInputTextFlags_EnterReturnsTrue);
 
//             ImGui::EndDisabled();
 
//             if (wait > 0)
//                 ImGui::TextColored(COLOR_YELLOW, "Locked. Try again in %d s.", wait);
 
//             if (!endPopupMessage.empty())
//                 ImGui::TextColored(COLOR_RED, "%s", endPopupMessage.c_str());
 
//             ImGui::Spacing();
 
//             ImGui::BeginDisabled(wait > 0);
//             const bool click = ImGui::Button("End Task", ImVec2(120.0f, 0.0f));
//             ImGui::EndDisabled();
 
//             if ((enter || click) && wait == 0)
//             {
//                 std::string authMessage;
//                 const bool authorised = backend.verifyPassword(endPassword, authMessage);
 
//                 Wipe(endPassword, sizeof(endPassword));
 
//                 if (!authorised)
//                 {
//                     endPopupMessage = authMessage;
//                 }
//                 else
//                 {
//                     std::string result;
 
//                     procMessageOk = backend.endTask(pendingPid, result, pendingName);
//                     procMessage   = result;
 
//                     if (procMessageOk && pendingAlertID != 0)
//                         handledAlertIDs.insert(pendingAlertID);
 
//                     ImGui::CloseCurrentPopup();
//                 }
//             }
 
//             ImGui::SameLine();
//         }
 
//         if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
//         {
//             Wipe(endPassword, sizeof(endPassword));
//             Wipe(lockNew, sizeof(lockNew));
//             Wipe(lockConfirm, sizeof(lockConfirm));
//             ImGui::CloseCurrentPopup();
//         }
 
//         ImGui::EndPopup();
//     }
// }
 
// // ------------------------------------------------------------
// // ALERT TABLE
// // compact = true: dashboard preview. false: full Alerts page.
// // ------------------------------------------------------------
 
// static void DrawAlerts(bool compact)
// {
//     ImGui::BeginChild(
//         "AlertsPanel",
//         ImVec2(0.0f, compact ? 270.0f : 0.0f),
//         true
//     );
 
//     SectionHeading(
//         compact ? "Recent security alerts" : "All security alerts"
//     );
 
//     // Severity filter + search (full page only)
//     if (!compact)
//     {
//         for (int i = 0; i < IM_ARRAYSIZE(severityFilterLabels); ++i)
//         {
//             const bool active = (severityFilter == i);
 
//             if (active)
//             {
//                 ImGui::PushStyleColor(ImGuiCol_Button, COLOR_BLUE);
//             }
//             else
//             {
//                 ImGui::PushStyleColor(ImGuiCol_Button, COLOR_FRAME);
//             }
 
//             if (ImGui::Button(severityFilterLabels[i], ImVec2(96.0f, 0.0f)))
//                 severityFilter = i;
 
//             ImGui::PopStyleColor();
 
//             if (i + 1 < IM_ARRAYSIZE(severityFilterLabels))
//                 ImGui::SameLine();
//         }
 
//         ImGui::SameLine();
//         ImGui::SetNextItemWidth(260.0f);
//         ImGui::InputTextWithHint("##alertsearch", "Search alerts",
//             alertSearch, IM_ARRAYSIZE(alertSearch));
 
//         ImGui::Spacing();
//     }
 
//     if (!procMessage.empty())
//         ImGui::TextColored(procMessageOk ? COLOR_GREEN : COLOR_RED,
//             "%s", procMessage.c_str());
 
//     DrawEndTaskPopup();
 
//     if (ImGui::BeginTable(
//             "AlertsTable",
//             5,
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);
//         ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 110.0f);
//         ImGui::TableSetupColumn("Severity", ImGuiTableColumnFlags_WidthFixed, 130.0f);
//         ImGui::TableSetupColumn("Source", ImGuiTableColumnFlags_WidthFixed, 200.0f);
//         ImGui::TableSetupColumn("Description");
//         ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 100.0f);
//         ImGui::TableHeadersRow();
 
//         int shown = 0;
 
//         for (const auto& alert : alerts)
//         {
//             if (!compact && severityFilter != 0 &&
//                 alert.severity != severityFilterLabels[severityFilter])
//             {
//                 continue;
//             }
 
//             if (!compact &&
//                 !ContainsNoCase(alert.description.c_str(), alertSearch) &&
//                 !ContainsNoCase(alert.source.c_str(), alertSearch))
//             {
//                 continue;
//             }
 
//             ++shown;
 
//             ImGui::TableNextRow();
 
//             ImGui::TableSetColumnIndex(0);
//             {
//                 const ImVec2 p = ImGui::GetCursorScreenPos();
 
//                 ImGui::GetWindowDrawList()->AddCircleFilled(
//                     ImVec2(p.x + 4.0f, p.y + ImGui::GetTextLineHeight() * 0.5f),
//                     4.0f,
//                     ImGui::GetColorU32(SeverityColor(alert.severity.c_str())));
 
//                 ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 16.0f);
//             }
//             ImGui::TextDisabled("%s", alert.time.c_str());
 
//             ImGui::TableSetColumnIndex(1);
//             SeverityBadge(alert.severity.c_str());
 
//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextUnformatted(alert.source.c_str());
 
//             ImGui::TableSetColumnIndex(3);
//             ImGui::TextUnformatted(alert.description.c_str());
 
//             // Resource alerts can offer End Task for the app causing them
//             ImGui::TableSetColumnIndex(4);
 
//             if (alert.canEndTask)
//             {
//                 if (handledAlertIDs.count(alert.id))
//                 {
//                     ImGui::TextColored(COLOR_GREEN, "Ended");
//                 }
//                 else
//                 {
//                     ImGui::PushID(alert.id);
 
//                     ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.55f, 0.12f, 0.15f, 1.0f));
//                     ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.18f, 0.22f, 1.0f));
//                     ImGui::PushStyleColor(ImGuiCol_ButtonActive,  COLOR_RED);
 
//                     if (ImGui::SmallButton("End Task"))
//                     {
//                         pendingAlertID   = alert.id;
//                         pendingPid       = alert.culpritPid;
//                         pendingName      = alert.culpritName;
//                         pendingMem       = alert.culpritMem;
//                         openEndTaskPopup = true;
//                     }
 
//                     ImGui::PopStyleColor(3);
//                     ImGui::PopID();
//                 }
//             }
//             else
//             {
//                 ImGui::TextDisabled("-");
//             }
//         }
 
//         if (shown == 0)
//         {
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(3);
//             ImGui::TextDisabled("No alerts match the current filters.");
//         }
 
//         ImGui::EndTable();
//     }
 
//     ImGui::EndChild();
// }
 
// // ------------------------------------------------------------
// // PROCESS TABLE
// // Sortable columns, case-insensitive search, CPU usage bars.
// // ------------------------------------------------------------
 
// static void DrawProcesses()
// {
//     ImGui::BeginChild(
//         "ProcessesPanel",
//         ImVec2(0.0f, 0.0f),
//         true
//     );
 
//     SectionHeading("Running processes",
//         "apps can be ended here; system and background processes are protected");
 
//     ImGui::SetNextItemWidth(300.0f);
 
//     ImGui::InputTextWithHint(
//         "##ProcessSearch",
//         "Search by name or PID",
//         processSearch,
//         IM_ARRAYSIZE(processSearch)
//     );
 
//     ImGui::SameLine();
//     ImGui::Checkbox("Apps only", &procAppsOnly);
 
//     ImGui::SameLine();
 
//     char highLabel[64];
//     std::snprintf(highLabel, sizeof(highLabel),
//         "High memory only (%.0f MB+)", HIGH_MEMORY_MB);
//     ImGui::Checkbox(highLabel, &procHighMemOnly);
 
//     if (!procMessage.empty())
//         ImGui::TextColored(procMessageOk ? COLOR_GREEN : COLOR_RED,
//             "%s", procMessage.c_str());
 
//     ImGui::Spacing();
 
//     if (ImGui::BeginTable(
//             "ProcessTable",
//             6,
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_Sortable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);
 
//         ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed, 85.0f, 0);
//         ImGui::TableSetupColumn("Process name", ImGuiTableColumnFlags_WidthStretch, 0.0f, 1);
//         ImGui::TableSetupColumn(
//             "CPU %",
//             ImGuiTableColumnFlags_WidthFixed |
//             ImGuiTableColumnFlags_PreferSortDescending,
//             180.0f, 2);
//         ImGui::TableSetupColumn(
//             "Memory MB",
//             ImGuiTableColumnFlags_WidthFixed |
//             ImGuiTableColumnFlags_DefaultSort |
//             ImGuiTableColumnFlags_PreferSortDescending,
//             125.0f, 3);
//         ImGui::TableSetupColumn("Running time", ImGuiTableColumnFlags_WidthFixed, 135.0f, 4);
//         ImGui::TableSetupColumn("Action",
//             ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoSort,
//             100.0f, 5);
 
//         ImGui::TableHeadersRow();
 
//         // Build a sorted list of indices
//         std::vector<int> order(processes.size());
 
//         for (size_t i = 0; i < order.size(); ++i)
//             order[i] = (int)i;
 
//         if (ImGuiTableSortSpecs* specs = ImGui::TableGetSortSpecs())
//         {
//             if (specs->SpecsCount > 0)
//             {
//                 const ImGuiTableColumnSortSpecs& spec = specs->Specs[0];
//                 const bool asc =
//                     spec.SortDirection == ImGuiSortDirection_Ascending;
 
//                 std::sort(order.begin(), order.end(),
//                     [&](int a, int b)
//                     {
//                         const ProcessInfo& pa = processes[a];
//                         const ProcessInfo& pb = processes[b];
 
//                         int cmp = 0;
 
//                         switch (spec.ColumnUserID)
//                         {
//                         case 0: cmp = (pa.pid > pb.pid) - (pa.pid < pb.pid); break;
//                         case 1: cmp = pa.name.compare(pb.name); break;
//                         case 2: cmp = (pa.cpu > pb.cpu) - (pa.cpu < pb.cpu); break;
//                         case 3: cmp = (pa.memory > pb.memory) - (pa.memory < pb.memory); break;
//                         case 4: cmp = (pa.runningSeconds > pb.runningSeconds) - (pa.runningSeconds < pb.runningSeconds); break;
//                         }
 
//                         return asc ? (cmp < 0) : (cmp > 0);
//                     });
//             }
//         }
 
//         int shown = 0;
 
//         for (int index : order)
//         {
//             const ProcessInfo& process = processes[index];
 
//             if (procAppsOnly && !process.isApp)
//                 continue;
 
//             if (procHighMemOnly && process.memory < HIGH_MEMORY_MB)
//                 continue;
 
//             char pidText[32];
//             std::snprintf(pidText, sizeof(pidText), "%d", process.pid);
 
//             const bool matches =
//                 ContainsNoCase(process.name.c_str(), processSearch) ||
//                 ContainsNoCase(pidText, processSearch);
 
//             if (!matches)
//                 continue;
 
//             ++shown;
 
//             ImGui::TableNextRow();
 
//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextDisabled("%d", process.pid);
 
//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextUnformatted(process.name.c_str());
 
//             if (process.isApp)
//             {
//                 ImGui::SameLine();
//                 ImGui::TextDisabled("- %s", process.title.c_str());
//             }
 
//             // CPU bar, scaled so the busiest realistic process fills it
//             ImGui::TableSetColumnIndex(2);
 
//             char cpuLabel[16];
//             std::snprintf(cpuLabel, sizeof(cpuLabel), "%.1f", process.cpu);
 
//             ImGui::PushStyleColor(ImGuiCol_PlotHistogram, WithAlpha(LoadColor(process.cpu * 4.0f), 0.85f));
//             ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_FRAME);
 
//             ImGui::ProgressBar(
//                 std::min(process.cpu / 25.0f, 1.0f),
//                 ImVec2(-FLT_MIN, 18.0f),
//                 cpuLabel
//             );
 
//             ImGui::PopStyleColor(2);
 
//             // Memory: yellow when high, red when very high
//             ImGui::TableSetColumnIndex(3);
 
//             if (process.memory >= HIGH_MEMORY_MB * 2.0f)
//                 ImGui::TextColored(COLOR_RED, "%.1f", process.memory);
//             else if (process.memory >= HIGH_MEMORY_MB)
//                 ImGui::TextColored(COLOR_YELLOW, "%.1f", process.memory);
//             else
//                 ImGui::Text("%.1f", process.memory);
 
//             ImGui::TableSetColumnIndex(4);
//             ImGui::TextDisabled("%s", process.runningTime.c_str());
 
//             // Only apps (windowed programs) get an End Task button
//             ImGui::TableSetColumnIndex(5);
 
//             if (process.isApp)
//             {
//                 ImGui::PushID(process.pid);
 
//                 ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.55f, 0.12f, 0.15f, 1.0f));
//                 ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.18f, 0.22f, 1.0f));
//                 ImGui::PushStyleColor(ImGuiCol_ButtonActive,  COLOR_RED);
 
//                 if (ImGui::SmallButton("End Task"))
//                 {
//                     pendingAlertID   = 0;
//                     pendingPid       = process.pid;
//                     pendingName      = process.name;
//                     pendingMem       = process.memory;
//                     openEndTaskPopup = true;
//                 }
 
//                 ImGui::PopStyleColor(3);
//                 ImGui::PopID();
//             }
//             else
//             {
//                 ImGui::TextDisabled("-");
//             }
//         }
 
//         if (shown == 0)
//         {
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextDisabled("No process matches the current filters.");
//         }
 
//         ImGui::EndTable();
//     }
 
//     DrawEndTaskPopup();
 
//     ImGui::EndChild();
// }
 
// // ------------------------------------------------------------
// // EVENT PAGE
// // Newest-first activity list for Login and File monitoring.
// // ------------------------------------------------------------
 
// static void DrawEventPage(
//     const char* title,
//     const std::vector<EventInfo>& events,
//     const char* emptyText,
//     const char* hint)
// {
//     ImGui::BeginChild(
//         title,
//         ImVec2(0.0f, 0.0f),
//         true
//     );
 
//     SectionHeading(title, hint);
 
//     if (ImGui::BeginTable(
//             "EventTable",
//             3,
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);
//         ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 190.0f);
//         ImGui::TableSetupColumn("Event", ImGuiTableColumnFlags_WidthFixed, 170.0f);
//         ImGui::TableSetupColumn("Description");
//         ImGui::TableHeadersRow();
 
//         for (const EventInfo& event : events)
//         {
//             ImGui::TableNextRow();
 
//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextDisabled("%s", event.time.c_str());
 
//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextUnformatted(event.type.c_str());
 
//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextUnformatted(event.description.c_str());
//         }
 
//         if (events.empty())
//         {
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextDisabled("%s", emptyText);
//         }
 
//         ImGui::EndTable();
//     }
 
//     ImGui::EndChild();
// }

// static void DrawSystemAnalysis()
// {
//     ImGui::BeginChild("AnalysisPanel", ImVec2(0.0f, 0.0f), true);
 
//     if (!analysisData.valid)
//     {
//         ImGui::TextDisabled("Collecting the first samples...");
//         ImGui::EndChild();
//         return;
//     }
 
//     const BackendAnalysis& a = analysisData;
 
//     // ---- Resource usage ----
//     SectionHeading("Resource usage");
 
//     char cpuText[16];
//     char ramText[16];
 
//     if (a.cpuAvailable)
//         std::snprintf(cpuText, sizeof(cpuText), "%.1f%%", a.cpuPercent);
//     else
//         std::snprintf(cpuText, sizeof(cpuText), "n/a");
 
//     if (a.memoryAvailable)
//         std::snprintf(ramText, sizeof(ramText), "%.1f%%", a.memoryPercent);
//     else
//         std::snprintf(ramText, sizeof(ramText), "n/a");
 
//     ImGui::Text("CPU:");
//     ImGui::SameLine();
//     ImGui::TextColored(LoadColor((float)a.cpuPercent), "%s", cpuText);
 
//     ImGui::Text("Memory:");
//     ImGui::SameLine();
//     ImGui::TextColored(LoadColor((float)a.memoryPercent), "%s", ramText);
//     ImGui::SameLine();
//     ImGui::TextDisabled("(%llu MB used of %llu MB, %llu MB free)",
//         (unsigned long long)a.usedMemoryMB,
//         (unsigned long long)a.totalMemoryMB,
//         (unsigned long long)a.availableMemoryMB);
 
//     ImGui::Spacing();
 
//     const float half =
//         (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;
 
//     if (!a.cpuHistory.empty())
//     {
//         ImGui::BeginGroup();
//         ImGui::TextDisabled("CPU usage (last 60 s)");
//         DrawAreaChart("##cpuh",
//             a.cpuHistory.data(), (int)a.cpuHistory.size(), 0,
//             100.0f, ImVec2(half, 170.0f),
//             LoadColor((float)a.cpuPercent));
//         ImGui::EndGroup();
//     }
 
//     ImGui::SameLine();
 
//     if (!a.ramHistory.empty())
//     {
//         ImGui::BeginGroup();
//         ImGui::TextDisabled("Memory usage (last 60 s)");
//         DrawAreaChart("##ramh",
//             a.ramHistory.data(), (int)a.ramHistory.size(), 0,
//             100.0f, ImVec2(half, 170.0f),
//             LoadColor((float)a.memoryPercent));
//         ImGui::EndGroup();
//     }
 
//     ImGui::Spacing();
//     ImGui::Separator();
//     ImGui::Spacing();
 
//     // ---- Findings ----
//     SectionHeading("Findings", "A diagnostic signal, not proof of an intrusion");
 
//     ImGui::Text(
//         "Processes: %llu    High CPU (>=80%%): %llu    High memory (>=1 GB): %llu",
//         (unsigned long long)a.processCount,
//         (unsigned long long)a.highCpuProcessCount,
//         (unsigned long long)a.highMemoryProcessCount);
 
//     if (a.findings.empty())
//     {
//         ImGui::TextColored(COLOR_GREEN, "No resource concerns detected.");
//     }
//     else
//     {
//         for (const std::string& finding : a.findings){
//             ImGui::TextColored(COLOR_YELLOW, "- %s", finding.c_str());
//         }
//     }
 
//     ImGui::Spacing();
//     ImGui::Separator();
//     ImGui::Spacing();
 
//     // ---- Top CPU processes ----
//     SectionHeading("Top processes by CPU");
 
//     if (ImGui::BeginTable(
//          "TopCpuTable",4,ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH |ImGuiTableFlags_SizingStretchProp))
//     {
//         ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed, 85.0f);
//         ImGui::TableSetupColumn("Process name");
//         ImGui::TableSetupColumn("CPU %", ImGuiTableColumnFlags_WidthFixed, 100.0f);
//         ImGui::TableSetupColumn("Memory MB", ImGuiTableColumnFlags_WidthFixed, 125.0f);
//         ImGui::TableHeadersRow();
 
//         for (const BackendProcess& p : a.topCpuProcesses)
//         {
//             ImGui::TableNextRow();
 
//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextDisabled("%d", p.pid);
 
//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextUnformatted(p.name.c_str());
 
//             ImGui::TableSetColumnIndex(2);
//             ImGui::Text("%.1f", p.cpuPercent);
 
//             ImGui::TableSetColumnIndex(3);
//             ImGui::Text("%.1f", p.memoryMB);
//         }
 
//         ImGui::EndTable();
//     }
 
//     ImGui::EndChild();
// }
 
// // ------------------------------------------------------------
// // FILE SCANNER PAGE
// // ------------------------------------------------------------
 
// static std::string FormatBytes(unsigned long long b)
// {
//     char t[32];
 
//     if (b >= (1ULL << 30))      std::snprintf(t, sizeof(t), "%.2f GB", b / 1073741824.0);
//     else if (b >= (1ULL << 20)) std::snprintf(t, sizeof(t), "%.1f MB", b / 1048576.0);
//     else if (b >= 1024ULL)      std::snprintf(t, sizeof(t), "%.1f KB", b / 1024.0);
//     else                        std::snprintf(t, sizeof(t), "%llu B", b);
 
//     return t;
// }
 
// // Rotating-arc spinner. Reserves a (2*radius) square at the cursor.
// // active = false draws a plain ring instead (idle / finished).
// static void DrawSpinner(float radius, float thickness, const ImVec4& color, bool active)
// {
//     const ImVec2 pos = ImGui::GetCursorScreenPos();
//     const ImVec2 center(pos.x + radius, pos.y + radius);
//     ImDrawList* draw = ImGui::GetWindowDrawList();
//     const float t = (float)ImGui::GetTime();
 
//     if (!active)
//     {
//         draw->AddCircle(center, radius, ImGui::GetColorU32(color), 48, thickness);
//     }
//     else
//     {
//         // faint full ring + a rotating arc on top
//         draw->AddCircle(center, radius,
//             ImGui::GetColorU32(WithAlpha(color, 0.18f)), 48, thickness);
 
//         const float start = t * 5.0f;
 
//         draw->PathClear();
//         draw->PathArcTo(center, radius, start, start + 2.4f, 32);
//         draw->PathStroke(ImGui::GetColorU32(color), 0, thickness);
 
//         // pulsing dot in the middle
//         const float pulse = 0.75f + 0.25f * std::sin(t * 4.0f);
//         draw->AddCircleFilled(center, radius * 0.30f * pulse,
//             ImGui::GetColorU32(WithAlpha(color, 0.85f)), 24);
//     }
 
//     ImGui::Dummy(ImVec2(radius * 2.0f, radius * 2.0f));
// }
 
// // "C:\a\b\file.txt" -> dir "C:\a\b", name "file.txt"
// static void SplitPath(const std::string& full, std::string& dir, std::string& name)
// {
//     const std::size_t pos = full.find_last_of("\\/");
 
//     if (pos == std::string::npos)
//     {
//         dir.clear();
//         name = full;
//     }
//     else
//     {
//         dir  = full.substr(0, pos);
//         name = full.substr(pos + 1);
//     }
// }
 
// static void DrawFileScanner()
// {
//     // Refresh a few times per second, not every frame
//     static double lastRefresh = -1.0;
//     const double now = ImGui::GetTime();
 
//     if (lastRefresh < 0.0 || now - lastRefresh >= 0.25)
//     {
//         lastRefresh  = now;
//         scanStatus   = backend.getScanStatus();
//         scanFindings = backend.getScanFindings();
//     }
 
//     const bool running = scanStatus.running;
 
//     ImGui::BeginChild("ScannerPanel", ImVec2(0.0f, 0.0f), true);
 
//     SectionHeading("File Scanner", "Signature + heuristic scan");
 
//     // ---- Scan type ----
//     ImGui::BeginDisabled(running);
 
//     ImGui::RadioButton("Quick (user folders, temp, startup)", &scanTypeSel, 0);
//     ImGui::SameLine();
//     ImGui::RadioButton("Full (all fixed drives)", &scanTypeSel, 1);
//     ImGui::SameLine();
//     ImGui::RadioButton("Custom", &scanTypeSel, 2);
 
//     if (scanTypeSel == 2)
//     {
//         ImGui::SetNextItemWidth(-1.0f);
//         ImGui::InputTextWithHint("##scanpath",
//             "Folder or file, e.g. C:\\Users\\you\\Downloads",
//             scanPath, sizeof(scanPath));
//     }
 
//     ImGui::EndDisabled();
 
//     // ---- Buttons ----
//     if (!running)
//     {
//         if (ImGui::Button("Start Scan"))
//         {
//             if (scanTypeSel == 2 && scanPath[0] == '\0')
//                 scanMessage = "Enter a folder or file path first.";
//             else if (!backend.startScan((ScanType)scanTypeSel, scanPath))
//                 scanMessage = "A scan is already running.";
//             else
//                 scanMessage.clear();
//         }
//     }
//     else
//     {
//         if (ImGui::Button("Stop Scan"))
//             backend.cancelScan();
//     }
 
//     if (!running && scanStatus.finished)
//     {
//         ImGui::SameLine();
 
//         if (ImGui::Button("Export Report"))
//         {
//             std::string out;
 
//             scanMessage = backend.exportScanReport(out)
//                 ? "Report saved: " + out
//                 : "Could not write the report file.";
//         }
//     }
 
//     if (!scanMessage.empty())
//     {
//         ImGui::SameLine();
//         ImGui::TextDisabled("%s", scanMessage.c_str());
//     }
 
//     ImGui::Separator();
//     ImGui::Spacing();
 
//     // ========================================================
//     // Two panels side by side: live scan (left), threats (right)
//     // ========================================================
//     const float gap    = ImGui::GetStyle().ItemSpacing.x;
//     const float leftW  = (ImGui::GetContentRegionAvail().x - gap) * 0.58f;
//     const float panelH = 230.0f;
 
//     ImGui::PushStyleColor(ImGuiCol_ChildBg, COLOR_SIDEBAR);
 
//     // ---------------- LEFT: live scan ----------------
//     ImGui::BeginChild("ScanLive", ImVec2(leftW, panelH), true);
 
//     {
//         ImVec4 stateColor = COLOR_MUTED;
//         const char* stateText = "Ready to scan";
 
//         if (running)
//         {
//             stateColor = COLOR_CYAN;
//             stateText  = "Scanning";
//         }
//         else if (scanStatus.finished && scanStatus.cancelled)
//         {
//             stateColor = COLOR_YELLOW;
//             stateText  = "Scan stopped";
//         }
//         else if (scanStatus.finished)
//         {
//             stateColor = (scanStatus.malicious + scanStatus.suspicious > 0)
//                 ? COLOR_ORANGE : COLOR_GREEN;
//             stateText  = "Scan completed";
//         }
 
//         // Spinner (or a plain ring when idle)
//         DrawSpinner(26.0f, 4.0f, stateColor, running);
//         ImGui::SameLine(0.0f, 16.0f);
 
//         ImGui::BeginGroup();
 
//         ImGui::SetWindowFontScale(1.3f);
//         ImGui::PushStyleColor(ImGuiCol_Text, stateColor);
 
//         if (running)
//         {
//             const int dots = (int)(ImGui::GetTime() * 2.0) % 4;
//             ImGui::Text("%s%.*s", stateText, dots, "...");
//         }
//         else
//         {
//             ImGui::TextUnformatted(stateText);
//         }
 
//         ImGui::PopStyleColor();
//         ImGui::SetWindowFontScale(1.0f);
 
//         ImGui::TextDisabled("%s",
//             scanStatus.target.empty() ? "Choose a scan type and press Start"
//                                       : scanStatus.target.c_str());
 
//         ImGui::EndGroup();
 
//         ImGui::Spacing();
//         ImGui::Separator();
//         ImGui::Spacing();
 
//         // Which file is being scanned right now
//         ImGui::TextDisabled("%s", running ? "NOW SCANNING" : "LAST FILE SCANNED");
 
//         std::string dir, name;
//         SplitPath(scanStatus.currentFile, dir, name);
 
//         if (name.empty())
//         {
//             ImGui::TextDisabled("-");
//         }
//         else
//         {
//             ImGui::SetWindowFontScale(1.15f);
//             ImGui::PushStyleColor(ImGuiCol_Text, COLOR_TEXT);
//             ImGui::TextUnformatted(name.c_str());
//             ImGui::PopStyleColor();
//             ImGui::SetWindowFontScale(1.0f);
 
//             ImGui::PushStyleColor(ImGuiCol_Text, COLOR_MUTED);
//             ImGui::TextWrapped("%s", dir.c_str());
//             ImGui::PopStyleColor();
//         }
 
//         ImGui::Spacing();
 
//         // Live counters
//         ImGui::Text("Files: %llu", scanStatus.filesScanned);
//         ImGui::SameLine(0.0f, 18.0f);
//         ImGui::Text("Skipped: %llu", scanStatus.filesSkipped);
//         ImGui::SameLine(0.0f, 18.0f);
//         ImGui::Text("Data: %s", FormatBytes(scanStatus.bytesScanned).c_str());
//         ImGui::SameLine(0.0f, 18.0f);
//         ImGui::Text("Time: %s", FormatDuration(scanStatus.elapsedSeconds).c_str());
//     }
 
//     ImGui::EndChild();
 
//     ImGui::SameLine();
 
//     // ---------------- RIGHT: threats found ----------------
//     ImGui::BeginChild("ScanThreats", ImVec2(0.0f, panelH), true);
 
//     {
//         ImGui::PushStyleColor(ImGuiCol_Text, COLOR_CYAN);
//         ImGui::SetWindowFontScale(1.1f);
//         ImGui::TextUnformatted("Suspicious & malicious files");
//         ImGui::SetWindowFontScale(1.0f);
//         ImGui::PopStyleColor();
 
//         ImGui::TextColored(COLOR_RED, "Malicious: %d", scanStatus.malicious);
//         ImGui::SameLine(0.0f, 18.0f);
//         ImGui::TextColored(COLOR_YELLOW, "Suspicious: %d", scanStatus.suspicious);
 
//         ImGui::Separator();
 
//         if (scanFindings.empty())
//         {
//             ImGui::Spacing();
 
//             if (running)
//                 ImGui::TextDisabled("Nothing suspicious so far...");
//             else if (scanStatus.finished)
//                 ImGui::TextColored(COLOR_GREEN, "No threats found.");
//             else
//                 ImGui::TextDisabled("Findings will appear here.");
//         }
 
//         // Newest first
//         for (std::size_t i = scanFindings.size(); i-- > 0; )
//         {
//             const ScanFinding& f = scanFindings[i];
//             const ImVec4 c = (f.level == "Malicious") ? COLOR_RED : COLOR_YELLOW;
 
//             std::string dir, name;
//             SplitPath(f.path, dir, name);
 
//             ImGui::PushID((int)i);
 
//             ImGui::TextColored(c, "[%s]", f.level.c_str());
//             ImGui::SameLine();
//             ImGui::TextUnformatted(f.name.c_str());
 
//             ImGui::TextDisabled("%s", name.c_str());
 
//             if (ImGui::IsItemHovered())
//             {
//                 ImGui::BeginTooltip();
//                 ImGui::Text("%s", f.path.c_str());
//                 ImGui::Text("Reason: %s", f.reason.c_str());
//                 ImGui::Text("Score: %d", f.score);
//                 ImGui::EndTooltip();
//             }
 
//             ImGui::Separator();
//             ImGui::PopID();
//         }
//     }
 
//     ImGui::EndChild();
 
//     ImGui::PopStyleColor();   // ChildBg
 
//     ImGui::Spacing();
 
//     // ========================================================
//     // Full findings table (all details)
//     // ========================================================
//     SectionHeading("All findings", "hover a name for score and SHA-256");
 
//     if (ImGui::BeginTable(
//             "ScanFindings",
//             5,
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);
//         ImGui::TableSetupColumn("Time",   ImGuiTableColumnFlags_WidthFixed, 80.0f);
//         ImGui::TableSetupColumn("Level",  ImGuiTableColumnFlags_WidthFixed, 95.0f);
//         ImGui::TableSetupColumn("Name",   ImGuiTableColumnFlags_WidthFixed, 190.0f);
//         ImGui::TableSetupColumn("File");
//         ImGui::TableSetupColumn("Reason");
//         ImGui::TableHeadersRow();
 
//         for (const ScanFinding& f : scanFindings)
//         {
//             ImGui::TableNextRow();
 
//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextDisabled("%s", f.time.c_str());
 
//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextColored(
//                 f.level == "Malicious" ? COLOR_RED : COLOR_YELLOW,
//                 "%s", f.level.c_str());
 
//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextUnformatted(f.name.c_str());
 
//             if (ImGui::IsItemHovered())
//             {
//                 ImGui::BeginTooltip();
//                 ImGui::Text("Score: %d", f.score);
//                 ImGui::Text("Size:  %s", FormatBytes(f.size).c_str());
 
//                 if (!f.sha256.empty())
//                     ImGui::Text("SHA-256: %s", f.sha256.c_str());
 
//                 ImGui::EndTooltip();
//             }
 
//             ImGui::TableSetColumnIndex(3);
//             ImGui::TextUnformatted(f.path.c_str());
 
//             ImGui::TableSetColumnIndex(4);
//             ImGui::TextUnformatted(f.reason.c_str());
//         }
 
//         ImGui::EndTable();
//     }
 
//     ImGui::EndChild();
// }
 
// // ------------------------------------------------------------
// // LOGS & REPORTS PAGE
// // Lists every signature / rule the File Scanner checks for.
// // ------------------------------------------------------------
 
// static void DrawLogsReports()
// {
//     if (!LogsGate())
//         return;
 
//     static std::vector<SignatureInfo> signatures;
//     static bool loaded = false;
//     static char search[128] = "";
 
//     if (!loaded)
//     {
//         signatures = FileScanner::getSignatures();
//         loaded = true;
//     }
 
//     int hashCount = 0, contentCount = 0, nameCount = 0;
 
//     for (const SignatureInfo& s : signatures)
//     {
//         if (s.category == "Known hash")        ++hashCount;
//         else if (s.category == "File content") ++contentCount;
//         else                                   ++nameCount;
//     }
 
//     ImGui::BeginChild("LogsPanel", ImVec2(0.0f, 0.0f), true);
 
//     SectionHeading("Detection signatures", "what the File Scanner checks for");
 
//     if (ImGui::Button("Lock now"))
//     {
//         logsUnlocked = false;
//         lockMessage.clear();
//     }
 
//     ImGui::SameLine();
//     ImGui::TextDisabled("Locks again after 5 minutes idle or when you leave this page.");
 
//     if (ImGui::CollapsingHeader("Change password"))
//     {
//         static std::string changeMessage;
//         static bool        changeOk = false;
 
//         ImGui::SetNextItemWidth(260.0f);
//         ImGui::InputTextWithHint("##oldpw", "Current password",
//             lockOld, sizeof(lockOld), ImGuiInputTextFlags_Password);
 
//         ImGui::SetNextItemWidth(260.0f);
//         ImGui::InputTextWithHint("##chnew", "New password",
//             lockNew, sizeof(lockNew), ImGuiInputTextFlags_Password);
 
//         ImGui::SetNextItemWidth(260.0f);
//         ImGui::InputTextWithHint("##chconf", "Confirm new password",
//             lockConfirm, sizeof(lockConfirm), ImGuiInputTextFlags_Password);
 
//         if (ImGui::Button("Update password"))
//         {
//             if (std::strcmp(lockNew, lockConfirm) != 0)
//             {
//                 changeMessage = "New passwords do not match.";
//                 changeOk      = false;
//             }
//             else
//             {
//                 changeOk = backend.setPassword(lockNew, lockOld, changeMessage);
 
//                 if (changeOk)
//                 {
//                     Wipe(lockOld, sizeof(lockOld));
//                     Wipe(lockNew, sizeof(lockNew));
//                     Wipe(lockConfirm, sizeof(lockConfirm));
//                 }
//             }
//         }
 
//         if (!changeMessage.empty())
//             ImGui::TextColored(changeOk ? COLOR_GREEN : COLOR_RED,
//                 "%s", changeMessage.c_str());
//     }
 
//     ImGui::Spacing();
 
//     ImGui::PushTextWrapPos(0.0f);
//     ImGui::TextColored(COLOR_MUTED,
//         "A file is reported Malicious on an instant match (EICAR test string or a "
//         "known-bad SHA-256). Otherwise every matching rule adds its weight to a "
//         "score, and a file scoring %d or more is reported Suspicious.",
//         FileScanner::suspiciousThreshold());
//     ImGui::PopTextWrapPos();
 
//     ImGui::Spacing();
 
//     ImGui::Text("%d rules", (int)signatures.size());
//     ImGui::SameLine(0.0f, 18.0f);
//     ImGui::TextColored(COLOR_RED,    "Known hashes: %d", hashCount);
//     ImGui::SameLine(0.0f, 18.0f);
//     ImGui::TextColored(COLOR_CYAN,   "File content: %d", contentCount);
//     ImGui::SameLine(0.0f, 18.0f);
//     ImGui::TextColored(COLOR_YELLOW, "Name / location: %d", nameCount);
 
//     ImGui::Spacing();
 
//     ImGui::SetNextItemWidth(300.0f);
//     ImGui::InputTextWithHint("##sigsearch", "Search signatures", search, IM_ARRAYSIZE(search));
 
//     ImGui::SameLine();
 
//     if (ImGui::Button("Reload"))
//         signatures = FileScanner::getSignatures();   // re-reads hids_hashes.txt
 
//     ImGui::Spacing();
 
//     if (ImGui::BeginTable(
//             "SignatureTable",
//             4,
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);
//         ImGui::TableSetupColumn("Type",      ImGuiTableColumnFlags_WidthFixed, 160.0f);
//         ImGui::TableSetupColumn("Name",      ImGuiTableColumnFlags_WidthStretch, 1.0f);
//         ImGui::TableSetupColumn("Pattern / condition", ImGuiTableColumnFlags_WidthStretch, 1.4f);
//         ImGui::TableSetupColumn("Result",    ImGuiTableColumnFlags_WidthFixed, 100.0f);
//         ImGui::TableHeadersRow();
 
//         int shown = 0;
 
//         for (const SignatureInfo& s : signatures)
//         {
//             if (!ContainsNoCase(s.name.c_str(), search) &&
//                 !ContainsNoCase(s.pattern.c_str(), search) &&
//                 !ContainsNoCase(s.category.c_str(), search))
//                 continue;
 
//             ++shown;
 
//             ImGui::TableNextRow();
 
//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextColored(
//                 s.category == "Known hash"   ? COLOR_RED :
//                 s.category == "File content" ? COLOR_CYAN : COLOR_YELLOW,
//                 "%s", s.category.c_str());
 
//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextUnformatted(s.name.c_str());
 
//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextDisabled("%s", s.pattern.c_str());
 
//             ImGui::TableSetColumnIndex(3);
 
//             if (s.instantMalicious)
//                 ImGui::TextColored(COLOR_RED, "Malicious");
//             else
//                 ImGui::TextColored(
//                     s.weight >= 4 ? COLOR_ORANGE :
//                     s.weight >= 2 ? COLOR_YELLOW : COLOR_MUTED,
//                     "+%d score", s.weight);
//         }
 
//         if (shown == 0)
//         {
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextDisabled("No signature matches \"%s\".", search);
//         }
 
//         ImGui::EndTable();
//     }
 
//     ImGui::EndChild();
// }
 
// // ------------------------------------------------------------
// // NOTIFICATION SETTINGS (shown on the Settings page)
// // ------------------------------------------------------------
 
// static void DrawNotificationSettings()
// {
//     ImGui::BeginChild("NotificationSettings", ImVec2(0.0f, 150.0f), true,
//                       ImGuiWindowFlags_NoScrollbar);
 
//     SectionHeading("Desktop notifications",
//         "pop-up toasts when a new alert is raised");
 
//     bool enabled = backend.notificationsEnabled();
 
//     if (ImGui::Checkbox("Show desktop notifications", &enabled))
//         backend.setNotificationsEnabled(enabled);
 
//     ImGui::BeginDisabled(!enabled);
 
//     static const char* levels[] = {
//         "Low and above", "Medium and above",
//         "High and above", "Critical only"
//     };
 
//     int level = backend.notificationMinSeverity();
 
//     ImGui::SetNextItemWidth(220.0f);
 
//     if (ImGui::Combo("Notify for", &level, levels, IM_ARRAYSIZE(levels)))
//         backend.setNotificationMinSeverity(level);
 
//     if (ImGui::Button("Send test notification"))
//         backend.sendTestNotification();
 
//     ImGui::EndDisabled();
 
//     ImGui::EndChild();
// }
 
// static void DrawMainContent()
// {
//     RefreshFromBackend();
 
//     DrawTopBar();
//     ImGui::Separator();
//     ImGui::Spacing();
 
//     // Leaving Logs & Reports locks it again; page changes restart the fade
//     static int    lastPage      = -1;
//     static double pageChangedAt = 0.0;
 
//     if (selectedPage != lastPage)
//     {
//         if (lastPage == LOGS_PAGE_INDEX)
//         {
//             logsUnlocked = false;
//             lockMessage.clear();
//             Wipe(lockOld, sizeof(lockOld));
//             Wipe(lockNew, sizeof(lockNew));
//             Wipe(lockConfirm, sizeof(lockConfirm));
//         }
 
//         lastPage      = selectedPage;
//         pageChangedAt = ImGui::GetTime();
//     }
 
//     // Demo banner: impossible to miss, so sample rows are never mistaken for real ones
//     if (demoMode)
//     {
//         DrawPill("DEMO DATA", COLOR_YELLOW);
//         ImGui::SameLine();
//         ImGui::TextColored(COLOR_YELLOW,
//             "Rows tagged [DEMO] are samples, not from this machine. Turn off in Settings.");
//         ImGui::Spacing();
//     }
 
//     // Quick fade-in when switching pages
//     const float fade = ClampFloat(
//         (float)((ImGui::GetTime() - pageChangedAt) / 0.18), 0.0f, 1.0f);
 
//     ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.25f + 0.75f * fade);
 
//     switch (selectedPage)
//     {
//     case 0:
//         DrawMetrics();
//         ImGui::Spacing();
//         DrawThreatPanel();
//         ImGui::Spacing();
//         DrawAlerts(true);
//         ImGui::Spacing();
//         DrawProcesses();
//         break;
 
//     case 1:
//         DrawProcesses();
//         break;
 
//     case 2:
//         DrawEventPage("Failed sign-in attempts", loginEvents,
//             "No failed sign-ins recorded since monitoring started.",
//             "Reads the Windows Security log - run as Administrator");
//         break;
 
//     case 3:
//         DrawEventPage("File activity", fileEvents,
//             "No changes detected in the watched folder yet.",
//             "Watching the HIDS_Monitor folder");
//         break;
 
//     case 4:
//         DrawAlerts(false);
//         break;
 
//     case 5:
//         DrawLogsReports();
//         break;
 
//     case 6:
//         DrawSystemAnalysis();
//         break;
 
//     case 7:
//         DrawFileScanner();
//         break;
 
//     case 8:
//         DrawNotificationSettings();
//         ImGui::Spacing();
//         DrawSettingsPage(monitoringActive);
//         break;
//     }
 
//     ImGui::PopStyleVar();
// }
 
// int main()
// {
//     if (!glfwInit())
//     {
//         std::fprintf(stderr, "ERROR: Failed to initialize GLFW.\n");
//         return 1;
//     }
 
//     // Request OpenGL 3.0
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
 
//     GLFWwindow* window = glfwCreateWindow(
//         1400,
//         900,
//         "HIDS - Host Intrusion Detection System",
//         nullptr,
//         nullptr
//     );
 
//     if (window == nullptr)
//     {
//         std::fprintf(stderr, "ERROR: Failed to create dashboard window.\n");
//         glfwTerminate();
//         return 1;
//     }
 
//     glfwMakeContextCurrent(window);
//     glfwSwapInterval(1);
 
//     // Native window handle for the tray icon / desktop notifications
// #ifdef _WIN32
//     backend.attachWindow(glfwGetWin32Window(window));
// #endif
 
//     IMGUI_CHECKVERSION();
//     ImGui::CreateContext();
 
//     ImGuiIO& io = ImGui::GetIO();
 
//     io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
 
//     // Don't write imgui.ini next to the executable
//     io.IniFilename = nullptr;
 
//     LoadFonts(io);
//     LoadUiConfig();           // saved theme / interface size / demo flag
//     ApplyTheme(themeIndex);   // also builds the ImGui style
//     InitUiScale(io);
 
//     // Seed sparkline history so the cards aren't empty on first frame
//     for (int i = 0; i < HISTORY_SIZE; ++i)
//     {
//         cpuHistory[i] = cpuNow;
//         ramHistory[i] = ramNow;
//     }
 
//     ImGui_ImplGlfw_InitForOpenGL(window, true);
//     ImGui_ImplOpenGL3_Init("#version 130");
 
//     // Start the monitors (login, process, file) on background threads
//     backend.start();
 
//     // Main application loop
//     while (!glfwWindowShouldClose(window))
//     {
//         glfwPollEvents();
 
//         // Send queued desktop notifications (must run on the UI thread)
//         backend.flushNotifications();
 
//         // Theme clicks are queued and applied here, never mid-frame
//         ApplyPendingTheme();
 
//         ImGui_ImplOpenGL3_NewFrame();
//         ImGui_ImplGlfw_NewFrame();
//         ImGui::NewFrame();
 
//         // Gradient backdrop; the root window is transparent so it shows through
//         DrawBackdrop();
//         ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
 
//         // Full-window root
//         ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
//         ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
 
//         ImGui::Begin(
//             "HIDSRoot",
//             nullptr,
//             ImGuiWindowFlags_NoTitleBar |
//             ImGuiWindowFlags_NoResize |
//             ImGuiWindowFlags_NoMove |
//             ImGuiWindowFlags_NoCollapse |
//             ImGuiWindowFlags_NoScrollbar |
//             ImGuiWindowFlags_NoBringToFrontOnFocus
//         );
 
//         DrawSidebar();
//         ImGui::SameLine();
 
//         ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
 
//         ImGui::BeginChild(
//             "MainContent",
//             ImVec2(0.0f, 0.0f),
//             false
//         );
 
//         DrawMainContent();
 
//         ImGui::EndChild();
//         ImGui::PopStyleColor();
 
//         ImGui::End();
//         ImGui::PopStyleColor();   // WindowBg
 
//         ImGui::Render();
 
//         int displayWidth = 0;
//         int displayHeight = 0;
 
//         glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
//         glViewport(0, 0, displayWidth, displayHeight);
 
//         glClearColor(
//             HIDS_COLOR_BACKGROUND.x,
//             HIDS_COLOR_BACKGROUND.y,
//             HIDS_COLOR_BACKGROUND.z,
//             HIDS_COLOR_BACKGROUND.w
//         );
 
//         glClear(GL_COLOR_BUFFER_BIT);
 
//         ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
 
//         glfwSwapBuffers(window);
//     }
 
//     backend.shutdown();
 
//     ImGui_ImplOpenGL3_Shutdown();
//     ImGui_ImplGlfw_Shutdown();
//     ImGui::DestroyContext();
 
//     glfwDestroyWindow(window);
//     glfwTerminate();
 
//     return 0;
// }
 
// #include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"
// #include <GLFW/glfw3.h>

// // Native window handle (needed for the tray icon / desktop notifications)
// #ifdef _WIN32
// #include <windows.h>
// #define GLFW_EXPOSE_NATIVE_WIN32
// #include <GLFW/glfw3native.h>
// #endif

// #include <algorithm>
// #include <cctype>
// #include <cfloat>
// #include <cmath>
// #include <cstdio>
// #include <cstdlib>
// #include <cstring>
// #include <set>
// #include <string>
// #include <vector>
// #include "HIDSBackend.h"
// #include "DashboardTheme.h"   // themes, colours, icons, Settings page

// // ------------------------------------------------------------
// // DATA STRUCTURES
// // ------------------------------------------------------------

// struct ProcessInfo {
//     int pid;
//     std::string name;
//     float cpu;
//     float memory;
//     double runningSeconds;
//     std::string runningTime;   // formatted for display
//     bool isApp = false;        // can be ended from the Processes page
//     std::string title;         // window title (apps only)
// };

// struct AlertInfo {
//     int id = 0;
//     std::string time;
//     std::string severity;
//     std::string source;
//     std::string description;

//     bool        canEndTask = false;   // resource alert with a culprit app
//     int         culpritPid = 0;
//     std::string culpritName;
//     float       culpritMem = 0.0f;
// };

// struct EventInfo {
//     std::string time;
//     std::string type;
//     std::string description;
// };

// static HIDSBackend backend;

// static std::vector<ProcessInfo> processes;
// static std::vector<AlertInfo>   alerts;
// static std::vector<EventInfo>   loginEvents;
// static std::vector<EventInfo>   fileEvents;
// static BackendAnalysis          analysisData;

// // File scanner page state
// static ScanStatus               scanStatus;
// static std::vector<ScanFinding> scanFindings;
// static int                      scanTypeSel = 0;      // 0 Quick, 1 Full, 2 Custom
// static char                     scanPath[512] = "";
// static std::string              scanMessage;
// static int processCount = 0;

// static int  selectedPage = 0;
// static char processSearch[128] = "";
// static bool monitoringActive = false;

// static int  severityFilter = 0;   // 0 = All, 1..4 = Critical..Low
// static const char* severityFilterLabels[] =
//     { "All", "Critical", "High", "Medium", "Low" };
// static char alertSearch[128] = "";

// static const char* pages[] = {
//     "Dashboard",
//     "Processes",
//     "Login Monitoring",
//     "File Monitoring",
//     "Alerts",
//     "Logs & Reports",
//     "System Analysis",
//     "File Scanner",
//     "Settings"
// };

// static const char* pageSubtitles[] = {
//     "Overview of this machine's security status",
//     "Everything currently running on this host",
//     "Sign-in activity on this host",
//     "Changes to monitored files and folders",
//     "Events raised by the HIDS monitors",
//     "Detection signatures and scan reports",
//     "Deeper inspection of system behaviour",
//     "Scan files for known malware and suspicious traits",
//     "Appearance, interface size, notifications and data source"
// };

// // Live metrics (ring buffers feed the sparklines)
// static const int HISTORY_SIZE = 60;
// static float cpuHistory[HISTORY_SIZE] = {};
// static float ramHistory[HISTORY_SIZE] = {};
// static int   historyOffset = 0;
// static float cpuNow = 0.0f;
// static float ramNow = 0.0f;

// // Threat activity (feeds the dashboard histogram)
// static float threatHistory[HISTORY_SIZE] = {};
// static float threatAccum = 0.0f;

// static float ClampFloat(float v, float lo, float hi)
// {
//     return v < lo ? lo : (v > hi ? hi : v);
// }

// // ------------------------------------------------------------
// // FONT LOADING
// // Uses a real TTF if one is found; otherwise scales the default
// // bitmap font so the dashboard is still readable.
// // ------------------------------------------------------------

// static bool FileExists(const char* path)
// {
//     FILE* f = std::fopen(path, "rb");
//     if (f == nullptr)
//         return false;

//     std::fclose(f);
//     return true;
// }

// static void LoadFonts(ImGuiIO& io)
// {
//     const char* candidates[] = {
//         "C:/Windows/Fonts/segoeui.ttf",
//         "C:/Windows/Fonts/arial.ttf",
//         "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
//         "/usr/share/fonts/TTF/DejaVuSans.ttf",
//         "/System/Library/Fonts/Supplemental/Arial.ttf"
//     };

//     for (const char* path : candidates)
//     {
//         if (!FileExists(path))
//             continue;

//         ImFont* font = io.Fonts->AddFontFromFileTTF(path, 18.0f);

//         if (font != nullptr)
//         {
//             io.FontDefault = font;
//             io.FontGlobalScale = 1.0f;
//             return;
//         }
//     }

//     io.Fonts->AddFontDefault();
//     io.FontGlobalScale = 1.25f;
// }

// // ------------------------------------------------------------
// // STYLE REFINEMENT
// // Flatter, more modern look. Applied after ApplyTheme(); colours
// // still come from the theme.
// // ------------------------------------------------------------

// static void RefineStyle()
// {
//     ImGuiStyle& s = ImGui::GetStyle();

//     s.WindowRounding    = 0.0f;
//     s.ChildRounding     = 10.0f;
//     s.FrameRounding     = 6.0f;
//     s.PopupRounding     = 8.0f;
//     s.GrabRounding      = 6.0f;
//     s.ScrollbarRounding = 8.0f;
//     s.TabRounding       = 6.0f;

//     s.WindowBorderSize  = 0.0f;
//     s.ChildBorderSize   = 1.0f;
//     s.FrameBorderSize   = 0.0f;
//     s.PopupBorderSize   = 1.0f;

//     s.WindowPadding     = ImVec2(16.0f, 14.0f);
//     s.FramePadding      = ImVec2(12.0f, 7.0f);
//     s.ItemSpacing       = ImVec2(10.0f, 10.0f);
//     s.CellPadding       = ImVec2(10.0f, 7.0f);
//     s.ScrollbarSize     = 12.0f;
// }

// static bool ContainsNoCase(const char* text, const char* search)
// {
//     if (search[0] == '\0')
//         return true;

//     const size_t textLen = std::strlen(text);
//     const size_t searchLen = std::strlen(search);

//     if (searchLen > textLen)
//         return false;

//     for (size_t i = 0; i + searchLen <= textLen; ++i)
//     {
//         size_t j = 0;

//         while (j < searchLen &&
//                std::tolower((unsigned char)text[i + j]) ==
//                std::tolower((unsigned char)search[j]))
//         {
//             ++j;
//         }

//         if (j == searchLen)
//             return true;
//     }

//     return false;
// }

// static ImVec4 SeverityColor(const char* severity)
// {
//     if (std::strcmp(severity, "Critical") == 0) return COLOR_RED;
//     if (std::strcmp(severity, "High") == 0)     return COLOR_ORANGE;
//     if (std::strcmp(severity, "Medium") == 0)   return COLOR_YELLOW;

//     return COLOR_CYAN;
// }

// static int CountSeverity(const char* severity)
// {
//     int count = 0;

//     for (const auto& alert : alerts)
//         if (alert.severity == severity)
//             ++count;

//     return count;
// }

// // Usage colour: green -> yellow -> red as load rises
// static ImVec4 LoadColor(float percent)
// {
//     if (percent >= 85.0f) return COLOR_RED;
//     if (percent >= 65.0f) return COLOR_YELLOW;

//     return COLOR_GREEN;
// }

// static void SectionHeading(const char* title, const char* hint = nullptr)
// {
//     {
//         const ImVec2 p = ImGui::GetCursorScreenPos();
//         const float  h = ImGui::GetTextLineHeight() * 1.1f;

//         ImGui::GetWindowDrawList()->AddRectFilled(
//             ImVec2(p.x, p.y + 1.0f), ImVec2(p.x + 4.0f, p.y + h),
//             ImGui::GetColorU32(COLOR_CYAN), 2.0f);

//         ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 12.0f);
//     }

//     ImGui::PushStyleColor(ImGuiCol_Text, COLOR_CYAN);
//     ImGui::SetWindowFontScale(1.1f);
//     ImGui::TextUnformatted(title);
//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::PopStyleColor();

//     if (hint != nullptr)
//     {
//         ImGui::SameLine();
//         ImGui::TextDisabled("  %s", hint);
//     }

//     ImGui::Spacing();
// }

// // Coloured pill, e.g. for alert severity
// static void SeverityBadge(const char* severity)
// {
//     const ImVec4 color = SeverityColor(severity);
//     const ImVec2 textSize = ImGui::CalcTextSize(severity);
//     const ImVec2 pad(10.0f, 2.0f);

//     const ImVec2 pos = ImGui::GetCursorScreenPos();
//     const ImVec2 size(textSize.x + pad.x * 2.0f, textSize.y + pad.y * 2.0f);

//     ImDrawList* draw = ImGui::GetWindowDrawList();

//     draw->AddRectFilled(
//         pos,
//         ImVec2(pos.x + size.x, pos.y + size.y),
//         ImGui::GetColorU32(WithAlpha(color, 0.16f)),
//         size.y * 0.5f
//     );

//     draw->AddRect(
//         pos,
//         ImVec2(pos.x + size.x, pos.y + size.y),
//         ImGui::GetColorU32(WithAlpha(color, 0.65f)),
//         size.y * 0.5f
//     );

//     draw->AddText(
//         ImVec2(pos.x + pad.x, pos.y + pad.y),
//         ImGui::GetColorU32(color),
//         severity
//     );

//     ImGui::Dummy(size);
// }

// // Pulsing dot used by the monitoring status
// static void StatusDot(const ImVec4& color, bool pulse)
// {
//     const float radius = 5.0f;
//     const ImVec2 pos = ImGui::GetCursorScreenPos();
//     const ImVec2 center(pos.x + radius + 2.0f, pos.y + ImGui::GetFrameHeight() * 0.5f);

//     ImDrawList* draw = ImGui::GetWindowDrawList();

//     if (pulse)
//     {
//         const float t = (float)ImGui::GetTime();
//         const float wave = 0.5f + 0.5f * std::sin(t * 3.0f);

//         draw->AddCircleFilled(
//             center,
//             radius + 3.0f + wave * 3.0f,
//             ImGui::GetColorU32(WithAlpha(color, 0.18f * (1.0f - wave) + 0.05f))
//         );
//     }

//     draw->AddCircleFilled(center, radius, ImGui::GetColorU32(color));

//     ImGui::Dummy(ImVec2(radius * 2.0f + 6.0f, ImGui::GetFrameHeight()));
// }

// static std::string FormatDuration(double seconds)
// {
//     const unsigned long long total =
//         seconds > 0.0 ? (unsigned long long)seconds : 0ULL;
//     char text[32];

//     std::snprintf(
//         text, sizeof(text), "%02llu:%02llu:%02llu",
//         total / 3600ULL, (total / 60ULL) % 60ULL, total % 60ULL
//     );
//     return text;
// }

// static float SeverityWeight(const std::string& s)
// {
//     if (s == "Critical") return 4.0f;
//     if (s == "High")     return 3.0f;
//     if (s == "Medium")   return 2.0f;
//     return 1.0f;
// }

// // Sample rows. Always labelled so they can't be mistaken for real ones:
// // source "DEMO", "[DEMO]" prefix, negative IDs/PIDs, no End Task button.
// static void AppendDemoData()
// {
//     struct DemoAlert { const char* sev; const char* text; };

//     static const DemoAlert demoAlerts[] = {
//         { "Critical", "[DEMO] Known-bad hash match - C:\\Demo\\sample.exe" },
//         { "High",     "[DEMO] Multiple Failed Logins - 5 attempts in 60 s" },
//         { "Medium",   "[DEMO] Suspicious PowerShell Process - powershell.exe" },
//         { "Low",      "[DEMO] Password Changed - sample entry" }
//     };

//     int id = -1;

//     for (const DemoAlert& d : demoAlerts)
//     {
//         AlertInfo a;
//         a.id          = id--;
//         a.time        = "--:--:--";
//         a.severity    = d.sev;
//         a.source      = "DEMO";
//         a.description = d.text;
//         alerts.push_back(a);              // after the real ones
//     }

//     ProcessInfo p;
//     p.pid            = -1;
//     p.name           = "demo_service.exe [DEMO]";
//     p.cpu            = 12.5f;
//     p.memory         = 640.0f;
//     p.runningSeconds = 4000.0;
//     p.runningTime    = FormatDuration(p.runningSeconds);
//     p.isApp          = false;             // can never be ended
//     processes.push_back(p);

//     loginEvents.push_back({ "--:--:--", "LOGIN_FAILURE",
//         "[DEMO] Failed sign-in for user 'demo'" });
//     fileEvents.push_back({ "--:--:--", "FILE_CREATED",
//         "[DEMO] HIDS_Monitor\\sample_script.ps1" });
// }

// static void RefreshFromBackend()
// {
//     static double lastRefresh = -1.0;
//     static unsigned long lastSample = 0;

//     const double now = ImGui::GetTime();

//     if (lastRefresh >= 0.0 && now - lastRefresh < 0.5)
//         return;

//     lastRefresh = now;

//     // CPU, RAM and process count
//     const BackendStats stats = backend.getStats();

//     cpuNow = stats.cpuPercent;
//     ramNow = stats.ramPercent;
//     processCount = stats.processCount;

//     if (stats.sample != lastSample)
//     {
//         lastSample = stats.sample;

//         cpuHistory[historyOffset]    = cpuNow;
//         ramHistory[historyOffset]    = ramNow;
//         threatHistory[historyOffset] = threatAccum;
//         threatAccum = 0.0f;
//         historyOffset = (historyOffset + 1) % HISTORY_SIZE;
//     }

//     analysisData = backend.getAnalysis();
//     processes.clear();

//     for (const BackendProcess& p : backend.getProcesses())
//     {
//         ProcessInfo row;

//         row.pid            = p.pid;
//         row.name           = p.name;
//         row.cpu            = p.cpuPercent;
//         row.memory         = p.memoryMB;
//         row.runningSeconds = p.runningSeconds;
//         row.runningTime    = FormatDuration(p.runningSeconds);
//         row.isApp          = p.isApp;
//         row.title          = p.windowTitle;

//         processes.push_back(row);
//     }
//     alerts.clear();

//     for (const BackendAlert& a : backend.getAlerts())
//     {
//         AlertInfo row;

//         row.time        = a.time;
//         row.severity    = a.severity;
//         row.source      = a.source;
//         row.description = a.description;
//         row.id          = a.id;
//         row.canEndTask  = a.canEndTask;
//         row.culpritPid  = a.culpritPid;
//         row.culpritName = a.culpritName;
//         row.culpritMem  = a.culpritMemoryMB;

//         alerts.push_back(row);
//     }

//     // Weigh alerts that appeared since the last refresh
//     {
//         static std::set<int> seenAlerts;
//         static bool seeded = false;

//         for (const AlertInfo& a : alerts)
//             if (seenAlerts.insert(a.id).second && seeded)
//                 threatAccum += SeverityWeight(a.severity);

//         seeded = true;   // alerts already present at start-up aren't a "spike"
//     }

//     // Login and file activity (already newest first)
//     loginEvents.clear();
//     fileEvents.clear();

//     for (const BackendEvent& e : backend.getEvents())
//     {
//         EventInfo row;

//         row.time        = e.time;
//         row.type        = e.type;
//         row.description = e.description;

//         if (e.type.rfind("LOGIN_", 0) == 0)
//             loginEvents.push_back(row);
//         else if (e.type.rfind("FILE_", 0) == 0)
//             fileEvents.push_back(row);
//     }

//     if (demoMode)
//         AppendDemoData();
// }

// // ------------------------------------------------------------
// // METRIC CARD
// // Accent stripe on the left, big value, optional sparkline.
// // linkPage >= 0 makes the card clickable.
// // ------------------------------------------------------------

// static void MetricCard(
//     const char* title,
//     const char* value,
//     const char* subtitle,
//     const ImVec4& accent,
//     float width,
//     const float* history = nullptr,
//     int linkPage = -1,
//     float gauge = -1.0f)          // 0..100 draws a ring gauge
// {
//     ImGui::BeginChild(
//         title,
//         ImVec2(width, 142.0f),
//         true,
//         ImGuiWindowFlags_NoScrollbar
//     );

//     // Accent stripe
//     {
//         const ImVec2 pos = ImGui::GetWindowPos();
//         const ImVec2 size = ImGui::GetWindowSize();

//         ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + 4.0f, pos.y + size.y),
//             ImGui::GetColorU32(accent),
//             ImGui::GetStyle().ChildRounding,
//             ImDrawFlags_RoundCornersLeft
//         );
//     }

//     // Soft glow + ring gauge (or page icon) on the right side
//     {
//         const ImVec2 wp = ImGui::GetWindowPos();
//         const ImVec2 ws = ImGui::GetWindowSize();
//         ImDrawList*  d  = ImGui::GetWindowDrawList();
//         const ImVec2 gc(wp.x + ws.x - 54.0f, wp.y + 62.0f);

//         DrawGlow(d, gc, 46.0f, accent);

//         if (gauge >= 0.0f)
//             DrawGauge(d, gc, 24.0f, 6.0f, gauge / 100.0f, accent);
//         else if (linkPage >= 0)
//             DrawPageIcon(d, gc, 14.0f, linkPage,
//                 ImGui::GetColorU32(WithAlpha(accent, 0.85f)));
//     }

//     ImGui::TextDisabled("%s", title);
//     ImGui::PushStyleColor(ImGuiCol_Text, accent);
//     ImGui::SetWindowFontScale(1.55f);
//     ImGui::TextUnformatted(value);
//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::PopStyleColor();

//     if (history != nullptr)
//     {
//         DrawAreaChart("##spark", history, HISTORY_SIZE, historyOffset,
//             100.0f, ImVec2(ImGui::GetContentRegionAvail().x, 32.0f),
//             accent, "%", false);
//     }
//     else
//     {
//         ImGui::Spacing();
//         ImGui::TextDisabled("%s", subtitle);
//     }

//     // Hover highlight + click to open the related page
//     if (linkPage >= 0 && ImGui::IsWindowHovered())
//     {
//         const ImVec2 pos  = ImGui::GetWindowPos();
//         const ImVec2 size = ImGui::GetWindowSize();

//         ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

//         ImGui::GetWindowDrawList()->AddRect(
//             pos, ImVec2(pos.x + size.x, pos.y + size.y),
//             ImGui::GetColorU32(WithAlpha(accent, 0.85f)),
//             ImGui::GetStyle().ChildRounding, 0, 1.5f);

//         ImGui::SetTooltip("Open %s", pages[linkPage]);

//         if (ImGui::IsMouseClicked(0))
//             selectedPage = linkPage;
//     }

//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // NAVIGATION: persistent sidebar grouped by security domain,
// // plus a top bar that shows the current page.
// // ------------------------------------------------------------

// struct HidsFeatureRoute
// {
//     const char* title;
//     const char* description;
//     int page;
// };

// struct HidsDomainRoute
// {
//     const char* title;
//     const char* description;
//     const char* icon;
//     const HidsFeatureRoute* features;
//     int featureCount;
// };

// static const HidsFeatureRoute monitoringFeatures[] =
// {
//     { "Security Dashboard", "Live host health, threat activity and overview", 0 },
//     { "Process Monitoring", "Inspect running processes and resource usage", 1 },
//     { "Login Monitoring", "Review failed sign-in activity", 2 },
//     { "File Monitoring", "Review changes in the monitored folder", 3 }
// };

// static const HidsFeatureRoute threatFeatures[] =
// {
//     { "Alert Center", "Review and filter security alerts", 4 },
//     { "File Scanner", "Run quick, full and custom file scans", 7 }
// };

// static const HidsFeatureRoute analysisFeatures[] =
// {
//     { "System Analysis", "Resource trends, anomalies and process findings", 6 }
// };

// static const HidsFeatureRoute reportFeatures[] =
// {
//     { "Logs & Reports", "Review protected detection signatures and reports", 5 }
// };

// static const HidsFeatureRoute configurationFeatures[] =
// {
//     { "Settings & Notifications", "Themes, interface scale and desktop alerts", 8 }
// };

// static const HidsDomainRoute hidsDomains[] =
// {
//     { "Monitoring", "Observe activity and changes across this host",
//       "MON", monitoringFeatures, IM_ARRAYSIZE(monitoringFeatures) },
//     { "Threat Detection", "Investigate alerts and scan suspicious files",
//       "THR", threatFeatures, IM_ARRAYSIZE(threatFeatures) },
//     { "Analysis", "Understand system behaviour and resource anomalies",
//       "ANA", analysisFeatures, IM_ARRAYSIZE(analysisFeatures) },
//     { "Reports & Logs", "Access security records, signatures and reports",
//       "LOG", reportFeatures, IM_ARRAYSIZE(reportFeatures) },
//     { "Configuration", "Manage appearance, notifications and preferences",
//       "CFG", configurationFeatures, IM_ARRAYSIZE(configurationFeatures) }
// };

// static const float SIDEBAR_WIDTH = 252.0f;

// static const char* domainLabels[] =
//     { "MONITORING", "THREAT DETECTION", "ANALYSIS", "REPORTS & LOGS", "CONFIGURATION" };

// static int DomainOfPage(int page)
// {
//     for (int d = 0; d < IM_ARRAYSIZE(hidsDomains); ++d)
//         for (int f = 0; f < hidsDomains[d].featureCount; ++f)
//             if (hidsDomains[d].features[f].page == page)
//                 return d;

//     return 0;
// }

// static void StartEngine()
// {
//     backend.start();
//     monitoringActive = backend.isRunning();
// }

// static void StopEngine()
// {
//     backend.stop();
//     monitoringActive = false;
// }

// // Logo tile + product name
// static void DrawBrand()
// {
//     ImDrawList* d = ImGui::GetWindowDrawList();
//     const ImVec2 p = ImGui::GetCursorScreenPos();
//     const float  s = 38.0f;

//     d->AddRectFilled(p, ImVec2(p.x + s, p.y + s),
//         ImGui::GetColorU32(WithAlpha(COLOR_CYAN, 0.16f)), 10.0f);
//     d->AddRect(p, ImVec2(p.x + s, p.y + s),
//         ImGui::GetColorU32(WithAlpha(COLOR_CYAN, 0.65f)), 10.0f, 0, 1.2f);

//     // little shield
//     const ImVec2 c(p.x + s * 0.5f, p.y + s * 0.5f);
//     ImVec2 shield[5] = {
//         ImVec2(c.x - 8.0f, c.y - 9.0f), ImVec2(c.x + 8.0f, c.y - 9.0f),
//         ImVec2(c.x + 8.0f, c.y + 1.0f), ImVec2(c.x,         c.y + 10.0f),
//         ImVec2(c.x - 8.0f, c.y + 1.0f)
//     };
//     d->AddConvexPolyFilled(shield, 5, ImGui::GetColorU32(COLOR_CYAN));

//     ImGui::Dummy(ImVec2(s, s));
//     ImGui::SameLine(0.0f, 12.0f);

//     ImGui::BeginGroup();
//     ImGui::SetWindowFontScale(1.25f);
//     ImGui::TextUnformatted("HIDS");
//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::TextDisabled("Security Platform");
//     ImGui::EndGroup();
// }

// // Status line + one Start/Stop button
// static void DrawEngineControl()
// {
//     const ImVec4 col = monitoringActive ? COLOR_GREEN : COLOR_YELLOW;

//     StatusDot(col, monitoringActive);
//     ImGui::SameLine(0.0f, 4.0f);
//     ImGui::AlignTextToFramePadding();
//     ImGui::TextColored(col, "%s",
//         monitoringActive ? "Protection active" : "Protection stopped");

//     if (!monitoringActive)
//     {
//         ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.02f, 0.48f, 0.31f, 1.0f));
//         ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.04f, 0.62f, 0.40f, 1.0f));
//         ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.03f, 0.40f, 0.26f, 1.0f));
//         if (ImGui::Button("Start protection", ImVec2(-1.0f, 38.0f)))
//             StartEngine();
//         ImGui::PopStyleColor(3);
//     }
//     else
//     {
//         ImGui::PushStyleColor(ImGuiCol_Button,        WithAlpha(COLOR_RED, 0.14f));
//         ImGui::PushStyleColor(ImGuiCol_ButtonHovered, WithAlpha(COLOR_RED, 0.30f));
//         ImGui::PushStyleColor(ImGuiCol_ButtonActive,  WithAlpha(COLOR_RED, 0.45f));
//         if (ImGui::Button("Stop protection", ImVec2(-1.0f, 38.0f)))
//             StopEngine();
//         ImGui::PopStyleColor(3);
//     }
// }

// // One sidebar row: accent bar when selected, optional count badge on the right
// static bool NavItem(const char* label, bool selected, int badge)
// {
//     ImGui::PushID(label);

//     const float h = 38.0f;
//     const bool clicked = ImGui::Selectable("##nav", selected, 0, ImVec2(0.0f, h));

//     const ImVec2 mn = ImGui::GetItemRectMin();
//     const ImVec2 mx = ImGui::GetItemRectMax();
//     ImDrawList*  d  = ImGui::GetWindowDrawList();

//     if (selected)
//         d->AddRectFilled(ImVec2(mn.x, mn.y + 8.0f), ImVec2(mn.x + 3.0f, mx.y - 8.0f),
//             ImGui::GetColorU32(COLOR_CYAN), 2.0f);

//     d->AddText(
//         ImVec2(mn.x + 16.0f, mn.y + (h - ImGui::GetTextLineHeight()) * 0.5f),
//         ImGui::GetColorU32(selected ? COLOR_TEXT : COLOR_MUTED), label);

//     if (badge > 0)
//     {
//         char text[16];

//         if (badge > 99)
//             std::snprintf(text, sizeof(text), "99+");
//         else
//             std::snprintf(text, sizeof(text), "%d", badge);

//         const ImVec4 bc = CountSeverity("Critical") > 0 ? COLOR_RED : COLOR_ORANGE;
//         const ImVec2 ts = ImGui::CalcTextSize(text);
//         const float  w  = ts.x + 14.0f;
//         const float  hh = ts.y + 2.0f;
//         const ImVec2 a(mx.x - w - 10.0f, mn.y + (h - hh) * 0.5f);

//         d->AddRectFilled(a, ImVec2(a.x + w, a.y + hh),
//             ImGui::GetColorU32(WithAlpha(bc, 0.22f)), hh * 0.5f);
//         d->AddText(ImVec2(a.x + 7.0f, a.y + 1.0f), ImGui::GetColorU32(bc), text);
//     }

//     ImGui::PopID();
//     return clicked;
// }

// static void DrawSidebar()
// {
//     ImGui::PushStyleColor(ImGuiCol_ChildBg, COLOR_SIDEBAR);
//     ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14.0f, 16.0f));
//     ImGui::BeginChild("Sidebar", ImVec2(SIDEBAR_WIDTH, 0.0f), true,
//                       ImGuiWindowFlags_NoScrollbar);
//     ImGui::PopStyleVar();

//     DrawBrand();
//     ImGui::Spacing();
//     ImGui::Separator();
//     ImGui::Spacing();

//     DrawEngineControl();

//     ImGui::Spacing();
//     ImGui::Separator();

//     for (int d = 0; d < IM_ARRAYSIZE(hidsDomains); ++d)
//     {
//         ImGui::Spacing();
//         ImGui::TextColored(WithAlpha(COLOR_MUTED, 0.75f), "%s", domainLabels[d]);

//         const HidsDomainRoute& domain = hidsDomains[d];

//         for (int i = 0; i < domain.featureCount; ++i)
//         {
//             const HidsFeatureRoute& f = domain.features[i];
//             const int badge = (f.page == 4) ? (int)alerts.size() : 0;

//             if (NavItem(f.title, selectedPage == f.page, badge))
//                 selectedPage = f.page;
//         }
//     }

//     // Footer pinned to the bottom
//     const float footerY = ImGui::GetWindowHeight() - 84.0f;

//     if (ImGui::GetCursorPosY() < footerY)
//         ImGui::SetCursorPosY(footerY);

//     ImGui::Separator();
//     ImGui::Spacing();
//     DataSourceBadge(monitoringActive);
//     ImGui::TextDisabled("HIDS Prototype v2.0");

//     ImGui::EndChild();
//     ImGui::PopStyleColor();
// }

// static void DrawTopBar()
// {
//     ImGui::BeginChild("TopBar", ImVec2(0.0f, 84.0f), false,
//                       ImGuiWindowFlags_NoScrollbar);

//     ImGui::BeginGroup();
//     ImGui::TextColored(COLOR_CYAN, "%s", domainLabels[DomainOfPage(selectedPage)]);
//     ImGui::SetWindowFontScale(1.5f);
//     ImGui::TextUnformatted(pages[selectedPage]);
//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::TextDisabled("%s", pageSubtitles[selectedPage]);
//     ImGui::EndGroup();

//     const ImVec4 col = monitoringActive ? COLOR_GREEN : COLOR_YELLOW;

//     ImGui::SameLine(ImGui::GetWindowWidth() - 210.0f);
//     StatusDot(col, monitoringActive);
//     ImGui::SameLine(0.0f, 4.0f);
//     ImGui::AlignTextToFramePadding();
//     ImGui::TextColored(col, "%s",
//         monitoringActive ? "Monitoring active" : "Monitoring stopped");

//     ImGui::EndChild();
// }

// // Call-to-action shown on the dashboard only while protection is off
// static void DrawEngineBanner()
// {
//     if (monitoringActive)
//         return;

//     ImGui::BeginChild("EngineBanner", ImVec2(0.0f, 92.0f), true,
//                       ImGuiWindowFlags_NoScrollbar);

//     {
//         const ImVec2 p  = ImGui::GetWindowPos();
//         const ImVec2 sz = ImGui::GetWindowSize();

//         ImGui::GetWindowDrawList()->AddRectFilled(
//             p, ImVec2(p.x + 4.0f, p.y + sz.y),
//             ImGui::GetColorU32(COLOR_YELLOW),
//             ImGui::GetStyle().ChildRounding, ImDrawFlags_RoundCornersLeft);
//     }

//     ImGui::BeginGroup();
//     ImGui::SetWindowFontScale(1.2f);
//     ImGui::TextUnformatted("Protection is stopped");
//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::TextDisabled("Start the engine to collect host events, watch processes "
//                         "and the monitored folder, and raise alerts.");
//     ImGui::EndGroup();

//     ImGui::SameLine(ImGui::GetWindowWidth() - 200.0f);
//     ImGui::SetCursorPosY(26.0f);

//     ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.02f, 0.48f, 0.31f, 1.0f));
//     ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.04f, 0.62f, 0.40f, 1.0f));
//     ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.03f, 0.40f, 0.26f, 1.0f));
//     if (ImGui::Button("Start protection", ImVec2(176.0f, 40.0f)))
//         StartEngine();
//     ImGui::PopStyleColor(3);

//     ImGui::EndChild();
//     ImGui::Spacing();
// }

// static void DrawMetrics()
// {
//     const float spacing = ImGui::GetStyle().ItemSpacing.x;

//     const float cardWidth =
//         (ImGui::GetContentRegionAvail().x - spacing * 3.0f) / 4.0f;

//     char cpuText[16];
//     char ramText[16];
//     char alertText[16];

//     // Displayed values glide toward the live ones instead of jumping
//     static float cpuShown = 0.0f;
//     static float ramShown = 0.0f;

//     const float ease = ClampFloat(ImGui::GetIO().DeltaTime * 6.0f, 0.0f, 1.0f);
//     cpuShown += (cpuNow - cpuShown) * ease;
//     ramShown += (ramNow - ramShown) * ease;

//     std::snprintf(cpuText, sizeof(cpuText), "%.0f%%", cpuShown);
//     std::snprintf(ramText, sizeof(ramText), "%.0f%%", ramShown);
//     std::snprintf(alertText, sizeof(alertText), "%d", (int)alerts.size());

//     const ImVec4 alertColor = CountSeverity("Critical") > 0 ? COLOR_RED : COLOR_YELLOW;

//     char alertSubtitle[64];
//     std::snprintf(
//         alertSubtitle, sizeof(alertSubtitle),
//         "%d critical, %d high",
//         CountSeverity("Critical"), CountSeverity("High")
//     );

//     MetricCard("CPU usage", cpuText, "", LoadColor(cpuNow), cardWidth, cpuHistory, 6, cpuShown);
//     ImGui::SameLine();

//     MetricCard("RAM usage", ramText, "", LoadColor(ramNow), cardWidth, ramHistory, 6, ramShown);
//     ImGui::SameLine();

//     char processText[16];
//     std::snprintf(processText, sizeof(processText), "%d", processCount);

//     MetricCard("Running processes", processText, "On this host", COLOR_BLUE, cardWidth, nullptr, 1);
//     ImGui::SameLine();

//     MetricCard("Total alerts", alertText, alertSubtitle, alertColor, cardWidth, nullptr, 4);
// }

// // ------------------------------------------------------------
// // THREAT ACTIVITY PANEL (dashboard)
// // Histogram of new alerts per second + clickable severity bars.
// // ------------------------------------------------------------

// static void DrawThreatPanel()
// {
//     ImGui::BeginChild("ThreatPanel", ImVec2(0.0f, 196.0f), true,
//                       ImGuiWindowFlags_NoScrollbar);

//     SectionHeading("Threat activity",
//         "new alerts per second, weighted by severity (last 60 s)");

//     float peak = 6.0f;
//     for (float v : threatHistory)
//         peak = std::max(peak, v);

//     const float leftW = ImGui::GetContentRegionAvail().x * 0.62f;

//     DrawBarChart("##threat", threatHistory, HISTORY_SIZE, historyOffset,
//         peak, ImVec2(leftW, 122.0f));

//     ImGui::SameLine();
//     ImGui::BeginGroup();

//     static const char* names[] = { "Critical", "High", "Medium", "Low" };
//     const int total = std::max(1, (int)alerts.size());

//     for (int i = 0; i < 4; ++i)
//     {
//         const int count = CountSeverity(names[i]);

//         char label[40];
//         std::snprintf(label, sizeof(label), "%s  %d", names[i], count);

//         ImGui::PushID(i);
//         ImGui::PushStyleColor(ImGuiCol_PlotHistogram,
//             WithAlpha(SeverityColor(names[i]), 0.85f));
//         ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_FRAME);

//         ImGui::ProgressBar((float)count / (float)total,
//             ImVec2(ImGui::GetContentRegionAvail().x, 20.0f), label);

//         ImGui::PopStyleColor(2);

//         if (ImGui::IsItemHovered())
//         {
//             ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
//             ImGui::SetTooltip("Show %s alerts", names[i]);
//         }

//         if (ImGui::IsItemClicked())
//         {
//             selectedPage   = 4;        // Alerts
//             severityFilter = i + 1;
//         }

//         ImGui::PopID();
//     }

//     ImGui::EndGroup();
//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // PASSWORD LOCK (Logs & Reports page and End Task)
// // ------------------------------------------------------------

// static const double LOCK_IDLE_SECONDS = 300.0;     // auto-lock after 5 min idle
// static const int    LOGS_PAGE_INDEX   = 5;

// static bool        logsUnlocked   = false;
// static double      logsLastActive = 0.0;
// static char        lockOld[64]     = "";
// static char        lockNew[64]     = "";
// static char        lockConfirm[64] = "";
// static char        lockEntry[64]   = "";
// static std::string lockMessage;
// static bool        lockMessageOk   = false;

// static void Wipe(char* buffer, std::size_t size)
// {
//     std::memset(buffer, 0, size);
// }

// // "Create a password" form. Returns true on the frame it is created.
// static bool DrawSetPasswordForm()
// {
//     bool created = false;

//     ImGui::TextDisabled("Create a password (at least 6 characters)");

//     ImGui::SetNextItemWidth(260.0f);
//     ImGui::InputTextWithHint("##newpw", "New password",
//         lockNew, sizeof(lockNew), ImGuiInputTextFlags_Password);

//     ImGui::SetNextItemWidth(260.0f);
//     ImGui::InputTextWithHint("##confpw", "Confirm password",
//         lockConfirm, sizeof(lockConfirm), ImGuiInputTextFlags_Password);

//     if (ImGui::Button("Set Password"))
//     {
//         if (std::strcmp(lockNew, lockConfirm) != 0)
//         {
//             lockMessage   = "Passwords do not match.";
//             lockMessageOk = false;
//         }
//         else
//         {
//             std::string message;

//             if (backend.setPassword(lockNew, "", message))
//             {
//                 created = true;
//                 Wipe(lockNew, sizeof(lockNew));
//                 Wipe(lockConfirm, sizeof(lockConfirm));
//             }

//             lockMessage   = message;
//             lockMessageOk = created;
//         }
//     }

//     return created;
// }

// static void DrawLockScreen()
// {
//     ImGui::BeginChild("LockScreen", ImVec2(0.0f, 0.0f), true);

//     SectionHeading("Logs & Reports are locked",
//         "enter the password to see the detection signatures");

//     ImGui::Spacing();

//     if (!backend.isLockConfigured())
//     {
//         if (DrawSetPasswordForm())
//             logsUnlocked = true;          // whoever creates it is let in
//     }
//     else
//     {
//         const int wait = backend.lockoutSecondsLeft();

//         ImGui::BeginDisabled(wait > 0);

//         ImGui::SetNextItemWidth(260.0f);
//         const bool enter = ImGui::InputTextWithHint("##unlockpw", "Password",
//             lockEntry, sizeof(lockEntry),
//             ImGuiInputTextFlags_Password | ImGuiInputTextFlags_EnterReturnsTrue);

//         ImGui::SameLine();
//         const bool click = ImGui::Button("Unlock");

//         ImGui::EndDisabled();

//         if (wait > 0)
//             ImGui::TextColored(COLOR_YELLOW, "Locked. Try again in %d s.", wait);

//         if ((enter || click) && wait == 0)
//         {
//             std::string message;

//             if (backend.verifyPassword(lockEntry, message))
//             {
//                 logsUnlocked = true;
//                 lockMessage.clear();
//             }
//             else
//             {
//                 lockMessage   = message;
//                 lockMessageOk = false;
//             }

//             Wipe(lockEntry, sizeof(lockEntry));
//         }
//     }

//     if (!lockMessage.empty())
//         ImGui::TextColored(lockMessageOk ? COLOR_GREEN : COLOR_RED,
//             "%s", lockMessage.c_str());

//     ImGui::EndChild();
// }

// // true = unlocked, the page may draw its content
// static bool LogsGate()
// {
//     const double now = ImGui::GetTime();
//     ImGuiIO& io = ImGui::GetIO();

//     const bool active =
//         io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f ||
//         io.MouseWheel != 0.0f || ImGui::IsMouseClicked(0) ||
//         ImGui::IsAnyItemActive();

//     if (active || !logsUnlocked)
//         logsLastActive = now;

//     if (logsUnlocked && now - logsLastActive > LOCK_IDLE_SECONDS)
//     {
//         logsUnlocked  = false;
//         lockMessage   = "Locked after 5 minutes of inactivity.";
//         lockMessageOk = false;
//     }

//     if (logsUnlocked)
//         return true;

//     DrawLockScreen();
//     return false;
// }

// // ------------------------------------------------------------
// // END TASK (shared by the Processes page and resource alerts)
// // ------------------------------------------------------------

// static const float HIGH_MEMORY_MB = 500.0f;

// static bool        procAppsOnly     = false;
// static bool        procHighMemOnly  = false;
// static std::string procMessage;                 // result of the last End Task
// static bool        procMessageOk    = true;
// static int         pendingPid       = 0;
// static std::string pendingName;
// static float       pendingMem       = 0.0f;
// static int         pendingAlertID   = 0;        // 0 = started from the Processes page
// static bool        openEndTaskPopup = false;
// static std::set<int> handledAlertIDs;           // alerts whose app was ended

// static char        endPassword[64] = "";
// static std::string endPopupMessage;

// // Call once inside the page that may open the confirmation dialog.
// // Ending a task needs the access password.
// static void DrawEndTaskPopup()
// {
//     if (openEndTaskPopup)
//     {
//         ImGui::OpenPopup("Confirm End Task");
//         openEndTaskPopup = false;

//         Wipe(endPassword, sizeof(endPassword));
//         endPopupMessage.clear();
//         lockMessage.clear();
//     }

//     const ImVec2 screen = ImGui::GetIO().DisplaySize;
//     ImGui::SetNextWindowPos(
//         ImVec2(screen.x * 0.5f, screen.y * 0.5f),
//         ImGuiCond_Appearing,
//         ImVec2(0.5f, 0.5f));

//     if (ImGui::BeginPopupModal("Confirm End Task", nullptr,
//             ImGuiWindowFlags_AlwaysAutoResize))
//     {
//         ImGui::Text("End %s (PID %d)?", pendingName.c_str(), pendingPid);
//         ImGui::TextDisabled("It is using %.0f MB. Unsaved work in this app will be lost.",
//             pendingMem);
//         ImGui::Spacing();

//         if (!backend.isLockConfigured())
//         {
//             ImGui::TextColored(COLOR_YELLOW,
//                 "Ending tasks is password protected. Create a password first.");
//             ImGui::Spacing();

//             DrawSetPasswordForm();

//             if (!lockMessage.empty())
//                 ImGui::TextColored(lockMessageOk ? COLOR_GREEN : COLOR_RED, "%s", lockMessage.c_str());
//         }
//         else
//         {
//             const int wait = backend.lockoutSecondsLeft();

//             ImGui::BeginDisabled(wait > 0);

//             ImGui::SetNextItemWidth(260.0f);
//             const bool enter = ImGui::InputTextWithHint("##endpw", "Password",
//                 endPassword, sizeof(endPassword),
//                 ImGuiInputTextFlags_Password | ImGuiInputTextFlags_EnterReturnsTrue);

//             ImGui::EndDisabled();

//             if (wait > 0)
//                 ImGui::TextColored(COLOR_YELLOW, "Locked. Try again in %d s.", wait);

//             if (!endPopupMessage.empty())
//                 ImGui::TextColored(COLOR_RED, "%s", endPopupMessage.c_str());

//             ImGui::Spacing();

//             ImGui::BeginDisabled(wait > 0);
//             const bool click = ImGui::Button("End Task", ImVec2(120.0f, 0.0f));
//             ImGui::EndDisabled();

//             if ((enter || click) && wait == 0)
//             {
//                 std::string authMessage;
//                 const bool authorised = backend.verifyPassword(endPassword, authMessage);

//                 Wipe(endPassword, sizeof(endPassword));

//                 if (!authorised)
//                 {
//                     endPopupMessage = authMessage;
//                 }
//                 else
//                 {
//                     std::string result;

//                     procMessageOk = backend.endTask(pendingPid, result, pendingName);
//                     procMessage   = result;

//                     if (procMessageOk && pendingAlertID != 0)
//                         handledAlertIDs.insert(pendingAlertID);

//                     ImGui::CloseCurrentPopup();
//                 }
//             }

//             ImGui::SameLine();
//         }

//         if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
//         {
//             Wipe(endPassword, sizeof(endPassword));
//             Wipe(lockNew, sizeof(lockNew));
//             Wipe(lockConfirm, sizeof(lockConfirm));
//             ImGui::CloseCurrentPopup();
//         }

//         ImGui::EndPopup();
//     }
// }

// // ------------------------------------------------------------
// // ALERT TABLE
// // compact = true: dashboard preview. false: full Alerts page.
// // ------------------------------------------------------------

// static void DrawAlerts(bool compact)
// {
//     ImGui::BeginChild(
//         "AlertsPanel",
//         ImVec2(0.0f, compact ? 270.0f : 0.0f),
//         true
//     );

//     SectionHeading(
//         compact ? "Recent security alerts" : "All security alerts"
//     );

//     // Severity filter + search (full page only)
//     if (!compact)
//     {
//         for (int i = 0; i < IM_ARRAYSIZE(severityFilterLabels); ++i)
//         {
//             const bool active = (severityFilter == i);

//             if (active)
//                 ImGui::PushStyleColor(ImGuiCol_Button, COLOR_BLUE);
//             else
//                 ImGui::PushStyleColor(ImGuiCol_Button, COLOR_FRAME);

//             if (ImGui::Button(severityFilterLabels[i], ImVec2(96.0f, 0.0f)))
//                 severityFilter = i;

//             ImGui::PopStyleColor();

//             if (i + 1 < IM_ARRAYSIZE(severityFilterLabels))
//                 ImGui::SameLine();
//         }

//         ImGui::SameLine();
//         ImGui::SetNextItemWidth(260.0f);
//         ImGui::InputTextWithHint("##alertsearch", "Search alerts",
//             alertSearch, IM_ARRAYSIZE(alertSearch));

//         ImGui::Spacing();
//     }

//     if (!procMessage.empty())
//         ImGui::TextColored(procMessageOk ? COLOR_GREEN : COLOR_RED,
//             "%s", procMessage.c_str());

//     DrawEndTaskPopup();

//     if (ImGui::BeginTable(
//             "AlertsTable",
//             5,
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);
//         ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 110.0f);
//         ImGui::TableSetupColumn("Severity", ImGuiTableColumnFlags_WidthFixed, 130.0f);
//         ImGui::TableSetupColumn("Source", ImGuiTableColumnFlags_WidthFixed, 200.0f);
//         ImGui::TableSetupColumn("Description");
//         ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 100.0f);
//         ImGui::TableHeadersRow();

//         int shown = 0;

//         for (const auto& alert : alerts)
//         {
//             if (!compact && severityFilter != 0 &&
//                 alert.severity != severityFilterLabels[severityFilter])
//             {
//                 continue;
//             }

//             if (!compact &&
//                 !ContainsNoCase(alert.description.c_str(), alertSearch) &&
//                 !ContainsNoCase(alert.source.c_str(), alertSearch))
//             {
//                 continue;
//             }

//             ++shown;

//             ImGui::TableNextRow();

//             ImGui::TableSetColumnIndex(0);
//             {
//                 const ImVec2 p = ImGui::GetCursorScreenPos();

//                 ImGui::GetWindowDrawList()->AddCircleFilled(
//                     ImVec2(p.x + 4.0f, p.y + ImGui::GetTextLineHeight() * 0.5f),
//                     4.0f,
//                     ImGui::GetColorU32(SeverityColor(alert.severity.c_str())));

//                 ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 16.0f);
//             }
//             ImGui::TextDisabled("%s", alert.time.c_str());

//             ImGui::TableSetColumnIndex(1);
//             SeverityBadge(alert.severity.c_str());

//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextUnformatted(alert.source.c_str());

//             ImGui::TableSetColumnIndex(3);
//             ImGui::TextUnformatted(alert.description.c_str());

//             // Resource alerts can offer End Task for the app causing them
//             ImGui::TableSetColumnIndex(4);

//             if (alert.canEndTask)
//             {
//                 if (handledAlertIDs.count(alert.id))
//                 {
//                     ImGui::TextColored(COLOR_GREEN, "Ended");
//                 }
//                 else
//                 {
//                     ImGui::PushID(alert.id);

//                     ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.55f, 0.12f, 0.15f, 1.0f));
//                     ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.18f, 0.22f, 1.0f));
//                     ImGui::PushStyleColor(ImGuiCol_ButtonActive,  COLOR_RED);

//                     if (ImGui::SmallButton("End Task"))
//                     {
//                         pendingAlertID   = alert.id;
//                         pendingPid       = alert.culpritPid;
//                         pendingName      = alert.culpritName;
//                         pendingMem       = alert.culpritMem;
//                         openEndTaskPopup = true;
//                     }

//                     ImGui::PopStyleColor(3);
//                     ImGui::PopID();
//                 }
//             }
//             else
//             {
//                 ImGui::TextDisabled("-");
//             }
//         }

//         if (shown == 0)
//         {
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(3);
//             ImGui::TextDisabled("No alerts match the current filters.");
//         }

//         ImGui::EndTable();
//     }

//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // PROCESS TABLE
// // Sortable columns, case-insensitive search, CPU usage bars.
// // ------------------------------------------------------------

// static void DrawProcesses()
// {
//     ImGui::BeginChild(
//         "ProcessesPanel",
//         ImVec2(0.0f, 0.0f),
//         true
//     );

//     SectionHeading("Running processes",
//         "apps can be ended here; system and background processes are protected");

//     ImGui::SetNextItemWidth(300.0f);

//     ImGui::InputTextWithHint(
//         "##ProcessSearch",
//         "Search by name or PID",
//         processSearch,
//         IM_ARRAYSIZE(processSearch)
//     );

//     ImGui::SameLine();
//     ImGui::Checkbox("Apps only", &procAppsOnly);

//     ImGui::SameLine();

//     char highLabel[64];
//     std::snprintf(highLabel, sizeof(highLabel),
//         "High memory only (%.0f MB+)", HIGH_MEMORY_MB);
//     ImGui::Checkbox(highLabel, &procHighMemOnly);

//     if (!procMessage.empty())
//         ImGui::TextColored(procMessageOk ? COLOR_GREEN : COLOR_RED,
//             "%s", procMessage.c_str());

//     ImGui::Spacing();

//     if (ImGui::BeginTable(
//             "ProcessTable",
//             6,
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_Sortable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);

//         ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed, 85.0f, 0);
//         ImGui::TableSetupColumn("Process name", ImGuiTableColumnFlags_WidthStretch, 0.0f, 1);
//         ImGui::TableSetupColumn(
//             "CPU %",
//             ImGuiTableColumnFlags_WidthFixed |
//             ImGuiTableColumnFlags_PreferSortDescending,
//             180.0f, 2);
//         ImGui::TableSetupColumn(
//             "Memory MB",
//             ImGuiTableColumnFlags_WidthFixed |
//             ImGuiTableColumnFlags_DefaultSort |
//             ImGuiTableColumnFlags_PreferSortDescending,
//             125.0f, 3);
//         ImGui::TableSetupColumn("Running time", ImGuiTableColumnFlags_WidthFixed, 135.0f, 4);
//         ImGui::TableSetupColumn("Action",
//             ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoSort,
//             100.0f, 5);

//         ImGui::TableHeadersRow();

//         // Build a sorted list of indices
//         std::vector<int> order(processes.size());

//         for (size_t i = 0; i < order.size(); ++i)
//             order[i] = (int)i;

//         if (ImGuiTableSortSpecs* specs = ImGui::TableGetSortSpecs())
//         {
//             if (specs->SpecsCount > 0)
//             {
//                 const ImGuiTableColumnSortSpecs& spec = specs->Specs[0];
//                 const bool asc =
//                     spec.SortDirection == ImGuiSortDirection_Ascending;

//                 std::sort(order.begin(), order.end(),
//                     [&](int a, int b)
//                     {
//                         const ProcessInfo& pa = processes[a];
//                         const ProcessInfo& pb = processes[b];

//                         int cmp = 0;

//                         switch (spec.ColumnUserID)
//                         {
//                         case 0: cmp = (pa.pid > pb.pid) - (pa.pid < pb.pid); break;
//                         case 1: cmp = pa.name.compare(pb.name); break;
//                         case 2: cmp = (pa.cpu > pb.cpu) - (pa.cpu < pb.cpu); break;
//                         case 3: cmp = (pa.memory > pb.memory) - (pa.memory < pb.memory); break;
//                         case 4: cmp = (pa.runningSeconds > pb.runningSeconds) - (pa.runningSeconds < pb.runningSeconds); break;
//                         }

//                         return asc ? (cmp < 0) : (cmp > 0);
//                     });
//             }
//         }

//         int shown = 0;

//         for (int index : order)
//         {
//             const ProcessInfo& process = processes[index];

//             if (procAppsOnly && !process.isApp)
//                 continue;

//             if (procHighMemOnly && process.memory < HIGH_MEMORY_MB)
//                 continue;

//             char pidText[32];
//             std::snprintf(pidText, sizeof(pidText), "%d", process.pid);

//             const bool matches =
//                 ContainsNoCase(process.name.c_str(), processSearch) ||
//                 ContainsNoCase(pidText, processSearch);

//             if (!matches)
//                 continue;

//             ++shown;

//             ImGui::TableNextRow();

//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextDisabled("%d", process.pid);

//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextUnformatted(process.name.c_str());

//             if (process.isApp)
//             {
//                 ImGui::SameLine();
//                 ImGui::TextDisabled("- %s", process.title.c_str());
//             }

//             // CPU bar, scaled so the busiest realistic process fills it
//             ImGui::TableSetColumnIndex(2);

//             char cpuLabel[16];
//             std::snprintf(cpuLabel, sizeof(cpuLabel), "%.1f", process.cpu);

//             ImGui::PushStyleColor(ImGuiCol_PlotHistogram, WithAlpha(LoadColor(process.cpu * 4.0f), 0.85f));
//             ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_FRAME);

//             ImGui::ProgressBar(
//                 std::min(process.cpu / 25.0f, 1.0f),
//                 ImVec2(-FLT_MIN, 18.0f),
//                 cpuLabel
//             );

//             ImGui::PopStyleColor(2);

//             // Memory: yellow when high, red when very high
//             ImGui::TableSetColumnIndex(3);

//             if (process.memory >= HIGH_MEMORY_MB * 2.0f)
//                 ImGui::TextColored(COLOR_RED, "%.1f", process.memory);
//             else if (process.memory >= HIGH_MEMORY_MB)
//                 ImGui::TextColored(COLOR_YELLOW, "%.1f", process.memory);
//             else
//                 ImGui::Text("%.1f", process.memory);

//             ImGui::TableSetColumnIndex(4);
//             ImGui::TextDisabled("%s", process.runningTime.c_str());

//             // Only apps (windowed programs) get an End Task button
//             ImGui::TableSetColumnIndex(5);

//             if (process.isApp)
//             {
//                 ImGui::PushID(process.pid);

//                 ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.55f, 0.12f, 0.15f, 1.0f));
//                 ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.18f, 0.22f, 1.0f));
//                 ImGui::PushStyleColor(ImGuiCol_ButtonActive,  COLOR_RED);

//                 if (ImGui::SmallButton("End Task"))
//                 {
//                     pendingAlertID   = 0;
//                     pendingPid       = process.pid;
//                     pendingName      = process.name;
//                     pendingMem       = process.memory;
//                     openEndTaskPopup = true;
//                 }

//                 ImGui::PopStyleColor(3);
//                 ImGui::PopID();
//             }
//             else
//             {
//                 ImGui::TextDisabled("-");
//             }
//         }

//         if (shown == 0)
//         {
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextDisabled("No process matches the current filters.");
//         }

//         ImGui::EndTable();
//     }

//     DrawEndTaskPopup();

//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // EVENT PAGE
// // Newest-first activity list for Login and File monitoring.
// // ------------------------------------------------------------

// static void DrawEventPage(
//     const char* title,
//     const std::vector<EventInfo>& events,
//     const char* emptyText,
//     const char* hint)
// {
//     ImGui::BeginChild(
//         title,
//         ImVec2(0.0f, 0.0f),
//         true
//     );

//     SectionHeading(title, hint);

//     if (ImGui::BeginTable(
//             "EventTable",
//             3,
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);
//         ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 190.0f);
//         ImGui::TableSetupColumn("Event", ImGuiTableColumnFlags_WidthFixed, 170.0f);
//         ImGui::TableSetupColumn("Description");
//         ImGui::TableHeadersRow();

//         for (const EventInfo& event : events)
//         {
//             ImGui::TableNextRow();

//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextDisabled("%s", event.time.c_str());

//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextUnformatted(event.type.c_str());

//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextUnformatted(event.description.c_str());
//         }

//         if (events.empty())
//         {
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextDisabled("%s", emptyText);
//         }

//         ImGui::EndTable();
//     }

//     ImGui::EndChild();
// }

// static void DrawSystemAnalysis()
// {
//     ImGui::BeginChild("AnalysisPanel", ImVec2(0.0f, 0.0f), true);

//     if (!analysisData.valid)
//     {
//         ImGui::TextDisabled("Collecting the first samples...");
//         ImGui::EndChild();
//         return;
//     }

//     const BackendAnalysis& a = analysisData;

//     // ---- Resource usage ----
//     SectionHeading("Resource usage");

//     char cpuText[16];
//     char ramText[16];

//     if (a.cpuAvailable)
//         std::snprintf(cpuText, sizeof(cpuText), "%.1f%%", a.cpuPercent);
//     else
//         std::snprintf(cpuText, sizeof(cpuText), "n/a");

//     if (a.memoryAvailable)
//         std::snprintf(ramText, sizeof(ramText), "%.1f%%", a.memoryPercent);
//     else
//         std::snprintf(ramText, sizeof(ramText), "n/a");

//     ImGui::Text("CPU:");
//     ImGui::SameLine();
//     ImGui::TextColored(LoadColor((float)a.cpuPercent), "%s", cpuText);

//     ImGui::Text("Memory:");
//     ImGui::SameLine();
//     ImGui::TextColored(LoadColor((float)a.memoryPercent), "%s", ramText);
//     ImGui::SameLine();
//     ImGui::TextDisabled("(%llu MB used of %llu MB, %llu MB free)",
//         (unsigned long long)a.usedMemoryMB,
//         (unsigned long long)a.totalMemoryMB,
//         (unsigned long long)a.availableMemoryMB);

//     ImGui::Spacing();

//     const float half =
//         (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

//     if (!a.cpuHistory.empty())
//     {
//         ImGui::BeginGroup();
//         ImGui::TextDisabled("CPU usage (last 60 s)");
//         DrawAreaChart("##cpuh",
//             a.cpuHistory.data(), (int)a.cpuHistory.size(), 0,
//             100.0f, ImVec2(half, 170.0f),
//             LoadColor((float)a.cpuPercent));
//         ImGui::EndGroup();
//     }

//     ImGui::SameLine();

//     if (!a.ramHistory.empty())
//     {
//         ImGui::BeginGroup();
//         ImGui::TextDisabled("Memory usage (last 60 s)");
//         DrawAreaChart("##ramh",
//             a.ramHistory.data(), (int)a.ramHistory.size(), 0,
//             100.0f, ImVec2(half, 170.0f),
//             LoadColor((float)a.memoryPercent));
//         ImGui::EndGroup();
//     }

//     ImGui::Spacing();
//     ImGui::Separator();
//     ImGui::Spacing();

//     // ---- Findings ----
//     SectionHeading("Findings", "A diagnostic signal, not proof of an intrusion");

//     ImGui::Text(
//         "Processes: %llu    High CPU (>=80%%): %llu    High memory (>=1 GB): %llu",
//         (unsigned long long)a.processCount,
//         (unsigned long long)a.highCpuProcessCount,
//         (unsigned long long)a.highMemoryProcessCount);

//     if (a.findings.empty())
//     {
//         ImGui::TextColored(COLOR_GREEN, "No resource concerns detected.");
//     }
//     else
//     {
//         for (const std::string& finding : a.findings)
//             ImGui::TextColored(COLOR_YELLOW, "- %s", finding.c_str());
//     }

//     ImGui::Spacing();
//     ImGui::Separator();
//     ImGui::Spacing();

//     // ---- Top CPU processes ----
//     SectionHeading("Top processes by CPU");

//     if (ImGui::BeginTable(
//             "TopCpuTable", 4,
//             ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_SizingStretchProp))
//     {
//         ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed, 85.0f);
//         ImGui::TableSetupColumn("Process name");
//         ImGui::TableSetupColumn("CPU %", ImGuiTableColumnFlags_WidthFixed, 100.0f);
//         ImGui::TableSetupColumn("Memory MB", ImGuiTableColumnFlags_WidthFixed, 125.0f);
//         ImGui::TableHeadersRow();

//         for (const BackendProcess& p : a.topCpuProcesses)
//         {
//             ImGui::TableNextRow();

//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextDisabled("%d", p.pid);

//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextUnformatted(p.name.c_str());

//             ImGui::TableSetColumnIndex(2);
//             ImGui::Text("%.1f", p.cpuPercent);

//             ImGui::TableSetColumnIndex(3);
//             ImGui::Text("%.1f", p.memoryMB);
//         }

//         ImGui::EndTable();
//     }

//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // FILE SCANNER PAGE
// // ------------------------------------------------------------

// static std::string FormatBytes(unsigned long long b)
// {
//     char t[32];

//     if (b >= (1ULL << 30))      std::snprintf(t, sizeof(t), "%.2f GB", b / 1073741824.0);
//     else if (b >= (1ULL << 20)) std::snprintf(t, sizeof(t), "%.1f MB", b / 1048576.0);
//     else if (b >= 1024ULL)      std::snprintf(t, sizeof(t), "%.1f KB", b / 1024.0);
//     else                        std::snprintf(t, sizeof(t), "%llu B", b);

//     return t;
// }

// // Rotating-arc spinner. Reserves a (2*radius) square at the cursor.
// // active = false draws a plain ring instead (idle / finished).
// static void DrawSpinner(float radius, float thickness, const ImVec4& color, bool active)
// {
//     const ImVec2 pos = ImGui::GetCursorScreenPos();
//     const ImVec2 center(pos.x + radius, pos.y + radius);
//     ImDrawList* draw = ImGui::GetWindowDrawList();
//     const float t = (float)ImGui::GetTime();

//     if (!active)
//     {
//         draw->AddCircle(center, radius, ImGui::GetColorU32(color), 48, thickness);
//     }
//     else
//     {
//         // faint full ring + a rotating arc on top
//         draw->AddCircle(center, radius,
//             ImGui::GetColorU32(WithAlpha(color, 0.18f)), 48, thickness);

//         const float start = t * 5.0f;

//         draw->PathClear();
//         draw->PathArcTo(center, radius, start, start + 2.4f, 32);
//         draw->PathStroke(ImGui::GetColorU32(color), 0, thickness);

//         // pulsing dot in the middle
//         const float pulse = 0.75f + 0.25f * std::sin(t * 4.0f);
//         draw->AddCircleFilled(center, radius * 0.30f * pulse,
//             ImGui::GetColorU32(WithAlpha(color, 0.85f)), 24);
//     }

//     ImGui::Dummy(ImVec2(radius * 2.0f, radius * 2.0f));
// }

// // "C:\a\b\file.txt" -> dir "C:\a\b", name "file.txt"
// static void SplitPath(const std::string& full, std::string& dir, std::string& name)
// {
//     const std::size_t pos = full.find_last_of("\\/");

//     if (pos == std::string::npos)
//     {
//         dir.clear();
//         name = full;
//     }
//     else
//     {
//         dir  = full.substr(0, pos);
//         name = full.substr(pos + 1);
//     }
// }

// static void DrawFileScanner()
// {
//     // Refresh a few times per second, not every frame
//     static double lastRefresh = -1.0;
//     const double now = ImGui::GetTime();

//     if (lastRefresh < 0.0 || now - lastRefresh >= 0.25)
//     {
//         lastRefresh  = now;
//         scanStatus   = backend.getScanStatus();
//         scanFindings = backend.getScanFindings();
//     }

//     const bool running = scanStatus.running;

//     ImGui::BeginChild("ScannerPanel", ImVec2(0.0f, 0.0f), true);

//     SectionHeading("File Scanner", "Signature + heuristic scan");

//     // ---- Scan type ----
//     ImGui::BeginDisabled(running);

//     ImGui::RadioButton("Quick (user folders, temp, startup)", &scanTypeSel, 0);
//     ImGui::SameLine();
//     ImGui::RadioButton("Full (all fixed drives)", &scanTypeSel, 1);
//     ImGui::SameLine();
//     ImGui::RadioButton("Custom", &scanTypeSel, 2);

//     if (scanTypeSel == 2)
//     {
//         ImGui::SetNextItemWidth(-1.0f);
//         ImGui::InputTextWithHint("##scanpath",
//             "Folder or file, e.g. C:\\Users\\you\\Downloads",
//             scanPath, sizeof(scanPath));
//     }

//     ImGui::EndDisabled();

//     // ---- Buttons ----
//     if (!running)
//     {
//         if (ImGui::Button("Start Scan"))
//         {
//             if (scanTypeSel == 2 && scanPath[0] == '\0')
//                 scanMessage = "Enter a folder or file path first.";
//             else if (!backend.startScan((ScanType)scanTypeSel, scanPath))
//                 scanMessage = "A scan is already running.";
//             else
//                 scanMessage.clear();
//         }
//     }
//     else
//     {
//         if (ImGui::Button("Stop Scan"))
//             backend.cancelScan();
//     }

//     if (!running && scanStatus.finished)
//     {
//         ImGui::SameLine();

//         if (ImGui::Button("Export Report"))
//         {
//             std::string out;

//             scanMessage = backend.exportScanReport(out)
//                 ? "Report saved: " + out
//                 : "Could not write the report file.";
//         }
//     }

//     if (!scanMessage.empty())
//     {
//         ImGui::SameLine();
//         ImGui::TextDisabled("%s", scanMessage.c_str());
//     }

//     ImGui::Separator();
//     ImGui::Spacing();

//     // ========================================================
//     // Two panels side by side: live scan (left), threats (right)
//     // ========================================================
//     const float gap    = ImGui::GetStyle().ItemSpacing.x;
//     const float leftW  = (ImGui::GetContentRegionAvail().x - gap) * 0.58f;
//     const float panelH = 230.0f;

//     ImGui::PushStyleColor(ImGuiCol_ChildBg, COLOR_SIDEBAR);

//     // ---------------- LEFT: live scan ----------------
//     ImGui::BeginChild("ScanLive", ImVec2(leftW, panelH), true);

//     {
//         ImVec4 stateColor = COLOR_MUTED;
//         const char* stateText = "Ready to scan";

//         if (running)
//         {
//             stateColor = COLOR_CYAN;
//             stateText  = "Scanning";
//         }
//         else if (scanStatus.finished && scanStatus.cancelled)
//         {
//             stateColor = COLOR_YELLOW;
//             stateText  = "Scan stopped";
//         }
//         else if (scanStatus.finished)
//         {
//             stateColor = (scanStatus.malicious + scanStatus.suspicious > 0)
//                 ? COLOR_ORANGE : COLOR_GREEN;
//             stateText  = "Scan completed";
//         }

//         // Spinner (or a plain ring when idle)
//         DrawSpinner(26.0f, 4.0f, stateColor, running);
//         ImGui::SameLine(0.0f, 16.0f);

//         ImGui::BeginGroup();

//         ImGui::SetWindowFontScale(1.3f);
//         ImGui::PushStyleColor(ImGuiCol_Text, stateColor);

//         if (running)
//         {
//             const int dots = (int)(ImGui::GetTime() * 2.0) % 4;
//             ImGui::Text("%s%.*s", stateText, dots, "...");
//         }
//         else
//         {
//             ImGui::TextUnformatted(stateText);
//         }

//         ImGui::PopStyleColor();
//         ImGui::SetWindowFontScale(1.0f);

//         ImGui::TextDisabled("%s",
//             scanStatus.target.empty() ? "Choose a scan type and press Start"
//                                       : scanStatus.target.c_str());

//         ImGui::EndGroup();

//         ImGui::Spacing();
//         ImGui::Separator();
//         ImGui::Spacing();

//         // Which file is being scanned right now
//         ImGui::TextDisabled("%s", running ? "NOW SCANNING" : "LAST FILE SCANNED");

//         std::string dir, name;
//         SplitPath(scanStatus.currentFile, dir, name);

//         if (name.empty())
//         {
//             ImGui::TextDisabled("-");
//         }
//         else
//         {
//             ImGui::SetWindowFontScale(1.15f);
//             ImGui::PushStyleColor(ImGuiCol_Text, COLOR_TEXT);
//             ImGui::TextUnformatted(name.c_str());
//             ImGui::PopStyleColor();
//             ImGui::SetWindowFontScale(1.0f);

//             ImGui::PushStyleColor(ImGuiCol_Text, COLOR_MUTED);
//             ImGui::TextWrapped("%s", dir.c_str());
//             ImGui::PopStyleColor();
//         }

//         ImGui::Spacing();

//         // Live counters
//         ImGui::Text("Files: %llu", scanStatus.filesScanned);
//         ImGui::SameLine(0.0f, 18.0f);
//         ImGui::Text("Skipped: %llu", scanStatus.filesSkipped);
//         ImGui::SameLine(0.0f, 18.0f);
//         ImGui::Text("Data: %s", FormatBytes(scanStatus.bytesScanned).c_str());
//         ImGui::SameLine(0.0f, 18.0f);
//         ImGui::Text("Time: %s", FormatDuration(scanStatus.elapsedSeconds).c_str());
//     }

//     ImGui::EndChild();

//     ImGui::SameLine();

//     // ---------------- RIGHT: threats found ----------------
//     ImGui::BeginChild("ScanThreats", ImVec2(0.0f, panelH), true);

//     {
//         ImGui::PushStyleColor(ImGuiCol_Text, COLOR_CYAN);
//         ImGui::SetWindowFontScale(1.1f);
//         ImGui::TextUnformatted("Suspicious & malicious files");
//         ImGui::SetWindowFontScale(1.0f);
//         ImGui::PopStyleColor();

//         ImGui::TextColored(COLOR_RED, "Malicious: %d", scanStatus.malicious);
//         ImGui::SameLine(0.0f, 18.0f);
//         ImGui::TextColored(COLOR_YELLOW, "Suspicious: %d", scanStatus.suspicious);

//         ImGui::Separator();

//         if (scanFindings.empty())
//         {
//             ImGui::Spacing();

//             if (running)
//                 ImGui::TextDisabled("Nothing suspicious so far...");
//             else if (scanStatus.finished)
//                 ImGui::TextColored(COLOR_GREEN, "No threats found.");
//             else
//                 ImGui::TextDisabled("Findings will appear here.");
//         }

//         // Newest first
//         for (std::size_t i = scanFindings.size(); i-- > 0; )
//         {
//             const ScanFinding& f = scanFindings[i];
//             const ImVec4 c = (f.level == "Malicious") ? COLOR_RED : COLOR_YELLOW;

//             std::string dir, name;
//             SplitPath(f.path, dir, name);

//             ImGui::PushID((int)i);

//             ImGui::TextColored(c, "[%s]", f.level.c_str());
//             ImGui::SameLine();
//             ImGui::TextUnformatted(f.name.c_str());

//             ImGui::TextDisabled("%s", name.c_str());

//             if (ImGui::IsItemHovered())
//             {
//                 ImGui::BeginTooltip();
//                 ImGui::Text("%s", f.path.c_str());
//                 ImGui::Text("Reason: %s", f.reason.c_str());
//                 ImGui::Text("Score: %d", f.score);
//                 ImGui::EndTooltip();
//             }

//             ImGui::Separator();
//             ImGui::PopID();
//         }
//     }

//     ImGui::EndChild();

//     ImGui::PopStyleColor();   // ChildBg

//     ImGui::Spacing();

//     // ========================================================
//     // Full findings table (all details)
//     // ========================================================
//     SectionHeading("All findings", "hover a name for score and SHA-256");

//     if (ImGui::BeginTable(
//             "ScanFindings",
//             5,
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);
//         ImGui::TableSetupColumn("Time",   ImGuiTableColumnFlags_WidthFixed, 80.0f);
//         ImGui::TableSetupColumn("Level",  ImGuiTableColumnFlags_WidthFixed, 95.0f);
//         ImGui::TableSetupColumn("Name",   ImGuiTableColumnFlags_WidthFixed, 190.0f);
//         ImGui::TableSetupColumn("File");
//         ImGui::TableSetupColumn("Reason");
//         ImGui::TableHeadersRow();

//         for (const ScanFinding& f : scanFindings)
//         {
//             ImGui::TableNextRow();

//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextDisabled("%s", f.time.c_str());

//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextColored(
//                 f.level == "Malicious" ? COLOR_RED : COLOR_YELLOW,
//                 "%s", f.level.c_str());

//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextUnformatted(f.name.c_str());

//             if (ImGui::IsItemHovered())
//             {
//                 ImGui::BeginTooltip();
//                 ImGui::Text("Score: %d", f.score);
//                 ImGui::Text("Size:  %s", FormatBytes(f.size).c_str());

//                 if (!f.sha256.empty())
//                     ImGui::Text("SHA-256: %s", f.sha256.c_str());

//                 ImGui::EndTooltip();
//             }

//             ImGui::TableSetColumnIndex(3);
//             ImGui::TextUnformatted(f.path.c_str());

//             ImGui::TableSetColumnIndex(4);
//             ImGui::TextUnformatted(f.reason.c_str());
//         }

//         ImGui::EndTable();
//     }

//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // LOGS & REPORTS PAGE
// // Lists every signature / rule the File Scanner checks for.
// // ------------------------------------------------------------

// static void DrawLogsReports()
// {
//     if (!LogsGate())
//         return;

//     static std::vector<SignatureInfo> signatures;
//     static bool loaded = false;
//     static char search[128] = "";

//     if (!loaded)
//     {
//         signatures = FileScanner::getSignatures();
//         loaded = true;
//     }

//     int hashCount = 0, contentCount = 0, nameCount = 0;

//     for (const SignatureInfo& s : signatures)
//     {
//         if (s.category == "Known hash")        ++hashCount;
//         else if (s.category == "File content") ++contentCount;
//         else                                   ++nameCount;
//     }

//     ImGui::BeginChild("LogsPanel", ImVec2(0.0f, 0.0f), true);

//     SectionHeading("Detection signatures", "what the File Scanner checks for");

//     if (ImGui::Button("Lock now"))
//     {
//         logsUnlocked = false;
//         lockMessage.clear();
//     }

//     ImGui::SameLine();
//     ImGui::TextDisabled("Locks again after 5 minutes idle or when you leave this page.");

//     if (ImGui::CollapsingHeader("Change password"))
//     {
//         static std::string changeMessage;
//         static bool        changeOk = false;

//         ImGui::SetNextItemWidth(260.0f);
//         ImGui::InputTextWithHint("##oldpw", "Current password",
//             lockOld, sizeof(lockOld), ImGuiInputTextFlags_Password);

//         ImGui::SetNextItemWidth(260.0f);
//         ImGui::InputTextWithHint("##chnew", "New password",
//             lockNew, sizeof(lockNew), ImGuiInputTextFlags_Password);

//         ImGui::SetNextItemWidth(260.0f);
//         ImGui::InputTextWithHint("##chconf", "Confirm new password",
//             lockConfirm, sizeof(lockConfirm), ImGuiInputTextFlags_Password);

//         if (ImGui::Button("Update password"))
//         {
//             if (std::strcmp(lockNew, lockConfirm) != 0)
//             {
//                 changeMessage = "New passwords do not match.";
//                 changeOk      = false;
//             }
//             else
//             {
//                 changeOk = backend.setPassword(lockNew, lockOld, changeMessage);

//                 if (changeOk)
//                 {
//                     Wipe(lockOld, sizeof(lockOld));
//                     Wipe(lockNew, sizeof(lockNew));
//                     Wipe(lockConfirm, sizeof(lockConfirm));
//                 }
//             }
//         }

//         if (!changeMessage.empty())
//             ImGui::TextColored(changeOk ? COLOR_GREEN : COLOR_RED,
//                 "%s", changeMessage.c_str());
//     }

//     ImGui::Spacing();

//     ImGui::PushTextWrapPos(0.0f);
//     ImGui::TextColored(COLOR_MUTED,
//         "A file is reported Malicious on an instant match (EICAR test string or a "
//         "known-bad SHA-256). Otherwise every matching rule adds its weight to a "
//         "score, and a file scoring %d or more is reported Suspicious.",
//         FileScanner::suspiciousThreshold());
//     ImGui::PopTextWrapPos();

//     ImGui::Spacing();

//     ImGui::Text("%d rules", (int)signatures.size());
//     ImGui::SameLine(0.0f, 18.0f);
//     ImGui::TextColored(COLOR_RED,    "Known hashes: %d", hashCount);
//     ImGui::SameLine(0.0f, 18.0f);
//     ImGui::TextColored(COLOR_CYAN,   "File content: %d", contentCount);
//     ImGui::SameLine(0.0f, 18.0f);
//     ImGui::TextColored(COLOR_YELLOW, "Name / location: %d", nameCount);

//     ImGui::Spacing();

//     ImGui::SetNextItemWidth(300.0f);
//     ImGui::InputTextWithHint("##sigsearch", "Search signatures", search, IM_ARRAYSIZE(search));

//     ImGui::SameLine();

//     if (ImGui::Button("Reload"))
//         signatures = FileScanner::getSignatures();   // re-reads hids_hashes.txt

//     ImGui::Spacing();

//     if (ImGui::BeginTable(
//             "SignatureTable",
//             4,
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);
//         ImGui::TableSetupColumn("Type",      ImGuiTableColumnFlags_WidthFixed, 160.0f);
//         ImGui::TableSetupColumn("Name",      ImGuiTableColumnFlags_WidthStretch, 1.0f);
//         ImGui::TableSetupColumn("Pattern / condition", ImGuiTableColumnFlags_WidthStretch, 1.4f);
//         ImGui::TableSetupColumn("Result",    ImGuiTableColumnFlags_WidthFixed, 100.0f);
//         ImGui::TableHeadersRow();

//         int shown = 0;

//         for (const SignatureInfo& s : signatures)
//         {
//             if (!ContainsNoCase(s.name.c_str(), search) &&
//                 !ContainsNoCase(s.pattern.c_str(), search) &&
//                 !ContainsNoCase(s.category.c_str(), search))
//                 continue;

//             ++shown;

//             ImGui::TableNextRow();

//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextColored(
//                 s.category == "Known hash"   ? COLOR_RED :
//                 s.category == "File content" ? COLOR_CYAN : COLOR_YELLOW,
//                 "%s", s.category.c_str());

//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextUnformatted(s.name.c_str());

//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextDisabled("%s", s.pattern.c_str());

//             ImGui::TableSetColumnIndex(3);

//             if (s.instantMalicious)
//                 ImGui::TextColored(COLOR_RED, "Malicious");
//             else
//                 ImGui::TextColored(
//                     s.weight >= 4 ? COLOR_ORANGE :
//                     s.weight >= 2 ? COLOR_YELLOW : COLOR_MUTED,
//                     "+%d score", s.weight);
//         }

//         if (shown == 0)
//         {
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextDisabled("No signature matches \"%s\".", search);
//         }

//         ImGui::EndTable();
//     }

//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // NOTIFICATION SETTINGS (shown on the Settings page)
// // ------------------------------------------------------------

// static void DrawNotificationSettings()
// {
//     ImGui::BeginChild("NotificationSettings", ImVec2(0.0f, 150.0f), true,
//                       ImGuiWindowFlags_NoScrollbar);

//     SectionHeading("Desktop notifications",
//         "pop-up toasts when a new alert is raised");

//     bool enabled = backend.notificationsEnabled();

//     if (ImGui::Checkbox("Show desktop notifications", &enabled))
//         backend.setNotificationsEnabled(enabled);

//     ImGui::BeginDisabled(!enabled);

//     static const char* levels[] = {
//         "Low and above", "Medium and above",
//         "High and above", "Critical only"
//     };

//     int level = backend.notificationMinSeverity();

//     ImGui::SetNextItemWidth(220.0f);

//     if (ImGui::Combo("Notify for", &level, levels, IM_ARRAYSIZE(levels)))
//         backend.setNotificationMinSeverity(level);

//     if (ImGui::Button("Send test notification"))
//         backend.sendTestNotification();

//     ImGui::EndDisabled();

//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // MAIN CONTENT (everything right of the sidebar)
// // ------------------------------------------------------------

// static void DrawMainContent()
// {
//     RefreshFromBackend();

//     // Leaving Logs & Reports locks it again and clears typed credentials.
//     static int    lastPage      = -1;
//     static double pageChangedAt = 0.0;

//     if (selectedPage != lastPage)
//     {
//         if (lastPage == LOGS_PAGE_INDEX)
//         {
//             logsUnlocked = false;
//             lockMessage.clear();
//             Wipe(lockOld, sizeof(lockOld));
//             Wipe(lockNew, sizeof(lockNew));
//             Wipe(lockConfirm, sizeof(lockConfirm));
//         }

//         lastPage      = selectedPage;
//         pageChangedAt = ImGui::GetTime();
//     }

//     DrawTopBar();
//     ImGui::Separator();
//     ImGui::Spacing();

//     if (demoMode)
//     {
//         DrawPill("DEMO DATA", COLOR_YELLOW);
//         ImGui::SameLine();
//         ImGui::TextColored(COLOR_YELLOW,
//             "Sample rows are labelled [DEMO] and are not real host findings.");
//         ImGui::Spacing();
//     }

//     const float fade = ClampFloat(
//         (float)((ImGui::GetTime() - pageChangedAt) / 0.18),
//         0.0f, 1.0f
//     );
//     ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.25f + 0.75f * fade);

//     switch (selectedPage)
//     {
//     case 0:
//         DrawEngineBanner();
//         DrawMetrics();
//         ImGui::Spacing();
//         DrawThreatPanel();
//         ImGui::Spacing();
//         DrawAlerts(true);
//         break;

//     case 1:
//         DrawProcesses();
//         break;

//     case 2:
//         DrawEventPage("Failed sign-in attempts", loginEvents,
//             "No failed sign-ins recorded since monitoring started.",
//             "Reads the Windows Security log - run as Administrator");
//         break;

//     case 3:
//         DrawEventPage("File activity", fileEvents,
//             "No changes detected in the watched folder yet.",
//             "Watching the HIDS_Monitor folder");
//         break;

//     case 4:
//         DrawAlerts(false);
//         break;

//     case 5:
//         DrawLogsReports();
//         break;

//     case 6:
//         DrawSystemAnalysis();
//         break;

//     case 7:
//         DrawFileScanner();
//         break;

//     case 8:
//         DrawNotificationSettings();
//         ImGui::Spacing();
//         DrawSettingsPage(monitoringActive);
//         break;
//     }

//     ImGui::PopStyleVar();
// }

// int main()
// {
//     if (!glfwInit())
//     {
//         std::fprintf(stderr, "ERROR: Failed to initialize GLFW.\n");
//         return 1;
//     }

//     // Request OpenGL 3.0
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

//     GLFWwindow* window = glfwCreateWindow(
//         1400,
//         900,
//         "HIDS - Host Intrusion Detection System",
//         nullptr,
//         nullptr
//     );

//     if (window == nullptr)
//     {
//         std::fprintf(stderr, "ERROR: Failed to create dashboard window.\n");
//         glfwTerminate();
//         return 1;
//     }

//     glfwMakeContextCurrent(window);
//     glfwSwapInterval(1);

//     // Native window handle for the tray icon / desktop notifications
// #ifdef _WIN32
//     backend.attachWindow(glfwGetWin32Window(window));
// #endif

//     IMGUI_CHECKVERSION();
//     ImGui::CreateContext();

//     ImGuiIO& io = ImGui::GetIO();

//     io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

//     // Don't write imgui.ini next to the executable
//     io.IniFilename = nullptr;

//     LoadFonts(io);
//     LoadUiConfig();           // saved theme / interface size / demo flag
//     ApplyTheme(themeIndex);   // also builds the ImGui style
//     RefineStyle();            // flatter, more modern spacing and rounding
//     InitUiScale(io);

//     int styledTheme = themeIndex;

//     // Seed sparkline history so the cards aren't empty on first frame
//     for (int i = 0; i < HISTORY_SIZE; ++i)
//     {
//         cpuHistory[i] = cpuNow;
//         ramHistory[i] = ramNow;
//     }

//     ImGui_ImplGlfw_InitForOpenGL(window, true);
//     ImGui_ImplOpenGL3_Init("#version 130");

//     // Monitoring starts only when the user presses Start protection.
//     monitoringActive = backend.isRunning();

//     // Main application loop
//     while (!glfwWindowShouldClose(window))
//     {
//         glfwPollEvents();

//         // Send queued desktop notifications (must run on the UI thread)
//         backend.flushNotifications();

//         // Theme clicks are queued and applied here, never mid-frame
//         ApplyPendingTheme();

//         // A theme change rebuilds the style, so re-apply our refinements
//         if (themeIndex != styledTheme)
//         {
//             RefineStyle();
//             styledTheme = themeIndex;
//         }

//         ImGui_ImplOpenGL3_NewFrame();
//         ImGui_ImplGlfw_NewFrame();
//         ImGui::NewFrame();

//         // Gradient backdrop; the root window is transparent so it shows through
//         DrawBackdrop();
//         ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));

//         // Full-window root
//         ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
//         ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

//         ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 12.0f));

//         ImGui::Begin(
//             "HIDSRoot",
//             nullptr,
//             ImGuiWindowFlags_NoTitleBar |
//             ImGuiWindowFlags_NoResize |
//             ImGuiWindowFlags_NoMove |
//             ImGuiWindowFlags_NoCollapse |
//             ImGuiWindowFlags_NoScrollbar |
//             ImGuiWindowFlags_NoBringToFrontOnFocus
//         );

//         ImGui::PopStyleVar();

//         // Persistent sidebar + content area
//         DrawSidebar();
//         ImGui::SameLine(0.0f, 12.0f);

//         ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));

//         ImGui::BeginChild(
//             "MainContent",
//             ImVec2(0.0f, 0.0f),
//             false
//         );

//         DrawMainContent();

//         ImGui::EndChild();
//         ImGui::PopStyleColor();

//         ImGui::End();
//         ImGui::PopStyleColor();   // WindowBg

//         ImGui::Render();

//         int displayWidth = 0;
//         int displayHeight = 0;

//         glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
//         glViewport(0, 0, displayWidth, displayHeight);

//         glClearColor(
//             HIDS_COLOR_BACKGROUND.x,
//             HIDS_COLOR_BACKGROUND.y,
//             HIDS_COLOR_BACKGROUND.z,
//             HIDS_COLOR_BACKGROUND.w
//         );

//         glClear(GL_COLOR_BUFFER_BIT);

//         ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

//         glfwSwapBuffers(window);
//     }

//     backend.shutdown();

//     ImGui_ImplOpenGL3_Shutdown();
//     ImGui_ImplGlfw_Shutdown();
//     ImGui::DestroyContext();

//     glfwDestroyWindow(window);
//     glfwTerminate();

//     return 0;
// }




//hello

// #include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"
// #include <GLFW/glfw3.h>

// // Native window handle (needed for the tray icon / desktop notifications)
// #ifdef _WIN32
// #include <windows.h>
// #define GLFW_EXPOSE_NATIVE_WIN32
// #include <GLFW/glfw3native.h>
// #endif

// #include <algorithm>
// #include <cctype>
// #include <cfloat>
// #include <cmath>
// #include <cstdio>
// #include <cstdlib>
// #include <cstring>
// #include <set>
// #include <string>
// #include <vector>
// #include "HIDSBackend.h"
// #include "DashboardTheme.h"   // themes, colours, icons, Settings page

// // ------------------------------------------------------------
// // DATA STRUCTURES
// // ------------------------------------------------------------

// struct ProcessInfo {
//     int pid;
//     std::string name;
//     float cpu;
//     float memory;
//     double runningSeconds;
//     std::string runningTime;   // formatted for display
//     bool isApp = false;        // can be ended from the Processes page
//     std::string title;         // window title (apps only)
// };

// struct AlertInfo {
//     int id = 0;
//     std::string time;
//     std::string severity;
//     std::string source;
//     std::string description;

//     bool        canEndTask = false;   // resource alert with a culprit app
//     int         culpritPid = 0;
//     std::string culpritName;
//     float       culpritMem = 0.0f;
// };

// struct EventInfo {
//     std::string time;
//     std::string type;
//     std::string description;
// };

// static HIDSBackend backend;

// static std::vector<ProcessInfo> processes;
// static std::vector<AlertInfo>   alerts;
// static std::vector<EventInfo>   loginEvents;
// static std::vector<EventInfo>   fileEvents;
// static BackendAnalysis          analysisData;

// // File scanner page state
// static ScanStatus               scanStatus;
// static std::vector<ScanFinding> scanFindings;
// static int                      scanTypeSel = 0;      // 0 Quick, 1 Full, 2 Custom
// static char                     scanPath[512] = "";
// static std::string              scanMessage;
// static int processCount = 0;

// static int  selectedPage = 0;
// static char processSearch[128] = "";
// static bool monitoringActive = false;

// static int  severityFilter = 0;   // 0 = All, 1..4 = Critical..Low
// static const char* severityFilterLabels[] =
//     { "All", "Critical", "High", "Medium", "Low" };
// static char alertSearch[128] = "";

// static const char* pages[] = {
//     "Dashboard",
//     "Processes",
//     "Login Monitoring",
//     "File Monitoring",
//     "Alerts",
//     "Logs & Reports",
//     "System Analysis",
//     "File Scanner",
//     "Settings"
// };

// static const char* pageSubtitles[] = {
//     "Overview of this machine's security status",
//     "Everything currently running on this host",
//     "Sign-in activity on this host",
//     "Changes to monitored files and folders",
//     "Events raised by the HIDS monitors",
//     "Detection signatures and scan reports",
//     "Deeper inspection of system behaviour",
//     "Scan files for known malware and suspicious traits",
//     "Appearance, interface size, notifications and data source"
// };

// // Live metrics (ring buffers feed the sparklines)
// static const int HISTORY_SIZE = 60;
// static float cpuHistory[HISTORY_SIZE] = {};
// static float ramHistory[HISTORY_SIZE] = {};
// static int   historyOffset = 0;
// static float cpuNow = 0.0f;
// static float ramNow = 0.0f;

// // Threat activity (feeds the dashboard histogram)
// static float threatHistory[HISTORY_SIZE] = {};
// static float threatAccum = 0.0f;

// static float ClampFloat(float v, float lo, float hi)
// {
//     return v < lo ? lo : (v > hi ? hi : v);
// }

// // ------------------------------------------------------------
// // FONT LOADING
// // Uses a real TTF if one is found; otherwise scales the default
// // bitmap font so the dashboard is still readable.
// // ------------------------------------------------------------

// static bool FileExists(const char* path)
// {
//     FILE* f = std::fopen(path, "rb");
//     if (f == nullptr)
//         return false;

//     std::fclose(f);
//     return true;
// }

// static void LoadFonts(ImGuiIO& io)
// {
//     const char* candidates[] = {
//         "C:/Windows/Fonts/segoeui.ttf",
//         "C:/Windows/Fonts/arial.ttf",
//         "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
//         "/usr/share/fonts/TTF/DejaVuSans.ttf",
//         "/System/Library/Fonts/Supplemental/Arial.ttf"
//     };

//     for (const char* path : candidates)
//     {
//         if (!FileExists(path))
//             continue;

//         ImFont* font = io.Fonts->AddFontFromFileTTF(path, 18.0f);

//         if (font != nullptr)
//         {
//             io.FontDefault = font;
//             io.FontGlobalScale = 1.0f;
//             return;
//         }
//     }

//     io.Fonts->AddFontDefault();
//     io.FontGlobalScale = 1.25f;
// }

// // ------------------------------------------------------------
// // STYLE REFINEMENT
// // Flatter, more modern look. Applied after ApplyTheme(); colours
// // still come from the theme.
// // ------------------------------------------------------------

// static void RefineStyle()
// {
//     ImGuiStyle& s = ImGui::GetStyle();

//     s.WindowRounding    = 0.0f;
//     s.ChildRounding     = 10.0f;
//     s.FrameRounding     = 6.0f;
//     s.PopupRounding     = 8.0f;
//     s.GrabRounding      = 6.0f;
//     s.ScrollbarRounding = 8.0f;
//     s.TabRounding       = 6.0f;

//     s.WindowBorderSize  = 0.0f;
//     s.ChildBorderSize   = 1.0f;
//     s.FrameBorderSize   = 0.0f;
//     s.PopupBorderSize   = 1.0f;

//     s.WindowPadding     = ImVec2(16.0f, 14.0f);
//     s.FramePadding      = ImVec2(12.0f, 7.0f);
//     s.ItemSpacing       = ImVec2(10.0f, 10.0f);
//     s.CellPadding       = ImVec2(10.0f, 7.0f);
//     s.ScrollbarSize     = 12.0f;
// }

// static bool ContainsNoCase(const char* text, const char* search)
// {
//     if (search[0] == '\0')
//         return true;

//     const size_t textLen = std::strlen(text);
//     const size_t searchLen = std::strlen(search);

//     if (searchLen > textLen)
//         return false;

//     for (size_t i = 0; i + searchLen <= textLen; ++i)
//     {
//         size_t j = 0;

//         while (j < searchLen &&
//                std::tolower((unsigned char)text[i + j]) ==
//                std::tolower((unsigned char)search[j]))
//         {
//             ++j;
//         }

//         if (j == searchLen)
//             return true;
//     }

//     return false;
// }

// static ImVec4 SeverityColor(const char* severity)
// {
//     if (std::strcmp(severity, "Critical") == 0) return COLOR_RED;
//     if (std::strcmp(severity, "High") == 0)     return COLOR_ORANGE;
//     if (std::strcmp(severity, "Medium") == 0)   return COLOR_YELLOW;

//     return COLOR_CYAN;
// }

// static int CountSeverity(const char* severity)
// {
//     int count = 0;

//     for (const auto& alert : alerts)
//         if (alert.severity == severity)
//             ++count;

//     return count;
// }

// // Usage colour: green -> yellow -> red as load rises
// static ImVec4 LoadColor(float percent)
// {
//     if (percent >= 85.0f) return COLOR_RED;
//     if (percent >= 65.0f) return COLOR_YELLOW;

//     return COLOR_GREEN;
// }

// static void SectionHeading(const char* title, const char* hint = nullptr)
// {
//     {
//         const ImVec2 p = ImGui::GetCursorScreenPos();
//         const float  h = ImGui::GetTextLineHeight() * 1.1f;

//         ImGui::GetWindowDrawList()->AddRectFilled(
//             ImVec2(p.x, p.y + 1.0f), ImVec2(p.x + 4.0f, p.y + h),
//             ImGui::GetColorU32(COLOR_CYAN), 2.0f);

//         ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 12.0f);
//     }

//     ImGui::PushStyleColor(ImGuiCol_Text, COLOR_CYAN);
//     ImGui::SetWindowFontScale(1.1f);
//     ImGui::TextUnformatted(title);
//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::PopStyleColor();

//     if (hint != nullptr)
//     {
//         ImGui::SameLine();
//         ImGui::TextDisabled("  %s", hint);
//     }

//     ImGui::Spacing();
// }

// // Coloured pill, e.g. for alert severity
// static void SeverityBadge(const char* severity)
// {
//     const ImVec4 color = SeverityColor(severity);
//     const ImVec2 textSize = ImGui::CalcTextSize(severity);
//     const ImVec2 pad(10.0f, 2.0f);

//     const ImVec2 pos = ImGui::GetCursorScreenPos();
//     const ImVec2 size(textSize.x + pad.x * 2.0f, textSize.y + pad.y * 2.0f);

//     ImDrawList* draw = ImGui::GetWindowDrawList();

//     draw->AddRectFilled(
//         pos,
//         ImVec2(pos.x + size.x, pos.y + size.y),
//         ImGui::GetColorU32(WithAlpha(color, 0.16f)),
//         size.y * 0.5f
//     );

//     draw->AddRect(
//         pos,
//         ImVec2(pos.x + size.x, pos.y + size.y),
//         ImGui::GetColorU32(WithAlpha(color, 0.65f)),
//         size.y * 0.5f
//     );

//     draw->AddText(
//         ImVec2(pos.x + pad.x, pos.y + pad.y),
//         ImGui::GetColorU32(color),
//         severity
//     );

//     ImGui::Dummy(size);
// }

// // Pulsing dot used by the monitoring status
// static void StatusDot(const ImVec4& color, bool pulse)
// {
//     const float radius = 5.0f;
//     const ImVec2 pos = ImGui::GetCursorScreenPos();
//     const ImVec2 center(pos.x + radius + 2.0f, pos.y + ImGui::GetFrameHeight() * 0.5f);

//     ImDrawList* draw = ImGui::GetWindowDrawList();

//     if (pulse)
//     {
//         const float t = (float)ImGui::GetTime();
//         const float wave = 0.5f + 0.5f * std::sin(t * 3.0f);

//         draw->AddCircleFilled(
//             center,
//             radius + 3.0f + wave * 3.0f,
//             ImGui::GetColorU32(WithAlpha(color, 0.18f * (1.0f - wave) + 0.05f))
//         );
//     }

//     draw->AddCircleFilled(center, radius, ImGui::GetColorU32(color));

//     ImGui::Dummy(ImVec2(radius * 2.0f + 6.0f, ImGui::GetFrameHeight()));
// }

// static std::string FormatDuration(double seconds)
// {
//     const unsigned long long total =
//         seconds > 0.0 ? (unsigned long long)seconds : 0ULL;
//     char text[32];

//     std::snprintf(
//         text, sizeof(text), "%02llu:%02llu:%02llu",
//         total / 3600ULL, (total / 60ULL) % 60ULL, total % 60ULL
//     );
//     return text;
// }

// static float SeverityWeight(const std::string& s)
// {
//     if (s == "Critical") return 4.0f;
//     if (s == "High")     return 3.0f;
//     if (s == "Medium")   return 2.0f;
//     return 1.0f;
// }

// // Sample rows. Always labelled so they can't be mistaken for real ones:
// // source "DEMO", "[DEMO]" prefix, negative IDs/PIDs, no End Task button.
// static void AppendDemoData()
// {
//     struct DemoAlert { const char* sev; const char* text; };

//     static const DemoAlert demoAlerts[] = {
//         { "Critical", "[DEMO] Known-bad hash match - C:\\Demo\\sample.exe" },
//         { "High",     "[DEMO] Multiple Failed Logins - 5 attempts in 60 s" },
//         { "Medium",   "[DEMO] Suspicious PowerShell Process - powershell.exe" },
//         { "Low",      "[DEMO] Password Changed - sample entry" }
//     };

//     int id = -1;

//     for (const DemoAlert& d : demoAlerts)
//     {
//         AlertInfo a;
//         a.id          = id--;
//         a.time        = "--:--:--";
//         a.severity    = d.sev;
//         a.source      = "DEMO";
//         a.description = d.text;
//         alerts.push_back(a);              // after the real ones
//     }

//     ProcessInfo p;
//     p.pid            = -1;
//     p.name           = "demo_service.exe [DEMO]";
//     p.cpu            = 12.5f;
//     p.memory         = 640.0f;
//     p.runningSeconds = 4000.0;
//     p.runningTime    = FormatDuration(p.runningSeconds);
//     p.isApp          = false;             // can never be ended
//     processes.push_back(p);

//     loginEvents.push_back({ "--:--:--", "LOGIN_FAILURE",
//         "[DEMO] Failed sign-in for user 'demo'" });
//     fileEvents.push_back({ "--:--:--", "FILE_CREATED",
//         "[DEMO] HIDS_Monitor\\sample_script.ps1" });
// }

// static void RefreshFromBackend()
// {
//     static double lastRefresh = -1.0;
//     static unsigned long lastSample = 0;

//     const double now = ImGui::GetTime();

//     if (lastRefresh >= 0.0 && now - lastRefresh < 0.5)
//         return;

//     lastRefresh = now;

//     // CPU, RAM and process count
//     const BackendStats stats = backend.getStats();

//     cpuNow = stats.cpuPercent;
//     ramNow = stats.ramPercent;
//     processCount = stats.processCount;

//     if (stats.sample != lastSample)
//     {
//         lastSample = stats.sample;

//         cpuHistory[historyOffset]    = cpuNow;
//         ramHistory[historyOffset]    = ramNow;
//         threatHistory[historyOffset] = threatAccum;
//         threatAccum = 0.0f;
//         historyOffset = (historyOffset + 1) % HISTORY_SIZE;
//     }

//     analysisData = backend.getAnalysis();
//     processes.clear();

//     for (const BackendProcess& p : backend.getProcesses())
//     {
//         ProcessInfo row;

//         row.pid            = p.pid;
//         row.name           = p.name;
//         row.cpu            = p.cpuPercent;
//         row.memory         = p.memoryMB;
//         row.runningSeconds = p.runningSeconds;
//         row.runningTime    = FormatDuration(p.runningSeconds);
//         row.isApp          = p.isApp;
//         row.title          = p.windowTitle;

//         processes.push_back(row);
//     }
//     alerts.clear();

//     for (const BackendAlert& a : backend.getAlerts())
//     {
//         AlertInfo row;

//         row.time        = a.time;
//         row.severity    = a.severity;
//         row.source      = a.source;
//         row.description = a.description;
//         row.id          = a.id;
//         row.canEndTask  = a.canEndTask;
//         row.culpritPid  = a.culpritPid;
//         row.culpritName = a.culpritName;
//         row.culpritMem  = a.culpritMemoryMB;

//         alerts.push_back(row);
//     }

//     // Weigh alerts that appeared since the last refresh
//     {
//         static std::set<int> seenAlerts;
//         static bool seeded = false;

//         for (const AlertInfo& a : alerts)
//             if (seenAlerts.insert(a.id).second && seeded)
//                 threatAccum += SeverityWeight(a.severity);

//         seeded = true;   // alerts already present at start-up aren't a "spike"
//     }

//     // Login and file activity (already newest first)
//     loginEvents.clear();
//     fileEvents.clear();

//     for (const BackendEvent& e : backend.getEvents())
//     {
//         EventInfo row;

//         row.time        = e.time;
//         row.type        = e.type;
//         row.description = e.description;

//         if (e.type.rfind("LOGIN_", 0) == 0)
//             loginEvents.push_back(row);
//         else if (e.type.rfind("FILE_", 0) == 0)
//             fileEvents.push_back(row);
//     }

//     if (demoMode)
//         AppendDemoData();
// }

// // ------------------------------------------------------------
// // METRIC CARD
// // Accent stripe on the left, big value, optional sparkline.
// // linkPage >= 0 makes the card clickable.
// // ------------------------------------------------------------

// static void MetricCard(
//     const char* title,
//     const char* value,
//     const char* subtitle,
//     const ImVec4& accent,
//     float width,
//     const float* history = nullptr,
//     int linkPage = -1,
//     float gauge = -1.0f)          // 0..100 draws a ring gauge
// {
//     ImGui::BeginChild(
//         title,
//         ImVec2(width, 142.0f),
//         true,
//         ImGuiWindowFlags_NoScrollbar
//     );

//     // Accent stripe
//     {
//         const ImVec2 pos = ImGui::GetWindowPos();
//         const ImVec2 size = ImGui::GetWindowSize();

//         ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + 4.0f, pos.y + size.y),
//             ImGui::GetColorU32(accent),
//             ImGui::GetStyle().ChildRounding,
//             ImDrawFlags_RoundCornersLeft
//         );
//     }

//     // Soft glow + ring gauge (or page icon) on the right side
//     {
//         const ImVec2 wp = ImGui::GetWindowPos();
//         const ImVec2 ws = ImGui::GetWindowSize();
//         ImDrawList*  d  = ImGui::GetWindowDrawList();
//         const ImVec2 gc(wp.x + ws.x - 54.0f, wp.y + 62.0f);

//         DrawGlow(d, gc, 46.0f, accent);

//         if (gauge >= 0.0f)
//             DrawGauge(d, gc, 24.0f, 6.0f, gauge / 100.0f, accent);
//         else if (linkPage >= 0)
//             DrawPageIcon(d, gc, 14.0f, linkPage,
//                 ImGui::GetColorU32(WithAlpha(accent, 0.85f)));
//     }

//     ImGui::TextDisabled("%s", title);
//     ImGui::PushStyleColor(ImGuiCol_Text, accent);
//     ImGui::SetWindowFontScale(1.55f);
//     ImGui::TextUnformatted(value);
//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::PopStyleColor();

//     if (history != nullptr)
//     {
//         DrawAreaChart("##spark", history, HISTORY_SIZE, historyOffset,
//             100.0f, ImVec2(ImGui::GetContentRegionAvail().x, 32.0f),
//             accent, "%", false);
//     }
//     else
//     {
//         ImGui::Spacing();
//         ImGui::TextDisabled("%s", subtitle);
//     }

//     // Hover highlight + click to open the related page
//     if (linkPage >= 0 && ImGui::IsWindowHovered())
//     {
//         const ImVec2 pos  = ImGui::GetWindowPos();
//         const ImVec2 size = ImGui::GetWindowSize();

//         ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

//         ImGui::GetWindowDrawList()->AddRect(
//             pos, ImVec2(pos.x + size.x, pos.y + size.y),
//             ImGui::GetColorU32(WithAlpha(accent, 0.85f)),
//             ImGui::GetStyle().ChildRounding, 0, 1.5f);

//         ImGui::SetTooltip("Open %s", pages[linkPage]);

//         if (ImGui::IsMouseClicked(0))
//             selectedPage = linkPage;
//     }

//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // NAVIGATION: persistent sidebar grouped by security domain,
// // plus a top bar that shows the current page.
// // ------------------------------------------------------------

// struct HidsFeatureRoute
// {
//     const char* title;
//     const char* description;
//     int page;
// };

// struct HidsDomainRoute
// {
//     const char* title;
//     const char* description;
//     const char* icon;
//     const HidsFeatureRoute* features;
//     int featureCount;
// };

// static const HidsFeatureRoute monitoringFeatures[] =
// {
//     { "Security Dashboard", "Live host health, threat activity and overview", 0 },
//     { "Process Monitoring", "Inspect running processes and resource usage", 1 },
//     { "Login Monitoring", "Review failed sign-in activity", 2 },
//     { "File Monitoring", "Review changes in the monitored folder", 3 }
// };

// static const HidsFeatureRoute threatFeatures[] =
// {
//     { "Alert Center", "Review and filter security alerts", 4 },
//     { "File Scanner", "Run quick, full and custom file scans", 7 }
// };

// static const HidsFeatureRoute analysisFeatures[] =
// {
//     { "System Analysis", "Resource trends, anomalies and process findings", 6 }
// };

// static const HidsFeatureRoute reportFeatures[] =
// {
//     { "Logs & Reports", "Review protected detection signatures and reports", 5 }
// };

// static const HidsFeatureRoute configurationFeatures[] =
// {
//     { "Settings & Notifications", "Themes, interface scale and desktop alerts", 8 }
// };

// static const HidsDomainRoute hidsDomains[] =
// {
//     { "Monitoring", "Observe activity and changes across this host",
//       "MON", monitoringFeatures, IM_ARRAYSIZE(monitoringFeatures) },
//     { "Threat Detection", "Investigate alerts and scan suspicious files",
//       "THR", threatFeatures, IM_ARRAYSIZE(threatFeatures) },
//     { "Analysis", "Understand system behaviour and resource anomalies",
//       "ANA", analysisFeatures, IM_ARRAYSIZE(analysisFeatures) },
//     { "Reports & Logs", "Access security records, signatures and reports",
//       "LOG", reportFeatures, IM_ARRAYSIZE(reportFeatures) },
//     { "Configuration", "Manage appearance, notifications and preferences",
//       "CFG", configurationFeatures, IM_ARRAYSIZE(configurationFeatures) }
// };

// static const float SIDEBAR_WIDTH = 252.0f;

// // ---- Screen flow: Welcome -> Hub -> Domain -> Feature page ----
// enum AppScreen { SCREEN_WELCOME = 0, SCREEN_HUB, SCREEN_DOMAIN, SCREEN_PAGE };
// static int appScreen      = SCREEN_WELCOME;
// static int selectedDomain = 0;

// static const char* domainLabels[] =
//     { "MONITORING", "THREAT DETECTION", "ANALYSIS", "REPORTS & LOGS", "CONFIGURATION" };

// static int DomainOfPage(int page)
// {
//     for (int d = 0; d < IM_ARRAYSIZE(hidsDomains); ++d)
//         for (int f = 0; f < hidsDomains[d].featureCount; ++f)
//             if (hidsDomains[d].features[f].page == page)
//                 return d;

//     return 0;
// }

// static void StartEngine()
// {
//     backend.start();
//     monitoringActive = backend.isRunning();
// }

// static void StopEngine()
// {
//     backend.stop();
//     monitoringActive = false;
// }

// // Logo tile + product name
// static void DrawBrand()
// {
//     ImDrawList* d = ImGui::GetWindowDrawList();
//     const ImVec2 p = ImGui::GetCursorScreenPos();
//     const float  s = 38.0f;

//     d->AddRectFilled(p, ImVec2(p.x + s, p.y + s),
//         ImGui::GetColorU32(WithAlpha(COLOR_CYAN, 0.16f)), 10.0f);
//     d->AddRect(p, ImVec2(p.x + s, p.y + s),
//         ImGui::GetColorU32(WithAlpha(COLOR_CYAN, 0.65f)), 10.0f, 0, 1.2f);

//     // little shield
//     const ImVec2 c(p.x + s * 0.5f, p.y + s * 0.5f);
//     ImVec2 shield[5] = {
//         ImVec2(c.x - 8.0f, c.y - 9.0f), ImVec2(c.x + 8.0f, c.y - 9.0f),
//         ImVec2(c.x + 8.0f, c.y + 1.0f), ImVec2(c.x,         c.y + 10.0f),
//         ImVec2(c.x - 8.0f, c.y + 1.0f)
//     };
//     d->AddConvexPolyFilled(shield, 5, ImGui::GetColorU32(COLOR_CYAN));

//     ImGui::Dummy(ImVec2(s, s));
//     ImGui::SameLine(0.0f, 12.0f);

//     ImGui::BeginGroup();
//     ImGui::SetWindowFontScale(1.25f);
//     ImGui::TextUnformatted("HIDS");
//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::TextDisabled("Security Platform");
//     ImGui::EndGroup();
// }

// // Status line + one Start/Stop button
// static void DrawEngineControl()
// {
//     const ImVec4 col = monitoringActive ? COLOR_GREEN : COLOR_YELLOW;

//     StatusDot(col, monitoringActive);
//     ImGui::SameLine(0.0f, 4.0f);
//     ImGui::AlignTextToFramePadding();
//     ImGui::TextColored(col, "%s",
//         monitoringActive ? "Protection active" : "Protection stopped");

//     if (!monitoringActive)
//     {
//         ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.02f, 0.48f, 0.31f, 1.0f));
//         ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.04f, 0.62f, 0.40f, 1.0f));
//         ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.03f, 0.40f, 0.26f, 1.0f));
//         if (ImGui::Button("Start protection", ImVec2(-1.0f, 38.0f)))
//             StartEngine();
//         ImGui::PopStyleColor(3);
//     }
//     else
//     {
//         ImGui::PushStyleColor(ImGuiCol_Button,        WithAlpha(COLOR_RED, 0.14f));
//         ImGui::PushStyleColor(ImGuiCol_ButtonHovered, WithAlpha(COLOR_RED, 0.30f));
//         ImGui::PushStyleColor(ImGuiCol_ButtonActive,  WithAlpha(COLOR_RED, 0.45f));
//         if (ImGui::Button("Stop protection", ImVec2(-1.0f, 38.0f)))
//             StopEngine();
//         ImGui::PopStyleColor(3);
//     }
// }

// // One sidebar row: accent bar when selected, optional count badge on the right
// static bool NavItem(const char* label, bool selected, int badge)
// {
//     ImGui::PushID(label);

//     const float h = 38.0f;
//     const bool clicked = ImGui::Selectable("##nav", selected, 0, ImVec2(0.0f, h));

//     const ImVec2 mn = ImGui::GetItemRectMin();
//     const ImVec2 mx = ImGui::GetItemRectMax();
//     ImDrawList*  d  = ImGui::GetWindowDrawList();

//     if (selected)
//         d->AddRectFilled(ImVec2(mn.x, mn.y + 8.0f), ImVec2(mn.x + 3.0f, mx.y - 8.0f),
//             ImGui::GetColorU32(COLOR_CYAN), 2.0f);

//     d->AddText(
//         ImVec2(mn.x + 16.0f, mn.y + (h - ImGui::GetTextLineHeight()) * 0.5f),
//         ImGui::GetColorU32(selected ? COLOR_TEXT : COLOR_MUTED), label);

//     if (badge > 0)
//     {
//         char text[16];

//         if (badge > 99)
//             std::snprintf(text, sizeof(text), "99+");
//         else
//             std::snprintf(text, sizeof(text), "%d", badge);

//         const ImVec4 bc = CountSeverity("Critical") > 0 ? COLOR_RED : COLOR_ORANGE;
//         const ImVec2 ts = ImGui::CalcTextSize(text);
//         const float  w  = ts.x + 14.0f;
//         const float  hh = ts.y + 2.0f;
//         const ImVec2 a(mx.x - w - 10.0f, mn.y + (h - hh) * 0.5f);

//         d->AddRectFilled(a, ImVec2(a.x + w, a.y + hh),
//             ImGui::GetColorU32(WithAlpha(bc, 0.22f)), hh * 0.5f);
//         d->AddText(ImVec2(a.x + 7.0f, a.y + 1.0f), ImGui::GetColorU32(bc), text);
//     }

//     ImGui::PopID();
//     return clicked;
// }

// static void DrawSidebar()
// {
//     ImGui::PushStyleColor(ImGuiCol_ChildBg, COLOR_SIDEBAR);
//     ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14.0f, 16.0f));
//     ImGui::BeginChild("Sidebar", ImVec2(SIDEBAR_WIDTH, 0.0f), true,
//                       ImGuiWindowFlags_NoScrollbar);
//     ImGui::PopStyleVar();

//     DrawBrand();
//     ImGui::Spacing();

//     // Back to the home hub
//     if (ImGui::Button("Home", ImVec2(-1.0f, 32.0f)))
//         appScreen = SCREEN_HUB;

//     ImGui::Spacing();
//     ImGui::Separator();
//     ImGui::Spacing();

//     DrawEngineControl();

//     ImGui::Spacing();
//     ImGui::Separator();

//     for (int d = 0; d < IM_ARRAYSIZE(hidsDomains); ++d)
//     {
//         ImGui::Spacing();
//         ImGui::TextColored(WithAlpha(COLOR_MUTED, 0.75f), "%s", domainLabels[d]);

//         const HidsDomainRoute& domain = hidsDomains[d];

//         for (int i = 0; i < domain.featureCount; ++i)
//         {
//             const HidsFeatureRoute& f = domain.features[i];
//             const int badge = (f.page == 4) ? (int)alerts.size() : 0;

//             if (NavItem(f.title, selectedPage == f.page, badge))
//             {
//                 selectedPage   = f.page;
//                 selectedDomain = d;
//             }
//         }
//     }

//     // Footer pinned to the bottom
//     const float footerY = ImGui::GetWindowHeight() - 84.0f;

//     if (ImGui::GetCursorPosY() < footerY)
//         ImGui::SetCursorPosY(footerY);

//     ImGui::Separator();
//     ImGui::Spacing();
//     DataSourceBadge(monitoringActive);
//     ImGui::TextDisabled("HIDS Prototype v2.0");

//     ImGui::EndChild();
//     ImGui::PopStyleColor();
// }

// static void DrawTopBar()
// {
//     ImGui::BeginChild("TopBar", ImVec2(0.0f, 84.0f), false,
//                       ImGuiWindowFlags_NoScrollbar);

//     ImGui::BeginGroup();

//     // Breadcrumb: Home > Domain
//     if (ImGui::SmallButton("Home"))
//         appScreen = SCREEN_HUB;

//     ImGui::SameLine();
//     ImGui::TextDisabled(">");
//     ImGui::SameLine();

//     if (ImGui::SmallButton(hidsDomains[DomainOfPage(selectedPage)].title))
//     {
//         selectedDomain = DomainOfPage(selectedPage);
//         appScreen      = SCREEN_DOMAIN;
//     }

//     ImGui::SetWindowFontScale(1.5f);
//     ImGui::TextUnformatted(pages[selectedPage]);
//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::TextDisabled("%s", pageSubtitles[selectedPage]);
//     ImGui::EndGroup();

//     const ImVec4 col = monitoringActive ? COLOR_GREEN : COLOR_YELLOW;

//     ImGui::SameLine(ImGui::GetWindowWidth() - 210.0f);
//     StatusDot(col, monitoringActive);
//     ImGui::SameLine(0.0f, 4.0f);
//     ImGui::AlignTextToFramePadding();
//     ImGui::TextColored(col, "%s",
//         monitoringActive ? "Monitoring active" : "Monitoring stopped");

//     ImGui::EndChild();
// }

// // Call-to-action shown on the dashboard only while protection is off
// static void DrawEngineBanner()
// {
//     if (monitoringActive)
//         return;

//     ImGui::BeginChild("EngineBanner", ImVec2(0.0f, 92.0f), true,
//                       ImGuiWindowFlags_NoScrollbar);

//     {
//         const ImVec2 p  = ImGui::GetWindowPos();
//         const ImVec2 sz = ImGui::GetWindowSize();

//         ImGui::GetWindowDrawList()->AddRectFilled(
//             p, ImVec2(p.x + 4.0f, p.y + sz.y),
//             ImGui::GetColorU32(COLOR_YELLOW),
//             ImGui::GetStyle().ChildRounding, ImDrawFlags_RoundCornersLeft);
//     }

//     ImGui::BeginGroup();
//     ImGui::SetWindowFontScale(1.2f);
//     ImGui::TextUnformatted("Protection is stopped");
//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::TextDisabled("Start the engine to collect host events, watch processes "
//                         "and the monitored folder, and raise alerts.");
//     ImGui::EndGroup();

//     ImGui::SameLine(ImGui::GetWindowWidth() - 200.0f);
//     ImGui::SetCursorPosY(26.0f);

//     ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.02f, 0.48f, 0.31f, 1.0f));
//     ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.04f, 0.62f, 0.40f, 1.0f));
//     ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.03f, 0.40f, 0.26f, 1.0f));
//     if (ImGui::Button("Start protection", ImVec2(176.0f, 40.0f)))
//         StartEngine();
//     ImGui::PopStyleColor(3);

//     ImGui::EndChild();
//     ImGui::Spacing();
// }

// // ============================================================
// // WELCOME SCREEN -> HOME HUB -> DOMAIN SCREEN
// // ============================================================

// static ImVec4 DomainAccent(int d)
// {
//     switch (d)
//     {
//     case 0:  return COLOR_CYAN;
//     case 1:  return COLOR_ORANGE;
//     case 2:  return COLOR_BLUE;
//     case 3:  return COLOR_GREEN;
//     default: return COLOR_YELLOW;
//     }
// }

// // Which page icon represents each domain
// static const int domainIconPage[5] = { 0, 4, 6, 5, 8 };

// static void CenteredText(const char* text, float scale, const ImVec4& col)
// {
//     ImGui::SetWindowFontScale(scale);
//     const float w = ImGui::CalcTextSize(text).x;
//     ImGui::SetCursorPosX((ImGui::GetWindowWidth() - w) * 0.5f);
//     ImGui::TextColored(col, "%s", text);
//     ImGui::SetWindowFontScale(1.0f);
// }

// static void DrawShieldLogo(ImDrawList* d, const ImVec2& c, float s, const ImVec4& col)
// {
//     ImVec2 shield[5] = {
//         ImVec2(c.x - 0.80f * s, c.y - 0.90f * s),
//         ImVec2(c.x + 0.80f * s, c.y - 0.90f * s),
//         ImVec2(c.x + 0.80f * s, c.y + 0.10f * s),
//         ImVec2(c.x,             c.y + 1.00f * s),
//         ImVec2(c.x - 0.80f * s, c.y + 0.10f * s)
//     };
//     d->AddConvexPolyFilled(shield, 5, ImGui::GetColorU32(col));

//     ImVec2 tick[3] = {
//         ImVec2(c.x - 0.36f * s, c.y - 0.05f * s),
//         ImVec2(c.x - 0.08f * s, c.y + 0.25f * s),
//         ImVec2(c.x + 0.42f * s, c.y - 0.38f * s)
//     };
//     d->AddPolyline(tick, 3, IM_COL32(8, 16, 28, 255), 0, s * 0.16f);
// }

// // ------------------------------------------------------------
// // WELCOME SCREEN - the very first thing the user sees
// // ------------------------------------------------------------
// static void DrawWelcomeScreen()
// {
//     const ImVec2 disp = ImGui::GetIO().DisplaySize;
//     const float  t    = (float)ImGui::GetTime();
//     ImDrawList*  bg   = ImGui::GetBackgroundDrawList();
//     const ImVec2 c(disp.x * 0.5f, disp.y * 0.30f);

//     // Floating particles
//     for (int i = 0; i < 46; ++i)
//     {
//         const float fx = std::fmod(i * 0.6180339f, 1.0f);
//         const float fy = std::fmod(i * 0.3819660f + t * (0.006f + 0.004f * (i % 4)), 1.0f);
//         const float a  = 0.08f + 0.08f * (0.5f + 0.5f * std::sin(t * 0.8f + i));

//         bg->AddCircleFilled(ImVec2(fx * disp.x, fy * disp.y), 1.2f + (i % 3),
//             ImGui::GetColorU32(WithAlpha(COLOR_CYAN, a)));
//     }

//     // Soft glow + expanding radar rings
//     DrawGlow(bg, c, 170.0f, COLOR_CYAN);

//     for (int k = 0; k < 3; ++k)
//     {
//         const float phase  = std::fmod(t * 0.30f + k / 3.0f, 1.0f);
//         const float radius = 66.0f + phase * 190.0f;

//         bg->AddCircle(c, radius,
//             ImGui::GetColorU32(WithAlpha(COLOR_CYAN, 0.28f * (1.0f - phase))), 96, 1.6f);
//     }

//     // Logo badge + rotating arc
//     bg->AddCircleFilled(c, 64.0f, ImGui::GetColorU32(WithAlpha(COLOR_CYAN, 0.12f)), 64);
//     bg->AddCircle(c, 64.0f, ImGui::GetColorU32(WithAlpha(COLOR_CYAN, 0.55f)), 64, 1.5f);

//     bg->PathClear();
//     bg->PathArcTo(c, 72.0f, t * 1.6f, t * 1.6f + 1.9f, 32);
//     bg->PathStroke(ImGui::GetColorU32(COLOR_CYAN), 0, 3.0f);

//     DrawShieldLogo(bg, c, 30.0f, COLOR_CYAN);

//     // Text block
//     ImGui::SetCursorPosY(c.y + 112.0f);

//     CenteredText("HIDS", 3.0f, COLOR_TEXT);
//     CenteredText("Host Intrusion Detection System", 1.35f, COLOR_CYAN);
//     ImGui::Spacing();
//     CenteredText("Watch processes, sign-ins and files. Catch threats early.",
//         1.0f, COLOR_MUTED);

//     ImGui::Dummy(ImVec2(0.0f, 22.0f));

//     // Start button
//     const ImVec2 btn(280.0f, 58.0f);
//     ImGui::SetCursorPosX((ImGui::GetWindowWidth() - btn.x) * 0.5f);

//     ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.02f, 0.48f, 0.31f, 1.0f));
//     ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.04f, 0.62f, 0.40f, 1.0f));
//     ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.03f, 0.40f, 0.26f, 1.0f));
//     ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 29.0f);
//     ImGui::SetWindowFontScale(1.25f);

//     const bool pressed = ImGui::Button(monitoringActive ? "Enter dashboard" : "Start HIDS", btn);

//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::PopStyleVar();
//     ImGui::PopStyleColor(3);

//     // Pulsing halo around the button
//     {
//         const ImVec2 a = ImGui::GetItemRectMin();
//         const ImVec2 b = ImGui::GetItemRectMax();
//         const float  w = 0.5f + 0.5f * std::sin(t * 2.5f);

//         ImGui::GetWindowDrawList()->AddRect(
//             ImVec2(a.x - 3.0f - w * 4.0f, a.y - 3.0f - w * 4.0f),
//             ImVec2(b.x + 3.0f + w * 4.0f, b.y + 3.0f + w * 4.0f),
//             ImGui::GetColorU32(WithAlpha(COLOR_GREEN, 0.40f * (1.0f - w) + 0.05f)),
//             34.0f, 0, 2.0f);

//         if (ImGui::IsItemHovered())
//             ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
//     }

//     if (pressed)
//     {
//         if (!monitoringActive)
//             StartEngine();

//         appScreen = SCREEN_HUB;
//     }

//     ImGui::Dummy(ImVec2(0.0f, 6.0f));

//     // Skip link
//     {
//         const char* skip = "Open dashboard without starting protection";
//         const float w = ImGui::CalcTextSize(skip).x;

//         ImGui::SetCursorPosX((ImGui::GetWindowWidth() - w) * 0.5f);
//         ImGui::TextDisabled("%s", skip);

//         if (ImGui::IsItemHovered())
//             ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

//         if (ImGui::IsItemClicked())
//             appScreen = SCREEN_HUB;
//     }

//     // Footer
//     ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 52.0f);
//     CenteredText("Monitoring   -   Threat Detection   -   Analysis   -   Reports   -   Configuration",
//         1.0f, WithAlpha(COLOR_MUTED, 0.8f));
//     CenteredText("HIDS Prototype v2.0", 0.9f, WithAlpha(COLOR_MUTED, 0.6f));
// }

// // ------------------------------------------------------------
// // Shared card used by the hub and the domain screens.
// // Returns true when the card is clicked.
// // ------------------------------------------------------------
// static bool HubCard(
//     const char* id, const char* title, const char* desc, const char* footer,
//     int iconPage, const ImVec4& accent, const ImVec2& size)
// {
//     bool clicked = false;

//     ImGui::PushID(id);
//     ImGui::BeginChild("card", size, true, ImGuiWindowFlags_NoScrollbar);

//     const ImVec2 wp = ImGui::GetWindowPos();
//     const ImVec2 ws = ImGui::GetWindowSize();
//     ImDrawList*  d  = ImGui::GetWindowDrawList();
//     const bool   hov = ImGui::IsWindowHovered();
//     const float  rounding = ImGui::GetStyle().ChildRounding;

//     // Accent bar across the top
//     d->AddRectFilled(wp, ImVec2(wp.x + ws.x, wp.y + 4.0f),
//         ImGui::GetColorU32(accent), rounding, ImDrawFlags_RoundCornersTop);

//     // Icon badge
//     const ImVec2 ic(wp.x + 20.0f + 32.0f, wp.y + 26.0f + 32.0f);

//     DrawGlow(d, ic, hov ? 78.0f : 60.0f, accent);
//     d->AddCircleFilled(ic, 32.0f,
//         ImGui::GetColorU32(WithAlpha(accent, hov ? 0.30f : 0.16f)), 48);
//     d->AddCircle(ic, 32.0f,
//         ImGui::GetColorU32(WithAlpha(accent, 0.75f)), 48, 1.6f);
//     DrawPageIcon(d, ic, 15.0f, iconPage, ImGui::GetColorU32(accent));

//     // Text
//     ImGui::SetCursorPosY(26.0f + 64.0f + 14.0f);

//     ImGui::SetWindowFontScale(1.25f);
//     ImGui::TextUnformatted(title);
//     ImGui::SetWindowFontScale(1.0f);

//     ImGui::PushStyleColor(ImGuiCol_Text, COLOR_MUTED);
//     ImGui::TextWrapped("%s", desc);
//     ImGui::PopStyleColor();

//     // Footer pinned to the bottom
//     ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 36.0f);
//     ImGui::TextColored(accent, "%s   >", footer);

//     // Hover outline + click
//     if (hov)
//     {
//         d->AddRect(wp, ImVec2(wp.x + ws.x, wp.y + ws.y),
//             ImGui::GetColorU32(WithAlpha(accent, 0.90f)), rounding, 0, 1.8f);

//         ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

//         if (ImGui::IsMouseClicked(0))
//             clicked = true;
//     }

//     ImGui::EndChild();
//     ImGui::PopID();

//     return clicked;
// }

// // Top-right engine status + Start/Stop, used on the hub and domain screens
// static void DrawHeaderStatus(float topY)
// {
//     const ImVec4 col = monitoringActive ? COLOR_GREEN : COLOR_YELLOW;

//     ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 360.0f, topY + 4.0f));
//     StatusDot(col, monitoringActive);
//     ImGui::SameLine(0.0f, 4.0f);
//     ImGui::AlignTextToFramePadding();
//     ImGui::TextColored(col, "%s",
//         monitoringActive ? "Protection active" : "Protection stopped");
//     ImGui::SameLine(0.0f, 14.0f);

//     if (monitoringActive)
//     {
//         ImGui::PushStyleColor(ImGuiCol_Button,        WithAlpha(COLOR_RED, 0.14f));
//         ImGui::PushStyleColor(ImGuiCol_ButtonHovered, WithAlpha(COLOR_RED, 0.30f));
//         ImGui::PushStyleColor(ImGuiCol_ButtonActive,  WithAlpha(COLOR_RED, 0.45f));
//         if (ImGui::Button("Stop", ImVec2(84.0f, 0.0f)))
//             StopEngine();
//         ImGui::PopStyleColor(3);
//     }
//     else
//     {
//         ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.02f, 0.48f, 0.31f, 1.0f));
//         ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.04f, 0.62f, 0.40f, 1.0f));
//         ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.03f, 0.40f, 0.26f, 1.0f));
//         if (ImGui::Button("Start", ImVec2(84.0f, 0.0f)))
//             StartEngine();
//         ImGui::PopStyleColor(3);
//     }
// }

// // ------------------------------------------------------------
// // HOME HUB - one big card per security domain
// // ------------------------------------------------------------
// static void DrawHubScreen()
// {
//     RefreshFromBackend();

//     const float topY = ImGui::GetCursorPosY();
//     DrawBrand();
//     DrawHeaderStatus(topY);

//     ImGui::Dummy(ImVec2(0.0f, 8.0f));
//     ImGui::Separator();
//     ImGui::Dummy(ImVec2(0.0f, 18.0f));

//     CenteredText("Security Console", 2.0f, COLOR_TEXT);
//     CenteredText("Choose an area to explore", 1.1f, COLOR_MUTED);

//     // Live summary line
//     {
//         char line[160];
//         std::snprintf(line, sizeof(line),
//             "CPU %.0f%%      RAM %.0f%%      Processes %d      Alerts %d",
//             cpuNow, ramNow, processCount, (int)alerts.size());

//         ImGui::Spacing();
//         CenteredText(line, 1.0f, WithAlpha(COLOR_CYAN, 0.9f));
//     }

//     ImGui::Dummy(ImVec2(0.0f, 20.0f));

//     // Card grid
//     const int   count = IM_ARRAYSIZE(hidsDomains);
//     const float gap   = 16.0f;
//     const float avail = ImGui::GetContentRegionAvail().x;
//     const int   cols  = (avail >= 1100.0f) ? 5 : 3;
//     const float cardW = (avail - gap * (cols - 1)) / cols;
//     const float cardH = (cols == 5) ? 270.0f : 230.0f;

//     ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(gap, gap));

//     for (int i = 0; i < count; ++i)
//     {
//         if (i % cols != 0)
//             ImGui::SameLine();

//         const HidsDomainRoute& dom = hidsDomains[i];

//         char footer[48];
//         std::snprintf(footer, sizeof(footer), "%d feature%s",
//             dom.featureCount, dom.featureCount == 1 ? "" : "s");

//         if (HubCard(dom.title, dom.title, dom.description, footer,
//                 domainIconPage[i], DomainAccent(i), ImVec2(cardW, cardH)))
//         {
//             selectedDomain = i;
//             appScreen      = SCREEN_DOMAIN;
//         }
//     }

//     ImGui::PopStyleVar();

//     ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 34.0f);
//     CenteredText("Click an area to open it   -   Esc goes back", 0.95f,
//         WithAlpha(COLOR_MUTED, 0.7f));
// }

// // ------------------------------------------------------------
// // DOMAIN SCREEN - the features inside one domain
// // ------------------------------------------------------------
// static void DrawDomainScreen()
// {
//     RefreshFromBackend();

//     const HidsDomainRoute& dom = hidsDomains[selectedDomain];
//     const ImVec4 accent = DomainAccent(selectedDomain);

//     const float topY = ImGui::GetCursorPosY();

//     if (ImGui::Button("<  Home", ImVec2(100.0f, 34.0f)))
//         appScreen = SCREEN_HUB;

//     ImGui::SameLine(0.0f, 14.0f);
//     ImGui::AlignTextToFramePadding();
//     ImGui::TextDisabled("Home  >");
//     ImGui::SameLine(0.0f, 6.0f);
//     ImGui::TextColored(accent, "%s", dom.title);

//     DrawHeaderStatus(topY);

//     ImGui::Dummy(ImVec2(0.0f, 8.0f));
//     ImGui::Separator();
//     ImGui::Dummy(ImVec2(0.0f, 18.0f));

//     // Domain banner
//     {
//         const ImVec2 p = ImGui::GetCursorScreenPos();
//         const ImVec2 ic(p.x + 36.0f, p.y + 36.0f);
//         ImDrawList*  d = ImGui::GetWindowDrawList();

//         DrawGlow(d, ic, 70.0f, accent);
//         d->AddCircleFilled(ic, 34.0f, ImGui::GetColorU32(WithAlpha(accent, 0.18f)), 48);
//         d->AddCircle(ic, 34.0f, ImGui::GetColorU32(WithAlpha(accent, 0.75f)), 48, 1.6f);
//         DrawPageIcon(d, ic, 16.0f, domainIconPage[selectedDomain], ImGui::GetColorU32(accent));

//         ImGui::Dummy(ImVec2(72.0f, 72.0f));
//         ImGui::SameLine(0.0f, 16.0f);

//         ImGui::BeginGroup();
//         ImGui::SetWindowFontScale(1.8f);
//         ImGui::TextUnformatted(dom.title);
//         ImGui::SetWindowFontScale(1.0f);
//         ImGui::TextDisabled("%s", dom.description);
//         ImGui::EndGroup();
//     }

//     ImGui::Dummy(ImVec2(0.0f, 22.0f));

//     // Feature cards
//     const float gap   = 16.0f;
//     const float avail = ImGui::GetContentRegionAvail().x;
//     const int   cols  = 3;
//     const float cardW = (avail - gap * (cols - 1)) / cols;
//     const float cardH = 220.0f;

//     ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(gap, gap));

//     for (int i = 0; i < dom.featureCount; ++i)
//     {
//         if (i % cols != 0)
//             ImGui::SameLine();

//         const HidsFeatureRoute& f = dom.features[i];

//         char footer[48];

//         if (f.page == 4)
//             std::snprintf(footer, sizeof(footer), "%d alert%s", (int)alerts.size(),
//                 alerts.size() == 1 ? "" : "s");
//         else if (f.page == 1)
//             std::snprintf(footer, sizeof(footer), "%d processes", processCount);
//         else
//             std::snprintf(footer, sizeof(footer), "Open");

//         if (HubCard(f.title, f.title, f.description, footer,
//                 f.page, accent, ImVec2(cardW, cardH)))
//         {
//             selectedPage = f.page;
//             appScreen    = SCREEN_PAGE;
//         }
//     }

//     ImGui::PopStyleVar();
// }

// // ------------------------------------------------------------
// // Entry point for every screen that is NOT the sidebar layout
// // ------------------------------------------------------------
// static void DrawFullScreen()
// {
//     static int    lastScreen = -1;
//     static double changedAt  = 0.0;

//     if (appScreen != lastScreen)
//     {
//         lastScreen = appScreen;
//         changedAt  = ImGui::GetTime();
//     }

//     if (ImGui::IsKeyPressed(ImGuiKey_Escape, false) && appScreen == SCREEN_DOMAIN)
//         appScreen = SCREEN_HUB;

//     const float fade = ClampFloat((float)((ImGui::GetTime() - changedAt) / 0.25), 0.0f, 1.0f);
//     ImGui::PushStyleVar(ImGuiStyleVar_Alpha, fade);

//     if (appScreen == SCREEN_WELCOME)     DrawWelcomeScreen();
//     else if (appScreen == SCREEN_HUB)    DrawHubScreen();
//     else if (appScreen == SCREEN_DOMAIN) DrawDomainScreen();

//     ImGui::PopStyleVar();
// }

// // ============================================================
// // DASHBOARD WIDGETS
// // ============================================================

// static void DrawMetrics()
// {
//     const float spacing = ImGui::GetStyle().ItemSpacing.x;

//     const float cardWidth =
//         (ImGui::GetContentRegionAvail().x - spacing * 3.0f) / 4.0f;

//     char cpuText[16];
//     char ramText[16];
//     char alertText[16];

//     // Displayed values glide toward the live ones instead of jumping
//     static float cpuShown = 0.0f;
//     static float ramShown = 0.0f;

//     const float ease = ClampFloat(ImGui::GetIO().DeltaTime * 6.0f, 0.0f, 1.0f);
//     cpuShown += (cpuNow - cpuShown) * ease;
//     ramShown += (ramNow - ramShown) * ease;

//     std::snprintf(cpuText, sizeof(cpuText), "%.0f%%", cpuShown);
//     std::snprintf(ramText, sizeof(ramText), "%.0f%%", ramShown);
//     std::snprintf(alertText, sizeof(alertText), "%d", (int)alerts.size());

//     const ImVec4 alertColor = CountSeverity("Critical") > 0 ? COLOR_RED : COLOR_YELLOW;

//     char alertSubtitle[64];
//     std::snprintf(
//         alertSubtitle, sizeof(alertSubtitle),
//         "%d critical, %d high",
//         CountSeverity("Critical"), CountSeverity("High")
//     );

//     MetricCard("CPU usage", cpuText, "", LoadColor(cpuNow), cardWidth, cpuHistory, 6, cpuShown);
//     ImGui::SameLine();

//     MetricCard("RAM usage", ramText, "", LoadColor(ramNow), cardWidth, ramHistory, 6, ramShown);
//     ImGui::SameLine();

//     char processText[16];
//     std::snprintf(processText, sizeof(processText), "%d", processCount);

//     MetricCard("Running processes", processText, "On this host", COLOR_BLUE, cardWidth, nullptr, 1);
//     ImGui::SameLine();

//     MetricCard("Total alerts", alertText, alertSubtitle, alertColor, cardWidth, nullptr, 4);
// }

// // ------------------------------------------------------------
// // THREAT ACTIVITY PANEL (dashboard)
// // Histogram of new alerts per second + clickable severity bars.
// // ------------------------------------------------------------

// static void DrawThreatPanel()
// {
//     ImGui::BeginChild("ThreatPanel", ImVec2(0.0f, 196.0f), true,
//                       ImGuiWindowFlags_NoScrollbar);

//     SectionHeading("Threat activity",
//         "new alerts per second, weighted by severity (last 60 s)");

//     float peak = 6.0f;
//     for (float v : threatHistory)
//         peak = std::max(peak, v);

//     const float leftW = ImGui::GetContentRegionAvail().x * 0.62f;

//     DrawBarChart("##threat", threatHistory, HISTORY_SIZE, historyOffset,
//         peak, ImVec2(leftW, 122.0f));

//     ImGui::SameLine();
//     ImGui::BeginGroup();

//     static const char* names[] = { "Critical", "High", "Medium", "Low" };
//     const int total = std::max(1, (int)alerts.size());

//     for (int i = 0; i < 4; ++i)
//     {
//         const int count = CountSeverity(names[i]);

//         char label[40];
//         std::snprintf(label, sizeof(label), "%s  %d", names[i], count);

//         ImGui::PushID(i);
//         ImGui::PushStyleColor(ImGuiCol_PlotHistogram,
//             WithAlpha(SeverityColor(names[i]), 0.85f));
//         ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_FRAME);

//         ImGui::ProgressBar((float)count / (float)total,
//             ImVec2(ImGui::GetContentRegionAvail().x, 20.0f), label);

//         ImGui::PopStyleColor(2);

//         if (ImGui::IsItemHovered())
//         {
//             ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
//             ImGui::SetTooltip("Show %s alerts", names[i]);
//         }

//         if (ImGui::IsItemClicked())
//         {
//             selectedPage   = 4;        // Alerts
//             severityFilter = i + 1;
//         }

//         ImGui::PopID();
//     }

//     ImGui::EndGroup();
//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // PASSWORD LOCK (Logs & Reports page and End Task)
// // ------------------------------------------------------------

// static const double LOCK_IDLE_SECONDS = 300.0;     // auto-lock after 5 min idle
// static const int    LOGS_PAGE_INDEX   = 5;

// static bool        logsUnlocked   = false;
// static double      logsLastActive = 0.0;
// static char        lockOld[64]     = "";
// static char        lockNew[64]     = "";
// static char        lockConfirm[64] = "";
// static char        lockEntry[64]   = "";
// static std::string lockMessage;
// static bool        lockMessageOk   = false;

// static void Wipe(char* buffer, std::size_t size)
// {
//     std::memset(buffer, 0, size);
// }

// // "Create a password" form. Returns true on the frame it is created.
// static bool DrawSetPasswordForm()
// {
//     bool created = false;

//     ImGui::TextDisabled("Create a password (at least 6 characters)");

//     ImGui::SetNextItemWidth(260.0f);
//     ImGui::InputTextWithHint("##newpw", "New password",
//         lockNew, sizeof(lockNew), ImGuiInputTextFlags_Password);

//     ImGui::SetNextItemWidth(260.0f);
//     ImGui::InputTextWithHint("##confpw", "Confirm password",
//         lockConfirm, sizeof(lockConfirm), ImGuiInputTextFlags_Password);

//     if (ImGui::Button("Set Password"))
//     {
//         if (std::strcmp(lockNew, lockConfirm) != 0)
//         {
//             lockMessage   = "Passwords do not match.";
//             lockMessageOk = false;
//         }
//         else
//         {
//             std::string message;

//             if (backend.setPassword(lockNew, "", message))
//             {
//                 created = true;
//                 Wipe(lockNew, sizeof(lockNew));
//                 Wipe(lockConfirm, sizeof(lockConfirm));
//             }

//             lockMessage   = message;
//             lockMessageOk = created;
//         }
//     }

//     return created;
// }

// static void DrawLockScreen()
// {
//     ImGui::BeginChild("LockScreen", ImVec2(0.0f, 0.0f), true);

//     SectionHeading("Logs & Reports are locked",
//         "enter the password to see the detection signatures");

//     ImGui::Spacing();

//     if (!backend.isLockConfigured())
//     {
//         if (DrawSetPasswordForm())
//             logsUnlocked = true;          // whoever creates it is let in
//     }
//     else
//     {
//         const int wait = backend.lockoutSecondsLeft();

//         ImGui::BeginDisabled(wait > 0);

//         ImGui::SetNextItemWidth(260.0f);
//         const bool enter = ImGui::InputTextWithHint("##unlockpw", "Password",
//             lockEntry, sizeof(lockEntry),
//             ImGuiInputTextFlags_Password | ImGuiInputTextFlags_EnterReturnsTrue);

//         ImGui::SameLine();
//         const bool click = ImGui::Button("Unlock");

//         ImGui::EndDisabled();

//         if (wait > 0)
//             ImGui::TextColored(COLOR_YELLOW, "Locked. Try again in %d s.", wait);

//         if ((enter || click) && wait == 0)
//         {
//             std::string message;

//             if (backend.verifyPassword(lockEntry, message))
//             {
//                 logsUnlocked = true;
//                 lockMessage.clear();
//             }
//             else
//             {
//                 lockMessage   = message;
//                 lockMessageOk = false;
//             }

//             Wipe(lockEntry, sizeof(lockEntry));
//         }
//     }

//     if (!lockMessage.empty())
//         ImGui::TextColored(lockMessageOk ? COLOR_GREEN : COLOR_RED,
//             "%s", lockMessage.c_str());

//     ImGui::EndChild();
// }

// // true = unlocked, the page may draw its content
// static bool LogsGate()
// {
//     const double now = ImGui::GetTime();
//     ImGuiIO& io = ImGui::GetIO();

//     const bool active =
//         io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f ||
//         io.MouseWheel != 0.0f || ImGui::IsMouseClicked(0) ||
//         ImGui::IsAnyItemActive();

//     if (active || !logsUnlocked)
//         logsLastActive = now;

//     if (logsUnlocked && now - logsLastActive > LOCK_IDLE_SECONDS)
//     {
//         logsUnlocked  = false;
//         lockMessage   = "Locked after 5 minutes of inactivity.";
//         lockMessageOk = false;
//     }

//     if (logsUnlocked)
//         return true;

//     DrawLockScreen();
//     return false;
// }

// // ------------------------------------------------------------
// // END TASK (shared by the Processes page and resource alerts)
// // ------------------------------------------------------------

// static const float HIGH_MEMORY_MB = 500.0f;

// static bool        procAppsOnly     = false;
// static bool        procHighMemOnly  = false;
// static std::string procMessage;                 // result of the last End Task
// static bool        procMessageOk    = true;
// static int         pendingPid       = 0;
// static std::string pendingName;
// static float       pendingMem       = 0.0f;
// static int         pendingAlertID   = 0;        // 0 = started from the Processes page
// static bool        openEndTaskPopup = false;
// static std::set<int> handledAlertIDs;           // alerts whose app was ended

// static char        endPassword[64] = "";
// static std::string endPopupMessage;

// // Call once inside the page that may open the confirmation dialog.
// // Ending a task needs the access password.
// static void DrawEndTaskPopup()
// {
//     if (openEndTaskPopup)
//     {
//         ImGui::OpenPopup("Confirm End Task");
//         openEndTaskPopup = false;

//         Wipe(endPassword, sizeof(endPassword));
//         endPopupMessage.clear();
//         lockMessage.clear();
//     }

//     const ImVec2 screen = ImGui::GetIO().DisplaySize;
//     ImGui::SetNextWindowPos(
//         ImVec2(screen.x * 0.5f, screen.y * 0.5f),
//         ImGuiCond_Appearing,
//         ImVec2(0.5f, 0.5f));

//     if (ImGui::BeginPopupModal("Confirm End Task", nullptr,
//             ImGuiWindowFlags_AlwaysAutoResize))
//     {
//         ImGui::Text("End %s (PID %d)?", pendingName.c_str(), pendingPid);
//         ImGui::TextDisabled("It is using %.0f MB. Unsaved work in this app will be lost.",
//             pendingMem);
//         ImGui::Spacing();

//         if (!backend.isLockConfigured())
//         {
//             ImGui::TextColored(COLOR_YELLOW,
//                 "Ending tasks is password protected. Create a password first.");
//             ImGui::Spacing();

//             DrawSetPasswordForm();

//             if (!lockMessage.empty())
//                 ImGui::TextColored(lockMessageOk ? COLOR_GREEN : COLOR_RED, "%s", lockMessage.c_str());
//         }
//         else
//         {
//             const int wait = backend.lockoutSecondsLeft();

//             ImGui::BeginDisabled(wait > 0);

//             ImGui::SetNextItemWidth(260.0f);
//             const bool enter = ImGui::InputTextWithHint("##endpw", "Password",
//                 endPassword, sizeof(endPassword),
//                 ImGuiInputTextFlags_Password | ImGuiInputTextFlags_EnterReturnsTrue);

//             ImGui::EndDisabled();

//             if (wait > 0)
//                 ImGui::TextColored(COLOR_YELLOW, "Locked. Try again in %d s.", wait);

//             if (!endPopupMessage.empty())
//                 ImGui::TextColored(COLOR_RED, "%s", endPopupMessage.c_str());

//             ImGui::Spacing();

//             ImGui::BeginDisabled(wait > 0);
//             const bool click = ImGui::Button("End Task", ImVec2(120.0f, 0.0f));
//             ImGui::EndDisabled();

//             if ((enter || click) && wait == 0)
//             {
//                 std::string authMessage;
//                 const bool authorised = backend.verifyPassword(endPassword, authMessage);

//                 Wipe(endPassword, sizeof(endPassword));

//                 if (!authorised)
//                 {
//                     endPopupMessage = authMessage;
//                 }
//                 else
//                 {
//                     std::string result;

//                     procMessageOk = backend.endTask(pendingPid, result, pendingName);
//                     procMessage   = result;

//                     if (procMessageOk && pendingAlertID != 0)
//                         handledAlertIDs.insert(pendingAlertID);

//                     ImGui::CloseCurrentPopup();
//                 }
//             }

//             ImGui::SameLine();
//         }

//         if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
//         {
//             Wipe(endPassword, sizeof(endPassword));
//             Wipe(lockNew, sizeof(lockNew));
//             Wipe(lockConfirm, sizeof(lockConfirm));
//             ImGui::CloseCurrentPopup();
//         }

//         ImGui::EndPopup();
//     }
// }

// // ------------------------------------------------------------
// // ALERT TABLE
// // compact = true: dashboard preview. false: full Alerts page.
// // ------------------------------------------------------------

// static void DrawAlerts(bool compact)
// {
//     ImGui::BeginChild(
//         "AlertsPanel",
//         ImVec2(0.0f, compact ? 270.0f : 0.0f),
//         true
//     );

//     SectionHeading(
//         compact ? "Recent security alerts" : "All security alerts"
//     );

//     // Severity filter + search (full page only)
//     if (!compact)
//     {
//         for (int i = 0; i < IM_ARRAYSIZE(severityFilterLabels); ++i)
//         {
//             const bool active = (severityFilter == i);

//             if (active)
//                 ImGui::PushStyleColor(ImGuiCol_Button, COLOR_BLUE);
//             else
//                 ImGui::PushStyleColor(ImGuiCol_Button, COLOR_FRAME);

//             if (ImGui::Button(severityFilterLabels[i], ImVec2(96.0f, 0.0f)))
//                 severityFilter = i;

//             ImGui::PopStyleColor();

//             if (i + 1 < IM_ARRAYSIZE(severityFilterLabels))
//                 ImGui::SameLine();
//         }

//         ImGui::SameLine();
//         ImGui::SetNextItemWidth(260.0f);
//         ImGui::InputTextWithHint("##alertsearch", "Search alerts",
//             alertSearch, IM_ARRAYSIZE(alertSearch));

//         ImGui::Spacing();
//     }

//     if (!procMessage.empty())
//         ImGui::TextColored(procMessageOk ? COLOR_GREEN : COLOR_RED,
//             "%s", procMessage.c_str());

//     DrawEndTaskPopup();

//     if (ImGui::BeginTable(
//             "AlertsTable",
//             5,
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);
//         ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 110.0f);
//         ImGui::TableSetupColumn("Severity", ImGuiTableColumnFlags_WidthFixed, 130.0f);
//         ImGui::TableSetupColumn("Source", ImGuiTableColumnFlags_WidthFixed, 200.0f);
//         ImGui::TableSetupColumn("Description");
//         ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 100.0f);
//         ImGui::TableHeadersRow();

//         int shown = 0;

//         for (const auto& alert : alerts)
//         {
//             if (!compact && severityFilter != 0 &&
//                 alert.severity != severityFilterLabels[severityFilter])
//             {
//                 continue;
//             }

//             if (!compact &&
//                 !ContainsNoCase(alert.description.c_str(), alertSearch) &&
//                 !ContainsNoCase(alert.source.c_str(), alertSearch))
//             {
//                 continue;
//             }

//             ++shown;

//             ImGui::TableNextRow();

//             ImGui::TableSetColumnIndex(0);
//             {
//                 const ImVec2 p = ImGui::GetCursorScreenPos();

//                 ImGui::GetWindowDrawList()->AddCircleFilled(
//                     ImVec2(p.x + 4.0f, p.y + ImGui::GetTextLineHeight() * 0.5f),
//                     4.0f,
//                     ImGui::GetColorU32(SeverityColor(alert.severity.c_str())));

//                 ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 16.0f);
//             }
//             ImGui::TextDisabled("%s", alert.time.c_str());

//             ImGui::TableSetColumnIndex(1);
//             SeverityBadge(alert.severity.c_str());

//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextUnformatted(alert.source.c_str());

//             ImGui::TableSetColumnIndex(3);
//             ImGui::TextUnformatted(alert.description.c_str());

//             // Resource alerts can offer End Task for the app causing them
//             ImGui::TableSetColumnIndex(4);

//             if (alert.canEndTask)
//             {
//                 if (handledAlertIDs.count(alert.id))
//                 {
//                     ImGui::TextColored(COLOR_GREEN, "Ended");
//                 }
//                 else
//                 {
//                     ImGui::PushID(alert.id);

//                     ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.55f, 0.12f, 0.15f, 1.0f));
//                     ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.18f, 0.22f, 1.0f));
//                     ImGui::PushStyleColor(ImGuiCol_ButtonActive,  COLOR_RED);

//                     if (ImGui::SmallButton("End Task"))
//                     {
//                         pendingAlertID   = alert.id;
//                         pendingPid       = alert.culpritPid;
//                         pendingName      = alert.culpritName;
//                         pendingMem       = alert.culpritMem;
//                         openEndTaskPopup = true;
//                     }

//                     ImGui::PopStyleColor(3);
//                     ImGui::PopID();
//                 }
//             }
//             else
//             {
//                 ImGui::TextDisabled("-");
//             }
//         }

//         if (shown == 0)
//         {
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(3);
//             ImGui::TextDisabled("No alerts match the current filters.");
//         }

//         ImGui::EndTable();
//     }

//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // PROCESS TABLE
// // Sortable columns, case-insensitive search, CPU usage bars.
// // ------------------------------------------------------------

// static void DrawProcesses()
// {
//     ImGui::BeginChild(
//         "ProcessesPanel",
//         ImVec2(0.0f, 0.0f),
//         true
//     );

//     SectionHeading("Running processes",
//         "apps can be ended here; system and background processes are protected");

//     ImGui::SetNextItemWidth(300.0f);

//     ImGui::InputTextWithHint(
//         "##ProcessSearch",
//         "Search by name or PID",
//         processSearch,
//         IM_ARRAYSIZE(processSearch)
//     );

//     ImGui::SameLine();
//     ImGui::Checkbox("Apps only", &procAppsOnly);

//     ImGui::SameLine();

//     char highLabel[64];
//     std::snprintf(highLabel, sizeof(highLabel),
//         "High memory only (%.0f MB+)", HIGH_MEMORY_MB);
//     ImGui::Checkbox(highLabel, &procHighMemOnly);

//     if (!procMessage.empty())
//         ImGui::TextColored(procMessageOk ? COLOR_GREEN : COLOR_RED,
//             "%s", procMessage.c_str());

//     ImGui::Spacing();

//     if (ImGui::BeginTable(
//             "ProcessTable",
//             6,
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_Sortable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);

//         ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed, 85.0f, 0);
//         ImGui::TableSetupColumn("Process name", ImGuiTableColumnFlags_WidthStretch, 0.0f, 1);
//         ImGui::TableSetupColumn(
//             "CPU %",
//             ImGuiTableColumnFlags_WidthFixed |
//             ImGuiTableColumnFlags_PreferSortDescending,
//             180.0f, 2);
//         ImGui::TableSetupColumn(
//             "Memory MB",
//             ImGuiTableColumnFlags_WidthFixed |
//             ImGuiTableColumnFlags_DefaultSort |
//             ImGuiTableColumnFlags_PreferSortDescending,
//             125.0f, 3);
//         ImGui::TableSetupColumn("Running time", ImGuiTableColumnFlags_WidthFixed, 135.0f, 4);
//         ImGui::TableSetupColumn("Action",
//             ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoSort,
//             100.0f, 5);

//         ImGui::TableHeadersRow();

//         // Build a sorted list of indices
//         std::vector<int> order(processes.size());

//         for (size_t i = 0; i < order.size(); ++i)
//             order[i] = (int)i;

//         if (ImGuiTableSortSpecs* specs = ImGui::TableGetSortSpecs())
//         {
//             if (specs->SpecsCount > 0)
//             {
//                 const ImGuiTableColumnSortSpecs& spec = specs->Specs[0];
//                 const bool asc =
//                     spec.SortDirection == ImGuiSortDirection_Ascending;

//                 std::sort(order.begin(), order.end(),
//                     [&](int a, int b)
//                     {
//                         const ProcessInfo& pa = processes[a];
//                         const ProcessInfo& pb = processes[b];

//                         int cmp = 0;

//                         switch (spec.ColumnUserID)
//                         {
//                         case 0: cmp = (pa.pid > pb.pid) - (pa.pid < pb.pid); break;
//                         case 1: cmp = pa.name.compare(pb.name); break;
//                         case 2: cmp = (pa.cpu > pb.cpu) - (pa.cpu < pb.cpu); break;
//                         case 3: cmp = (pa.memory > pb.memory) - (pa.memory < pb.memory); break;
//                         case 4: cmp = (pa.runningSeconds > pb.runningSeconds) - (pa.runningSeconds < pb.runningSeconds); break;
//                         }

//                         return asc ? (cmp < 0) : (cmp > 0);
//                     });
//             }
//         }

//         int shown = 0;

//         for (int index : order)
//         {
//             const ProcessInfo& process = processes[index];

//             if (procAppsOnly && !process.isApp)
//                 continue;

//             if (procHighMemOnly && process.memory < HIGH_MEMORY_MB)
//                 continue;

//             char pidText[32];
//             std::snprintf(pidText, sizeof(pidText), "%d", process.pid);

//             const bool matches =
//                 ContainsNoCase(process.name.c_str(), processSearch) ||
//                 ContainsNoCase(pidText, processSearch);

//             if (!matches)
//                 continue;

//             ++shown;

//             ImGui::TableNextRow();

//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextDisabled("%d", process.pid);

//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextUnformatted(process.name.c_str());

//             if (process.isApp)
//             {
//                 ImGui::SameLine();
//                 ImGui::TextDisabled("- %s", process.title.c_str());
//             }

//             // CPU bar, scaled so the busiest realistic process fills it
//             ImGui::TableSetColumnIndex(2);

//             char cpuLabel[16];
//             std::snprintf(cpuLabel, sizeof(cpuLabel), "%.1f", process.cpu);

//             ImGui::PushStyleColor(ImGuiCol_PlotHistogram, WithAlpha(LoadColor(process.cpu * 4.0f), 0.85f));
//             ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_FRAME);

//             ImGui::ProgressBar(
//                 std::min(process.cpu / 25.0f, 1.0f),
//                 ImVec2(-FLT_MIN, 18.0f),
//                 cpuLabel
//             );

//             ImGui::PopStyleColor(2);

//             // Memory: yellow when high, red when very high
//             ImGui::TableSetColumnIndex(3);

//             if (process.memory >= HIGH_MEMORY_MB * 2.0f)
//                 ImGui::TextColored(COLOR_RED, "%.1f", process.memory);
//             else if (process.memory >= HIGH_MEMORY_MB)
//                 ImGui::TextColored(COLOR_YELLOW, "%.1f", process.memory);
//             else
//                 ImGui::Text("%.1f", process.memory);

//             ImGui::TableSetColumnIndex(4);
//             ImGui::TextDisabled("%s", process.runningTime.c_str());

//             // Only apps (windowed programs) get an End Task button
//             ImGui::TableSetColumnIndex(5);

//             if (process.isApp)
//             {
//                 ImGui::PushID(process.pid);

//                 ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.55f, 0.12f, 0.15f, 1.0f));
//                 ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.18f, 0.22f, 1.0f));
//                 ImGui::PushStyleColor(ImGuiCol_ButtonActive,  COLOR_RED);

//                 if (ImGui::SmallButton("End Task"))
//                 {
//                     pendingAlertID   = 0;
//                     pendingPid       = process.pid;
//                     pendingName      = process.name;
//                     pendingMem       = process.memory;
//                     openEndTaskPopup = true;
//                 }

//                 ImGui::PopStyleColor(3);
//                 ImGui::PopID();
//             }
//             else
//             {
//                 ImGui::TextDisabled("-");
//             }
//         }

//         if (shown == 0)
//         {
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextDisabled("No process matches the current filters.");
//         }

//         ImGui::EndTable();
//     }

//     DrawEndTaskPopup();

//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // EVENT PAGE
// // Newest-first activity list for Login and File monitoring.
// // ------------------------------------------------------------

// static void DrawEventPage(
//     const char* title,
//     const std::vector<EventInfo>& events,
//     const char* emptyText,
//     const char* hint)
// {
//     ImGui::BeginChild(
//         title,
//         ImVec2(0.0f, 0.0f),
//         true
//     );

//     SectionHeading(title, hint);

//     if (ImGui::BeginTable(
//             "EventTable",
//             3,
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);
//         ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 190.0f);
//         ImGui::TableSetupColumn("Event", ImGuiTableColumnFlags_WidthFixed, 170.0f);
//         ImGui::TableSetupColumn("Description");
//         ImGui::TableHeadersRow();

//         for (const EventInfo& event : events)
//         {
//             ImGui::TableNextRow();

//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextDisabled("%s", event.time.c_str());

//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextUnformatted(event.type.c_str());

//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextUnformatted(event.description.c_str());
//         }

//         if (events.empty())
//         {
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextDisabled("%s", emptyText);
//         }

//         ImGui::EndTable();
//     }

//     ImGui::EndChild();
// }

// static void DrawSystemAnalysis()
// {
//     ImGui::BeginChild("AnalysisPanel", ImVec2(0.0f, 0.0f), true);

//     if (!analysisData.valid)
//     {
//         ImGui::TextDisabled("Collecting the first samples...");
//         ImGui::EndChild();
//         return;
//     }

//     const BackendAnalysis& a = analysisData;

//     // ---- Resource usage ----
//     SectionHeading("Resource usage");

//     char cpuText[16];
//     char ramText[16];

//     if (a.cpuAvailable)
//         std::snprintf(cpuText, sizeof(cpuText), "%.1f%%", a.cpuPercent);
//     else
//         std::snprintf(cpuText, sizeof(cpuText), "n/a");

//     if (a.memoryAvailable)
//         std::snprintf(ramText, sizeof(ramText), "%.1f%%", a.memoryPercent);
//     else
//         std::snprintf(ramText, sizeof(ramText), "n/a");

//     ImGui::Text("CPU:");
//     ImGui::SameLine();
//     ImGui::TextColored(LoadColor((float)a.cpuPercent), "%s", cpuText);

//     ImGui::Text("Memory:");
//     ImGui::SameLine();
//     ImGui::TextColored(LoadColor((float)a.memoryPercent), "%s", ramText);
//     ImGui::SameLine();
//     ImGui::TextDisabled("(%llu MB used of %llu MB, %llu MB free)",
//         (unsigned long long)a.usedMemoryMB,
//         (unsigned long long)a.totalMemoryMB,
//         (unsigned long long)a.availableMemoryMB);

//     ImGui::Spacing();

//     const float half =
//         (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

//     if (!a.cpuHistory.empty())
//     {
//         ImGui::BeginGroup();
//         ImGui::TextDisabled("CPU usage (last 60 s)");
//         DrawAreaChart("##cpuh",
//             a.cpuHistory.data(), (int)a.cpuHistory.size(), 0,
//             100.0f, ImVec2(half, 170.0f),
//             LoadColor((float)a.cpuPercent));
//         ImGui::EndGroup();
//     }

//     ImGui::SameLine();

//     if (!a.ramHistory.empty())
//     {
//         ImGui::BeginGroup();
//         ImGui::TextDisabled("Memory usage (last 60 s)");
//         DrawAreaChart("##ramh",
//             a.ramHistory.data(), (int)a.ramHistory.size(), 0,
//             100.0f, ImVec2(half, 170.0f),
//             LoadColor((float)a.memoryPercent));
//         ImGui::EndGroup();
//     }

//     ImGui::Spacing();
//     ImGui::Separator();
//     ImGui::Spacing();

//     // ---- Findings ----
//     SectionHeading("Findings", "A diagnostic signal, not proof of an intrusion");

//     ImGui::Text(
//         "Processes: %llu    High CPU (>=80%%): %llu    High memory (>=1 GB): %llu",
//         (unsigned long long)a.processCount,
//         (unsigned long long)a.highCpuProcessCount,
//         (unsigned long long)a.highMemoryProcessCount);

//     if (a.findings.empty())
//     {
//         ImGui::TextColored(COLOR_GREEN, "No resource concerns detected.");
//     }
//     else
//     {
//         for (const std::string& finding : a.findings)
//             ImGui::TextColored(COLOR_YELLOW, "- %s", finding.c_str());
//     }

//     ImGui::Spacing();
//     ImGui::Separator();
//     ImGui::Spacing();

//     // ---- Top CPU processes ----
//     SectionHeading("Top processes by CPU");

//     if (ImGui::BeginTable(
//             "TopCpuTable", 4,
//             ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_SizingStretchProp))
//     {
//         ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed, 85.0f);
//         ImGui::TableSetupColumn("Process name");
//         ImGui::TableSetupColumn("CPU %", ImGuiTableColumnFlags_WidthFixed, 100.0f);
//         ImGui::TableSetupColumn("Memory MB", ImGuiTableColumnFlags_WidthFixed, 125.0f);
//         ImGui::TableHeadersRow();

//         for (const BackendProcess& p : a.topCpuProcesses)
//         {
//             ImGui::TableNextRow();

//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextDisabled("%d", p.pid);

//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextUnformatted(p.name.c_str());

//             ImGui::TableSetColumnIndex(2);
//             ImGui::Text("%.1f", p.cpuPercent);

//             ImGui::TableSetColumnIndex(3);
//             ImGui::Text("%.1f", p.memoryMB);
//         }

//         ImGui::EndTable();
//     }

//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // FILE SCANNER PAGE
// // ------------------------------------------------------------

// static std::string FormatBytes(unsigned long long b)
// {
//     char t[32];

//     if (b >= (1ULL << 30))      std::snprintf(t, sizeof(t), "%.2f GB", b / 1073741824.0);
//     else if (b >= (1ULL << 20)) std::snprintf(t, sizeof(t), "%.1f MB", b / 1048576.0);
//     else if (b >= 1024ULL)      std::snprintf(t, sizeof(t), "%.1f KB", b / 1024.0);
//     else                        std::snprintf(t, sizeof(t), "%llu B", b);

//     return t;
// }

// // Rotating-arc spinner. Reserves a (2*radius) square at the cursor.
// // active = false draws a plain ring instead (idle / finished).
// static void DrawSpinner(float radius, float thickness, const ImVec4& color, bool active)
// {
//     const ImVec2 pos = ImGui::GetCursorScreenPos();
//     const ImVec2 center(pos.x + radius, pos.y + radius);
//     ImDrawList* draw = ImGui::GetWindowDrawList();
//     const float t = (float)ImGui::GetTime();

//     if (!active)
//     {
//         draw->AddCircle(center, radius, ImGui::GetColorU32(color), 48, thickness);
//     }
//     else
//     {
//         // faint full ring + a rotating arc on top
//         draw->AddCircle(center, radius,
//             ImGui::GetColorU32(WithAlpha(color, 0.18f)), 48, thickness);

//         const float start = t * 5.0f;

//         draw->PathClear();
//         draw->PathArcTo(center, radius, start, start + 2.4f, 32);
//         draw->PathStroke(ImGui::GetColorU32(color), 0, thickness);

//         // pulsing dot in the middle
//         const float pulse = 0.75f + 0.25f * std::sin(t * 4.0f);
//         draw->AddCircleFilled(center, radius * 0.30f * pulse,
//             ImGui::GetColorU32(WithAlpha(color, 0.85f)), 24);
//     }

//     ImGui::Dummy(ImVec2(radius * 2.0f, radius * 2.0f));
// }

// // "C:\a\b\file.txt" -> dir "C:\a\b", name "file.txt"
// static void SplitPath(const std::string& full, std::string& dir, std::string& name)
// {
//     const std::size_t pos = full.find_last_of("\\/");

//     if (pos == std::string::npos)
//     {
//         dir.clear();
//         name = full;
//     }
//     else
//     {
//         dir  = full.substr(0, pos);
//         name = full.substr(pos + 1);
//     }
// }

// static void DrawFileScanner()
// {
//     // Refresh a few times per second, not every frame
//     static double lastRefresh = -1.0;
//     const double now = ImGui::GetTime();

//     if (lastRefresh < 0.0 || now - lastRefresh >= 0.25)
//     {
//         lastRefresh  = now;
//         scanStatus   = backend.getScanStatus();
//         scanFindings = backend.getScanFindings();
//     }

//     const bool running = scanStatus.running;

//     ImGui::BeginChild("ScannerPanel", ImVec2(0.0f, 0.0f), true);

//     SectionHeading("File Scanner", "Signature + heuristic scan");

//     // ---- Scan type ----
//     ImGui::BeginDisabled(running);

//     ImGui::RadioButton("Quick (user folders, temp, startup)", &scanTypeSel, 0);
//     ImGui::SameLine();
//     ImGui::RadioButton("Full (all fixed drives)", &scanTypeSel, 1);
//     ImGui::SameLine();
//     ImGui::RadioButton("Custom", &scanTypeSel, 2);

//     if (scanTypeSel == 2)
//     {
//         ImGui::SetNextItemWidth(-1.0f);
//         ImGui::InputTextWithHint("##scanpath",
//             "Folder or file, e.g. C:\\Users\\you\\Downloads",
//             scanPath, sizeof(scanPath));
//     }

//     ImGui::EndDisabled();

//     // ---- Buttons ----
//     if (!running)
//     {
//         if (ImGui::Button("Start Scan"))
//         {
//             if (scanTypeSel == 2 && scanPath[0] == '\0')
//                 scanMessage = "Enter a folder or file path first.";
//             else if (!backend.startScan((ScanType)scanTypeSel, scanPath))
//                 scanMessage = "A scan is already running.";
//             else
//                 scanMessage.clear();
//         }
//     }
//     else
//     {
//         if (ImGui::Button("Stop Scan"))
//             backend.cancelScan();
//     }

//     if (!running && scanStatus.finished)
//     {
//         ImGui::SameLine();

//         if (ImGui::Button("Export Report"))
//         {
//             std::string out;

//             scanMessage = backend.exportScanReport(out)
//                 ? "Report saved: " + out
//                 : "Could not write the report file.";
//         }
//     }

//     if (!scanMessage.empty())
//     {
//         ImGui::SameLine();
//         ImGui::TextDisabled("%s", scanMessage.c_str());
//     }

//     ImGui::Separator();
//     ImGui::Spacing();

//     // ========================================================
//     // Two panels side by side: live scan (left), threats (right)
//     // ========================================================
//     const float gap    = ImGui::GetStyle().ItemSpacing.x;
//     const float leftW  = (ImGui::GetContentRegionAvail().x - gap) * 0.58f;
//     const float panelH = 230.0f;

//     ImGui::PushStyleColor(ImGuiCol_ChildBg, COLOR_SIDEBAR);

//     // ---------------- LEFT: live scan ----------------
//     ImGui::BeginChild("ScanLive", ImVec2(leftW, panelH), true);

//     {
//         ImVec4 stateColor = COLOR_MUTED;
//         const char* stateText = "Ready to scan";

//         if (running)
//         {
//             stateColor = COLOR_CYAN;
//             stateText  = "Scanning";
//         }
//         else if (scanStatus.finished && scanStatus.cancelled)
//         {
//             stateColor = COLOR_YELLOW;
//             stateText  = "Scan stopped";
//         }
//         else if (scanStatus.finished)
//         {
//             stateColor = (scanStatus.malicious + scanStatus.suspicious > 0)
//                 ? COLOR_ORANGE : COLOR_GREEN;
//             stateText  = "Scan completed";
//         }

//         // Spinner (or a plain ring when idle)
//         DrawSpinner(26.0f, 4.0f, stateColor, running);
//         ImGui::SameLine(0.0f, 16.0f);

//         ImGui::BeginGroup();

//         ImGui::SetWindowFontScale(1.3f);
//         ImGui::PushStyleColor(ImGuiCol_Text, stateColor);

//         if (running)
//         {
//             const int dots = (int)(ImGui::GetTime() * 2.0) % 4;
//             ImGui::Text("%s%.*s", stateText, dots, "...");
//         }
//         else
//         {
//             ImGui::TextUnformatted(stateText);
//         }

//         ImGui::PopStyleColor();
//         ImGui::SetWindowFontScale(1.0f);

//         ImGui::TextDisabled("%s",
//             scanStatus.target.empty() ? "Choose a scan type and press Start"
//                                       : scanStatus.target.c_str());

//         ImGui::EndGroup();

//         ImGui::Spacing();
//         ImGui::Separator();
//         ImGui::Spacing();

//         // Which file is being scanned right now
//         ImGui::TextDisabled("%s", running ? "NOW SCANNING" : "LAST FILE SCANNED");

//         std::string dir, name;
//         SplitPath(scanStatus.currentFile, dir, name);

//         if (name.empty())
//         {
//             ImGui::TextDisabled("-");
//         }
//         else
//         {
//             ImGui::SetWindowFontScale(1.15f);
//             ImGui::PushStyleColor(ImGuiCol_Text, COLOR_TEXT);
//             ImGui::TextUnformatted(name.c_str());
//             ImGui::PopStyleColor();
//             ImGui::SetWindowFontScale(1.0f);

//             ImGui::PushStyleColor(ImGuiCol_Text, COLOR_MUTED);
//             ImGui::TextWrapped("%s", dir.c_str());
//             ImGui::PopStyleColor();
//         }

//         ImGui::Spacing();

//         // Live counters
//         ImGui::Text("Files: %llu", scanStatus.filesScanned);
//         ImGui::SameLine(0.0f, 18.0f);
//         ImGui::Text("Skipped: %llu", scanStatus.filesSkipped);
//         ImGui::SameLine(0.0f, 18.0f);
//         ImGui::Text("Data: %s", FormatBytes(scanStatus.bytesScanned).c_str());
//         ImGui::SameLine(0.0f, 18.0f);
//         ImGui::Text("Time: %s", FormatDuration(scanStatus.elapsedSeconds).c_str());
//     }

//     ImGui::EndChild();

//     ImGui::SameLine();

//     // ---------------- RIGHT: threats found ----------------
//     ImGui::BeginChild("ScanThreats", ImVec2(0.0f, panelH), true);

//     {
//         ImGui::PushStyleColor(ImGuiCol_Text, COLOR_CYAN);
//         ImGui::SetWindowFontScale(1.1f);
//         ImGui::TextUnformatted("Suspicious & malicious files");
//         ImGui::SetWindowFontScale(1.0f);
//         ImGui::PopStyleColor();

//         ImGui::TextColored(COLOR_RED, "Malicious: %d", scanStatus.malicious);
//         ImGui::SameLine(0.0f, 18.0f);
//         ImGui::TextColored(COLOR_YELLOW, "Suspicious: %d", scanStatus.suspicious);

//         ImGui::Separator();

//         if (scanFindings.empty())
//         {
//             ImGui::Spacing();

//             if (running)
//                 ImGui::TextDisabled("Nothing suspicious so far...");
//             else if (scanStatus.finished)
//                 ImGui::TextColored(COLOR_GREEN, "No threats found.");
//             else
//                 ImGui::TextDisabled("Findings will appear here.");
//         }

//         // Newest first
//         for (std::size_t i = scanFindings.size(); i-- > 0; )
//         {
//             const ScanFinding& f = scanFindings[i];
//             const ImVec4 c = (f.level == "Malicious") ? COLOR_RED : COLOR_YELLOW;

//             std::string dir, name;
//             SplitPath(f.path, dir, name);

//             ImGui::PushID((int)i);

//             ImGui::TextColored(c, "[%s]", f.level.c_str());
//             ImGui::SameLine();
//             ImGui::TextUnformatted(f.name.c_str());

//             ImGui::TextDisabled("%s", name.c_str());

//             if (ImGui::IsItemHovered())
//             {
//                 ImGui::BeginTooltip();
//                 ImGui::Text("%s", f.path.c_str());
//                 ImGui::Text("Reason: %s", f.reason.c_str());
//                 ImGui::Text("Score: %d", f.score);
//                 ImGui::EndTooltip();
//             }

//             ImGui::Separator();
//             ImGui::PopID();
//         }
//     }

//     ImGui::EndChild();

//     ImGui::PopStyleColor();   // ChildBg

//     ImGui::Spacing();

//     // ========================================================
//     // Full findings table (all details)
//     // ========================================================
//     SectionHeading("All findings", "hover a name for score and SHA-256");

//     if (ImGui::BeginTable(
//             "ScanFindings",
//             5,
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);
//         ImGui::TableSetupColumn("Time",   ImGuiTableColumnFlags_WidthFixed, 80.0f);
//         ImGui::TableSetupColumn("Level",  ImGuiTableColumnFlags_WidthFixed, 95.0f);
//         ImGui::TableSetupColumn("Name",   ImGuiTableColumnFlags_WidthFixed, 190.0f);
//         ImGui::TableSetupColumn("File");
//         ImGui::TableSetupColumn("Reason");
//         ImGui::TableHeadersRow();

//         for (const ScanFinding& f : scanFindings)
//         {
//             ImGui::TableNextRow();

//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextDisabled("%s", f.time.c_str());

//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextColored(
//                 f.level == "Malicious" ? COLOR_RED : COLOR_YELLOW,
//                 "%s", f.level.c_str());

//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextUnformatted(f.name.c_str());

//             if (ImGui::IsItemHovered())
//             {
//                 ImGui::BeginTooltip();
//                 ImGui::Text("Score: %d", f.score);
//                 ImGui::Text("Size:  %s", FormatBytes(f.size).c_str());

//                 if (!f.sha256.empty())
//                     ImGui::Text("SHA-256: %s", f.sha256.c_str());

//                 ImGui::EndTooltip();
//             }

//             ImGui::TableSetColumnIndex(3);
//             ImGui::TextUnformatted(f.path.c_str());

//             ImGui::TableSetColumnIndex(4);
//             ImGui::TextUnformatted(f.reason.c_str());
//         }

//         ImGui::EndTable();
//     }

//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // LOGS & REPORTS PAGE
// // Lists every signature / rule the File Scanner checks for.
// // ------------------------------------------------------------

// static void DrawLogsReports()
// {
//     if (!LogsGate())
//         return;

//     static std::vector<SignatureInfo> signatures;
//     static bool loaded = false;
//     static char search[128] = "";

//     if (!loaded)
//     {
//         signatures = FileScanner::getSignatures();
//         loaded = true;
//     }

//     int hashCount = 0, contentCount = 0, nameCount = 0;

//     for (const SignatureInfo& s : signatures)
//     {
//         if (s.category == "Known hash")        ++hashCount;
//         else if (s.category == "File content") ++contentCount;
//         else                                   ++nameCount;
//     }

//     ImGui::BeginChild("LogsPanel", ImVec2(0.0f, 0.0f), true);

//     SectionHeading("Detection signatures", "what the File Scanner checks for");

//     if (ImGui::Button("Lock now"))
//     {
//         logsUnlocked = false;
//         lockMessage.clear();
//     }

//     ImGui::SameLine();
//     ImGui::TextDisabled("Locks again after 5 minutes idle or when you leave this page.");

//     if (ImGui::CollapsingHeader("Change password"))
//     {
//         static std::string changeMessage;
//         static bool        changeOk = false;

//         ImGui::SetNextItemWidth(260.0f);
//         ImGui::InputTextWithHint("##oldpw", "Current password",
//             lockOld, sizeof(lockOld), ImGuiInputTextFlags_Password);

//         ImGui::SetNextItemWidth(260.0f);
//         ImGui::InputTextWithHint("##chnew", "New password",
//             lockNew, sizeof(lockNew), ImGuiInputTextFlags_Password);

//         ImGui::SetNextItemWidth(260.0f);
//         ImGui::InputTextWithHint("##chconf", "Confirm new password",
//             lockConfirm, sizeof(lockConfirm), ImGuiInputTextFlags_Password);

//         if (ImGui::Button("Update password"))
//         {
//             if (std::strcmp(lockNew, lockConfirm) != 0)
//             {
//                 changeMessage = "New passwords do not match.";
//                 changeOk      = false;
//             }
//             else
//             {
//                 changeOk = backend.setPassword(lockNew, lockOld, changeMessage);

//                 if (changeOk)
//                 {
//                     Wipe(lockOld, sizeof(lockOld));
//                     Wipe(lockNew, sizeof(lockNew));
//                     Wipe(lockConfirm, sizeof(lockConfirm));
//                 }
//             }
//         }

//         if (!changeMessage.empty())
//             ImGui::TextColored(changeOk ? COLOR_GREEN : COLOR_RED,
//                 "%s", changeMessage.c_str());
//     }

//     ImGui::Spacing();

//     ImGui::PushTextWrapPos(0.0f);
//     ImGui::TextColored(COLOR_MUTED,
//         "A file is reported Malicious on an instant match (EICAR test string or a "
//         "known-bad SHA-256). Otherwise every matching rule adds its weight to a "
//         "score, and a file scoring %d or more is reported Suspicious.",
//         FileScanner::suspiciousThreshold());
//     ImGui::PopTextWrapPos();

//     ImGui::Spacing();

//     ImGui::Text("%d rules", (int)signatures.size());
//     ImGui::SameLine(0.0f, 18.0f);
//     ImGui::TextColored(COLOR_RED,    "Known hashes: %d", hashCount);
//     ImGui::SameLine(0.0f, 18.0f);
//     ImGui::TextColored(COLOR_CYAN,   "File content: %d", contentCount);
//     ImGui::SameLine(0.0f, 18.0f);
//     ImGui::TextColored(COLOR_YELLOW, "Name / location: %d", nameCount);

//     ImGui::Spacing();

//     ImGui::SetNextItemWidth(300.0f);
//     ImGui::InputTextWithHint("##sigsearch", "Search signatures", search, IM_ARRAYSIZE(search));

//     ImGui::SameLine();

//     if (ImGui::Button("Reload"))
//         signatures = FileScanner::getSignatures();   // re-reads hids_hashes.txt

//     ImGui::Spacing();

//     if (ImGui::BeginTable(
//             "SignatureTable",
//             4,
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);
//         ImGui::TableSetupColumn("Type",      ImGuiTableColumnFlags_WidthFixed, 160.0f);
//         ImGui::TableSetupColumn("Name",      ImGuiTableColumnFlags_WidthStretch, 1.0f);
//         ImGui::TableSetupColumn("Pattern / condition", ImGuiTableColumnFlags_WidthStretch, 1.4f);
//         ImGui::TableSetupColumn("Result",    ImGuiTableColumnFlags_WidthFixed, 100.0f);
//         ImGui::TableHeadersRow();

//         int shown = 0;

//         for (const SignatureInfo& s : signatures)
//         {
//             if (!ContainsNoCase(s.name.c_str(), search) &&
//                 !ContainsNoCase(s.pattern.c_str(), search) &&
//                 !ContainsNoCase(s.category.c_str(), search))
//                 continue;

//             ++shown;

//             ImGui::TableNextRow();

//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextColored(
//                 s.category == "Known hash"   ? COLOR_RED :
//                 s.category == "File content" ? COLOR_CYAN : COLOR_YELLOW,
//                 "%s", s.category.c_str());

//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextUnformatted(s.name.c_str());

//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextDisabled("%s", s.pattern.c_str());

//             ImGui::TableSetColumnIndex(3);

//             if (s.instantMalicious)
//                 ImGui::TextColored(COLOR_RED, "Malicious");
//             else
//                 ImGui::TextColored(
//                     s.weight >= 4 ? COLOR_ORANGE :
//                     s.weight >= 2 ? COLOR_YELLOW : COLOR_MUTED,
//                     "+%d score", s.weight);
//         }

//         if (shown == 0)
//         {
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextDisabled("No signature matches \"%s\".", search);
//         }

//         ImGui::EndTable();
//     }

//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // NOTIFICATION SETTINGS (shown on the Settings page)
// // ------------------------------------------------------------

// static void DrawNotificationSettings()
// {
//     ImGui::BeginChild("NotificationSettings", ImVec2(0.0f, 150.0f), true,
//                       ImGuiWindowFlags_NoScrollbar);

//     SectionHeading("Desktop notifications",
//         "pop-up toasts when a new alert is raised");

//     bool enabled = backend.notificationsEnabled();

//     if (ImGui::Checkbox("Show desktop notifications", &enabled))
//         backend.setNotificationsEnabled(enabled);

//     ImGui::BeginDisabled(!enabled);

//     static const char* levels[] = {
//         "Low and above", "Medium and above",
//         "High and above", "Critical only"
//     };

//     int level = backend.notificationMinSeverity();

//     ImGui::SetNextItemWidth(220.0f);

//     if (ImGui::Combo("Notify for", &level, levels, IM_ARRAYSIZE(levels)))
//         backend.setNotificationMinSeverity(level);

//     if (ImGui::Button("Send test notification"))
//         backend.sendTestNotification();

//     ImGui::EndDisabled();

//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // MAIN CONTENT (everything right of the sidebar)
// // ------------------------------------------------------------

// static void DrawMainContent()
// {
//     RefreshFromBackend();

//     // Leaving Logs & Reports locks it again and clears typed credentials.
//     static int    lastPage      = -1;
//     static double pageChangedAt = 0.0;

//     if (selectedPage != lastPage)
//     {
//         if (lastPage == LOGS_PAGE_INDEX)
//         {
//             logsUnlocked = false;
//             lockMessage.clear();
//             Wipe(lockOld, sizeof(lockOld));
//             Wipe(lockNew, sizeof(lockNew));
//             Wipe(lockConfirm, sizeof(lockConfirm));
//         }

//         lastPage      = selectedPage;
//         pageChangedAt = ImGui::GetTime();
//     }

//     DrawTopBar();
//     ImGui::Separator();
//     ImGui::Spacing();

//     if (demoMode)
//     {
//         DrawPill("DEMO DATA", COLOR_YELLOW);
//         ImGui::SameLine();
//         ImGui::TextColored(COLOR_YELLOW,
//             "Sample rows are labelled [DEMO] and are not real host findings.");
//         ImGui::Spacing();
//     }

//     const float fade = ClampFloat(
//         (float)((ImGui::GetTime() - pageChangedAt) / 0.18),
//         0.0f, 1.0f
//     );
//     ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.25f + 0.75f * fade);

//     switch (selectedPage)
//     {
//     case 0:
//         DrawEngineBanner();
//         DrawMetrics();
//         ImGui::Spacing();
//         DrawThreatPanel();
//         ImGui::Spacing();
//         DrawAlerts(true);
//         break;

//     case 1:
//         DrawProcesses();
//         break;

//     case 2:
//         DrawEventPage("Failed sign-in attempts", loginEvents,
//             "No failed sign-ins recorded since monitoring started.",
//             "Reads the Windows Security log - run as Administrator");
//         break;

//     case 3:
//         DrawEventPage("File activity", fileEvents,
//             "No changes detected in the watched folder yet.",
//             "Watching the HIDS_Monitor folder");
//         break;

//     case 4:
//         DrawAlerts(false);
//         break;

//     case 5:
//         DrawLogsReports();
//         break;

//     case 6:
//         DrawSystemAnalysis();
//         break;

//     case 7:
//         DrawFileScanner();
//         break;

//     case 8:
//         DrawNotificationSettings();
//         ImGui::Spacing();
//         DrawSettingsPage(monitoringActive);
//         break;
//     }

//     ImGui::PopStyleVar();
// }

// int main()
// {
//     if (!glfwInit())
//     {
//         std::fprintf(stderr, "ERROR: Failed to initialize GLFW.\n");
//         return 1;
//     }

//     // Request OpenGL 3.0
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

//     GLFWwindow* window = glfwCreateWindow(
//         1400,
//         900,
//         "HIDS - Host Intrusion Detection System",
//         nullptr,
//         nullptr
//     );

//     if (window == nullptr)
//     {
//         std::fprintf(stderr, "ERROR: Failed to create dashboard window.\n");
//         glfwTerminate();
//         return 1;
//     }

//     glfwMakeContextCurrent(window);
//     glfwSwapInterval(1);

//     // Native window handle for the tray icon / desktop notifications
// #ifdef _WIN32
//     backend.attachWindow(glfwGetWin32Window(window));
// #endif

//     IMGUI_CHECKVERSION();
//     ImGui::CreateContext();

//     ImGuiIO& io = ImGui::GetIO();

//     io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

//     // Don't write imgui.ini next to the executable
//     io.IniFilename = nullptr;

//     LoadFonts(io);
//     LoadUiConfig();           // saved theme / interface size / demo flag
//     ApplyTheme(themeIndex);   // also builds the ImGui style
//     RefineStyle();            // flatter, more modern spacing and rounding
//     InitUiScale(io);

//     int styledTheme = themeIndex;

//     // Seed sparkline history so the cards aren't empty on first frame
//     for (int i = 0; i < HISTORY_SIZE; ++i)
//     {
//         cpuHistory[i] = cpuNow;
//         ramHistory[i] = ramNow;
//     }

//     ImGui_ImplGlfw_InitForOpenGL(window, true);
//     ImGui_ImplOpenGL3_Init("#version 130");

//     // Monitoring starts only when the user presses Start HIDS / Start protection.
//     monitoringActive = backend.isRunning();

//     // Main application loop
//     while (!glfwWindowShouldClose(window))
//     {
//         glfwPollEvents();

//         // Send queued desktop notifications (must run on the UI thread)
//         backend.flushNotifications();

//         // Theme clicks are queued and applied here, never mid-frame
//         ApplyPendingTheme();

//         // A theme change rebuilds the style, so re-apply our refinements
//         if (themeIndex != styledTheme)
//         {
//             RefineStyle();
//             styledTheme = themeIndex;
//         }

//         ImGui_ImplOpenGL3_NewFrame();
//         ImGui_ImplGlfw_NewFrame();
//         ImGui::NewFrame();

//         // Gradient backdrop; the root window is transparent so it shows through
//         DrawBackdrop();
//         ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));

//         // Full-window root
//         ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
//         ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

//         ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 12.0f));

//         ImGui::Begin(
//             "HIDSRoot",
//             nullptr,
//             ImGuiWindowFlags_NoTitleBar |
//             ImGuiWindowFlags_NoResize |
//             ImGuiWindowFlags_NoMove |
//             ImGuiWindowFlags_NoCollapse |
//             ImGuiWindowFlags_NoScrollbar |
//             ImGuiWindowFlags_NoBringToFrontOnFocus
//         );

//         ImGui::PopStyleVar();

//         if (appScreen == SCREEN_PAGE)
//         {
//             // Persistent sidebar + content area
//             DrawSidebar();
//             ImGui::SameLine(0.0f, 12.0f);

//             ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));

//             ImGui::BeginChild(
//                 "MainContent",
//                 ImVec2(0.0f, 0.0f),
//                 false
//             );

//             DrawMainContent();

//             ImGui::EndChild();
//             ImGui::PopStyleColor();
//         }
//         else
//         {
//             // Welcome screen, home hub and domain screens (no sidebar)
//             DrawFullScreen();
//         }

//         ImGui::End();
//         ImGui::PopStyleColor();   // WindowBg

//         ImGui::Render();

//         int displayWidth = 0;
//         int displayHeight = 0;

//         glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
//         glViewport(0, 0, displayWidth, displayHeight);

//         glClearColor(
//             HIDS_COLOR_BACKGROUND.x,
//             HIDS_COLOR_BACKGROUND.y,
//             HIDS_COLOR_BACKGROUND.z,
//             HIDS_COLOR_BACKGROUND.w
//         );

//         glClear(GL_COLOR_BUFFER_BIT);

//         ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

//         glfwSwapBuffers(window);
//     }

//     backend.shutdown();

//     ImGui_ImplOpenGL3_Shutdown();
//     ImGui_ImplGlfw_Shutdown();
//     ImGui::DestroyContext();

//     glfwDestroyWindow(window);
//     glfwTerminate();

//     return 0;
// }




















































// #include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"
// #include <GLFW/glfw3.h>

// // Native window handle (needed for the tray icon / desktop notifications)
// #ifdef _WIN32
// #include <windows.h>
// #define GLFW_EXPOSE_NATIVE_WIN32
// #include <GLFW/glfw3native.h>
// #endif

// #include <algorithm>
// #include <cctype>
// #include <cfloat>
// #include <cmath>
// #include <cstdio>
// #include <cstdlib>
// #include <cstring>
// #include <set>
// #include <string>
// #include <vector>
// #include "HIDSBackend.h"
// #include "DashboardTheme.h"   // themes, colours, icons, Settings page

// // ------------------------------------------------------------
// // DATA STRUCTURES
// // ------------------------------------------------------------

// struct ProcessInfo {
//     int pid;
//     std::string name;
//     float cpu;
//     float memory;
//     double runningSeconds;
//     std::string runningTime;   // formatted for display
//     bool isApp = false;        // can be ended from the Processes page
//     std::string title;         // window title (apps only)
// };

// struct AlertInfo {
//     int id = 0;
//     std::string time;
//     std::string severity;
//     std::string source;
//     std::string description;

//     bool        canEndTask = false;   // resource alert with a culprit app
//     int         culpritPid = 0;
//     std::string culpritName;
//     float       culpritMem = 0.0f;
// };

// struct EventInfo {
//     std::string time;
//     std::string type;
//     std::string description;
// };

// static HIDSBackend backend;

// static std::vector<ProcessInfo> processes;
// static std::vector<AlertInfo>   alerts;
// static std::vector<EventInfo>   loginEvents;
// static std::vector<EventInfo>   fileEvents;
// static BackendAnalysis          analysisData;

// // File scanner page state
// static ScanStatus               scanStatus;
// static std::vector<ScanFinding> scanFindings;
// static int                      scanTypeSel = 0;      // 0 Quick, 1 Full, 2 Custom
// static char                     scanPath[512] = "";
// static std::string              scanMessage;
// static int processCount = 0;

// static int  selectedPage = 0;
// static char processSearch[128] = "";
// static bool monitoringActive = false;
// static double engineStartedAt = -1.0;   // for the "running for" timer

// static int  severityFilter = 0;   // 0 = All, 1..4 = Critical..Low
// static const char* severityFilterLabels[] =
//     { "All", "Critical", "High", "Medium", "Low" };
// static char alertSearch[128] = "";

// static const char* pages[] = {
//     "Dashboard",
//     "Processes",
//     "Login Monitoring",
//     "File Monitoring",
//     "Alerts",
//     "Logs & Reports",
//     "System Analysis",
//     "File Scanner",
//     "Settings"
// };

// static const char* pageSubtitles[] = {
//     "Overview of this machine's security status",
//     "Everything currently running on this host",
//     "Sign-in activity on this host",
//     "Changes to monitored files and folders",
//     "Events raised by the HIDS monitors",
//     "Detection signatures and scan reports",
//     "Deeper inspection of system behaviour",
//     "Scan files for known malware and suspicious traits",
//     "Appearance, interface size, notifications and data source"
// };

// // Live metrics (ring buffers feed the sparklines)
// static const int HISTORY_SIZE = 60;
// static float cpuHistory[HISTORY_SIZE] = {};
// static float ramHistory[HISTORY_SIZE] = {};
// static int   historyOffset = 0;
// static float cpuNow = 0.0f;
// static float ramNow = 0.0f;

// // Threat activity (feeds the dashboard histogram)
// static float threatHistory[HISTORY_SIZE] = {};
// static float threatAccum = 0.0f;

// // ---- Screen flow: Welcome -> Hub -> Domain -> Feature page ----
// enum AppScreen { SCREEN_WELCOME = 0, SCREEN_HUB, SCREEN_DOMAIN, SCREEN_PAGE };
// static int    appScreen       = SCREEN_WELCOME;
// static int    selectedDomain  = 0;
// static double screenChangedAt = 0.0;

// static void OpenPage(int page);   // defined after the route tables

// static float ClampFloat(float v, float lo, float hi)
// {
//     return v < lo ? lo : (v > hi ? hi : v);
// }

// // ------------------------------------------------------------
// // FONT LOADING
// // ------------------------------------------------------------

// static bool FileExists(const char* path)
// {
//     FILE* f = std::fopen(path, "rb");
//     if (f == nullptr)
//         return false;

//     std::fclose(f);
//     return true;
// }

// static void LoadFonts(ImGuiIO& io)
// {
//     const char* candidates[] = {
//         "C:/Windows/Fonts/segoeui.ttf",
//         "C:/Windows/Fonts/arial.ttf",
//         "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
//         "/usr/share/fonts/TTF/DejaVuSans.ttf",
//         "/System/Library/Fonts/Supplemental/Arial.ttf"
//     };

//     for (const char* path : candidates)
//     {
//         if (!FileExists(path))
//             continue;

//         ImFont* font = io.Fonts->AddFontFromFileTTF(path, 18.0f);

//         if (font != nullptr)
//         {
//             io.FontDefault = font;
//             io.FontGlobalScale = 1.0f;
//             return;
//         }
//     }

//     io.Fonts->AddFontDefault();
//     io.FontGlobalScale = 1.25f;
// }

// // ------------------------------------------------------------
// // STYLE REFINEMENT
// // ------------------------------------------------------------

// static void RefineStyle()
// {
//     ImGuiStyle& s = ImGui::GetStyle();

//     s.WindowRounding    = 0.0f;
//     s.ChildRounding     = 10.0f;
//     s.FrameRounding     = 6.0f;
//     s.PopupRounding     = 8.0f;
//     s.GrabRounding      = 6.0f;
//     s.ScrollbarRounding = 8.0f;
//     s.TabRounding       = 6.0f;

//     s.WindowBorderSize  = 0.0f;
//     s.ChildBorderSize   = 1.0f;
//     s.FrameBorderSize   = 0.0f;
//     s.PopupBorderSize   = 1.0f;

//     s.WindowPadding     = ImVec2(16.0f, 14.0f);
//     s.FramePadding      = ImVec2(12.0f, 7.0f);
//     s.ItemSpacing       = ImVec2(10.0f, 10.0f);
//     s.CellPadding       = ImVec2(10.0f, 7.0f);
//     s.ScrollbarSize     = 12.0f;
// }

// static bool ContainsNoCase(const char* text, const char* search)
// {
//     if (search[0] == '\0')
//         return true;

//     const size_t textLen = std::strlen(text);
//     const size_t searchLen = std::strlen(search);

//     if (searchLen > textLen)
//         return false;

//     for (size_t i = 0; i + searchLen <= textLen; ++i)
//     {
//         size_t j = 0;

//         while (j < searchLen &&
//                std::tolower((unsigned char)text[i + j]) ==
//                std::tolower((unsigned char)search[j]))
//         {
//             ++j;
//         }

//         if (j == searchLen)
//             return true;
//     }

//     return false;
// }

// static ImVec4 SeverityColor(const char* severity)
// {
//     if (std::strcmp(severity, "Critical") == 0) return COLOR_RED;
//     if (std::strcmp(severity, "High") == 0)     return COLOR_ORANGE;
//     if (std::strcmp(severity, "Medium") == 0)   return COLOR_YELLOW;

//     return COLOR_CYAN;
// }

// static int CountSeverity(const char* severity)
// {
//     int count = 0;

//     for (const auto& alert : alerts)
//         if (alert.severity == severity)
//             ++count;

//     return count;
// }

// // Usage colour: green -> yellow -> red as load rises
// static ImVec4 LoadColor(float percent)
// {
//     if (percent >= 85.0f) return COLOR_RED;
//     if (percent >= 65.0f) return COLOR_YELLOW;

//     return COLOR_GREEN;
// }

// static void SectionHeading(const char* title, const char* hint = nullptr)
// {
//     {
//         const ImVec2 p = ImGui::GetCursorScreenPos();
//         const float  h = ImGui::GetTextLineHeight() * 1.1f;

//         ImGui::GetWindowDrawList()->AddRectFilled(
//             ImVec2(p.x, p.y + 1.0f), ImVec2(p.x + 4.0f, p.y + h),
//             ImGui::GetColorU32(COLOR_CYAN), 2.0f);

//         ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 12.0f);
//     }

//     ImGui::PushStyleColor(ImGuiCol_Text, COLOR_CYAN);
//     ImGui::SetWindowFontScale(1.1f);
//     ImGui::TextUnformatted(title);
//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::PopStyleColor();

//     if (hint != nullptr)
//     {
//         ImGui::SameLine();
//         ImGui::TextDisabled("  %s", hint);
//     }

//     ImGui::Spacing();
// }

// // Coloured pill, e.g. for alert severity
// static void SeverityBadge(const char* severity)
// {
//     const ImVec4 color = SeverityColor(severity);
//     const ImVec2 textSize = ImGui::CalcTextSize(severity);
//     const ImVec2 pad(10.0f, 2.0f);

//     const ImVec2 pos = ImGui::GetCursorScreenPos();
//     const ImVec2 size(textSize.x + pad.x * 2.0f, textSize.y + pad.y * 2.0f);

//     ImDrawList* draw = ImGui::GetWindowDrawList();

//     draw->AddRectFilled(
//         pos,
//         ImVec2(pos.x + size.x, pos.y + size.y),
//         ImGui::GetColorU32(WithAlpha(color, 0.16f)),
//         size.y * 0.5f
//     );

//     draw->AddRect(
//         pos,
//         ImVec2(pos.x + size.x, pos.y + size.y),
//         ImGui::GetColorU32(WithAlpha(color, 0.65f)),
//         size.y * 0.5f
//     );

//     draw->AddText(
//         ImVec2(pos.x + pad.x, pos.y + pad.y),
//         ImGui::GetColorU32(color),
//         severity
//     );

//     ImGui::Dummy(size);
// }

// // Pulsing dot used by the monitoring status
// static void StatusDot(const ImVec4& color, bool pulse)
// {
//     const float radius = 5.0f;
//     const ImVec2 pos = ImGui::GetCursorScreenPos();
//     const ImVec2 center(pos.x + radius + 2.0f, pos.y + ImGui::GetFrameHeight() * 0.5f);

//     ImDrawList* draw = ImGui::GetWindowDrawList();

//     if (pulse)
//     {
//         const float t = (float)ImGui::GetTime();
//         const float wave = 0.5f + 0.5f * std::sin(t * 3.0f);

//         draw->AddCircleFilled(
//             center,
//             radius + 3.0f + wave * 3.0f,
//             ImGui::GetColorU32(WithAlpha(color, 0.18f * (1.0f - wave) + 0.05f))
//         );
//     }

//     draw->AddCircleFilled(center, radius, ImGui::GetColorU32(color));

//     ImGui::Dummy(ImVec2(radius * 2.0f + 6.0f, ImGui::GetFrameHeight()));
// }

// static std::string FormatDuration(double seconds)
// {
//     const unsigned long long total =
//         seconds > 0.0 ? (unsigned long long)seconds : 0ULL;
//     char text[32];

//     std::snprintf(
//         text, sizeof(text), "%02llu:%02llu:%02llu",
//         total / 3600ULL, (total / 60ULL) % 60ULL, total % 60ULL
//     );
//     return text;
// }

// static float SeverityWeight(const std::string& s)
// {
//     if (s == "Critical") return 4.0f;
//     if (s == "High")     return 3.0f;
//     if (s == "Medium")   return 2.0f;
//     return 1.0f;
// }

// // Sample rows. Always labelled so they can't be mistaken for real ones.
// static void AppendDemoData()
// {
//     struct DemoAlert { const char* sev; const char* text; };

//     static const DemoAlert demoAlerts[] = {
//         { "Critical", "[DEMO] Known-bad hash match - C:\\Demo\\sample.exe" },
//         { "High",     "[DEMO] Multiple Failed Logins - 5 attempts in 60 s" },
//         { "Medium",   "[DEMO] Suspicious PowerShell Process - powershell.exe" },
//         { "Low",      "[DEMO] Password Changed - sample entry" }
//     };

//     int id = -1;

//     for (const DemoAlert& d : demoAlerts)
//     {
//         AlertInfo a;
//         a.id          = id--;
//         a.time        = "--:--:--";
//         a.severity    = d.sev;
//         a.source      = "DEMO";
//         a.description = d.text;
//         alerts.push_back(a);
//     }

//     ProcessInfo p;
//     p.pid            = -1;
//     p.name           = "demo_service.exe [DEMO]";
//     p.cpu            = 12.5f;
//     p.memory         = 640.0f;
//     p.runningSeconds = 4000.0;
//     p.runningTime    = FormatDuration(p.runningSeconds);
//     p.isApp          = false;
//     processes.push_back(p);

//     loginEvents.push_back({ "--:--:--", "LOGIN_FAILURE",
//         "[DEMO] Failed sign-in for user 'demo'" });
//     fileEvents.push_back({ "--:--:--", "FILE_CREATED",
//         "[DEMO] HIDS_Monitor\\sample_script.ps1" });
// }

// static void RefreshFromBackend()
// {
//     static double lastRefresh = -1.0;
//     static unsigned long lastSample = 0;

//     const double now = ImGui::GetTime();

//     if (lastRefresh >= 0.0 && now - lastRefresh < 0.5)
//         return;

//     lastRefresh = now;

//     // CPU, RAM and process count
//     const BackendStats stats = backend.getStats();

//     cpuNow = stats.cpuPercent;
//     ramNow = stats.ramPercent;
//     processCount = stats.processCount;

//     if (stats.sample != lastSample)
//     {
//         lastSample = stats.sample;

//         cpuHistory[historyOffset]    = cpuNow;
//         ramHistory[historyOffset]    = ramNow;
//         threatHistory[historyOffset] = threatAccum;
//         threatAccum = 0.0f;
//         historyOffset = (historyOffset + 1) % HISTORY_SIZE;
//     }

//     analysisData = backend.getAnalysis();
//     processes.clear();

//     for (const BackendProcess& p : backend.getProcesses())
//     {
//         ProcessInfo row;

//         row.pid            = p.pid;
//         row.name           = p.name;
//         row.cpu            = p.cpuPercent;
//         row.memory         = p.memoryMB;
//         row.runningSeconds = p.runningSeconds;
//         row.runningTime    = FormatDuration(p.runningSeconds);
//         row.isApp          = p.isApp;
//         row.title          = p.windowTitle;

//         processes.push_back(row);
//     }
//     alerts.clear();

//     for (const BackendAlert& a : backend.getAlerts())
//     {
//         AlertInfo row;

//         row.time        = a.time;
//         row.severity    = a.severity;
//         row.source      = a.source;
//         row.description = a.description;
//         row.id          = a.id;
//         row.canEndTask  = a.canEndTask;
//         row.culpritPid  = a.culpritPid;
//         row.culpritName = a.culpritName;
//         row.culpritMem  = a.culpritMemoryMB;

//         alerts.push_back(row);
//     }

//     // Weigh alerts that appeared since the last refresh
//     {
//         static std::set<int> seenAlerts;
//         static bool seeded = false;

//         for (const AlertInfo& a : alerts)
//             if (seenAlerts.insert(a.id).second && seeded)
//                 threatAccum += SeverityWeight(a.severity);

//         seeded = true;
//     }

//     // Login and file activity (already newest first)
//     loginEvents.clear();
//     fileEvents.clear();

//     for (const BackendEvent& e : backend.getEvents())
//     {
//         EventInfo row;

//         row.time        = e.time;
//         row.type        = e.type;
//         row.description = e.description;

//         if (e.type.rfind("LOGIN_", 0) == 0)
//             loginEvents.push_back(row);
//         else if (e.type.rfind("FILE_", 0) == 0)
//             fileEvents.push_back(row);
//     }

//     if (demoMode)
//         AppendDemoData();
// }

// // ------------------------------------------------------------
// // METRIC CARD
// // ------------------------------------------------------------

// static void MetricCard(
//     const char* title,
//     const char* value,
//     const char* subtitle,
//     const ImVec4& accent,
//     float width,
//     const float* history = nullptr,
//     int linkPage = -1,
//     float gauge = -1.0f)          // 0..100 draws a ring gauge
// {
//     ImGui::BeginChild(
//         title,
//         ImVec2(width, 150.0f),
//         true,
//         ImGuiWindowFlags_NoScrollbar
//     );

//     // Accent stripe
//     {
//         const ImVec2 pos = ImGui::GetWindowPos();
//         const ImVec2 size = ImGui::GetWindowSize();

//         ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + 4.0f, pos.y + size.y),
//             ImGui::GetColorU32(accent),
//             ImGui::GetStyle().ChildRounding,
//             ImDrawFlags_RoundCornersLeft
//         );
//     }

//     // Soft glow + ring gauge (or page icon) on the right side
//     {
//         const ImVec2 wp = ImGui::GetWindowPos();
//         const ImVec2 ws = ImGui::GetWindowSize();
//         ImDrawList*  d  = ImGui::GetWindowDrawList();
//         const ImVec2 gc(wp.x + ws.x - 50.0f, wp.y + 58.0f);

//         DrawGlow(d, gc, 46.0f, accent);

//         if (gauge >= 0.0f)
//             DrawGauge(d, gc, 24.0f, 6.0f, gauge / 100.0f, accent);
//         else if (linkPage >= 0)
//             DrawPageIcon(d, gc, 14.0f, linkPage,
//                 ImGui::GetColorU32(WithAlpha(accent, 0.85f)));
//     }

//     ImGui::TextDisabled("%s", title);
//     ImGui::PushStyleColor(ImGuiCol_Text, accent);
//     ImGui::SetWindowFontScale(1.55f);
//     ImGui::TextUnformatted(value);
//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::PopStyleColor();

//     if (history != nullptr)
//     {
//         DrawAreaChart("##spark", history, HISTORY_SIZE, historyOffset,
//             100.0f, ImVec2(ImGui::GetContentRegionAvail().x, 32.0f),
//             accent, "%", false);
//     }
//     else
//     {
//         ImGui::Spacing();
//         ImGui::TextDisabled("%s", subtitle);
//     }

//     // Hover highlight + click to open the related page
//     if (linkPage >= 0 && ImGui::IsWindowHovered())
//     {
//         const ImVec2 pos  = ImGui::GetWindowPos();
//         const ImVec2 size = ImGui::GetWindowSize();

//         ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

//         ImGui::GetWindowDrawList()->AddRect(
//             pos, ImVec2(pos.x + size.x, pos.y + size.y),
//             ImGui::GetColorU32(WithAlpha(accent, 0.85f)),
//             ImGui::GetStyle().ChildRounding, 0, 1.5f);

//         ImGui::SetTooltip("Open %s", pages[linkPage]);

//         if (ImGui::IsMouseClicked(0))
//             OpenPage(linkPage);
//     }

//     ImGui::EndChild();
// }

// // Four live cards (used by the dashboard and the home hub)
// static void DrawMetrics()
// {
//     const float spacing = ImGui::GetStyle().ItemSpacing.x;

//     const float cardWidth =
//         (ImGui::GetContentRegionAvail().x - spacing * 3.0f) / 4.0f;

//     char cpuText[16];
//     char ramText[16];
//     char alertText[16];
//     char processText[16];

//     // Displayed values glide toward the live ones instead of jumping
//     static float cpuShown = 0.0f;
//     static float ramShown = 0.0f;

//     const float ease = ClampFloat(ImGui::GetIO().DeltaTime * 6.0f, 0.0f, 1.0f);
//     cpuShown += (cpuNow - cpuShown) * ease;
//     ramShown += (ramNow - ramShown) * ease;

//     std::snprintf(cpuText, sizeof(cpuText), "%.0f%%", cpuShown);
//     std::snprintf(ramText, sizeof(ramText), "%.0f%%", ramShown);
//     std::snprintf(alertText, sizeof(alertText), "%d", (int)alerts.size());
//     std::snprintf(processText, sizeof(processText), "%d", processCount);

//     const ImVec4 alertColor = CountSeverity("Critical") > 0 ? COLOR_RED : COLOR_YELLOW;

//     char alertSubtitle[64];
//     std::snprintf(
//         alertSubtitle, sizeof(alertSubtitle),
//         "%d critical, %d high",
//         CountSeverity("Critical"), CountSeverity("High")
//     );

//     MetricCard("CPU usage", cpuText, "", LoadColor(cpuNow), cardWidth, cpuHistory, 6, cpuShown);
//     ImGui::SameLine();

//     MetricCard("RAM usage", ramText, "", LoadColor(ramNow), cardWidth, ramHistory, 6, ramShown);
//     ImGui::SameLine();

//     MetricCard("Running processes", processText, "On this host", COLOR_BLUE, cardWidth, nullptr, 1);
//     ImGui::SameLine();

//     MetricCard("Total alerts", alertText, alertSubtitle, alertColor, cardWidth, nullptr, 4);
// }

// // ------------------------------------------------------------
// // NAVIGATION DATA
// // ------------------------------------------------------------

// struct HidsFeatureRoute
// {
//     const char* title;
//     const char* description;
//     int page;
// };

// struct HidsDomainRoute
// {
//     const char* title;
//     const char* description;
//     const char* icon;
//     const HidsFeatureRoute* features;
//     int featureCount;
// };

// static const HidsFeatureRoute monitoringFeatures[] =
// {
//     { "Security Dashboard", "Live host health, threat activity and overview", 0 },
//     { "Process Monitoring", "Inspect running processes and resource usage", 1 },
//     { "Login Monitoring", "Review failed sign-in activity", 2 },
//     { "File Monitoring", "Review changes in the monitored folder", 3 }
// };

// static const HidsFeatureRoute threatFeatures[] =
// {
//     { "Alert Center", "Review and filter security alerts", 4 },
//     { "File Scanner", "Run quick, full and custom file scans", 7 }
// };

// static const HidsFeatureRoute analysisFeatures[] =
// {
//     { "System Analysis", "Resource trends, anomalies and process findings", 6 }
// };

// static const HidsFeatureRoute reportFeatures[] =
// {
//     { "Logs & Reports", "Review protected detection signatures and reports", 5 }
// };

// static const HidsFeatureRoute configurationFeatures[] =
// {
//     { "Settings & Notifications", "Themes, interface scale and desktop alerts", 8 }
// };

// static const HidsDomainRoute hidsDomains[] =
// {
//     { "Monitoring", "Observe activity and changes across this host",
//       "MON", monitoringFeatures, IM_ARRAYSIZE(monitoringFeatures) },
//     { "Threat Detection", "Investigate alerts and scan suspicious files",
//       "THR", threatFeatures, IM_ARRAYSIZE(threatFeatures) },
//     { "Analysis", "Understand system behaviour and resource anomalies",
//       "ANA", analysisFeatures, IM_ARRAYSIZE(analysisFeatures) },
//     { "Reports & Logs", "Access security records, signatures and reports",
//       "LOG", reportFeatures, IM_ARRAYSIZE(reportFeatures) },
//     { "Configuration", "Manage appearance, notifications and preferences",
//       "CFG", configurationFeatures, IM_ARRAYSIZE(configurationFeatures) }
// };

// static int DomainOfPage(int page)
// {
//     for (int d = 0; d < IM_ARRAYSIZE(hidsDomains); ++d)
//         for (int f = 0; f < hidsDomains[d].featureCount; ++f)
//             if (hidsDomains[d].features[f].page == page)
//                 return d;

//     return 0;
// }

// // Jump straight to a feature page (from a card, a chart click, ...)
// static void OpenPage(int page)
// {
//     selectedPage   = page;
//     selectedDomain = DomainOfPage(page);
//     appScreen      = SCREEN_PAGE;
// }

// static ImVec4 DomainAccent(int d)
// {
//     switch (d)
//     {
//     case 0:  return COLOR_CYAN;
//     case 1:  return COLOR_ORANGE;
//     case 2:  return COLOR_BLUE;
//     case 3:  return COLOR_GREEN;
//     default: return COLOR_YELLOW;
//     }
// }

// // Which page icon represents each domain
// static const int domainIconPage[5] = { 0, 4, 6, 5, 8 };

// static void StartEngine()
// {
//     backend.start();
//     monitoringActive = backend.isRunning();

//     if (monitoringActive)
//         engineStartedAt = ImGui::GetTime();
// }

// // Stopping protection always returns to the welcome screen
// static void StopEngine()
// {
//     backend.stop();
//     monitoringActive = false;
//     engineStartedAt  = -1.0;
//     appScreen        = SCREEN_WELCOME;
// }

// // ------------------------------------------------------------
// // SHARED UI PIECES
// // ------------------------------------------------------------

// static void PushGreenButton()
// {
//     ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.02f, 0.48f, 0.31f, 1.0f));
//     ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.04f, 0.62f, 0.40f, 1.0f));
//     ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.03f, 0.40f, 0.26f, 1.0f));
// }

// static void PushRedButton()
// {
//     ImGui::PushStyleColor(ImGuiCol_Button,        WithAlpha(COLOR_RED, 0.14f));
//     ImGui::PushStyleColor(ImGuiCol_ButtonHovered, WithAlpha(COLOR_RED, 0.30f));
//     ImGui::PushStyleColor(ImGuiCol_ButtonActive,  WithAlpha(COLOR_RED, 0.45f));
// }

// static void CenteredText(const char* text, float scale, const ImVec4& col)
// {
//     ImGui::SetWindowFontScale(scale);
//     const float w = ImGui::CalcTextSize(text).x;
//     ImGui::SetCursorPosX(std::max(0.0f, (ImGui::GetWindowWidth() - w) * 0.5f));
//     ImGui::TextColored(col, "%s", text);
//     ImGui::SetWindowFontScale(1.0f);
// }

// // Draws the hint at the bottom of the window without ever overlapping content
// static void FooterHint(const char* text, float scale = 0.95f)
// {
//     const float y = ImGui::GetWindowHeight() - 38.0f;

//     if (ImGui::GetCursorPosY() < y)
//         ImGui::SetCursorPosY(y);

//     CenteredText(text, scale, WithAlpha(COLOR_MUTED, 0.7f));
// }

// // Logo tile + product name. Returns true when clicked (-> welcome screen).
// static bool DrawBrand(bool clickable = false)
// {
//     ImDrawList* d = ImGui::GetWindowDrawList();
//     const ImVec2 p = ImGui::GetCursorScreenPos();
//     const float  s = 40.0f;

//     d->AddRectFilled(p, ImVec2(p.x + s, p.y + s),
//         ImGui::GetColorU32(WithAlpha(COLOR_CYAN, 0.16f)), 10.0f);
//     d->AddRect(p, ImVec2(p.x + s, p.y + s),
//         ImGui::GetColorU32(WithAlpha(COLOR_CYAN, 0.65f)), 10.0f, 0, 1.2f);

//     const ImVec2 c(p.x + s * 0.5f, p.y + s * 0.5f);
//     ImVec2 shield[5] = {
//         ImVec2(c.x - 8.0f, c.y - 9.0f), ImVec2(c.x + 8.0f, c.y - 9.0f),
//         ImVec2(c.x + 8.0f, c.y + 1.0f), ImVec2(c.x,         c.y + 10.0f),
//         ImVec2(c.x - 8.0f, c.y + 1.0f)
//     };
//     d->AddConvexPolyFilled(shield, 5, ImGui::GetColorU32(COLOR_CYAN));

//     ImGui::Dummy(ImVec2(s, s));
//     ImGui::SameLine(0.0f, 12.0f);

//     ImGui::BeginGroup();
//     ImGui::SetWindowFontScale(1.25f);
//     ImGui::TextUnformatted("HIDS");
//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::TextDisabled("Security Platform");
//     ImGui::EndGroup();

//     if (clickable)
//     {
//         const ImVec2 mx = ImGui::GetItemRectMax();

//         if (ImGui::IsWindowHovered() &&
//             ImGui::IsMouseHoveringRect(p, ImVec2(mx.x, p.y + s)))
//         {
//             ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
//             ImGui::SetTooltip("Back to the welcome screen");

//             if (ImGui::IsMouseClicked(0))
//                 return true;
//         }
//     }

//     return false;
// }

// // Right-aligned engine status + Start/Stop button
// static void DrawHeaderStatus(float topY)
// {
//     const ImVec4 col = monitoringActive ? COLOR_GREEN : COLOR_YELLOW;
//     const char* label = monitoringActive ? "Protection active" : "Protection stopped";

//     const float textW = ImGui::CalcTextSize(label).x;
//     const float total = 16.0f + 4.0f + textW + 14.0f + 84.0f;
//     const float x = std::max(
//         300.0f,
//         ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x - total - 8.0f);

//     ImGui::SetCursorPos(ImVec2(x, topY));
//     StatusDot(col, monitoringActive);
//     ImGui::SameLine(0.0f, 4.0f);
//     ImGui::AlignTextToFramePadding();
//     ImGui::TextColored(col, "%s", label);
//     ImGui::SameLine(0.0f, 14.0f);

//     if (monitoringActive)
//     {
//         PushRedButton();
//         if (ImGui::Button("Stop", ImVec2(84.0f, 0.0f)))
//             StopEngine();
//         ImGui::PopStyleColor(3);
//     }
//     else
//     {
//         PushGreenButton();
//         if (ImGui::Button("Start", ImVec2(84.0f, 0.0f)))
//             StartEngine();
//         ImGui::PopStyleColor(3);
//     }
// }

// // Header of a feature page: Back button, breadcrumb, icon, title.
// // No sidebar - just this feature and the way back.
// static void DrawPageHeader()
// {
//     ImGui::BeginChild("PageHeader", ImVec2(0.0f, 112.0f), false,
//                       ImGuiWindowFlags_NoScrollbar);

//     const int     domain = DomainOfPage(selectedPage);
//     const ImVec4  accent = DomainAccent(domain);
//     const float   padX   = ImGui::GetCursorPosX();
//     const float   topY   = ImGui::GetCursorPosY();

//     // Row 1: back + breadcrumb
//     if (ImGui::Button("<  Back", ImVec2(100.0f, 34.0f)))
//     {
//         selectedDomain = domain;
//         appScreen      = SCREEN_DOMAIN;
//     }

//     ImGui::SameLine(0.0f, 14.0f);
//     ImGui::SetCursorPosY(topY + 7.0f);

//     if (ImGui::SmallButton("Home"))
//         appScreen = SCREEN_HUB;

//     ImGui::SameLine();
//     ImGui::TextDisabled(">");
//     ImGui::SameLine();

//     if (ImGui::SmallButton(hidsDomains[domain].title))
//     {
//         selectedDomain = domain;
//         appScreen      = SCREEN_DOMAIN;
//     }

//     ImGui::SameLine();
//     ImGui::TextDisabled(">");
//     ImGui::SameLine();
//     ImGui::TextColored(accent, "%s", pages[selectedPage]);

//     DrawHeaderStatus(topY + 1.0f);

//     // Row 2: icon + title + subtitle
//     ImGui::SetCursorPos(ImVec2(padX, topY + 46.0f));

//     {
//         const ImVec2 p  = ImGui::GetCursorScreenPos();
//         const ImVec2 ic(p.x + 24.0f, p.y + 26.0f);
//         ImDrawList*  d  = ImGui::GetWindowDrawList();

//         DrawGlow(d, ic, 44.0f, accent);
//         d->AddCircleFilled(ic, 24.0f, ImGui::GetColorU32(WithAlpha(accent, 0.18f)), 48);
//         d->AddCircle(ic, 24.0f, ImGui::GetColorU32(WithAlpha(accent, 0.75f)), 48, 1.5f);
//         DrawPageIcon(d, ic, 12.0f, selectedPage, ImGui::GetColorU32(accent));
//     }

//     ImGui::SetCursorPos(ImVec2(padX + 62.0f, topY + 44.0f));
//     ImGui::BeginGroup();
//     ImGui::SetWindowFontScale(1.5f);
//     ImGui::TextUnformatted(pages[selectedPage]);
//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::TextDisabled("%s", pageSubtitles[selectedPage]);
//     ImGui::EndGroup();

//     ImGui::EndChild();
// }

// // Call-to-action shown on the dashboard only while protection is off
// static void DrawEngineBanner()
// {
//     if (monitoringActive)
//         return;

//     const float btnW = 176.0f;

//     ImGui::BeginChild("EngineBanner", ImVec2(0.0f, 100.0f), true,
//                       ImGuiWindowFlags_NoScrollbar);

//     {
//         const ImVec2 p  = ImGui::GetWindowPos();
//         const ImVec2 sz = ImGui::GetWindowSize();

//         ImGui::GetWindowDrawList()->AddRectFilled(
//             p, ImVec2(p.x + 4.0f, p.y + sz.y),
//             ImGui::GetColorU32(COLOR_YELLOW),
//             ImGui::GetStyle().ChildRounding, ImDrawFlags_RoundCornersLeft);
//     }

//     const float wrapX = ImGui::GetWindowWidth() - btnW - 60.0f;

//     ImGui::SetCursorPos(ImVec2(20.0f, 14.0f));
//     ImGui::BeginGroup();
//     ImGui::PushTextWrapPos(wrapX);
//     ImGui::SetWindowFontScale(1.2f);
//     ImGui::TextUnformatted("Protection is stopped");
//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::TextDisabled("Start the engine to collect host events, watch processes "
//                         "and the monitored folder, and raise alerts.");
//     ImGui::PopTextWrapPos();
//     ImGui::EndGroup();

//     ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - btnW - 20.0f, 30.0f));

//     PushGreenButton();
//     if (ImGui::Button("Start protection", ImVec2(btnW, 40.0f)))
//         StartEngine();
//     ImGui::PopStyleColor(3);

//     ImGui::EndChild();
//     ImGui::Spacing();
// }

// static void DrawShieldLogo(ImDrawList* d, const ImVec2& c, float s, const ImVec4& col)
// {
//     ImVec2 shield[5] = {
//         ImVec2(c.x - 0.80f * s, c.y - 0.90f * s),
//         ImVec2(c.x + 0.80f * s, c.y - 0.90f * s),
//         ImVec2(c.x + 0.80f * s, c.y + 0.10f * s),
//         ImVec2(c.x,             c.y + 1.00f * s),
//         ImVec2(c.x - 0.80f * s, c.y + 0.10f * s)
//     };
//     d->AddConvexPolyFilled(shield, 5, ImGui::GetColorU32(col));

//     ImVec2 tick[3] = {
//         ImVec2(c.x - 0.36f * s, c.y - 0.05f * s),
//         ImVec2(c.x - 0.08f * s, c.y + 0.25f * s),
//         ImVec2(c.x + 0.42f * s, c.y - 0.38f * s)
//     };
//     d->AddPolyline(tick, 3, IM_COL32(8, 16, 28, 255), 0, s * 0.16f);
// }

// // ------------------------------------------------------------
// // WELCOME SCREEN
// // Reachable at any time. Shows whether HIDS is working.
// // ------------------------------------------------------------
// static void DrawWelcomeScreen()
// {
//     RefreshFromBackend();

//     const double now  = ImGui::GetTime();
//     const ImVec2 disp = ImGui::GetIO().DisplaySize;
//     const float  t    = (float)now;
//     ImDrawList*  bg   = ImGui::GetBackgroundDrawList();
//     const ImVec2 c(disp.x * 0.5f, disp.y * 0.27f);

//     const ImVec4 acc = monitoringActive ? COLOR_GREEN : COLOR_CYAN;

//     if (monitoringActive && engineStartedAt < 0.0)
//         engineStartedAt = now;

//     // Floating particles
//     for (int i = 0; i < 46; ++i)
//     {
//         const float fx = std::fmod(i * 0.6180339f, 1.0f);
//         const float fy = std::fmod(i * 0.3819660f + t * (0.006f + 0.004f * (i % 4)), 1.0f);
//         const float a  = 0.08f + 0.08f * (0.5f + 0.5f * std::sin(t * 0.8f + i));

//         bg->AddCircleFilled(ImVec2(fx * disp.x, fy * disp.y), 1.2f + (i % 3),
//             ImGui::GetColorU32(WithAlpha(acc, a)));
//     }

//     // Soft glow + expanding radar rings
//     DrawGlow(bg, c, 170.0f, acc);

//     for (int k = 0; k < 3; ++k)
//     {
//         const float phase  = std::fmod(t * 0.30f + k / 3.0f, 1.0f);
//         const float radius = 66.0f + phase * 190.0f;

//         bg->AddCircle(c, radius,
//             ImGui::GetColorU32(WithAlpha(acc, 0.28f * (1.0f - phase))), 96, 1.6f);
//     }

//     // Logo badge + rotating arc
//     bg->AddCircleFilled(c, 64.0f, ImGui::GetColorU32(WithAlpha(acc, 0.12f)), 64);
//     bg->AddCircle(c, 64.0f, ImGui::GetColorU32(WithAlpha(acc, 0.55f)), 64, 1.5f);

//     bg->PathClear();
//     bg->PathArcTo(c, 72.0f, t * 1.6f, t * 1.6f + 1.9f, 32);
//     bg->PathStroke(ImGui::GetColorU32(acc), 0, 3.0f);

//     DrawShieldLogo(bg, c, 30.0f, acc);

//     // Text block
//     ImGui::SetCursorPosY(c.y + 100.0f);

//     CenteredText("HIDS", 3.0f, COLOR_TEXT);
//     CenteredText("Host Intrusion Detection System", 1.35f, COLOR_CYAN);
//     ImGui::Dummy(ImVec2(0.0f, 4.0f));

//     if (monitoringActive)
//     {
//         // ---- HIDS is working ----
//         const char* label = "HIDS IS WORKING  -  this host is being monitored";
//         const float tw    = ImGui::CalcTextSize(label).x;
//         const float pillW = tw + 56.0f;
//         const float pillH = 34.0f;

//         ImGui::SetCursorPosX(std::max(0.0f, (ImGui::GetWindowWidth() - pillW) * 0.5f));

//         const ImVec2 p = ImGui::GetCursorScreenPos();
//         ImDrawList*  d = ImGui::GetWindowDrawList();
//         const float  wave = 0.5f + 0.5f * std::sin(t * 3.0f);

//         d->AddRectFilled(p, ImVec2(p.x + pillW, p.y + pillH),
//             ImGui::GetColorU32(WithAlpha(COLOR_GREEN, 0.14f)), pillH * 0.5f);
//         d->AddRect(p, ImVec2(p.x + pillW, p.y + pillH),
//             ImGui::GetColorU32(WithAlpha(COLOR_GREEN, 0.70f)), pillH * 0.5f, 0, 1.4f);
//         d->AddCircleFilled(ImVec2(p.x + 20.0f, p.y + pillH * 0.5f),
//             9.0f + wave * 2.0f,
//             ImGui::GetColorU32(WithAlpha(COLOR_GREEN, 0.20f * (1.0f - wave) + 0.05f)));
//         d->AddCircleFilled(ImVec2(p.x + 20.0f, p.y + pillH * 0.5f), 5.0f,
//             ImGui::GetColorU32(COLOR_GREEN));
//         d->AddText(ImVec2(p.x + 36.0f, p.y + (pillH - ImGui::GetTextLineHeight()) * 0.5f),
//             ImGui::GetColorU32(COLOR_GREEN), label);

//         ImGui::Dummy(ImVec2(pillW, pillH));
//         ImGui::Dummy(ImVec2(0.0f, 4.0f));

//         char line[200];
//         std::snprintf(line, sizeof(line),
//             "CPU %.0f%%      RAM %.0f%%      Processes %d      Alerts %d      Running for %s",
//             cpuNow, ramNow, processCount, (int)alerts.size(),
//             FormatDuration(now - engineStartedAt).c_str());

//         CenteredText(line, 1.0f, WithAlpha(COLOR_CYAN, 0.9f));
//     }
//     else
//     {
//         CenteredText("Watch processes, sign-ins and files. Catch threats early.",
//             1.0f, COLOR_MUTED);
//     }

//     ImGui::Dummy(ImVec2(0.0f, 22.0f));

//     // ---- Buttons ----
//     bool openConsole = false;

//     const ImVec2 mainBtn(monitoringActive ? 250.0f : 280.0f, 58.0f);
//     const ImVec2 stopBtn(210.0f, 58.0f);
//     const float  gapBtn = 16.0f;
//     const float  rowW   = monitoringActive ? mainBtn.x + gapBtn + stopBtn.x : mainBtn.x;

//     ImGui::SetCursorPosX(std::max(0.0f, (ImGui::GetWindowWidth() - rowW) * 0.5f));

//     PushGreenButton();
//     ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 29.0f);
//     ImGui::SetWindowFontScale(1.25f);

//     const bool pressed =
//         ImGui::Button(monitoringActive ? "Open console" : "Start HIDS", mainBtn);

//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::PopStyleVar();
//     ImGui::PopStyleColor(3);

//     // Pulsing halo around the main button
//     {
//         const ImVec2 a = ImGui::GetItemRectMin();
//         const ImVec2 b = ImGui::GetItemRectMax();
//         const float  w = 0.5f + 0.5f * std::sin(t * 2.5f);

//         ImGui::GetWindowDrawList()->AddRect(
//             ImVec2(a.x - 3.0f - w * 4.0f, a.y - 3.0f - w * 4.0f),
//             ImVec2(b.x + 3.0f + w * 4.0f, b.y + 3.0f + w * 4.0f),
//             ImGui::GetColorU32(WithAlpha(COLOR_GREEN, 0.40f * (1.0f - w) + 0.05f)),
//             34.0f, 0, 2.0f);

//         if (ImGui::IsItemHovered())
//             ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
//     }

//     if (pressed)
//     {
//         if (!monitoringActive)
//             StartEngine();

//         openConsole = true;
//     }

//     if (monitoringActive)
//     {
//         ImGui::SameLine(0.0f, gapBtn);

//         PushRedButton();
//         ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 29.0f);
//         ImGui::SetWindowFontScale(1.25f);

//         if (ImGui::Button("Stop protection", stopBtn))
//             StopEngine();            // stays on the welcome screen

//         ImGui::SetWindowFontScale(1.0f);
//         ImGui::PopStyleVar();
//         ImGui::PopStyleColor(3);
//     }

//     ImGui::Dummy(ImVec2(0.0f, 6.0f));

//     // Skip link (only while protection is off)
//     if (!monitoringActive)
//     {
//         const char* skip = "Open dashboard without starting protection";
//         const float w = ImGui::CalcTextSize(skip).x;

//         ImGui::SetCursorPosX(std::max(0.0f, (ImGui::GetWindowWidth() - w) * 0.5f));
//         ImGui::TextDisabled("%s", skip);

//         if (ImGui::IsItemHovered())
//             ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

//         if (ImGui::IsItemClicked())
//             openConsole = true;
//     }

//     if (openConsole)
//         appScreen = SCREEN_HUB;

//     // Footer
//     {
//         const float y = ImGui::GetWindowHeight() - 56.0f;

//         if (ImGui::GetCursorPosY() < y)
//             ImGui::SetCursorPosY(y);

//         CenteredText("Monitoring   -   Threat Detection   -   Analysis   -   Reports   -   Configuration",
//             1.0f, WithAlpha(COLOR_MUTED, 0.8f));
//         CenteredText("HIDS Prototype v2.0", 0.9f, WithAlpha(COLOR_MUTED, 0.6f));
//     }
// }

// // ------------------------------------------------------------
// // Shared card used by the hub and the domain screens.
// // ------------------------------------------------------------
// static bool HubCard(
//     const char* id, const char* title, const char* desc, const char* footer,
//     int iconPage, const ImVec4& accent, const ImVec2& size)
// {
//     bool clicked = false;

//     ImGui::PushID(id);
//     ImGui::BeginChild("card", size, true, ImGuiWindowFlags_NoScrollbar);

//     const ImVec2 wp = ImGui::GetWindowPos();
//     const ImVec2 ws = ImGui::GetWindowSize();
//     ImDrawList*  d  = ImGui::GetWindowDrawList();
//     const bool   hov = ImGui::IsWindowHovered();
//     const float  rounding = ImGui::GetStyle().ChildRounding;

//     // Soft tint fading down from the top
//     d->AddRectFilledMultiColor(
//         ImVec2(wp.x + 1.0f, wp.y + 4.0f),
//         ImVec2(wp.x + ws.x - 1.0f, wp.y + ws.y * 0.60f),
//         ImGui::GetColorU32(WithAlpha(accent, hov ? 0.20f : 0.11f)),
//         ImGui::GetColorU32(WithAlpha(accent, hov ? 0.20f : 0.11f)),
//         ImGui::GetColorU32(WithAlpha(accent, 0.0f)),
//         ImGui::GetColorU32(WithAlpha(accent, 0.0f)));

//     // Decorative rings in the top-right corner
//     for (int k = 0; k < 3; ++k)
//         d->AddCircle(ImVec2(wp.x + ws.x - 26.0f, wp.y + 30.0f),
//             34.0f + k * 22.0f,
//             ImGui::GetColorU32(WithAlpha(accent, 0.10f - k * 0.025f)), 64, 1.2f);

//     // Accent bar across the top
//     d->AddRectFilled(wp, ImVec2(wp.x + ws.x, wp.y + 4.0f),
//         ImGui::GetColorU32(accent), rounding, ImDrawFlags_RoundCornersTop);

//     // Icon badge
//     const ImVec2 ic(wp.x + 22.0f + 32.0f, wp.y + 26.0f + 32.0f);

//     DrawGlow(d, ic, hov ? 82.0f : 62.0f, accent);
//     d->AddCircleFilled(ic, 32.0f,
//         ImGui::GetColorU32(WithAlpha(accent, hov ? 0.32f : 0.16f)), 48);
//     d->AddCircle(ic, 32.0f,
//         ImGui::GetColorU32(WithAlpha(accent, 0.75f)), 48, 1.6f);
//     DrawPageIcon(d, ic, 15.0f, iconPage, ImGui::GetColorU32(accent));

//     // Text (wrapped inside the card so it never overlaps the footer)
//     ImGui::SetCursorPos(ImVec2(18.0f, 26.0f + 64.0f + 14.0f));
//     ImGui::BeginGroup();
//     ImGui::PushTextWrapPos(ws.x - 18.0f);

//     ImGui::SetWindowFontScale(1.25f);
//     ImGui::TextUnformatted(title);
//     ImGui::SetWindowFontScale(1.0f);

//     ImGui::PushStyleColor(ImGuiCol_Text, COLOR_MUTED);
//     ImGui::TextUnformatted(desc);
//     ImGui::PopStyleColor();

//     ImGui::PopTextWrapPos();
//     ImGui::EndGroup();

//     // Footer: label on the left, arrow badge on the right
//     ImGui::SetCursorPos(ImVec2(18.0f, ws.y - 38.0f));
//     ImGui::TextColored(accent, "%s", footer);

//     {
//         const ImVec2 ac(wp.x + ws.x - 30.0f, wp.y + ws.y - 28.0f);

//         d->AddCircleFilled(ac, 13.0f,
//             ImGui::GetColorU32(WithAlpha(accent, hov ? 0.40f : 0.18f)), 32);
//         d->AddCircle(ac, 13.0f, ImGui::GetColorU32(WithAlpha(accent, 0.70f)), 32, 1.2f);

//         const float nx = hov ? 1.5f : 0.0f;
//         ImVec2 arrow[3] = {
//             ImVec2(ac.x - 2.5f + nx, ac.y - 5.0f),
//             ImVec2(ac.x + 3.0f + nx, ac.y),
//             ImVec2(ac.x - 2.5f + nx, ac.y + 5.0f)
//         };
//         d->AddPolyline(arrow, 3, ImGui::GetColorU32(accent), 0, 2.0f);
//     }

//     // Hover outline + click
//     if (hov)
//     {
//         d->AddRect(wp, ImVec2(wp.x + ws.x, wp.y + ws.y),
//             ImGui::GetColorU32(WithAlpha(accent, 0.90f)), rounding, 0, 1.8f);

//         ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

//         if (ImGui::IsMouseClicked(0))
//             clicked = true;
//     }

//     ImGui::EndChild();
//     ImGui::PopID();

//     return clicked;
// }

// // ------------------------------------------------------------
// // HOME HUB - one big card per security domain + live metrics
// // ------------------------------------------------------------
// static void DrawHubScreen()
// {
//     RefreshFromBackend();

//     const float topY = ImGui::GetCursorPosY();

//     if (DrawBrand(true))
//         appScreen = SCREEN_WELCOME;

//     DrawHeaderStatus(topY + 4.0f);

//     ImGui::SetCursorPosY(topY + 52.0f);
//     ImGui::Separator();
//     ImGui::Dummy(ImVec2(0.0f, 14.0f));

//     CenteredText("Security Console", 2.0f, COLOR_TEXT);
//     CenteredText("Choose an area to explore", 1.1f, COLOR_MUTED);
//     ImGui::Dummy(ImVec2(0.0f, 18.0f));

//     // Card grid
//     const int   count = IM_ARRAYSIZE(hidsDomains);
//     const float gap   = 16.0f;
//     const float avail = ImGui::GetContentRegionAvail().x;
//     const int   cols  = (avail >= 1100.0f) ? 5 : 3;
//     const float cardW = (avail - gap * (cols - 1)) / cols;
//     const float cardH = (cols == 5) ? 290.0f : 215.0f;

//     ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(gap, gap));

//     for (int i = 0; i < count; ++i)
//     {
//         if (i % cols != 0)
//             ImGui::SameLine();

//         const HidsDomainRoute& dom = hidsDomains[i];

//         char footer[48];
//         std::snprintf(footer, sizeof(footer), "%d feature%s",
//             dom.featureCount, dom.featureCount == 1 ? "" : "s");

//         if (HubCard(dom.title, dom.title, dom.description, footer,
//                 domainIconPage[i], DomainAccent(i), ImVec2(cardW, cardH)))
//         {
//             selectedDomain = i;
//             appScreen      = SCREEN_DOMAIN;
//         }
//     }

//     ImGui::PopStyleVar();

//     ImGui::Dummy(ImVec2(0.0f, 6.0f));

//     // Live metrics strip (only if there is room; otherwise a text summary)
//     if (ImGui::GetContentRegionAvail().y >= 210.0f)
//     {
//         ImGui::TextDisabled("LIVE STATUS");
//         DrawMetrics();
//     }
//     else
//     {
//         char line[160];
//         std::snprintf(line, sizeof(line),
//             "CPU %.0f%%      RAM %.0f%%      Processes %d      Alerts %d",
//             cpuNow, ramNow, processCount, (int)alerts.size());

//         CenteredText(line, 1.0f, WithAlpha(COLOR_CYAN, 0.9f));
//     }

//     FooterHint("Click an area to open it   -   Click the logo for the welcome screen");
// }

// // ------------------------------------------------------------
// // DOMAIN SCREEN - the features inside one domain
// // ------------------------------------------------------------
// static void DrawDomainScreen()
// {
//     RefreshFromBackend();

//     const HidsDomainRoute& dom = hidsDomains[selectedDomain];
//     const ImVec4 accent = DomainAccent(selectedDomain);

//     const float topY = ImGui::GetCursorPosY();

//     if (ImGui::Button("<  Home", ImVec2(100.0f, 34.0f)))
//         appScreen = SCREEN_HUB;

//     ImGui::SameLine(0.0f, 14.0f);
//     ImGui::AlignTextToFramePadding();
//     ImGui::TextDisabled("Home  >");
//     ImGui::SameLine(0.0f, 6.0f);
//     ImGui::TextColored(accent, "%s", dom.title);

//     DrawHeaderStatus(topY + 1.0f);

//     ImGui::SetCursorPosY(topY + 46.0f);
//     ImGui::Separator();
//     ImGui::Dummy(ImVec2(0.0f, 16.0f));

//     // Domain banner
//     {
//         const ImVec2 p = ImGui::GetCursorScreenPos();
//         const ImVec2 ic(p.x + 36.0f, p.y + 36.0f);
//         ImDrawList*  d = ImGui::GetWindowDrawList();

//         DrawGlow(d, ic, 70.0f, accent);
//         d->AddCircleFilled(ic, 34.0f, ImGui::GetColorU32(WithAlpha(accent, 0.18f)), 48);
//         d->AddCircle(ic, 34.0f, ImGui::GetColorU32(WithAlpha(accent, 0.75f)), 48, 1.6f);
//         DrawPageIcon(d, ic, 16.0f, domainIconPage[selectedDomain], ImGui::GetColorU32(accent));

//         ImGui::Dummy(ImVec2(72.0f, 72.0f));
//         ImGui::SameLine(0.0f, 16.0f);

//         ImGui::BeginGroup();
//         ImGui::SetWindowFontScale(1.8f);
//         ImGui::TextUnformatted(dom.title);
//         ImGui::SetWindowFontScale(1.0f);
//         ImGui::TextDisabled("%s", dom.description);
//         ImGui::EndGroup();
//     }

//     ImGui::Dummy(ImVec2(0.0f, 20.0f));

//     // Feature cards
//     const float gap   = 16.0f;
//     const float avail = ImGui::GetContentRegionAvail().x;
//     const int   cols  = 3;
//     const float cardW = (avail - gap * (cols - 1)) / cols;
//     const float cardH = 240.0f;

//     ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(gap, gap));

//     for (int i = 0; i < dom.featureCount; ++i)
//     {
//         if (i % cols != 0)
//             ImGui::SameLine();

//         const HidsFeatureRoute& f = dom.features[i];

//         char footer[48];

//         if (f.page == 4)
//             std::snprintf(footer, sizeof(footer), "%d alert%s", (int)alerts.size(),
//                 alerts.size() == 1 ? "" : "s");
//         else if (f.page == 1)
//             std::snprintf(footer, sizeof(footer), "%d processes", processCount);
//         else
//             std::snprintf(footer, sizeof(footer), "Open");

//         if (HubCard(f.title, f.title, f.description, footer,
//                 f.page, accent, ImVec2(cardW, cardH)))
//         {
//             selectedPage = f.page;
//             appScreen    = SCREEN_PAGE;
//         }
//     }

//     ImGui::PopStyleVar();

//     FooterHint("Click a feature to open it   -   Esc goes back");
// }

// // ------------------------------------------------------------
// // Entry point for every screen that is NOT a feature page
// // ------------------------------------------------------------
// static void DrawFullScreen()
// {
//     const float fade = ClampFloat(
//         (float)((ImGui::GetTime() - screenChangedAt) / 0.25), 0.0f, 1.0f);
//     ImGui::PushStyleVar(ImGuiStyleVar_Alpha, fade);

//     if (appScreen == SCREEN_WELCOME)     DrawWelcomeScreen();
//     else if (appScreen == SCREEN_HUB)    DrawHubScreen();
//     else if (appScreen == SCREEN_DOMAIN) DrawDomainScreen();

//     ImGui::PopStyleVar();
// }

// // ============================================================
// // DASHBOARD WIDGETS
// // ============================================================

// static void DrawThreatPanel()
// {
//     ImGui::BeginChild("ThreatPanel", ImVec2(0.0f, 216.0f), true,
//                       ImGuiWindowFlags_NoScrollbar);

//     SectionHeading("Threat activity",
//         "new alerts per second, weighted by severity (last 60 s)");

//     float peak = 6.0f;
//     for (float v : threatHistory)
//         peak = std::max(peak, v);

//     const float leftW = ImGui::GetContentRegionAvail().x * 0.62f;

//     DrawBarChart("##threat", threatHistory, HISTORY_SIZE, historyOffset,
//         peak, ImVec2(leftW, 126.0f));

//     ImGui::SameLine();
//     ImGui::BeginGroup();

//     static const char* names[] = { "Critical", "High", "Medium", "Low" };
//     const int total = std::max(1, (int)alerts.size());

//     for (int i = 0; i < 4; ++i)
//     {
//         const int count = CountSeverity(names[i]);

//         char label[40];
//         std::snprintf(label, sizeof(label), "%s  %d", names[i], count);

//         ImGui::PushID(i);
//         ImGui::PushStyleColor(ImGuiCol_PlotHistogram,
//             WithAlpha(SeverityColor(names[i]), 0.85f));
//         ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_FRAME);

//         ImGui::ProgressBar((float)count / (float)total,
//             ImVec2(ImGui::GetContentRegionAvail().x, 22.0f), label);

//         ImGui::PopStyleColor(2);

//         if (ImGui::IsItemHovered())
//         {
//             ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
//             ImGui::SetTooltip("Show %s alerts", names[i]);
//         }

//         if (ImGui::IsItemClicked())
//         {
//             severityFilter = i + 1;
//             OpenPage(4);               // Alerts
//         }

//         ImGui::PopID();
//     }

//     ImGui::EndGroup();
//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // PASSWORD LOCK (Logs & Reports page and End Task)
// // ------------------------------------------------------------

// static const double LOCK_IDLE_SECONDS = 300.0;     // auto-lock after 5 min idle
// static const int    LOGS_PAGE_INDEX   = 5;

// static bool        logsUnlocked   = false;
// static double      logsLastActive = 0.0;
// static char        lockOld[64]     = "";
// static char        lockNew[64]     = "";
// static char        lockConfirm[64] = "";
// static char        lockEntry[64]   = "";
// static std::string lockMessage;
// static bool        lockMessageOk   = false;

// static void Wipe(char* buffer, std::size_t size)
// {
//     std::memset(buffer, 0, size);
// }

// // Locks Logs & Reports again and clears typed credentials
// static void LockLogs()
// {
//     logsUnlocked = false;
//     lockMessage.clear();
//     Wipe(lockOld, sizeof(lockOld));
//     Wipe(lockNew, sizeof(lockNew));
//     Wipe(lockConfirm, sizeof(lockConfirm));
//     Wipe(lockEntry, sizeof(lockEntry));
// }

// // Detects screen / page changes: drives the fade and re-locks Logs & Reports
// static void TrackScreenChange()
// {
//     static int prevScreen = -1;
//     static int prevPage   = -1;

//     const bool changed =
//         appScreen != prevScreen ||
//         (appScreen == SCREEN_PAGE && selectedPage != prevPage);

//     if (!changed)
//         return;

//     if (prevScreen == SCREEN_PAGE && prevPage == LOGS_PAGE_INDEX)
//         LockLogs();

//     prevScreen      = appScreen;
//     prevPage        = selectedPage;
//     screenChangedAt = ImGui::GetTime();
// }

// // Esc: page -> domain -> hub
// static void HandleBackKey()
// {
//     if (!ImGui::IsKeyPressed(ImGuiKey_Escape, false))
//         return;

//     if (ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId) ||
//         ImGui::IsAnyItemActive())
//         return;

//     if (appScreen == SCREEN_PAGE)
//     {
//         selectedDomain = DomainOfPage(selectedPage);
//         appScreen      = SCREEN_DOMAIN;
//     }
//     else if (appScreen == SCREEN_DOMAIN)
//     {
//         appScreen = SCREEN_HUB;
//     }
// }

// // "Create a password" form. Returns true on the frame it is created.
// static bool DrawSetPasswordForm()
// {
//     bool created = false;

//     ImGui::TextDisabled("Create a password (at least 6 characters)");

//     ImGui::SetNextItemWidth(260.0f);
//     ImGui::InputTextWithHint("##newpw", "New password",
//         lockNew, sizeof(lockNew), ImGuiInputTextFlags_Password);

//     ImGui::SetNextItemWidth(260.0f);
//     ImGui::InputTextWithHint("##confpw", "Confirm password",
//         lockConfirm, sizeof(lockConfirm), ImGuiInputTextFlags_Password);

//     if (ImGui::Button("Set Password"))
//     {
//         if (std::strcmp(lockNew, lockConfirm) != 0)
//         {
//             lockMessage   = "Passwords do not match.";
//             lockMessageOk = false;
//         }
//         else
//         {
//             std::string message;

//             if (backend.setPassword(lockNew, "", message))
//             {
//                 created = true;
//                 Wipe(lockNew, sizeof(lockNew));
//                 Wipe(lockConfirm, sizeof(lockConfirm));
//             }

//             lockMessage   = message;
//             lockMessageOk = created;
//         }
//     }

//     return created;
// }

// static void DrawLockScreen()
// {
//     ImGui::BeginChild("LockScreen", ImVec2(0.0f, 0.0f), true);

//     SectionHeading("Logs & Reports are locked",
//         "enter the password to see the detection signatures");

//     ImGui::Spacing();

//     if (!backend.isLockConfigured())
//     {
//         if (DrawSetPasswordForm())
//             logsUnlocked = true;          // whoever creates it is let in
//     }
//     else
//     {
//         const int wait = backend.lockoutSecondsLeft();

//         ImGui::BeginDisabled(wait > 0);

//         ImGui::SetNextItemWidth(260.0f);
//         const bool enter = ImGui::InputTextWithHint("##unlockpw", "Password",
//             lockEntry, sizeof(lockEntry),
//             ImGuiInputTextFlags_Password | ImGuiInputTextFlags_EnterReturnsTrue);

//         ImGui::SameLine();
//         const bool click = ImGui::Button("Unlock");

//         ImGui::EndDisabled();

//         if (wait > 0)
//             ImGui::TextColored(COLOR_YELLOW, "Locked. Try again in %d s.", wait);

//         if ((enter || click) && wait == 0)
//         {
//             std::string message;

//             if (backend.verifyPassword(lockEntry, message))
//             {
//                 logsUnlocked = true;
//                 lockMessage.clear();
//             }
//             else
//             {
//                 lockMessage   = message;
//                 lockMessageOk = false;
//             }

//             Wipe(lockEntry, sizeof(lockEntry));
//         }
//     }

//     if (!lockMessage.empty())
//         ImGui::TextColored(lockMessageOk ? COLOR_GREEN : COLOR_RED,
//             "%s", lockMessage.c_str());

//     ImGui::EndChild();
// }

// // true = unlocked, the page may draw its content
// static bool LogsGate()
// {
//     const double now = ImGui::GetTime();
//     ImGuiIO& io = ImGui::GetIO();

//     const bool active =
//         io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f ||
//         io.MouseWheel != 0.0f || ImGui::IsMouseClicked(0) ||
//         ImGui::IsAnyItemActive();

//     if (active || !logsUnlocked)
//         logsLastActive = now;

//     if (logsUnlocked && now - logsLastActive > LOCK_IDLE_SECONDS)
//     {
//         logsUnlocked  = false;
//         lockMessage   = "Locked after 5 minutes of inactivity.";
//         lockMessageOk = false;
//     }

//     if (logsUnlocked)
//         return true;

//     DrawLockScreen();
//     return false;
// }

// // ------------------------------------------------------------
// // END TASK (shared by the Processes page and resource alerts)
// // ------------------------------------------------------------

// static const float HIGH_MEMORY_MB = 500.0f;

// static bool        procAppsOnly     = false;
// static bool        procHighMemOnly  = false;
// static std::string procMessage;                 // result of the last End Task
// static bool        procMessageOk    = true;
// static int         pendingPid       = 0;
// static std::string pendingName;
// static float       pendingMem       = 0.0f;
// static int         pendingAlertID   = 0;        // 0 = started from the Processes page
// static bool        openEndTaskPopup = false;
// static std::set<int> handledAlertIDs;           // alerts whose app was ended

// static char        endPassword[64] = "";
// static std::string endPopupMessage;

// static void DrawEndTaskPopup()
// {
//     if (openEndTaskPopup)
//     {
//         ImGui::OpenPopup("Confirm End Task");
//         openEndTaskPopup = false;

//         Wipe(endPassword, sizeof(endPassword));
//         endPopupMessage.clear();
//         lockMessage.clear();
//     }

//     const ImVec2 screen = ImGui::GetIO().DisplaySize;
//     ImGui::SetNextWindowPos(
//         ImVec2(screen.x * 0.5f, screen.y * 0.5f),
//         ImGuiCond_Appearing,
//         ImVec2(0.5f, 0.5f));

//     if (ImGui::BeginPopupModal("Confirm End Task", nullptr,
//             ImGuiWindowFlags_AlwaysAutoResize))
//     {
//         ImGui::Text("End %s (PID %d)?", pendingName.c_str(), pendingPid);
//         ImGui::TextDisabled("It is using %.0f MB. Unsaved work in this app will be lost.",
//             pendingMem);
//         ImGui::Spacing();

//         if (!backend.isLockConfigured())
//         {
//             ImGui::TextColored(COLOR_YELLOW,
//                 "Ending tasks is password protected. Create a password first.");
//             ImGui::Spacing();

//             DrawSetPasswordForm();

//             if (!lockMessage.empty())
//                 ImGui::TextColored(lockMessageOk ? COLOR_GREEN : COLOR_RED, "%s", lockMessage.c_str());
//         }
//         else
//         {
//             const int wait = backend.lockoutSecondsLeft();

//             ImGui::BeginDisabled(wait > 0);

//             ImGui::SetNextItemWidth(260.0f);
//             const bool enter = ImGui::InputTextWithHint("##endpw", "Password",
//                 endPassword, sizeof(endPassword),
//                 ImGuiInputTextFlags_Password | ImGuiInputTextFlags_EnterReturnsTrue);

//             ImGui::EndDisabled();

//             if (wait > 0)
//                 ImGui::TextColored(COLOR_YELLOW, "Locked. Try again in %d s.", wait);

//             if (!endPopupMessage.empty())
//                 ImGui::TextColored(COLOR_RED, "%s", endPopupMessage.c_str());

//             ImGui::Spacing();

//             ImGui::BeginDisabled(wait > 0);
//             const bool click = ImGui::Button("End Task", ImVec2(120.0f, 0.0f));
//             ImGui::EndDisabled();

//             if ((enter || click) && wait == 0)
//             {
//                 std::string authMessage;
//                 const bool authorised = backend.verifyPassword(endPassword, authMessage);

//                 Wipe(endPassword, sizeof(endPassword));

//                 if (!authorised)
//                 {
//                     endPopupMessage = authMessage;
//                 }
//                 else
//                 {
//                     std::string result;

//                     procMessageOk = backend.endTask(pendingPid, result, pendingName);
//                     procMessage   = result;

//                     if (procMessageOk && pendingAlertID != 0)
//                         handledAlertIDs.insert(pendingAlertID);

//                     ImGui::CloseCurrentPopup();
//                 }
//             }

//             ImGui::SameLine();
//         }

//         if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
//         {
//             Wipe(endPassword, sizeof(endPassword));
//             Wipe(lockNew, sizeof(lockNew));
//             Wipe(lockConfirm, sizeof(lockConfirm));
//             ImGui::CloseCurrentPopup();
//         }

//         ImGui::EndPopup();
//     }
// }

// // ------------------------------------------------------------
// // ALERT TABLE
// // ------------------------------------------------------------

// static void DrawAlerts(bool compact)
// {
//     ImGui::BeginChild(
//         "AlertsPanel",
//         ImVec2(0.0f, compact ? 290.0f : 0.0f),
//         true
//     );

//     SectionHeading(
//         compact ? "Recent security alerts" : "All security alerts"
//     );

//     // Severity filter + search (full page only)
//     if (!compact)
//     {
//         for (int i = 0; i < IM_ARRAYSIZE(severityFilterLabels); ++i)
//         {
//             const bool active = (severityFilter == i);

//             if (active)
//                 ImGui::PushStyleColor(ImGuiCol_Button, COLOR_BLUE);
//             else
//                 ImGui::PushStyleColor(ImGuiCol_Button, COLOR_FRAME);

//             if (ImGui::Button(severityFilterLabels[i], ImVec2(96.0f, 0.0f)))
//                 severityFilter = i;

//             ImGui::PopStyleColor();

//             if (i + 1 < IM_ARRAYSIZE(severityFilterLabels))
//                 ImGui::SameLine();
//         }

//         ImGui::SameLine();
//         ImGui::SetNextItemWidth(260.0f);
//         ImGui::InputTextWithHint("##alertsearch", "Search alerts",
//             alertSearch, IM_ARRAYSIZE(alertSearch));

//         ImGui::Spacing();
//     }

//     if (!procMessage.empty())
//         ImGui::TextColored(procMessageOk ? COLOR_GREEN : COLOR_RED,
//             "%s", procMessage.c_str());

//     DrawEndTaskPopup();

//     if (ImGui::BeginTable(
//             "AlertsTable",
//             5,
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);
//         ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 110.0f);
//         ImGui::TableSetupColumn("Severity", ImGuiTableColumnFlags_WidthFixed, 130.0f);
//         ImGui::TableSetupColumn("Source", ImGuiTableColumnFlags_WidthFixed, 160.0f);
//         ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthStretch);
//         ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 100.0f);
//         ImGui::TableHeadersRow();

//         int shown = 0;

//         for (const auto& alert : alerts)
//         {
//             if (!compact && severityFilter != 0 &&
//                 alert.severity != severityFilterLabels[severityFilter])
//             {
//                 continue;
//             }

//             if (!compact &&
//                 !ContainsNoCase(alert.description.c_str(), alertSearch) &&
//                 !ContainsNoCase(alert.source.c_str(), alertSearch))
//             {
//                 continue;
//             }

//             ++shown;

//             ImGui::TableNextRow();

//             ImGui::TableSetColumnIndex(0);
//             {
//                 const ImVec2 p = ImGui::GetCursorScreenPos();

//                 ImGui::GetWindowDrawList()->AddCircleFilled(
//                     ImVec2(p.x + 4.0f, p.y + ImGui::GetTextLineHeight() * 0.5f),
//                     4.0f,
//                     ImGui::GetColorU32(SeverityColor(alert.severity.c_str())));

//                 ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 16.0f);
//             }
//             ImGui::TextDisabled("%s", alert.time.c_str());

//             ImGui::TableSetColumnIndex(1);
//             SeverityBadge(alert.severity.c_str());

//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextUnformatted(alert.source.c_str());

//             ImGui::TableSetColumnIndex(3);
//             ImGui::TextWrapped("%s", alert.description.c_str());

//             // Resource alerts can offer End Task for the app causing them
//             ImGui::TableSetColumnIndex(4);

//             if (alert.canEndTask)
//             {
//                 if (handledAlertIDs.count(alert.id))
//                 {
//                     ImGui::TextColored(COLOR_GREEN, "Ended");
//                 }
//                 else
//                 {
//                     ImGui::PushID(alert.id);

//                     ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.55f, 0.12f, 0.15f, 1.0f));
//                     ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.18f, 0.22f, 1.0f));
//                     ImGui::PushStyleColor(ImGuiCol_ButtonActive,  COLOR_RED);

//                     if (ImGui::SmallButton("End Task"))
//                     {
//                         pendingAlertID   = alert.id;
//                         pendingPid       = alert.culpritPid;
//                         pendingName      = alert.culpritName;
//                         pendingMem       = alert.culpritMem;
//                         openEndTaskPopup = true;
//                     }

//                     ImGui::PopStyleColor(3);
//                     ImGui::PopID();
//                 }
//             }
//             else
//             {
//                 ImGui::TextDisabled("-");
//             }
//         }

//         if (shown == 0)
//         {
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(3);
//             ImGui::TextDisabled("No alerts match the current filters.");
//         }

//         ImGui::EndTable();
//     }

//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // PROCESS TABLE
// // ------------------------------------------------------------

// static void DrawProcesses()
// {
//     ImGui::BeginChild(
//         "ProcessesPanel",
//         ImVec2(0.0f, 0.0f),
//         true
//     );

//     SectionHeading("Running processes",
//         "apps can be ended here; system and background processes are protected");

//     ImGui::SetNextItemWidth(300.0f);

//     ImGui::InputTextWithHint(
//         "##ProcessSearch",
//         "Search by name or PID",
//         processSearch,
//         IM_ARRAYSIZE(processSearch)
//     );

//     ImGui::SameLine();
//     ImGui::Checkbox("Apps only", &procAppsOnly);

//     ImGui::SameLine();

//     char highLabel[64];
//     std::snprintf(highLabel, sizeof(highLabel),
//         "High memory only (%.0f MB+)", HIGH_MEMORY_MB);
//     ImGui::Checkbox(highLabel, &procHighMemOnly);

//     if (!procMessage.empty())
//         ImGui::TextColored(procMessageOk ? COLOR_GREEN : COLOR_RED,
//             "%s", procMessage.c_str());

//     ImGui::Spacing();

//     if (ImGui::BeginTable(
//             "ProcessTable",
//             6,
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_Sortable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);

//         ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed, 85.0f, 0);
//         ImGui::TableSetupColumn("Process name", ImGuiTableColumnFlags_WidthStretch, 0.0f, 1);
//         ImGui::TableSetupColumn(
//             "CPU %",
//             ImGuiTableColumnFlags_WidthFixed |
//             ImGuiTableColumnFlags_PreferSortDescending,
//             180.0f, 2);
//         ImGui::TableSetupColumn(
//             "Memory MB",
//             ImGuiTableColumnFlags_WidthFixed |
//             ImGuiTableColumnFlags_DefaultSort |
//             ImGuiTableColumnFlags_PreferSortDescending,
//             125.0f, 3);
//         ImGui::TableSetupColumn("Running time", ImGuiTableColumnFlags_WidthFixed, 135.0f, 4);
//         ImGui::TableSetupColumn("Action",
//             ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoSort,
//             100.0f, 5);

//         ImGui::TableHeadersRow();

//         std::vector<int> order(processes.size());

//         for (size_t i = 0; i < order.size(); ++i)
//             order[i] = (int)i;

//         if (ImGuiTableSortSpecs* specs = ImGui::TableGetSortSpecs())
//         {
//             if (specs->SpecsCount > 0)
//             {
//                 const ImGuiTableColumnSortSpecs& spec = specs->Specs[0];
//                 const bool asc =
//                     spec.SortDirection == ImGuiSortDirection_Ascending;

//                 std::sort(order.begin(), order.end(),
//                     [&](int a, int b)
//                     {
//                         const ProcessInfo& pa = processes[a];
//                         const ProcessInfo& pb = processes[b];

//                         int cmp = 0;

//                         switch (spec.ColumnUserID)
//                         {
//                         case 0: cmp = (pa.pid > pb.pid) - (pa.pid < pb.pid); break;
//                         case 1: cmp = pa.name.compare(pb.name); break;
//                         case 2: cmp = (pa.cpu > pb.cpu) - (pa.cpu < pb.cpu); break;
//                         case 3: cmp = (pa.memory > pb.memory) - (pa.memory < pb.memory); break;
//                         case 4: cmp = (pa.runningSeconds > pb.runningSeconds) - (pa.runningSeconds < pb.runningSeconds); break;
//                         }

//                         return asc ? (cmp < 0) : (cmp > 0);
//                     });
//             }
//         }

//         int shown = 0;

//         for (int index : order)
//         {
//             const ProcessInfo& process = processes[index];

//             if (procAppsOnly && !process.isApp)
//                 continue;

//             if (procHighMemOnly && process.memory < HIGH_MEMORY_MB)
//                 continue;

//             char pidText[32];
//             std::snprintf(pidText, sizeof(pidText), "%d", process.pid);

//             const bool matches =
//                 ContainsNoCase(process.name.c_str(), processSearch) ||
//                 ContainsNoCase(pidText, processSearch);

//             if (!matches)
//                 continue;

//             ++shown;

//             ImGui::TableNextRow();

//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextDisabled("%d", process.pid);

//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextUnformatted(process.name.c_str());

//             if (process.isApp && !process.title.empty())
//             {
//                 ImGui::SameLine();
//                 ImGui::TextDisabled("- %s", process.title.c_str());
//             }

//             // CPU bar
//             ImGui::TableSetColumnIndex(2);

//             char cpuLabel[16];
//             std::snprintf(cpuLabel, sizeof(cpuLabel), "%.1f", process.cpu);

//             ImGui::PushStyleColor(ImGuiCol_PlotHistogram, WithAlpha(LoadColor(process.cpu * 4.0f), 0.85f));
//             ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_FRAME);

//             ImGui::ProgressBar(
//                 std::min(process.cpu / 25.0f, 1.0f),
//                 ImVec2(-FLT_MIN, 18.0f),
//                 cpuLabel
//             );

//             ImGui::PopStyleColor(2);

//             // Memory: yellow when high, red when very high
//             ImGui::TableSetColumnIndex(3);

//             if (process.memory >= HIGH_MEMORY_MB * 2.0f)
//                 ImGui::TextColored(COLOR_RED, "%.1f", process.memory);
//             else if (process.memory >= HIGH_MEMORY_MB)
//                 ImGui::TextColored(COLOR_YELLOW, "%.1f", process.memory);
//             else
//                 ImGui::Text("%.1f", process.memory);

//             ImGui::TableSetColumnIndex(4);
//             ImGui::TextDisabled("%s", process.runningTime.c_str());

//             // Only apps (windowed programs) get an End Task button
//             ImGui::TableSetColumnIndex(5);

//             if (process.isApp)
//             {
//                 ImGui::PushID(process.pid);

//                 ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.55f, 0.12f, 0.15f, 1.0f));
//                 ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.18f, 0.22f, 1.0f));
//                 ImGui::PushStyleColor(ImGuiCol_ButtonActive,  COLOR_RED);

//                 if (ImGui::SmallButton("End Task"))
//                 {
//                     pendingAlertID   = 0;
//                     pendingPid       = process.pid;
//                     pendingName      = process.name;
//                     pendingMem       = process.memory;
//                     openEndTaskPopup = true;
//                 }

//                 ImGui::PopStyleColor(3);
//                 ImGui::PopID();
//             }
//             else
//             {
//                 ImGui::TextDisabled("-");
//             }
//         }

//         if (shown == 0)
//         {
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextDisabled("No process matches the current filters.");
//         }

//         ImGui::EndTable();
//     }

//     DrawEndTaskPopup();

//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // EVENT PAGE
// // ------------------------------------------------------------

// static void DrawEventPage(
//     const char* title,
//     const std::vector<EventInfo>& events,
//     const char* emptyText,
//     const char* hint)
// {
//     ImGui::BeginChild(
//         title,
//         ImVec2(0.0f, 0.0f),
//         true
//     );

//     SectionHeading(title, hint);

//     if (ImGui::BeginTable(
//             "EventTable",
//             3,
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);
//         ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 190.0f);
//         ImGui::TableSetupColumn("Event", ImGuiTableColumnFlags_WidthFixed, 170.0f);
//         ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthStretch);
//         ImGui::TableHeadersRow();

//         for (const EventInfo& event : events)
//         {
//             ImGui::TableNextRow();

//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextDisabled("%s", event.time.c_str());

//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextUnformatted(event.type.c_str());

//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextWrapped("%s", event.description.c_str());
//         }

//         if (events.empty())
//         {
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextDisabled("%s", emptyText);
//         }

//         ImGui::EndTable();
//     }

//     ImGui::EndChild();
// }

// static void DrawSystemAnalysis()
// {
//     ImGui::BeginChild("AnalysisPanel", ImVec2(0.0f, 0.0f), true);

//     if (!analysisData.valid)
//     {
//         ImGui::TextDisabled("Collecting the first samples...");
//         ImGui::EndChild();
//         return;
//     }

//     const BackendAnalysis& a = analysisData;

//     // ---- Resource usage ----
//     SectionHeading("Resource usage");

//     char cpuText[16];
//     char ramText[16];

//     if (a.cpuAvailable)
//         std::snprintf(cpuText, sizeof(cpuText), "%.1f%%", a.cpuPercent);
//     else
//         std::snprintf(cpuText, sizeof(cpuText), "n/a");

//     if (a.memoryAvailable)
//         std::snprintf(ramText, sizeof(ramText), "%.1f%%", a.memoryPercent);
//     else
//         std::snprintf(ramText, sizeof(ramText), "n/a");

//     ImGui::Text("CPU:");
//     ImGui::SameLine();
//     ImGui::TextColored(LoadColor((float)a.cpuPercent), "%s", cpuText);

//     ImGui::Text("Memory:");
//     ImGui::SameLine();
//     ImGui::TextColored(LoadColor((float)a.memoryPercent), "%s", ramText);
//     ImGui::SameLine();
//     ImGui::TextDisabled("(%llu MB used of %llu MB, %llu MB free)",
//         (unsigned long long)a.usedMemoryMB,
//         (unsigned long long)a.totalMemoryMB,
//         (unsigned long long)a.availableMemoryMB);

//     ImGui::Spacing();

//     const float half =
//         (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

//     if (!a.cpuHistory.empty())
//     {
//         ImGui::BeginGroup();
//         ImGui::TextDisabled("CPU usage (last 60 s)");
//         DrawAreaChart("##cpuh",
//             a.cpuHistory.data(), (int)a.cpuHistory.size(), 0,
//             100.0f, ImVec2(half, 170.0f),
//             LoadColor((float)a.cpuPercent));
//         ImGui::EndGroup();
//     }

//     ImGui::SameLine();

//     if (!a.ramHistory.empty())
//     {
//         ImGui::BeginGroup();
//         ImGui::TextDisabled("Memory usage (last 60 s)");
//         DrawAreaChart("##ramh",
//             a.ramHistory.data(), (int)a.ramHistory.size(), 0,
//             100.0f, ImVec2(half, 170.0f),
//             LoadColor((float)a.memoryPercent));
//         ImGui::EndGroup();
//     }

//     ImGui::Spacing();
//     ImGui::Separator();
//     ImGui::Spacing();

//     // ---- Findings ----
//     SectionHeading("Findings", "A diagnostic signal, not proof of an intrusion");

//     ImGui::TextWrapped(
//         "Processes: %llu    High CPU (>=80%%): %llu    High memory (>=1 GB): %llu",
//         (unsigned long long)a.processCount,
//         (unsigned long long)a.highCpuProcessCount,
//         (unsigned long long)a.highMemoryProcessCount);

//     if (a.findings.empty())
//     {
//         ImGui::TextColored(COLOR_GREEN, "No resource concerns detected.");
//     }
//     else
//     {
//         for (const std::string& finding : a.findings)
//         {
//             ImGui::PushStyleColor(ImGuiCol_Text, COLOR_YELLOW);
//             ImGui::TextWrapped("- %s", finding.c_str());
//             ImGui::PopStyleColor();
//         }
//     }

//     ImGui::Spacing();
//     ImGui::Separator();
//     ImGui::Spacing();

//     // ---- Top CPU processes ----
//     SectionHeading("Top processes by CPU");

//     if (ImGui::BeginTable(
//             "TopCpuTable", 4,
//             ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_SizingStretchProp))
//     {
//         ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed, 85.0f);
//         ImGui::TableSetupColumn("Process name");
//         ImGui::TableSetupColumn("CPU %", ImGuiTableColumnFlags_WidthFixed, 100.0f);
//         ImGui::TableSetupColumn("Memory MB", ImGuiTableColumnFlags_WidthFixed, 125.0f);
//         ImGui::TableHeadersRow();

//         for (const BackendProcess& p : a.topCpuProcesses)
//         {
//             ImGui::TableNextRow();

//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextDisabled("%d", p.pid);

//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextUnformatted(p.name.c_str());

//             ImGui::TableSetColumnIndex(2);
//             ImGui::Text("%.1f", p.cpuPercent);

//             ImGui::TableSetColumnIndex(3);
//             ImGui::Text("%.1f", p.memoryMB);
//         }

//         ImGui::EndTable();
//     }

//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // FILE SCANNER PAGE
// // ------------------------------------------------------------

// static std::string FormatBytes(unsigned long long b)
// {
//     char t[32];

//     if (b >= (1ULL << 30))      std::snprintf(t, sizeof(t), "%.2f GB", b / 1073741824.0);
//     else if (b >= (1ULL << 20)) std::snprintf(t, sizeof(t), "%.1f MB", b / 1048576.0);
//     else if (b >= 1024ULL)      std::snprintf(t, sizeof(t), "%.1f KB", b / 1024.0);
//     else                        std::snprintf(t, sizeof(t), "%llu B", b);

//     return t;
// }

// // Rotating-arc spinner. Reserves a (2*radius) square at the cursor.
// static void DrawSpinner(float radius, float thickness, const ImVec4& color, bool active)
// {
//     const ImVec2 pos = ImGui::GetCursorScreenPos();
//     const ImVec2 center(pos.x + radius, pos.y + radius);
//     ImDrawList* draw = ImGui::GetWindowDrawList();
//     const float t = (float)ImGui::GetTime();

//     if (!active)
//     {
//         draw->AddCircle(center, radius, ImGui::GetColorU32(color), 48, thickness);
//     }
//     else
//     {
//         draw->AddCircle(center, radius,
//             ImGui::GetColorU32(WithAlpha(color, 0.18f)), 48, thickness);

//         const float start = t * 5.0f;

//         draw->PathClear();
//         draw->PathArcTo(center, radius, start, start + 2.4f, 32);
//         draw->PathStroke(ImGui::GetColorU32(color), 0, thickness);

//         const float pulse = 0.75f + 0.25f * std::sin(t * 4.0f);
//         draw->AddCircleFilled(center, radius * 0.30f * pulse,
//             ImGui::GetColorU32(WithAlpha(color, 0.85f)), 24);
//     }

//     ImGui::Dummy(ImVec2(radius * 2.0f, radius * 2.0f));
// }

// static void SplitPath(const std::string& full, std::string& dir, std::string& name)
// {
//     const std::size_t pos = full.find_last_of("\\/");

//     if (pos == std::string::npos)
//     {
//         dir.clear();
//         name = full;
//     }
//     else
//     {
//         dir  = full.substr(0, pos);
//         name = full.substr(pos + 1);
//     }
// }

// static void DrawFileScanner()
// {
//     static double lastRefresh = -1.0;
//     const double now = ImGui::GetTime();

//     if (lastRefresh < 0.0 || now - lastRefresh >= 0.25)
//     {
//         lastRefresh  = now;
//         scanStatus   = backend.getScanStatus();
//         scanFindings = backend.getScanFindings();
//     }

//     const bool running = scanStatus.running;

//     ImGui::BeginChild("ScannerPanel", ImVec2(0.0f, 0.0f), true);

//     SectionHeading("File Scanner", "Signature + heuristic scan");

//     // ---- Scan type ----
//     ImGui::BeginDisabled(running);

//     ImGui::RadioButton("Quick (user folders, temp, startup)", &scanTypeSel, 0);
//     ImGui::SameLine();
//     ImGui::RadioButton("Full (all fixed drives)", &scanTypeSel, 1);
//     ImGui::SameLine();
//     ImGui::RadioButton("Custom", &scanTypeSel, 2);

//     if (scanTypeSel == 2)
//     {
//         ImGui::SetNextItemWidth(-1.0f);
//         ImGui::InputTextWithHint("##scanpath",
//             "Folder or file, e.g. C:\\Users\\you\\Downloads",
//             scanPath, sizeof(scanPath));
//     }

//     ImGui::EndDisabled();

//     // ---- Buttons ----
//     if (!running)
//     {
//         if (ImGui::Button("Start Scan"))
//         {
//             if (scanTypeSel == 2 && scanPath[0] == '\0')
//                 scanMessage = "Enter a folder or file path first.";
//             else if (!backend.startScan((ScanType)scanTypeSel, scanPath))
//                 scanMessage = "A scan is already running.";
//             else
//                 scanMessage.clear();
//         }
//     }
//     else
//     {
//         if (ImGui::Button("Stop Scan"))
//             backend.cancelScan();
//     }

//     if (!running && scanStatus.finished)
//     {
//         ImGui::SameLine();

//         if (ImGui::Button("Export Report"))
//         {
//             std::string out;

//             scanMessage = backend.exportScanReport(out)
//                 ? "Report saved: " + out
//                 : "Could not write the report file.";
//         }
//     }

//     if (!scanMessage.empty())
//     {
//         ImGui::SameLine();
//         ImGui::TextDisabled("%s", scanMessage.c_str());
//     }

//     ImGui::Separator();
//     ImGui::Spacing();

//     // Two panels side by side: live scan (left), threats (right)
//     const float gap    = ImGui::GetStyle().ItemSpacing.x;
//     const float leftW  = (ImGui::GetContentRegionAvail().x - gap) * 0.58f;
//     const float panelH = 280.0f;

//     ImGui::PushStyleColor(ImGuiCol_ChildBg, COLOR_SIDEBAR);

//     // ---------------- LEFT: live scan ----------------
//     ImGui::BeginChild("ScanLive", ImVec2(leftW, panelH), true);

//     {
//         ImVec4 stateColor = COLOR_MUTED;
//         const char* stateText = "Ready to scan";

//         if (running)
//         {
//             stateColor = COLOR_CYAN;
//             stateText  = "Scanning";
//         }
//         else if (scanStatus.finished && scanStatus.cancelled)
//         {
//             stateColor = COLOR_YELLOW;
//             stateText  = "Scan stopped";
//         }
//         else if (scanStatus.finished)
//         {
//             stateColor = (scanStatus.malicious + scanStatus.suspicious > 0)
//                 ? COLOR_ORANGE : COLOR_GREEN;
//             stateText  = "Scan completed";
//         }

//         DrawSpinner(26.0f, 4.0f, stateColor, running);
//         ImGui::SameLine(0.0f, 16.0f);

//         ImGui::BeginGroup();

//         ImGui::SetWindowFontScale(1.3f);
//         ImGui::PushStyleColor(ImGuiCol_Text, stateColor);

//         if (running)
//         {
//             const int dots = (int)(ImGui::GetTime() * 2.0) % 4;
//             ImGui::Text("%s%.*s", stateText, dots, "...");
//         }
//         else
//         {
//             ImGui::TextUnformatted(stateText);
//         }

//         ImGui::PopStyleColor();
//         ImGui::SetWindowFontScale(1.0f);

//         ImGui::PushTextWrapPos(ImGui::GetCursorPosX() +
//             std::max(120.0f, ImGui::GetContentRegionAvail().x - 4.0f));
//         ImGui::TextDisabled("%s",
//             scanStatus.target.empty() ? "Choose a scan type and press Start"
//                                       : scanStatus.target.c_str());
//         ImGui::PopTextWrapPos();

//         ImGui::EndGroup();

//         ImGui::Spacing();
//         ImGui::Separator();
//         ImGui::Spacing();

//         ImGui::TextDisabled("%s", running ? "NOW SCANNING" : "LAST FILE SCANNED");

//         std::string dir, name;
//         SplitPath(scanStatus.currentFile, dir, name);

//         if (name.empty())
//         {
//             ImGui::TextDisabled("-");
//         }
//         else
//         {
//             ImGui::SetWindowFontScale(1.15f);
//             ImGui::PushStyleColor(ImGuiCol_Text, COLOR_TEXT);
//             ImGui::TextUnformatted(name.c_str());
//             ImGui::PopStyleColor();
//             ImGui::SetWindowFontScale(1.0f);

//             ImGui::PushStyleColor(ImGuiCol_Text, COLOR_MUTED);
//             ImGui::TextWrapped("%s", dir.c_str());
//             ImGui::PopStyleColor();
//         }

//         ImGui::Spacing();

//         // Live counters (wrap onto two lines instead of overflowing)
//         ImGui::Text("Files: %llu", scanStatus.filesScanned);
//         ImGui::SameLine(0.0f, 18.0f);
//         ImGui::Text("Skipped: %llu", scanStatus.filesSkipped);
//         ImGui::Text("Data: %s", FormatBytes(scanStatus.bytesScanned).c_str());
//         ImGui::SameLine(0.0f, 18.0f);
//         ImGui::Text("Time: %s", FormatDuration(scanStatus.elapsedSeconds).c_str());
//     }

//     ImGui::EndChild();

//     ImGui::SameLine();

//     // ---------------- RIGHT: threats found ----------------
//     ImGui::BeginChild("ScanThreats", ImVec2(0.0f, panelH), true);

//     {
//         ImGui::PushStyleColor(ImGuiCol_Text, COLOR_CYAN);
//         ImGui::SetWindowFontScale(1.1f);
//         ImGui::TextUnformatted("Suspicious & malicious files");
//         ImGui::SetWindowFontScale(1.0f);
//         ImGui::PopStyleColor();

//         ImGui::TextColored(COLOR_RED, "Malicious: %d", scanStatus.malicious);
//         ImGui::SameLine(0.0f, 18.0f);
//         ImGui::TextColored(COLOR_YELLOW, "Suspicious: %d", scanStatus.suspicious);

//         ImGui::Separator();

//         if (scanFindings.empty())
//         {
//             ImGui::Spacing();

//             if (running)
//                 ImGui::TextDisabled("Nothing suspicious so far...");
//             else if (scanStatus.finished)
//                 ImGui::TextColored(COLOR_GREEN, "No threats found.");
//             else
//                 ImGui::TextDisabled("Findings will appear here.");
//         }

//         // Newest first
//         for (std::size_t i = scanFindings.size(); i-- > 0; )
//         {
//             const ScanFinding& f = scanFindings[i];
//             const ImVec4 c = (f.level == "Malicious") ? COLOR_RED : COLOR_YELLOW;

//             std::string dir, name;
//             SplitPath(f.path, dir, name);

//             ImGui::PushID((int)i);

//             ImGui::TextColored(c, "[%s]", f.level.c_str());
//             ImGui::SameLine();
//             ImGui::TextUnformatted(f.name.c_str());

//             ImGui::TextDisabled("%s", name.c_str());

//             if (ImGui::IsItemHovered())
//             {
//                 ImGui::BeginTooltip();
//                 ImGui::Text("%s", f.path.c_str());
//                 ImGui::Text("Reason: %s", f.reason.c_str());
//                 ImGui::Text("Score: %d", f.score);
//                 ImGui::EndTooltip();
//             }

//             ImGui::Separator();
//             ImGui::PopID();
//         }
//     }

//     ImGui::EndChild();

//     ImGui::PopStyleColor();   // ChildBg

//     ImGui::Spacing();

//     // Full findings table
//     SectionHeading("All findings", "hover a name for score and SHA-256");

//     if (ImGui::BeginTable(
//             "ScanFindings",
//             5,
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);
//         ImGui::TableSetupColumn("Time",   ImGuiTableColumnFlags_WidthFixed, 80.0f);
//         ImGui::TableSetupColumn("Level",  ImGuiTableColumnFlags_WidthFixed, 95.0f);
//         ImGui::TableSetupColumn("Name",   ImGuiTableColumnFlags_WidthFixed, 190.0f);
//         ImGui::TableSetupColumn("File");
//         ImGui::TableSetupColumn("Reason");
//         ImGui::TableHeadersRow();

//         for (const ScanFinding& f : scanFindings)
//         {
//             ImGui::TableNextRow();

//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextDisabled("%s", f.time.c_str());

//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextColored(
//                 f.level == "Malicious" ? COLOR_RED : COLOR_YELLOW,
//                 "%s", f.level.c_str());

//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextUnformatted(f.name.c_str());

//             if (ImGui::IsItemHovered())
//             {
//                 ImGui::BeginTooltip();
//                 ImGui::Text("Score: %d", f.score);
//                 ImGui::Text("Size:  %s", FormatBytes(f.size).c_str());

//                 if (!f.sha256.empty())
//                     ImGui::Text("SHA-256: %s", f.sha256.c_str());

//                 ImGui::EndTooltip();
//             }

//             ImGui::TableSetColumnIndex(3);
//             ImGui::TextWrapped("%s", f.path.c_str());

//             ImGui::TableSetColumnIndex(4);
//             ImGui::TextWrapped("%s", f.reason.c_str());
//         }

//         ImGui::EndTable();
//     }

//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // LOGS & REPORTS PAGE
// // ------------------------------------------------------------

// static void DrawLogsReports()
// {
//     if (!LogsGate())
//         return;

//     static std::vector<SignatureInfo> signatures;
//     static bool loaded = false;
//     static char search[128] = "";

//     if (!loaded)
//     {
//         signatures = FileScanner::getSignatures();
//         loaded = true;
//     }

//     int hashCount = 0, contentCount = 0, nameCount = 0;

//     for (const SignatureInfo& s : signatures)
//     {
//         if (s.category == "Known hash")        ++hashCount;
//         else if (s.category == "File content") ++contentCount;
//         else                                   ++nameCount;
//     }

//     ImGui::BeginChild("LogsPanel", ImVec2(0.0f, 0.0f), true);

//     SectionHeading("Detection signatures", "what the File Scanner checks for");

//     if (ImGui::Button("Lock now"))
//     {
//         logsUnlocked = false;
//         lockMessage.clear();
//     }

//     ImGui::SameLine();
//     ImGui::TextDisabled("Locks again after 5 minutes idle or when you leave this page.");

//     if (ImGui::CollapsingHeader("Change password"))
//     {
//         static std::string changeMessage;
//         static bool        changeOk = false;

//         ImGui::SetNextItemWidth(260.0f);
//         ImGui::InputTextWithHint("##oldpw", "Current password",
//             lockOld, sizeof(lockOld), ImGuiInputTextFlags_Password);

//         ImGui::SetNextItemWidth(260.0f);
//         ImGui::InputTextWithHint("##chnew", "New password",
//             lockNew, sizeof(lockNew), ImGuiInputTextFlags_Password);

//         ImGui::SetNextItemWidth(260.0f);
//         ImGui::InputTextWithHint("##chconf", "Confirm new password",
//             lockConfirm, sizeof(lockConfirm), ImGuiInputTextFlags_Password);

//         if (ImGui::Button("Update password"))
//         {
//             if (std::strcmp(lockNew, lockConfirm) != 0)
//             {
//                 changeMessage = "New passwords do not match.";
//                 changeOk      = false;
//             }
//             else
//             {
//                 changeOk = backend.setPassword(lockNew, lockOld, changeMessage);

//                 if (changeOk)
//                 {
//                     Wipe(lockOld, sizeof(lockOld));
//                     Wipe(lockNew, sizeof(lockNew));
//                     Wipe(lockConfirm, sizeof(lockConfirm));
//                 }
//             }
//         }

//         if (!changeMessage.empty())
//             ImGui::TextColored(changeOk ? COLOR_GREEN : COLOR_RED,
//                 "%s", changeMessage.c_str());
//     }

//     ImGui::Spacing();

//     ImGui::PushTextWrapPos(0.0f);
//     ImGui::TextColored(COLOR_MUTED,
//         "A file is reported Malicious on an instant match (EICAR test string or a "
//         "known-bad SHA-256). Otherwise every matching rule adds its weight to a "
//         "score, and a file scoring %d or more is reported Suspicious.",
//         FileScanner::suspiciousThreshold());
//     ImGui::PopTextWrapPos();

//     ImGui::Spacing();

//     ImGui::Text("%d rules", (int)signatures.size());
//     ImGui::SameLine(0.0f, 18.0f);
//     ImGui::TextColored(COLOR_RED,    "Known hashes: %d", hashCount);
//     ImGui::SameLine(0.0f, 18.0f);
//     ImGui::TextColored(COLOR_CYAN,   "File content: %d", contentCount);
//     ImGui::SameLine(0.0f, 18.0f);
//     ImGui::TextColored(COLOR_YELLOW, "Name / location: %d", nameCount);

//     ImGui::Spacing();

//     ImGui::SetNextItemWidth(300.0f);
//     ImGui::InputTextWithHint("##sigsearch", "Search signatures", search, IM_ARRAYSIZE(search));

//     ImGui::SameLine();

//     if (ImGui::Button("Reload"))
//         signatures = FileScanner::getSignatures();   // re-reads hids_hashes.txt

//     ImGui::Spacing();

//     if (ImGui::BeginTable(
//             "SignatureTable",
//             4,
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);
//         ImGui::TableSetupColumn("Type",      ImGuiTableColumnFlags_WidthFixed, 160.0f);
//         ImGui::TableSetupColumn("Name",      ImGuiTableColumnFlags_WidthStretch, 1.0f);
//         ImGui::TableSetupColumn("Pattern / condition", ImGuiTableColumnFlags_WidthStretch, 1.4f);
//         ImGui::TableSetupColumn("Result",    ImGuiTableColumnFlags_WidthFixed, 100.0f);
//         ImGui::TableHeadersRow();

//         int shown = 0;

//         for (const SignatureInfo& s : signatures)
//         {
//             if (!ContainsNoCase(s.name.c_str(), search) &&
//                 !ContainsNoCase(s.pattern.c_str(), search) &&
//                 !ContainsNoCase(s.category.c_str(), search))
//                 continue;

//             ++shown;

//             ImGui::TableNextRow();

//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextColored(
//                 s.category == "Known hash"   ? COLOR_RED :
//                 s.category == "File content" ? COLOR_CYAN : COLOR_YELLOW,
//                 "%s", s.category.c_str());

//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextWrapped("%s", s.name.c_str());

//             ImGui::TableSetColumnIndex(2);
//             ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
//             ImGui::TextWrapped("%s", s.pattern.c_str());
//             ImGui::PopStyleColor();

//             ImGui::TableSetColumnIndex(3);

//             if (s.instantMalicious)
//                 ImGui::TextColored(COLOR_RED, "Malicious");
//             else
//                 ImGui::TextColored(
//                     s.weight >= 4 ? COLOR_ORANGE :
//                     s.weight >= 2 ? COLOR_YELLOW : COLOR_MUTED,
//                     "+%d score", s.weight);
//         }

//         if (shown == 0)
//         {
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextDisabled("No signature matches \"%s\".", search);
//         }

//         ImGui::EndTable();
//     }

//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // NOTIFICATION SETTINGS (shown on the Settings page)
// // ------------------------------------------------------------

// static void DrawNotificationSettings()
// {
//     ImGui::BeginChild("NotificationSettings", ImVec2(0.0f, 215.0f), true,
//                       ImGuiWindowFlags_NoScrollbar);

//     SectionHeading("Desktop notifications",
//         "pop-up toasts when a new alert is raised");

//     bool enabled = backend.notificationsEnabled();

//     if (ImGui::Checkbox("Show desktop notifications", &enabled))
//         backend.setNotificationsEnabled(enabled);

//     ImGui::BeginDisabled(!enabled);

//     static const char* levels[] = {
//         "Low and above", "Medium and above",
//         "High and above", "Critical only"
//     };

//     int level = backend.notificationMinSeverity();

//     ImGui::SetNextItemWidth(220.0f);

//     if (ImGui::Combo("Notify for", &level, levels, IM_ARRAYSIZE(levels)))
//         backend.setNotificationMinSeverity(level);

//     if (ImGui::Button("Send test notification"))
//         backend.sendTestNotification();

//     ImGui::EndDisabled();

//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // FEATURE PAGE: header with Back + only the selected feature
// // ------------------------------------------------------------

// static void DrawMainContent()
// {
//     RefreshFromBackend();

//     DrawPageHeader();
//     ImGui::Separator();
//     ImGui::Spacing();

//     if (demoMode)
//     {
//         DrawPill("DEMO DATA", COLOR_YELLOW);
//         ImGui::SameLine();
//         ImGui::TextColored(COLOR_YELLOW,
//             "Sample rows are labelled [DEMO] and are not real host findings.");
//         ImGui::Spacing();
//     }

//     const float fade = ClampFloat(
//         (float)((ImGui::GetTime() - screenChangedAt) / 0.18),
//         0.0f, 1.0f
//     );
//     ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.25f + 0.75f * fade);

//     switch (selectedPage)
//     {
//     case 0:
//         DrawEngineBanner();
//         DrawMetrics();
//         ImGui::Spacing();
//         DrawThreatPanel();
//         ImGui::Spacing();
//         DrawAlerts(true);
//         break;

//     case 1:
//         DrawProcesses();
//         break;

//     case 2:
//         DrawEventPage("Failed sign-in attempts", loginEvents,
//             "No failed sign-ins recorded since monitoring started.",
//             "Reads the Windows Security log - run as Administrator");
//         break;

//     case 3:
//         DrawEventPage("File activity", fileEvents,
//             "No changes detected in the watched folder yet.",
//             "Watching the HIDS_Monitor folder");
//         break;

//     case 4:
//         DrawAlerts(false);
//         break;

//     case 5:
//         DrawLogsReports();
//         break;

//     case 6:
//         DrawSystemAnalysis();
//         break;

//     case 7:
//         DrawFileScanner();
//         break;

//     case 8:
//         DrawNotificationSettings();
//         ImGui::Spacing();
//         DrawSettingsPage(monitoringActive);
//         break;
//     }

//     ImGui::PopStyleVar();
// }

// int main()
// {
//     if (!glfwInit())
//     {
//         std::fprintf(stderr, "ERROR: Failed to initialize GLFW.\n");
//         return 1;
//     }

//     // Request OpenGL 3.0
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

//     GLFWwindow* window = glfwCreateWindow(
//         1400,
//         900,
//         "HIDS - Host Intrusion Detection System",
//         nullptr,
//         nullptr
//     );

//     if (window == nullptr)
//     {
//         std::fprintf(stderr, "ERROR: Failed to create dashboard window.\n");
//         glfwTerminate();
//         return 1;
//     }

//     glfwMakeContextCurrent(window);
//     glfwSwapInterval(1);

//     // Native window handle for the tray icon / desktop notifications
// #ifdef _WIN32
//     backend.attachWindow(glfwGetWin32Window(window));
// #endif

//     IMGUI_CHECKVERSION();
//     ImGui::CreateContext();

//     ImGuiIO& io = ImGui::GetIO();

//     io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

//     // Don't write imgui.ini next to the executable
//     io.IniFilename = nullptr;

//     LoadFonts(io);
//     LoadUiConfig();           // saved theme / interface size / demo flag
//     ApplyTheme(themeIndex);   // also builds the ImGui style
//     RefineStyle();            // flatter, more modern spacing and rounding
//     InitUiScale(io);

//     int styledTheme = themeIndex;

//     // Seed sparkline history so the cards aren't empty on first frame
//     for (int i = 0; i < HISTORY_SIZE; ++i)
//     {
//         cpuHistory[i] = cpuNow;
//         ramHistory[i] = ramNow;
//     }

//     ImGui_ImplGlfw_InitForOpenGL(window, true);
//     ImGui_ImplOpenGL3_Init("#version 130");

//     // Monitoring starts only when the user presses Start HIDS / Start protection.
//     monitoringActive = backend.isRunning();

//     // Main application loop
//     while (!glfwWindowShouldClose(window))
//     {
//         glfwPollEvents();

//         // Send queued desktop notifications (must run on the UI thread)
//         backend.flushNotifications();

//         // Theme clicks are queued and applied here, never mid-frame
//         ApplyPendingTheme();

//         // A theme change rebuilds the style, so re-apply our refinements
//         if (themeIndex != styledTheme)
//         {
//             RefineStyle();
//             styledTheme = themeIndex;
//         }

//         ImGui_ImplOpenGL3_NewFrame();
//         ImGui_ImplGlfw_NewFrame();
//         ImGui::NewFrame();

//         TrackScreenChange();   // fade + re-lock Logs & Reports when leaving it
//         HandleBackKey();       // Esc: page -> domain -> hub

//         // Gradient backdrop; the root window is transparent so it shows through
//         DrawBackdrop();
//         ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));

//         // Full-window root
//         ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
//         ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

//         ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 12.0f));

//         ImGui::Begin(
//             "HIDSRoot",
//             nullptr,
//             ImGuiWindowFlags_NoTitleBar |
//             ImGuiWindowFlags_NoResize |
//             ImGuiWindowFlags_NoMove |
//             ImGuiWindowFlags_NoCollapse |
//             ImGuiWindowFlags_NoScrollbar |
//             ImGuiWindowFlags_NoBringToFrontOnFocus
//         );

//         ImGui::PopStyleVar();

//         if (appScreen == SCREEN_PAGE)
//         {
//             // A single feature page: header with Back, then only that feature.
//             ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));

//             ImGui::BeginChild(
//                 "MainContent",
//                 ImVec2(0.0f, 0.0f),
//                 false
//             );

//             DrawMainContent();

//             ImGui::EndChild();
//             ImGui::PopStyleColor();
//         }
//         else
//         {
//             // Welcome screen, home hub and domain screens
//             DrawFullScreen();
//         }

//         ImGui::End();
//         ImGui::PopStyleColor();   // WindowBg

//         ImGui::Render();

//         int displayWidth = 0;
//         int displayHeight = 0;

//         glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
//         glViewport(0, 0, displayWidth, displayHeight);

//         glClearColor(
//             HIDS_COLOR_BACKGROUND.x,
//             HIDS_COLOR_BACKGROUND.y,
//             HIDS_COLOR_BACKGROUND.z,
//             HIDS_COLOR_BACKGROUND.w
//         );

//         glClear(GL_COLOR_BUFFER_BIT);

//         ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

//         glfwSwapBuffers(window);
//     }

//     backend.shutdown();

//     ImGui_ImplOpenGL3_Shutdown();
//     ImGui_ImplGlfw_Shutdown();
//     ImGui::DestroyContext();

//     glfwDestroyWindow(window);
//     glfwTerminate();

//     return 0;
// }





#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

// Native window handle (needed for the tray icon / desktop notifications)
#ifdef _WIN32
#include <windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#include <algorithm>
#include <cctype>
#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <set>
#include <string>
#include <vector>
#include "HIDSBackend.h"
#include "DashboardTheme.h"   // themes, colours, icons, Settings page

static const float HIDS_PI = 3.14159265f;

// ------------------------------------------------------------
// DATA STRUCTURES
// ------------------------------------------------------------

struct ProcessInfo {
    int pid;
    std::string name;
    float cpu;
    float memory;
    double runningSeconds;
    std::string runningTime;   // formatted for display
    bool isApp = false;        // can be ended from the Processes page
    std::string title;         // window title (apps only)
};

struct AlertInfo {
    int id = 0;
    std::string time;
    std::string severity;
    std::string source;
    std::string description;

    bool        canEndTask = false;   // resource alert with a culprit app
    int         culpritPid = 0;
    std::string culpritName;
    float       culpritMem = 0.0f;
};

struct EventInfo {
    std::string time;
    std::string type;
    std::string description;
};

static HIDSBackend backend;

static std::vector<ProcessInfo> processes;
static std::vector<AlertInfo>   alerts;
static std::vector<EventInfo>   loginEvents;
static std::vector<EventInfo>   fileEvents;
static BackendAnalysis          analysisData;

// File scanner page state
static ScanStatus               scanStatus;
static std::vector<ScanFinding> scanFindings;
static int                      scanTypeSel = 0;      // 0 Quick, 1 Full, 2 Custom
static char                     scanPath[512] = "";
static std::string              scanMessage;
static int processCount = 0;

static int  selectedPage = 0;
static char processSearch[128] = "";
static bool monitoringActive = false;
static double engineStartedAt = -1.0;   // for the "running for" timer

static int  severityFilter = 0;   // 0 = All, 1..4 = Critical..Low
static const char* severityFilterLabels[] =
    { "All", "Critical", "High", "Medium", "Low" };
static char alertSearch[128] = "";

static const char* pages[] = {
    "Dashboard",
    "Processes",
    "Login Monitoring",
    "File Monitoring",
    "Alerts",
    "Logs & Reports",
    "System Analysis",
    "File Scanner",
    "Settings"
};

static const char* pageSubtitles[] = {
    "Overview of this machine's security status",
    "Everything currently running on this host",
    "Sign-in activity on this host",
    "Changes to monitored files and folders",
    "Events raised by the HIDS monitors",
    "Detection signatures and scan reports",
    "Deeper inspection of system behaviour",
    "Scan files for known malware and suspicious traits",
    "Appearance, interface size, notifications and data source"
};

// Live metrics (ring buffers feed the sparklines)
static const int HISTORY_SIZE = 60;
static float cpuHistory[HISTORY_SIZE] = {};
static float ramHistory[HISTORY_SIZE] = {};
static int   historyOffset = 0;
static float cpuNow = 0.0f;
static float ramNow = 0.0f;

// Threat activity (feeds the dashboard histogram)
static float threatHistory[HISTORY_SIZE] = {};
static float threatAccum = 0.0f;

// ---- Screen flow: Welcome -> Hub -> Domain -> Feature page ----
enum AppScreen { SCREEN_WELCOME = 0, SCREEN_HUB, SCREEN_DOMAIN, SCREEN_PAGE };
static int    appScreen       = SCREEN_WELCOME;
static int    selectedDomain  = 0;
static double screenChangedAt = 0.0;

static void OpenPage(int page);   // defined after the route tables

static float ClampFloat(float v, float lo, float hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

// ------------------------------------------------------------
// FONT LOADING
// ------------------------------------------------------------

static bool FileExists(const char* path)
{
    FILE* f = std::fopen(path, "rb");
    if (f == nullptr)
        return false;

    std::fclose(f);
    return true;
}

static void LoadFonts(ImGuiIO& io)
{
    const char* candidates[] = {
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf"
    };

    for (const char* path : candidates)
    {
        if (!FileExists(path))
            continue;

        ImFont* font = io.Fonts->AddFontFromFileTTF(path, 18.0f);

        if (font != nullptr)
        {
            io.FontDefault = font;
            io.FontGlobalScale = 1.0f;
            return;
        }
    }

    io.Fonts->AddFontDefault();
    io.FontGlobalScale = 1.25f;
}

// ------------------------------------------------------------
// STYLE REFINEMENT
// Rounded, flat look + tinted table headers and slim scrollbars.
// Applied after ApplyTheme(); colours still come from the theme.
// ------------------------------------------------------------

static void RefineStyle()
{
    ImGuiStyle& s = ImGui::GetStyle();

    s.WindowRounding    = 0.0f;
    s.ChildRounding     = 12.0f;
    s.FrameRounding     = 7.0f;
    s.PopupRounding     = 10.0f;
    s.GrabRounding      = 6.0f;
    s.ScrollbarRounding = 8.0f;
    s.TabRounding       = 6.0f;

    s.WindowBorderSize  = 0.0f;
    s.ChildBorderSize   = 1.0f;
    s.FrameBorderSize   = 0.0f;
    s.PopupBorderSize   = 1.0f;

    s.WindowPadding     = ImVec2(16.0f, 14.0f);
    s.FramePadding      = ImVec2(12.0f, 7.0f);
    s.ItemSpacing       = ImVec2(10.0f, 10.0f);
    s.CellPadding       = ImVec2(10.0f, 8.0f);
    s.ScrollbarSize     = 10.0f;

    ImVec4* c = s.Colors;
    c[ImGuiCol_TableHeaderBg]     = WithAlpha(COLOR_CYAN, 0.12f);
    c[ImGuiCol_TableBorderLight]  = WithAlpha(COLOR_CYAN, 0.07f);
    c[ImGuiCol_TableBorderStrong] = WithAlpha(COLOR_CYAN, 0.16f);
    c[ImGuiCol_TableRowBg]        = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    c[ImGuiCol_TableRowBgAlt]     = ImVec4(1.0f, 1.0f, 1.0f, 0.028f);
    c[ImGuiCol_ScrollbarBg]       = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    c[ImGuiCol_ScrollbarGrab]        = WithAlpha(COLOR_CYAN, 0.30f);
    c[ImGuiCol_ScrollbarGrabHovered] = WithAlpha(COLOR_CYAN, 0.55f);
    c[ImGuiCol_ScrollbarGrabActive]  = WithAlpha(COLOR_CYAN, 0.80f);
}

static bool ContainsNoCase(const char* text, const char* search)
{
    if (search[0] == '\0')
        return true;

    const size_t textLen = std::strlen(text);
    const size_t searchLen = std::strlen(search);

    if (searchLen > textLen)
        return false;

    for (size_t i = 0; i + searchLen <= textLen; ++i)
    {
        size_t j = 0;

        while (j < searchLen &&
               std::tolower((unsigned char)text[i + j]) ==
               std::tolower((unsigned char)search[j]))
        {
            ++j;
        }

        if (j == searchLen)
            return true;
    }

    return false;
}

static ImVec4 SeverityColor(const char* severity)
{
    if (std::strcmp(severity, "Critical") == 0) return COLOR_RED;
    if (std::strcmp(severity, "High") == 0)     return COLOR_ORANGE;
    if (std::strcmp(severity, "Medium") == 0)   return COLOR_YELLOW;

    return COLOR_CYAN;
}

static int CountSeverity(const char* severity)
{
    int count = 0;

    for (const auto& alert : alerts)
        if (alert.severity == severity)
            ++count;

    return count;
}

// Usage colour: green -> yellow -> red as load rises
static ImVec4 LoadColor(float percent)
{
    if (percent >= 85.0f) return COLOR_RED;
    if (percent >= 65.0f) return COLOR_YELLOW;

    return COLOR_GREEN;
}

static void SectionHeading(const char* title, const char* hint = nullptr)
{
    {
        const ImVec2 p = ImGui::GetCursorScreenPos();
        const float  h = ImGui::GetTextLineHeight() * 1.1f;

        ImGui::GetWindowDrawList()->AddRectFilled(
            ImVec2(p.x, p.y + 1.0f), ImVec2(p.x + 4.0f, p.y + h),
            ImGui::GetColorU32(COLOR_CYAN), 2.0f);

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 12.0f);
    }

    ImGui::PushStyleColor(ImGuiCol_Text, COLOR_CYAN);
    ImGui::SetWindowFontScale(1.1f);
    ImGui::TextUnformatted(title);
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();

    if (hint != nullptr)
    {
        ImGui::SameLine();
        ImGui::TextDisabled("  %s", hint);
    }

    ImGui::Spacing();
}

// Coloured pill, e.g. for alert severity
static void SeverityBadge(const char* severity)
{
    const ImVec4 color = SeverityColor(severity);
    const ImVec2 textSize = ImGui::CalcTextSize(severity);
    const ImVec2 pad(10.0f, 2.0f);

    const ImVec2 pos = ImGui::GetCursorScreenPos();
    const ImVec2 size(textSize.x + pad.x * 2.0f, textSize.y + pad.y * 2.0f);

    ImDrawList* draw = ImGui::GetWindowDrawList();

    draw->AddRectFilled(
        pos,
        ImVec2(pos.x + size.x, pos.y + size.y),
        ImGui::GetColorU32(WithAlpha(color, 0.16f)),
        size.y * 0.5f
    );

    draw->AddRect(
        pos,
        ImVec2(pos.x + size.x, pos.y + size.y),
        ImGui::GetColorU32(WithAlpha(color, 0.65f)),
        size.y * 0.5f
    );

    draw->AddText(
        ImVec2(pos.x + pad.x, pos.y + pad.y),
        ImGui::GetColorU32(color),
        severity
    );

    ImGui::Dummy(size);
}

// Pulsing dot used by the monitoring status
static void StatusDot(const ImVec4& color, bool pulse)
{
    const float radius = 5.0f;
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    const ImVec2 center(pos.x + radius + 2.0f, pos.y + ImGui::GetFrameHeight() * 0.5f);

    ImDrawList* draw = ImGui::GetWindowDrawList();

    if (pulse)
    {
        const float t = (float)ImGui::GetTime();
        const float wave = 0.5f + 0.5f * std::sin(t * 3.0f);

        draw->AddCircleFilled(
            center,
            radius + 3.0f + wave * 3.0f,
            ImGui::GetColorU32(WithAlpha(color, 0.18f * (1.0f - wave) + 0.05f))
        );
    }

    draw->AddCircleFilled(center, radius, ImGui::GetColorU32(color));

    ImGui::Dummy(ImVec2(radius * 2.0f + 6.0f, ImGui::GetFrameHeight()));
}

static std::string FormatDuration(double seconds)
{
    const unsigned long long total =
        seconds > 0.0 ? (unsigned long long)seconds : 0ULL;
    char text[32];

    std::snprintf(
        text, sizeof(text), "%02llu:%02llu:%02llu",
        total / 3600ULL, (total / 60ULL) % 60ULL, total % 60ULL
    );
    return text;
}

static float SeverityWeight(const std::string& s)
{
    if (s == "Critical") return 4.0f;
    if (s == "High")     return 3.0f;
    if (s == "Medium")   return 2.0f;
    return 1.0f;
}

// Sample rows. Always labelled so they can't be mistaken for real ones.
static void AppendDemoData()
{
    struct DemoAlert { const char* sev; const char* text; };

    static const DemoAlert demoAlerts[] = {
        { "Critical", "[DEMO] Known-bad hash match - C:\\Demo\\sample.exe" },
        { "High",     "[DEMO] Multiple Failed Logins - 5 attempts in 60 s" },
        { "Medium",   "[DEMO] Suspicious PowerShell Process - powershell.exe" },
        { "Low",      "[DEMO] Password Changed - sample entry" }
    };

    int id = -1;

    for (const DemoAlert& d : demoAlerts)
    {
        AlertInfo a;
        a.id          = id--;
        a.time        = "--:--:--";
        a.severity    = d.sev;
        a.source      = "DEMO";
        a.description = d.text;
        alerts.push_back(a);
    }

    ProcessInfo p;
    p.pid            = -1;
    p.name           = "demo_service.exe [DEMO]";
    p.cpu            = 12.5f;
    p.memory         = 640.0f;
    p.runningSeconds = 4000.0;
    p.runningTime    = FormatDuration(p.runningSeconds);
    p.isApp          = false;
    processes.push_back(p);

    loginEvents.push_back({ "--:--:--", "LOGIN_FAILURE",
        "[DEMO] Failed sign-in for user 'demo'" });
    fileEvents.push_back({ "--:--:--", "FILE_CREATED",
        "[DEMO] HIDS_Monitor\\sample_script.ps1" });
}

static void RefreshFromBackend()
{
    static double lastRefresh = -1.0;
    static unsigned long lastSample = 0;

    const double now = ImGui::GetTime();

    if (lastRefresh >= 0.0 && now - lastRefresh < 0.5)
        return;

    lastRefresh = now;

    // CPU, RAM and process count
    const BackendStats stats = backend.getStats();

    cpuNow = stats.cpuPercent;
    ramNow = stats.ramPercent;
    processCount = stats.processCount;

    if (stats.sample != lastSample)
    {
        lastSample = stats.sample;

        cpuHistory[historyOffset]    = cpuNow;
        ramHistory[historyOffset]    = ramNow;
        threatHistory[historyOffset] = threatAccum;
        threatAccum = 0.0f;
        historyOffset = (historyOffset + 1) % HISTORY_SIZE;
    }

    analysisData = backend.getAnalysis();
    processes.clear();

    for (const BackendProcess& p : backend.getProcesses())
    {
        ProcessInfo row;

        row.pid            = p.pid;
        row.name           = p.name;
        row.cpu            = p.cpuPercent;
        row.memory         = p.memoryMB;
        row.runningSeconds = p.runningSeconds;
        row.runningTime    = FormatDuration(p.runningSeconds);
        row.isApp          = p.isApp;
        row.title          = p.windowTitle;

        processes.push_back(row);
    }
    alerts.clear();

    for (const BackendAlert& a : backend.getAlerts())
    {
        AlertInfo row;

        row.time        = a.time;
        row.severity    = a.severity;
        row.source      = a.source;
        row.description = a.description;
        row.id          = a.id;
        row.canEndTask  = a.canEndTask;
        row.culpritPid  = a.culpritPid;
        row.culpritName = a.culpritName;
        row.culpritMem  = a.culpritMemoryMB;

        alerts.push_back(row);
    }

    // Weigh alerts that appeared since the last refresh
    {
        static std::set<int> seenAlerts;
        static bool seeded = false;

        for (const AlertInfo& a : alerts)
            if (seenAlerts.insert(a.id).second && seeded)
                threatAccum += SeverityWeight(a.severity);

        seeded = true;
    }

    // Login and file activity (already newest first)
    loginEvents.clear();
    fileEvents.clear();

    for (const BackendEvent& e : backend.getEvents())
    {
        EventInfo row;

        row.time        = e.time;
        row.type        = e.type;
        row.description = e.description;

        if (e.type.rfind("LOGIN_", 0) == 0)
            loginEvents.push_back(row);
        else if (e.type.rfind("FILE_", 0) == 0)
            fileEvents.push_back(row);
    }

    if (demoMode)
        AppendDemoData();
}

// ------------------------------------------------------------
// METRIC CARD
// ------------------------------------------------------------

static void MetricCard(
    const char* title,
    const char* value,
    const char* subtitle,
    const ImVec4& accent,
    float width,
    const float* history = nullptr,
    int linkPage = -1,
    float gauge = -1.0f)          // 0..100 draws a ring gauge
{
    ImGui::BeginChild(
        title,
        ImVec2(width, 150.0f),
        true,
        ImGuiWindowFlags_NoScrollbar
    );

    {
        const ImVec2 wp = ImGui::GetWindowPos();
        const ImVec2 ws = ImGui::GetWindowSize();
        ImDrawList*  d  = ImGui::GetWindowDrawList();

        // soft tint from the left
        d->AddRectFilledMultiColor(
            ImVec2(wp.x + 1.0f, wp.y + 1.0f),
            ImVec2(wp.x + ws.x * 0.65f, wp.y + ws.y - 1.0f),
            ImGui::GetColorU32(WithAlpha(accent, 0.10f)),
            ImGui::GetColorU32(WithAlpha(accent, 0.0f)),
            ImGui::GetColorU32(WithAlpha(accent, 0.0f)),
            ImGui::GetColorU32(WithAlpha(accent, 0.10f)));

        // accent stripe
        d->AddRectFilled(wp, ImVec2(wp.x + 4.0f, wp.y + ws.y),
            ImGui::GetColorU32(accent),
            ImGui::GetStyle().ChildRounding,
            ImDrawFlags_RoundCornersLeft);

        // glow + ring gauge (or page icon) on the right side
        const ImVec2 gc(wp.x + ws.x - 50.0f, wp.y + 58.0f);

        DrawGlow(d, gc, 46.0f, accent);

        if (gauge >= 0.0f)
            DrawGauge(d, gc, 24.0f, 6.0f, gauge / 100.0f, accent);
        else if (linkPage >= 0)
            DrawPageIcon(d, gc, 14.0f, linkPage,
                ImGui::GetColorU32(WithAlpha(accent, 0.85f)));
    }

    ImGui::TextDisabled("%s", title);
    ImGui::PushStyleColor(ImGuiCol_Text, accent);
    ImGui::SetWindowFontScale(1.55f);
    ImGui::TextUnformatted(value);
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();

    if (history != nullptr)
    {
        DrawAreaChart("##spark", history, HISTORY_SIZE, historyOffset,
            100.0f, ImVec2(ImGui::GetContentRegionAvail().x, 32.0f),
            accent, "%", false);
    }
    else
    {
        ImGui::Spacing();
        ImGui::TextDisabled("%s", subtitle);
    }

    // Hover highlight + click to open the related page
    if (linkPage >= 0 && ImGui::IsWindowHovered())
    {
        const ImVec2 pos  = ImGui::GetWindowPos();
        const ImVec2 size = ImGui::GetWindowSize();

        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        ImGui::GetWindowDrawList()->AddRect(
            pos, ImVec2(pos.x + size.x, pos.y + size.y),
            ImGui::GetColorU32(WithAlpha(accent, 0.85f)),
            ImGui::GetStyle().ChildRounding, 0, 1.5f);

        ImGui::SetTooltip("Open %s", pages[linkPage]);

        if (ImGui::IsMouseClicked(0))
            OpenPage(linkPage);
    }

    ImGui::EndChild();
}

// Four live cards (used by the dashboard and the home hub)
static void DrawMetrics()
{
    const float spacing = ImGui::GetStyle().ItemSpacing.x;

    const float cardWidth =
        (ImGui::GetContentRegionAvail().x - spacing * 3.0f) / 4.0f;

    char cpuText[16];
    char ramText[16];
    char alertText[16];
    char processText[16];

    // Displayed values glide toward the live ones instead of jumping
    static float cpuShown = 0.0f;
    static float ramShown = 0.0f;

    const float ease = ClampFloat(ImGui::GetIO().DeltaTime * 6.0f, 0.0f, 1.0f);
    cpuShown += (cpuNow - cpuShown) * ease;
    ramShown += (ramNow - ramShown) * ease;

    std::snprintf(cpuText, sizeof(cpuText), "%.0f%%", cpuShown);
    std::snprintf(ramText, sizeof(ramText), "%.0f%%", ramShown);
    std::snprintf(alertText, sizeof(alertText), "%d", (int)alerts.size());
    std::snprintf(processText, sizeof(processText), "%d", processCount);

    const ImVec4 alertColor = CountSeverity("Critical") > 0 ? COLOR_RED : COLOR_YELLOW;

    char alertSubtitle[64];
    std::snprintf(
        alertSubtitle, sizeof(alertSubtitle),
        "%d critical, %d high",
        CountSeverity("Critical"), CountSeverity("High")
    );

    MetricCard("CPU usage", cpuText, "", LoadColor(cpuNow), cardWidth, cpuHistory, 6, cpuShown);
    ImGui::SameLine();

    MetricCard("RAM usage", ramText, "", LoadColor(ramNow), cardWidth, ramHistory, 6, ramShown);
    ImGui::SameLine();

    MetricCard("Running processes", processText, "On this host", COLOR_BLUE, cardWidth, nullptr, 1);
    ImGui::SameLine();

    MetricCard("Total alerts", alertText, alertSubtitle, alertColor, cardWidth, nullptr, 4);
}

// ------------------------------------------------------------
// NAVIGATION DATA
// ------------------------------------------------------------

struct HidsFeatureRoute
{
    const char* title;
    const char* description;
    int page;
};

struct HidsDomainRoute
{
    const char* title;
    const char* description;
    const char* icon;
    const HidsFeatureRoute* features;
    int featureCount;
};

static const HidsFeatureRoute monitoringFeatures[] =
{
    { "Security Dashboard", "Live host health, threat activity and overview", 0 },
    { "Process Monitoring", "Inspect running processes and resource usage", 1 },
    { "Login Monitoring", "Review failed sign-in activity", 2 },
    { "File Monitoring", "Review changes in the monitored folder", 3 }
};

static const HidsFeatureRoute threatFeatures[] =
{
    { "Alert Center", "Review and filter security alerts", 4 },
    { "File Scanner", "Run quick, full and custom file scans", 7 }
};

static const HidsFeatureRoute analysisFeatures[] =
{
    { "System Analysis", "Resource trends, anomalies and process findings", 6 }
};

static const HidsFeatureRoute reportFeatures[] =
{
    { "Logs & Reports", "Review protected detection signatures and reports", 5 }
};

static const HidsFeatureRoute configurationFeatures[] =
{
    { "Settings & Notifications", "Themes, interface scale and desktop alerts", 8 }
};

static const HidsDomainRoute hidsDomains[] =
{
    { "Monitoring", "Observe activity and changes across this host",
      "MON", monitoringFeatures, IM_ARRAYSIZE(monitoringFeatures) },
    { "Threat Detection", "Investigate alerts and scan suspicious files",
      "THR", threatFeatures, IM_ARRAYSIZE(threatFeatures) },
    { "Analysis", "Understand system behaviour and resource anomalies",
      "ANA", analysisFeatures, IM_ARRAYSIZE(analysisFeatures) },
    { "Reports & Logs", "Access security records, signatures and reports",
      "LOG", reportFeatures, IM_ARRAYSIZE(reportFeatures) },
    { "Configuration", "Manage appearance, notifications and preferences",
      "CFG", configurationFeatures, IM_ARRAYSIZE(configurationFeatures) }
};

static int DomainOfPage(int page)
{
    for (int d = 0; d < IM_ARRAYSIZE(hidsDomains); ++d)
        for (int f = 0; f < hidsDomains[d].featureCount; ++f)
            if (hidsDomains[d].features[f].page == page)
                return d;

    return 0;
}

// Jump straight to a feature page (from a card, a chart click, ...)
static void OpenPage(int page)
{
    selectedPage   = page;
    selectedDomain = DomainOfPage(page);
    appScreen      = SCREEN_PAGE;
}

static ImVec4 DomainAccent(int d)
{
    switch (d)
    {
    case 0:  return COLOR_CYAN;
    case 1:  return COLOR_ORANGE;
    case 2:  return COLOR_BLUE;
    case 3:  return COLOR_GREEN;
    default: return COLOR_YELLOW;
    }
}

// Which page icon represents each domain
static const int domainIconPage[5] = { 0, 4, 6, 5, 8 };

static void StartEngine()
{
    backend.start();
    monitoringActive = backend.isRunning();

    if (monitoringActive)
        engineStartedAt = ImGui::GetTime();
}

// Stopping protection always returns to the welcome screen
static void StopEngine()
{
    backend.stop();
    monitoringActive = false;
    engineStartedAt  = -1.0;
    appScreen        = SCREEN_WELCOME;
}

// ------------------------------------------------------------
// SHARED UI PIECES
// ------------------------------------------------------------

static void PushGreenButton()
{
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.02f, 0.48f, 0.31f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.04f, 0.62f, 0.40f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.03f, 0.40f, 0.26f, 1.0f));
}

static void PushRedButton()
{
    ImGui::PushStyleColor(ImGuiCol_Button,        WithAlpha(COLOR_RED, 0.14f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, WithAlpha(COLOR_RED, 0.30f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  WithAlpha(COLOR_RED, 0.45f));
}

static void CenteredText(const char* text, float scale, const ImVec4& col)
{
    ImGui::SetWindowFontScale(scale);
    const float w = ImGui::CalcTextSize(text).x;
    ImGui::SetCursorPosX(std::max(0.0f, (ImGui::GetWindowWidth() - w) * 0.5f));
    ImGui::TextColored(col, "%s", text);
    ImGui::SetWindowFontScale(1.0f);
}

// Centred inside the current content area (use inside panels)
static void CenteredInContent(const char* text, float scale, const ImVec4& col)
{
    const float avail = ImGui::GetContentRegionAvail().x;

    ImGui::SetWindowFontScale(scale);
    const float w = ImGui::CalcTextSize(text).x;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + std::max(0.0f, (avail - w) * 0.5f));
    ImGui::TextColored(col, "%s", text);
    ImGui::SetWindowFontScale(1.0f);
}

// Draws the hint at the bottom of the window without ever overlapping content
static void FooterHint(const char* text, float scale = 0.95f)
{
    const float y = ImGui::GetWindowHeight() - 38.0f;

    if (ImGui::GetCursorPosY() < y)
        ImGui::SetCursorPosY(y);

    CenteredText(text, scale, WithAlpha(COLOR_MUTED, 0.7f));
}

static void DrawShieldLogo(ImDrawList* d, const ImVec2& c, float s, const ImVec4& col)
{
    ImVec2 shield[5] = {
        ImVec2(c.x - 0.80f * s, c.y - 0.90f * s),
        ImVec2(c.x + 0.80f * s, c.y - 0.90f * s),
        ImVec2(c.x + 0.80f * s, c.y + 0.10f * s),
        ImVec2(c.x,             c.y + 1.00f * s),
        ImVec2(c.x - 0.80f * s, c.y + 0.10f * s)
    };
    d->AddConvexPolyFilled(shield, 5, ImGui::GetColorU32(col));

    ImVec2 tick[3] = {
        ImVec2(c.x - 0.36f * s, c.y - 0.05f * s),
        ImVec2(c.x - 0.08f * s, c.y + 0.25f * s),
        ImVec2(c.x + 0.42f * s, c.y - 0.38f * s)
    };
    d->AddPolyline(tick, 3, IM_COL32(8, 16, 28, 255), 0, s * 0.16f);
}

static void DrawLockIcon(ImDrawList* d, const ImVec2& c, float s, const ImVec4& col)
{
    // shackle
    d->PathClear();
    d->PathArcTo(ImVec2(c.x, c.y - 0.05f * s), s * 0.60f, HIDS_PI, HIDS_PI * 2.0f, 24);
    d->PathStroke(ImGui::GetColorU32(col), 0, s * 0.26f);

    // body
    d->AddRectFilled(ImVec2(c.x - s * 0.95f, c.y - 0.05f * s),
                     ImVec2(c.x + s * 0.95f, c.y + s * 0.95f),
                     ImGui::GetColorU32(col), s * 0.18f);

    // keyhole
    d->AddCircleFilled(ImVec2(c.x, c.y + s * 0.38f), s * 0.17f,
                       IM_COL32(8, 16, 28, 255), 16);
}

// Clickable text link (breadcrumbs). Returns true when clicked.
static bool CrumbLink(const char* label, const ImVec4& col)
{
    ImGui::PushID(label);

    const ImVec2 sz = ImGui::CalcTextSize(label);
    const ImVec2 p  = ImGui::GetCursorScreenPos();

    const bool clicked = ImGui::InvisibleButton("##crumb", sz);
    const bool hov     = ImGui::IsItemHovered();

    ImDrawList*  d = ImGui::GetWindowDrawList();
    const ImU32  u = ImGui::GetColorU32(hov ? col : WithAlpha(col, 0.78f));

    d->AddText(p, u, label);

    if (hov)
    {
        d->AddLine(ImVec2(p.x, p.y + sz.y), ImVec2(p.x + sz.x, p.y + sz.y), u, 1.0f);
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    ImGui::PopID();
    return clicked;
}

// Small "value + label" pill. Caller uses SameLine() between chips.
static void StatChip(const char* label, const char* value, const ImVec4& col)
{
    const ImVec2 vs = ImGui::CalcTextSize(value);
    const ImVec2 ls = ImGui::CalcTextSize(label);
    const float  h  = 30.0f;
    const float  w  = 14.0f + 10.0f + vs.x + 8.0f + ls.x + 14.0f;

    const ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList*  d = ImGui::GetWindowDrawList();

    d->AddRectFilled(p, ImVec2(p.x + w, p.y + h),
        ImGui::GetColorU32(WithAlpha(col, 0.12f)), h * 0.5f);
    d->AddRect(p, ImVec2(p.x + w, p.y + h),
        ImGui::GetColorU32(WithAlpha(col, 0.45f)), h * 0.5f);

    d->AddCircleFilled(ImVec2(p.x + 16.0f, p.y + h * 0.5f), 4.0f,
        ImGui::GetColorU32(col));

    const float ty = p.y + (h - vs.y) * 0.5f;

    d->AddText(ImVec2(p.x + 28.0f, ty), ImGui::GetColorU32(col), value);
    d->AddText(ImVec2(p.x + 28.0f + vs.x + 8.0f, ty),
        ImGui::GetColorU32(COLOR_MUTED), label);

    ImGui::Dummy(ImVec2(w, h));
}

// Friendly graphic for pages / tables with nothing to show
static void EmptyState(const char* title, const char* hint, const ImVec4& col)
{
    const float avail = ImGui::GetContentRegionAvail().x;
    const float t     = (float)ImGui::GetTime();

    ImGui::Dummy(ImVec2(0.0f, 26.0f));

    const ImVec2 p = ImGui::GetCursorScreenPos();
    const ImVec2 c(p.x + avail * 0.5f, p.y + 46.0f);
    ImDrawList*  d = ImGui::GetWindowDrawList();

    const float phase = std::fmod(t * 0.6f, 1.0f);

    DrawGlow(d, c, 74.0f, col);
    d->AddCircleFilled(c, 36.0f, ImGui::GetColorU32(WithAlpha(col, 0.12f)), 48);
    d->AddCircle(c, 36.0f, ImGui::GetColorU32(WithAlpha(col, 0.70f)), 48, 1.8f);
    d->AddCircle(c, 36.0f + phase * 18.0f,
        ImGui::GetColorU32(WithAlpha(col, 0.35f * (1.0f - phase))), 48, 1.4f);

    ImVec2 tick[3] = {
        ImVec2(c.x - 12.0f, c.y + 1.0f),
        ImVec2(c.x - 4.0f,  c.y + 9.0f),
        ImVec2(c.x + 13.0f, c.y - 9.0f)
    };
    d->AddPolyline(tick, 3, ImGui::GetColorU32(col), 0, 3.2f);

    ImGui::Dummy(ImVec2(avail, 100.0f));

    CenteredInContent(title, 1.25f, COLOR_TEXT);
    CenteredInContent(hint, 1.0f, COLOR_MUTED);
}

// Panel used by every inner page: rounded card with a coloured top line
// and a soft gradient. Always pair with ImGui::EndChild().
static void BeginPanel(const char* id, const ImVec2& size,
                       const ImVec4& accent, bool noScroll = false)
{
    ImGui::BeginChild(id, size, true,
        noScroll ? ImGuiWindowFlags_NoScrollbar : 0);

    const ImVec2 wp = ImGui::GetWindowPos();
    const ImVec2 ws = ImGui::GetWindowSize();
    ImDrawList*  d  = ImGui::GetWindowDrawList();

    d->AddRectFilledMultiColor(
        ImVec2(wp.x + 1.0f, wp.y + 3.0f),
        ImVec2(wp.x + ws.x - 1.0f, wp.y + std::min(ws.y, 150.0f)),
        ImGui::GetColorU32(WithAlpha(accent, 0.09f)),
        ImGui::GetColorU32(WithAlpha(accent, 0.09f)),
        ImGui::GetColorU32(WithAlpha(accent, 0.0f)),
        ImGui::GetColorU32(WithAlpha(accent, 0.0f)));

    d->AddRectFilled(
        ImVec2(wp.x + 16.0f, wp.y),
        ImVec2(wp.x + ws.x - 16.0f, wp.y + 2.5f),
        ImGui::GetColorU32(WithAlpha(accent, 0.90f)), 2.0f);
}

// Logo tile + product name. Returns true when clicked (-> welcome screen).
static bool DrawBrand(bool clickable = false)
{
    ImDrawList* d = ImGui::GetWindowDrawList();
    const ImVec2 p = ImGui::GetCursorScreenPos();
    const float  s = 40.0f;

    d->AddRectFilled(p, ImVec2(p.x + s, p.y + s),
        ImGui::GetColorU32(WithAlpha(COLOR_CYAN, 0.16f)), 10.0f);
    d->AddRect(p, ImVec2(p.x + s, p.y + s),
        ImGui::GetColorU32(WithAlpha(COLOR_CYAN, 0.65f)), 10.0f, 0, 1.2f);

    const ImVec2 c(p.x + s * 0.5f, p.y + s * 0.5f);
    ImVec2 shield[5] = {
        ImVec2(c.x - 8.0f, c.y - 9.0f), ImVec2(c.x + 8.0f, c.y - 9.0f),
        ImVec2(c.x + 8.0f, c.y + 1.0f), ImVec2(c.x,         c.y + 10.0f),
        ImVec2(c.x - 8.0f, c.y + 1.0f)
    };
    d->AddConvexPolyFilled(shield, 5, ImGui::GetColorU32(COLOR_CYAN));

    ImGui::Dummy(ImVec2(s, s));
    ImGui::SameLine(0.0f, 12.0f);

    ImGui::BeginGroup();
    ImGui::SetWindowFontScale(1.25f);
    ImGui::TextUnformatted("HIDS");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::TextDisabled("Security Platform");
    ImGui::EndGroup();

    if (clickable)
    {
        const ImVec2 mx = ImGui::GetItemRectMax();

        if (ImGui::IsWindowHovered() &&
            ImGui::IsMouseHoveringRect(p, ImVec2(mx.x, p.y + s)))
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            ImGui::SetTooltip("Back to the welcome screen");

            if (ImGui::IsMouseClicked(0))
                return true;
        }
    }

    return false;
}

// Right-aligned engine status + Start/Stop button
static void DrawHeaderStatus(float topY)
{
    const ImVec4 col = monitoringActive ? COLOR_GREEN : COLOR_YELLOW;
    const char* label = monitoringActive ? "Protection active" : "Protection stopped";

    const float textW = ImGui::CalcTextSize(label).x;
    const float total = 16.0f + 4.0f + textW + 14.0f + 84.0f;
    const float x = std::max(
        300.0f,
        ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x - total - 8.0f);

    ImGui::SetCursorPos(ImVec2(x, topY));
    StatusDot(col, monitoringActive);
    ImGui::SameLine(0.0f, 4.0f);
    ImGui::AlignTextToFramePadding();
    ImGui::TextColored(col, "%s", label);
    ImGui::SameLine(0.0f, 14.0f);

    if (monitoringActive)
    {
        PushRedButton();
        if (ImGui::Button("Stop", ImVec2(84.0f, 0.0f)))
            StopEngine();
        ImGui::PopStyleColor(3);
    }
    else
    {
        PushGreenButton();
        if (ImGui::Button("Start", ImVec2(84.0f, 0.0f)))
            StartEngine();
        ImGui::PopStyleColor(3);
    }
}

// Soft drifting glows + faint dot grid behind every screen
static void DrawAmbient()
{
    ImDrawList*  bg   = ImGui::GetBackgroundDrawList();
    const ImVec2 disp = ImGui::GetIO().DisplaySize;
    const float  t    = (float)ImGui::GetTime();

    const ImVec4 a = monitoringActive ? COLOR_GREEN : COLOR_CYAN;

    DrawGlow(bg,
        ImVec2(disp.x * (0.12f + 0.04f * std::sin(t * 0.20f)), disp.y * 0.05f),
        360.0f, WithAlpha(a, 0.55f));

    DrawGlow(bg,
        ImVec2(disp.x * (0.90f + 0.03f * std::sin(t * 0.17f + 1.0f)), disp.y * 0.95f),
        420.0f, WithAlpha(COLOR_BLUE, 0.50f));

    if (appScreen != SCREEN_WELCOME)
    {
        const float step = 46.0f;
        const ImU32 dot  = ImGui::GetColorU32(WithAlpha(COLOR_CYAN, 0.055f));

        for (float y = step * 0.5f; y < disp.y; y += step)
            for (float x = step * 0.5f; x < disp.x; x += step)
                bg->AddRectFilled(ImVec2(x, y), ImVec2(x + 2.0f, y + 2.0f), dot);
    }
}

// Header of a feature page: Back button, breadcrumb, icon, title.
// No sidebar - just this feature and the way back.
static void DrawPageHeader()
{
    const int    domain = DomainOfPage(selectedPage);
    const ImVec4 accent = DomainAccent(domain);

    ImGui::BeginChild("PageHeader", ImVec2(0.0f, 134.0f), true,
                      ImGuiWindowFlags_NoScrollbar);

    const ImVec2 wp = ImGui::GetWindowPos();
    const ImVec2 ws = ImGui::GetWindowSize();
    ImDrawList*  d  = ImGui::GetWindowDrawList();

    // Card background: gradient + accent line along the bottom
    d->AddRectFilledMultiColor(
        ImVec2(wp.x + 1.0f, wp.y + 1.0f), ImVec2(wp.x + ws.x - 1.0f, wp.y + ws.y - 1.0f),
        ImGui::GetColorU32(WithAlpha(accent, 0.16f)),
        ImGui::GetColorU32(WithAlpha(accent, 0.02f)),
        ImGui::GetColorU32(WithAlpha(accent, 0.0f)),
        ImGui::GetColorU32(WithAlpha(accent, 0.05f)));

    d->AddRectFilledMultiColor(
        ImVec2(wp.x + 16.0f, wp.y + ws.y - 3.0f),
        ImVec2(wp.x + ws.x - 16.0f, wp.y + ws.y - 1.0f),
        ImGui::GetColorU32(WithAlpha(accent, 0.90f)),
        ImGui::GetColorU32(WithAlpha(accent, 0.0f)),
        ImGui::GetColorU32(WithAlpha(accent, 0.0f)),
        ImGui::GetColorU32(WithAlpha(accent, 0.90f)));

    const float padX = ImGui::GetCursorPosX();
    const float topY = ImGui::GetCursorPosY();
    const float lh   = ImGui::GetTextLineHeight();

    // Row 1: Back + breadcrumb (all on one baseline)
    if (ImGui::Button("<  Back", ImVec2(96.0f, 32.0f)))
    {
        selectedDomain = domain;
        appScreen      = SCREEN_DOMAIN;
    }

    ImGui::SameLine(0.0f, 18.0f);
    ImGui::SetCursorPosY(topY + (32.0f - lh) * 0.5f);

    const ImVec4 link = COLOR_MUTED;

    if (CrumbLink("Welcome", link))
        appScreen = SCREEN_WELCOME;

    ImGui::SameLine(0.0f, 8.0f);
    ImGui::TextDisabled(">");
    ImGui::SameLine(0.0f, 8.0f);

    if (CrumbLink("Home", link))
        appScreen = SCREEN_HUB;

    ImGui::SameLine(0.0f, 8.0f);
    ImGui::TextDisabled(">");
    ImGui::SameLine(0.0f, 8.0f);

    if (CrumbLink(hidsDomains[domain].title, link))
    {
        selectedDomain = domain;
        appScreen      = SCREEN_DOMAIN;
    }

    ImGui::SameLine(0.0f, 8.0f);
    ImGui::TextDisabled(">");
    ImGui::SameLine(0.0f, 8.0f);
    ImGui::TextColored(accent, "%s", pages[selectedPage]);

    DrawHeaderStatus(topY);

    // Row 2: icon + title + subtitle
    {
        const ImVec2 ic(wp.x + padX + 30.0f, wp.y + topY + 46.0f + 31.0f);

        DrawGlow(d, ic, 52.0f, accent);
        d->AddCircleFilled(ic, 28.0f, ImGui::GetColorU32(WithAlpha(accent, 0.18f)), 48);
        d->AddCircle(ic, 28.0f, ImGui::GetColorU32(WithAlpha(accent, 0.80f)), 48, 1.6f);
        DrawPageIcon(d, ic, 13.0f, selectedPage, ImGui::GetColorU32(accent));
    }

    ImGui::SetCursorPos(ImVec2(padX + 74.0f, topY + 46.0f));
    ImGui::BeginGroup();
    ImGui::SetWindowFontScale(1.55f);
    ImGui::TextUnformatted(pages[selectedPage]);
    ImGui::SetWindowFontScale(1.0f);
    ImGui::TextDisabled("%s", pageSubtitles[selectedPage]);
    ImGui::EndGroup();

    ImGui::EndChild();
}

// Call-to-action shown on the dashboard only while protection is off
static void DrawEngineBanner()
{
    if (monitoringActive)
        return;

    const float btnW = 176.0f;

    BeginPanel("EngineBanner", ImVec2(0.0f, 104.0f), COLOR_YELLOW, true);

    const float wrapX = ImGui::GetWindowWidth() - btnW - 60.0f;

    ImGui::SetCursorPos(ImVec2(20.0f, 16.0f));
    ImGui::BeginGroup();
    ImGui::PushTextWrapPos(wrapX);
    ImGui::SetWindowFontScale(1.2f);
    ImGui::TextUnformatted("Protection is stopped");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::TextDisabled("Start the engine to collect host events, watch processes "
                        "and the monitored folder, and raise alerts.");
    ImGui::PopTextWrapPos();
    ImGui::EndGroup();

    ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - btnW - 20.0f, 32.0f));

    PushGreenButton();
    if (ImGui::Button("Start protection", ImVec2(btnW, 40.0f)))
        StartEngine();
    ImGui::PopStyleColor(3);

    ImGui::EndChild();
    ImGui::Spacing();
}

// Shown on the dashboard while protection runs: overall threat level
static void DrawPostureBanner()
{
    if (!monitoringActive)
        return;

    const int crit = CountSeverity("Critical");
    const int high = CountSeverity("High");
    const int med  = CountSeverity("Medium");

    const char* level = "Low";
    ImVec4      col   = COLOR_GREEN;

    if (crit > 0)      { level = "Critical"; col = COLOR_RED;    }
    else if (high > 0) { level = "Elevated"; col = COLOR_ORANGE; }
    else if (med > 0)  { level = "Guarded";  col = COLOR_YELLOW; }

    const double now = ImGui::GetTime();

    if (engineStartedAt < 0.0)
        engineStartedAt = now;

    BeginPanel("PostureBanner", ImVec2(0.0f, 98.0f), col, true);

    const ImVec2 wp = ImGui::GetWindowPos();
    const ImVec2 ws = ImGui::GetWindowSize();
    ImDrawList*  d  = ImGui::GetWindowDrawList();
    const float  t  = (float)now;

    const ImVec2 c(wp.x + 56.0f, wp.y + 50.0f);

    DrawGlow(d, c, 64.0f, col);
    d->AddCircleFilled(c, 34.0f, ImGui::GetColorU32(WithAlpha(col, 0.14f)), 48);
    d->AddCircle(c, 34.0f, ImGui::GetColorU32(WithAlpha(col, 0.55f)), 48, 1.4f);

    d->PathClear();
    d->PathArcTo(c, 40.0f, t * 1.6f, t * 1.6f + 1.6f, 24);
    d->PathStroke(ImGui::GetColorU32(col), 0, 2.5f);

    DrawShieldLogo(d, c, 16.0f, col);

    ImGui::SetCursorPos(ImVec2(112.0f, 18.0f));
    ImGui::BeginGroup();
    ImGui::TextDisabled("THREAT LEVEL");
    ImGui::SetWindowFontScale(1.6f);
    ImGui::TextColored(col, "%s", level);
    ImGui::SetWindowFontScale(1.0f);
    ImGui::EndGroup();

    char l1[64];
    char l2[96];

    std::snprintf(l1, sizeof(l1), "Monitoring for %s",
        FormatDuration(now - engineStartedAt).c_str());
    std::snprintf(l2, sizeof(l2), "%d alerts   -   %d processes watched",
        (int)alerts.size(), processCount);

    ImGui::SetCursorPos(ImVec2(
        std::max(240.0f, ws.x - ImGui::CalcTextSize(l1).x - 24.0f), 26.0f));
    ImGui::TextColored(COLOR_GREEN, "%s", l1);

    ImGui::SetCursorPos(ImVec2(
        std::max(240.0f, ws.x - ImGui::CalcTextSize(l2).x - 24.0f), 52.0f));
    ImGui::TextDisabled("%s", l2);

    ImGui::EndChild();
    ImGui::Spacing();
}

// ------------------------------------------------------------
// WELCOME SCREEN
// Reachable at any time. Shows whether HIDS is working.
// ------------------------------------------------------------
static void DrawWelcomeScreen()
{
    RefreshFromBackend();

    const double now  = ImGui::GetTime();
    const ImVec2 disp = ImGui::GetIO().DisplaySize;
    const float  t    = (float)now;
    ImDrawList*  bg   = ImGui::GetBackgroundDrawList();
    const ImVec2 c(disp.x * 0.5f, disp.y * 0.27f);

    const ImVec4 acc = monitoringActive ? COLOR_GREEN : COLOR_CYAN;

    if (monitoringActive && engineStartedAt < 0.0)
        engineStartedAt = now;

    // Floating particles
    for (int i = 0; i < 46; ++i)
    {
        const float fx = std::fmod(i * 0.6180339f, 1.0f);
        const float fy = std::fmod(i * 0.3819660f + t * (0.006f + 0.004f * (i % 4)), 1.0f);
        const float a  = 0.08f + 0.08f * (0.5f + 0.5f * std::sin(t * 0.8f + i));

        bg->AddCircleFilled(ImVec2(fx * disp.x, fy * disp.y), 1.2f + (i % 3),
            ImGui::GetColorU32(WithAlpha(acc, a)));
    }

    // Soft glow + expanding radar rings
    DrawGlow(bg, c, 170.0f, acc);

    for (int k = 0; k < 3; ++k)
    {
        const float phase  = std::fmod(t * 0.30f + k / 3.0f, 1.0f);
        const float radius = 66.0f + phase * 190.0f;

        bg->AddCircle(c, radius,
            ImGui::GetColorU32(WithAlpha(acc, 0.28f * (1.0f - phase))), 96, 1.6f);
    }

    // Logo badge + rotating arc
    bg->AddCircleFilled(c, 64.0f, ImGui::GetColorU32(WithAlpha(acc, 0.12f)), 64);
    bg->AddCircle(c, 64.0f, ImGui::GetColorU32(WithAlpha(acc, 0.55f)), 64, 1.5f);

    bg->PathClear();
    bg->PathArcTo(c, 72.0f, t * 1.6f, t * 1.6f + 1.9f, 32);
    bg->PathStroke(ImGui::GetColorU32(acc), 0, 3.0f);

    DrawShieldLogo(bg, c, 30.0f, acc);

    // Text block
    ImGui::SetCursorPosY(c.y + 100.0f);

    CenteredText("HIDS", 3.0f, COLOR_TEXT);
    CenteredText("Host Intrusion Detection System", 1.35f, COLOR_CYAN);
    ImGui::Dummy(ImVec2(0.0f, 4.0f));

    if (monitoringActive)
    {
        // ---- HIDS is working ----
        const char* label = "HIDS IS WORKING  -  this host is being monitored";
        const float tw    = ImGui::CalcTextSize(label).x;
        const float pillW = tw + 56.0f;
        const float pillH = 34.0f;

        ImGui::SetCursorPosX(std::max(0.0f, (ImGui::GetWindowWidth() - pillW) * 0.5f));

        const ImVec2 p = ImGui::GetCursorScreenPos();
        ImDrawList*  d = ImGui::GetWindowDrawList();
        const float  wave = 0.5f + 0.5f * std::sin(t * 3.0f);

        d->AddRectFilled(p, ImVec2(p.x + pillW, p.y + pillH),
            ImGui::GetColorU32(WithAlpha(COLOR_GREEN, 0.14f)), pillH * 0.5f);
        d->AddRect(p, ImVec2(p.x + pillW, p.y + pillH),
            ImGui::GetColorU32(WithAlpha(COLOR_GREEN, 0.70f)), pillH * 0.5f, 0, 1.4f);
        d->AddCircleFilled(ImVec2(p.x + 20.0f, p.y + pillH * 0.5f),
            9.0f + wave * 2.0f,
            ImGui::GetColorU32(WithAlpha(COLOR_GREEN, 0.20f * (1.0f - wave) + 0.05f)));
        d->AddCircleFilled(ImVec2(p.x + 20.0f, p.y + pillH * 0.5f), 5.0f,
            ImGui::GetColorU32(COLOR_GREEN));
        d->AddText(ImVec2(p.x + 36.0f, p.y + (pillH - ImGui::GetTextLineHeight()) * 0.5f),
            ImGui::GetColorU32(COLOR_GREEN), label);

        ImGui::Dummy(ImVec2(pillW, pillH));
        ImGui::Dummy(ImVec2(0.0f, 4.0f));

        char line[200];
        std::snprintf(line, sizeof(line),
            "CPU %.0f%%      RAM %.0f%%      Processes %d      Alerts %d      Running for %s",
            cpuNow, ramNow, processCount, (int)alerts.size(),
            FormatDuration(now - engineStartedAt).c_str());

        CenteredText(line, 1.0f, WithAlpha(COLOR_CYAN, 0.9f));
    }
    else
    {
        CenteredText("Watch processes, sign-ins and files. Catch threats early.",
            1.0f, COLOR_MUTED);
    }

    ImGui::Dummy(ImVec2(0.0f, 22.0f));

    // ---- Buttons ----
    bool openConsole = false;

    const ImVec2 mainBtn(monitoringActive ? 250.0f : 280.0f, 58.0f);
    const ImVec2 stopBtn(210.0f, 58.0f);
    const float  gapBtn = 16.0f;
    const float  rowW   = monitoringActive ? mainBtn.x + gapBtn + stopBtn.x : mainBtn.x;

    ImGui::SetCursorPosX(std::max(0.0f, (ImGui::GetWindowWidth() - rowW) * 0.5f));

    PushGreenButton();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 29.0f);
    ImGui::SetWindowFontScale(1.25f);

    const bool pressed =
        ImGui::Button(monitoringActive ? "Open console" : "Start HIDS", mainBtn);

    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);

    // Pulsing halo around the main button
    {
        const ImVec2 a = ImGui::GetItemRectMin();
        const ImVec2 b = ImGui::GetItemRectMax();
        const float  w = 0.5f + 0.5f * std::sin(t * 2.5f);

        ImGui::GetWindowDrawList()->AddRect(
            ImVec2(a.x - 3.0f - w * 4.0f, a.y - 3.0f - w * 4.0f),
            ImVec2(b.x + 3.0f + w * 4.0f, b.y + 3.0f + w * 4.0f),
            ImGui::GetColorU32(WithAlpha(COLOR_GREEN, 0.40f * (1.0f - w) + 0.05f)),
            34.0f, 0, 2.0f);

        if (ImGui::IsItemHovered())
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    if (pressed)
    {
        if (!monitoringActive)
            StartEngine();

        openConsole = true;
    }

    if (monitoringActive)
    {
        ImGui::SameLine(0.0f, gapBtn);

        PushRedButton();
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 29.0f);
        ImGui::SetWindowFontScale(1.25f);

        if (ImGui::Button("Stop protection", stopBtn))
            StopEngine();            // stays on the welcome screen

        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);
    }

    ImGui::Dummy(ImVec2(0.0f, 6.0f));

    // Skip link (only while protection is off)
    if (!monitoringActive)
    {
        const char* skip = "Open dashboard without starting protection";
        const float w = ImGui::CalcTextSize(skip).x;

        ImGui::SetCursorPosX(std::max(0.0f, (ImGui::GetWindowWidth() - w) * 0.5f));
        ImGui::TextDisabled("%s", skip);

        if (ImGui::IsItemHovered())
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        if (ImGui::IsItemClicked())
            openConsole = true;
    }

    if (openConsole)
        appScreen = SCREEN_HUB;

    // Footer
    {
        const float y = ImGui::GetWindowHeight() - 56.0f;

        if (ImGui::GetCursorPosY() < y)
            ImGui::SetCursorPosY(y);

        CenteredText("Monitoring   -   Threat Detection   -   Analysis   -   Reports   -   Configuration",
            1.0f, WithAlpha(COLOR_MUTED, 0.8f));
        CenteredText("HIDS Prototype v2.0", 0.9f, WithAlpha(COLOR_MUTED, 0.6f));
    }
}

// ------------------------------------------------------------
// Shared card used by the hub and the domain screens.
// ------------------------------------------------------------
static bool HubCard(
    const char* id, const char* title, const char* desc, const char* footer,
    int iconPage, const ImVec4& accent, const ImVec2& size)
{
    bool clicked = false;

    ImGui::PushID(id);
    ImGui::BeginChild("card", size, true, ImGuiWindowFlags_NoScrollbar);

    const ImVec2 wp = ImGui::GetWindowPos();
    const ImVec2 ws = ImGui::GetWindowSize();
    ImDrawList*  d  = ImGui::GetWindowDrawList();
    const bool   hov = ImGui::IsWindowHovered();
    const float  rounding = ImGui::GetStyle().ChildRounding;

    // Soft tint fading down from the top
    d->AddRectFilledMultiColor(
        ImVec2(wp.x + 1.0f, wp.y + 4.0f),
        ImVec2(wp.x + ws.x - 1.0f, wp.y + ws.y * 0.60f),
        ImGui::GetColorU32(WithAlpha(accent, hov ? 0.20f : 0.11f)),
        ImGui::GetColorU32(WithAlpha(accent, hov ? 0.20f : 0.11f)),
        ImGui::GetColorU32(WithAlpha(accent, 0.0f)),
        ImGui::GetColorU32(WithAlpha(accent, 0.0f)));

    // Decorative rings in the top-right corner
    for (int k = 0; k < 3; ++k)
        d->AddCircle(ImVec2(wp.x + ws.x - 26.0f, wp.y + 30.0f),
            34.0f + k * 22.0f,
            ImGui::GetColorU32(WithAlpha(accent, 0.10f - k * 0.025f)), 64, 1.2f);

    // Accent bar across the top
    d->AddRectFilled(wp, ImVec2(wp.x + ws.x, wp.y + 4.0f),
        ImGui::GetColorU32(accent), rounding, ImDrawFlags_RoundCornersTop);

    // Icon badge
    const ImVec2 ic(wp.x + 22.0f + 32.0f, wp.y + 26.0f + 32.0f);

    DrawGlow(d, ic, hov ? 82.0f : 62.0f, accent);
    d->AddCircleFilled(ic, 32.0f,
        ImGui::GetColorU32(WithAlpha(accent, hov ? 0.32f : 0.16f)), 48);
    d->AddCircle(ic, 32.0f,
        ImGui::GetColorU32(WithAlpha(accent, 0.75f)), 48, 1.6f);
    DrawPageIcon(d, ic, 15.0f, iconPage, ImGui::GetColorU32(accent));

    // Text (wrapped inside the card so it never overlaps the footer)
    ImGui::SetCursorPos(ImVec2(18.0f, 26.0f + 64.0f + 14.0f));
    ImGui::BeginGroup();
    ImGui::PushTextWrapPos(ws.x - 18.0f);

    ImGui::SetWindowFontScale(1.25f);
    ImGui::TextUnformatted(title);
    ImGui::SetWindowFontScale(1.0f);

    ImGui::PushStyleColor(ImGuiCol_Text, COLOR_MUTED);
    ImGui::TextUnformatted(desc);
    ImGui::PopStyleColor();

    ImGui::PopTextWrapPos();
    ImGui::EndGroup();

    // Footer: label on the left, arrow badge on the right
    ImGui::SetCursorPos(ImVec2(18.0f, ws.y - 38.0f));
    ImGui::TextColored(accent, "%s", footer);

    {
        const ImVec2 ac(wp.x + ws.x - 30.0f, wp.y + ws.y - 28.0f);

        d->AddCircleFilled(ac, 13.0f,
            ImGui::GetColorU32(WithAlpha(accent, hov ? 0.40f : 0.18f)), 32);
        d->AddCircle(ac, 13.0f, ImGui::GetColorU32(WithAlpha(accent, 0.70f)), 32, 1.2f);

        const float nx = hov ? 1.5f : 0.0f;
        ImVec2 arrow[3] = {
            ImVec2(ac.x - 2.5f + nx, ac.y - 5.0f),
            ImVec2(ac.x + 3.0f + nx, ac.y),
            ImVec2(ac.x - 2.5f + nx, ac.y + 5.0f)
        };
        d->AddPolyline(arrow, 3, ImGui::GetColorU32(accent), 0, 2.0f);
    }

    // Hover outline + click
    if (hov)
    {
        d->AddRect(wp, ImVec2(wp.x + ws.x, wp.y + ws.y),
            ImGui::GetColorU32(WithAlpha(accent, 0.90f)), rounding, 0, 1.8f);

        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        if (ImGui::IsMouseClicked(0))
            clicked = true;
    }

    ImGui::EndChild();
    ImGui::PopID();

    return clicked;
}

// "HIDS IS WORKING / STOPPED" pill (hub)
static void DrawStatusPill()
{
    const ImVec4 col   = monitoringActive ? COLOR_GREEN : COLOR_YELLOW;
    const char*  label = monitoringActive ? "HIDS IS WORKING" : "HIDS IS STOPPED";
    const float  pillH = 30.0f;
    const float  pillW = ImGui::CalcTextSize(label).x + 56.0f;

    ImGui::SetCursorPosX(std::max(0.0f, (ImGui::GetWindowWidth() - pillW) * 0.5f));

    const ImVec2 p    = ImGui::GetCursorScreenPos();
    ImDrawList*  d    = ImGui::GetWindowDrawList();
    const float  wave = 0.5f + 0.5f * std::sin((float)ImGui::GetTime() * 3.0f);
    const ImVec2 dot(p.x + 18.0f, p.y + pillH * 0.5f);

    d->AddRectFilled(p, ImVec2(p.x + pillW, p.y + pillH),
        ImGui::GetColorU32(WithAlpha(col, 0.14f)), pillH * 0.5f);
    d->AddRect(p, ImVec2(p.x + pillW, p.y + pillH),
        ImGui::GetColorU32(WithAlpha(col, 0.70f)), pillH * 0.5f, 0, 1.4f);

    if (monitoringActive)
        d->AddCircleFilled(dot, 8.0f + wave * 3.0f,
            ImGui::GetColorU32(WithAlpha(col, 0.20f * (1.0f - wave) + 0.05f)));

    d->AddCircleFilled(dot, 5.0f, ImGui::GetColorU32(col));
    d->AddText(ImVec2(p.x + 34.0f, p.y + (pillH - ImGui::GetTextLineHeight()) * 0.5f),
        ImGui::GetColorU32(col), label);

    ImGui::Dummy(ImVec2(pillW, pillH));
}

// ------------------------------------------------------------
// HOME HUB - one big card per security domain + live metrics
// ------------------------------------------------------------
static void DrawHubScreen()
{
    RefreshFromBackend();

    const float topY = ImGui::GetCursorPosY();

    if (DrawBrand(true))
        appScreen = SCREEN_WELCOME;

    ImGui::SameLine(0.0f, 28.0f);
    ImGui::SetCursorPosY(topY + 4.0f);

    if (ImGui::Button("<  Welcome screen", ImVec2(180.0f, 34.0f)))
        appScreen = SCREEN_WELCOME;

    DrawHeaderStatus(topY + 4.0f);

    ImGui::SetCursorPosY(topY + 64.0f);
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 12.0f));

    CenteredText("Security Console", 2.0f, COLOR_TEXT);
    CenteredText("Choose an area to explore", 1.1f, COLOR_MUTED);
    ImGui::Dummy(ImVec2(0.0f, 6.0f));
    DrawStatusPill();
    ImGui::Dummy(ImVec2(0.0f, 14.0f));

    // Card grid
    const int   count = IM_ARRAYSIZE(hidsDomains);
    const float gap   = 16.0f;
    const float avail = ImGui::GetContentRegionAvail().x;
    const int   cols  = (avail >= 1100.0f) ? 5 : 3;
    const float cardW = (avail - gap * (cols - 1)) / cols;
    const float cardH = (cols == 5) ? 280.0f : 215.0f;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(gap, gap));

    for (int i = 0; i < count; ++i)
    {
        if (i % cols != 0)
            ImGui::SameLine();

        const HidsDomainRoute& dom = hidsDomains[i];

        char footer[48];
        std::snprintf(footer, sizeof(footer), "%d feature%s",
            dom.featureCount, dom.featureCount == 1 ? "" : "s");

        if (HubCard(dom.title, dom.title, dom.description, footer,
                domainIconPage[i], DomainAccent(i), ImVec2(cardW, cardH)))
        {
            selectedDomain = i;
            appScreen      = SCREEN_DOMAIN;
        }
    }

    ImGui::PopStyleVar();

    ImGui::Dummy(ImVec2(0.0f, 4.0f));

    // Live metrics strip (only if there is room; otherwise a text summary)
    if (ImGui::GetContentRegionAvail().y >= 210.0f)
    {
        ImGui::TextDisabled("LIVE STATUS");
        DrawMetrics();
    }
    else
    {
        char line[160];
        std::snprintf(line, sizeof(line),
            "CPU %.0f%%      RAM %.0f%%      Processes %d      Alerts %d",
            cpuNow, ramNow, processCount, (int)alerts.size());

        CenteredText(line, 1.0f, WithAlpha(COLOR_CYAN, 0.9f));
    }

    FooterHint("Click an area to open it   -   Esc goes back to the welcome screen");
}

// ------------------------------------------------------------
// DOMAIN SCREEN - the features inside one domain
// ------------------------------------------------------------
static void DrawDomainScreen()
{
    RefreshFromBackend();

    const HidsDomainRoute& dom = hidsDomains[selectedDomain];
    const ImVec4 accent = DomainAccent(selectedDomain);

    const float topY = ImGui::GetCursorPosY();
    const float lh   = ImGui::GetTextLineHeight();

    if (ImGui::Button("<  Home", ImVec2(100.0f, 34.0f)))
        appScreen = SCREEN_HUB;

    ImGui::SameLine(0.0f, 18.0f);
    ImGui::SetCursorPosY(topY + (34.0f - lh) * 0.5f);

    if (CrumbLink("Welcome", COLOR_MUTED))
        appScreen = SCREEN_WELCOME;

    ImGui::SameLine(0.0f, 8.0f);
    ImGui::TextDisabled(">");
    ImGui::SameLine(0.0f, 8.0f);

    if (CrumbLink("Home", COLOR_MUTED))
        appScreen = SCREEN_HUB;

    ImGui::SameLine(0.0f, 8.0f);
    ImGui::TextDisabled(">");
    ImGui::SameLine(0.0f, 8.0f);
    ImGui::TextColored(accent, "%s", dom.title);

    DrawHeaderStatus(topY + 1.0f);

    ImGui::SetCursorPosY(topY + 48.0f);
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 16.0f));

    // Domain banner
    {
        const ImVec2 p = ImGui::GetCursorScreenPos();
        const ImVec2 ic(p.x + 36.0f, p.y + 36.0f);
        ImDrawList*  d = ImGui::GetWindowDrawList();

        DrawGlow(d, ic, 70.0f, accent);
        d->AddCircleFilled(ic, 34.0f, ImGui::GetColorU32(WithAlpha(accent, 0.18f)), 48);
        d->AddCircle(ic, 34.0f, ImGui::GetColorU32(WithAlpha(accent, 0.75f)), 48, 1.6f);
        DrawPageIcon(d, ic, 16.0f, domainIconPage[selectedDomain], ImGui::GetColorU32(accent));

        ImGui::Dummy(ImVec2(72.0f, 72.0f));
        ImGui::SameLine(0.0f, 16.0f);

        ImGui::BeginGroup();
        ImGui::SetWindowFontScale(1.8f);
        ImGui::TextUnformatted(dom.title);
        ImGui::SetWindowFontScale(1.0f);
        ImGui::TextDisabled("%s", dom.description);
        ImGui::EndGroup();
    }

    ImGui::Dummy(ImVec2(0.0f, 20.0f));

    // Feature cards
    const float gap   = 16.0f;
    const float avail = ImGui::GetContentRegionAvail().x;
    const int   cols  = 3;
    const float cardW = (avail - gap * (cols - 1)) / cols;
    const float cardH = 240.0f;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(gap, gap));

    for (int i = 0; i < dom.featureCount; ++i)
    {
        if (i % cols != 0)
            ImGui::SameLine();

        const HidsFeatureRoute& f = dom.features[i];

        char footer[48];

        if (f.page == 4)
            std::snprintf(footer, sizeof(footer), "%d alert%s", (int)alerts.size(),
                alerts.size() == 1 ? "" : "s");
        else if (f.page == 1)
            std::snprintf(footer, sizeof(footer), "%d processes", processCount);
        else
            std::snprintf(footer, sizeof(footer), "Open");

        if (HubCard(f.title, f.title, f.description, footer,
                f.page, accent, ImVec2(cardW, cardH)))
        {
            selectedPage = f.page;
            appScreen    = SCREEN_PAGE;
        }
    }

    ImGui::PopStyleVar();

    FooterHint("Click a feature to open it   -   Esc goes back");
}

// ------------------------------------------------------------
// Entry point for every screen that is NOT a feature page
// ------------------------------------------------------------
static void DrawFullScreen()
{
    const float fade = ClampFloat(
        (float)((ImGui::GetTime() - screenChangedAt) / 0.25), 0.0f, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, fade);

    if (appScreen == SCREEN_WELCOME)     DrawWelcomeScreen();
    else if (appScreen == SCREEN_HUB)    DrawHubScreen();
    else if (appScreen == SCREEN_DOMAIN) DrawDomainScreen();

    ImGui::PopStyleVar();
}

// ============================================================
// DASHBOARD WIDGETS
// ============================================================

static void DrawThreatPanel()
{
    BeginPanel("ThreatPanel", ImVec2(0.0f, 222.0f), COLOR_CYAN, true);

    SectionHeading("Threat activity",
        "new alerts per second, weighted by severity (last 60 s)");

    float peak = 6.0f;
    for (float v : threatHistory)
        peak = std::max(peak, v);

    const float leftW = ImGui::GetContentRegionAvail().x * 0.62f;

    DrawBarChart("##threat", threatHistory, HISTORY_SIZE, historyOffset,
        peak, ImVec2(leftW, 126.0f));

    ImGui::SameLine();
    ImGui::BeginGroup();

    static const char* names[] = { "Critical", "High", "Medium", "Low" };
    const int total = std::max(1, (int)alerts.size());

    for (int i = 0; i < 4; ++i)
    {
        const int count = CountSeverity(names[i]);

        char label[40];
        std::snprintf(label, sizeof(label), "%s  %d", names[i], count);

        ImGui::PushID(i);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram,
            WithAlpha(SeverityColor(names[i]), 0.85f));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_FRAME);

        ImGui::ProgressBar((float)count / (float)total,
            ImVec2(ImGui::GetContentRegionAvail().x, 22.0f), label);

        ImGui::PopStyleColor(2);

        if (ImGui::IsItemHovered())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            ImGui::SetTooltip("Show %s alerts", names[i]);
        }

        if (ImGui::IsItemClicked())
        {
            severityFilter = i + 1;
            OpenPage(4);               // Alerts
        }

        ImGui::PopID();
    }

    ImGui::EndGroup();
    ImGui::EndChild();
}

// ------------------------------------------------------------
// PASSWORD LOCK (Logs & Reports page and End Task)
// ------------------------------------------------------------

static const double LOCK_IDLE_SECONDS = 300.0;     // auto-lock after 5 min idle
static const int    LOGS_PAGE_INDEX   = 5;

static bool        logsUnlocked   = false;
static double      logsLastActive = 0.0;
static char        lockOld[64]     = "";
static char        lockNew[64]     = "";
static char        lockConfirm[64] = "";
static char        lockEntry[64]   = "";
static std::string lockMessage;
static bool        lockMessageOk   = false;

static void Wipe(char* buffer, std::size_t size)
{
    std::memset(buffer, 0, size);
}

// Locks Logs & Reports again and clears typed credentials
static void LockLogs()
{
    logsUnlocked = false;
    lockMessage.clear();
    Wipe(lockOld, sizeof(lockOld));
    Wipe(lockNew, sizeof(lockNew));
    Wipe(lockConfirm, sizeof(lockConfirm));
    Wipe(lockEntry, sizeof(lockEntry));
}

// Detects screen / page changes: drives the fade and re-locks Logs & Reports
static void TrackScreenChange()
{
    static int prevScreen = -1;
    static int prevPage   = -1;

    const bool changed =
        appScreen != prevScreen ||
        (appScreen == SCREEN_PAGE && selectedPage != prevPage);

    if (!changed)
        return;

    if (prevScreen == SCREEN_PAGE && prevPage == LOGS_PAGE_INDEX)
        LockLogs();

    prevScreen      = appScreen;
    prevPage        = selectedPage;
    screenChangedAt = ImGui::GetTime();
}

// Esc: page -> domain -> hub -> welcome
static void HandleBackKey()
{
    if (!ImGui::IsKeyPressed(ImGuiKey_Escape, false))
        return;

    if (ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId) ||
        ImGui::IsAnyItemActive())
        return;

    if (appScreen == SCREEN_PAGE)
    {
        selectedDomain = DomainOfPage(selectedPage);
        appScreen      = SCREEN_DOMAIN;
    }
    else if (appScreen == SCREEN_DOMAIN)
    {
        appScreen = SCREEN_HUB;
    }
    else if (appScreen == SCREEN_HUB)
    {
        appScreen = SCREEN_WELCOME;
    }
}

// "Create a password" form. Returns true on the frame it is created.
static bool DrawSetPasswordForm()
{
    bool created = false;

    ImGui::TextDisabled("Create a password (at least 6 characters)");

    ImGui::SetNextItemWidth(260.0f);
    ImGui::InputTextWithHint("##newpw", "New password",
        lockNew, sizeof(lockNew), ImGuiInputTextFlags_Password);

    ImGui::SetNextItemWidth(260.0f);
    ImGui::InputTextWithHint("##confpw", "Confirm password",
        lockConfirm, sizeof(lockConfirm), ImGuiInputTextFlags_Password);

    if (ImGui::Button("Set Password"))
    {
        if (std::strcmp(lockNew, lockConfirm) != 0)
        {
            lockMessage   = "Passwords do not match.";
            lockMessageOk = false;
        }
        else
        {
            std::string message;

            if (backend.setPassword(lockNew, "", message))
            {
                created = true;
                Wipe(lockNew, sizeof(lockNew));
                Wipe(lockConfirm, sizeof(lockConfirm));
            }

            lockMessage   = message;
            lockMessageOk = created;
        }
    }

    return created;
}

static void DrawLockScreen()
{
    BeginPanel("LockScreen", ImVec2(0.0f, 0.0f), COLOR_GREEN);

    ImGui::Dummy(ImVec2(0.0f, 8.0f));

    // Padlock graphic on the left, form on the right
    {
        const ImVec2 p = ImGui::GetCursorScreenPos();
        const ImVec2 c(p.x + 48.0f, p.y + 46.0f);
        ImDrawList*  d = ImGui::GetWindowDrawList();

        DrawGlow(d, c, 80.0f, COLOR_GREEN);
        d->AddCircleFilled(c, 44.0f, ImGui::GetColorU32(WithAlpha(COLOR_GREEN, 0.12f)), 48);
        d->AddCircle(c, 44.0f, ImGui::GetColorU32(WithAlpha(COLOR_GREEN, 0.60f)), 48, 1.6f);
        DrawLockIcon(d, ImVec2(c.x, c.y - 4.0f), 18.0f, COLOR_GREEN);

        ImGui::Dummy(ImVec2(96.0f, 96.0f));
        ImGui::SameLine(0.0f, 18.0f);
    }

    ImGui::BeginGroup();

    SectionHeading("Logs & Reports are locked",
        "enter the password to see the detection signatures");

    if (!backend.isLockConfigured())
    {
        if (DrawSetPasswordForm())
            logsUnlocked = true;          // whoever creates it is let in
    }
    else
    {
        const int wait = backend.lockoutSecondsLeft();

        ImGui::BeginDisabled(wait > 0);

        ImGui::SetNextItemWidth(260.0f);
        const bool enter = ImGui::InputTextWithHint("##unlockpw", "Password",
            lockEntry, sizeof(lockEntry),
            ImGuiInputTextFlags_Password | ImGuiInputTextFlags_EnterReturnsTrue);

        ImGui::SameLine();
        const bool click = ImGui::Button("Unlock");

        ImGui::EndDisabled();

        if (wait > 0)
            ImGui::TextColored(COLOR_YELLOW, "Locked. Try again in %d s.", wait);

        if ((enter || click) && wait == 0)
        {
            std::string message;

            if (backend.verifyPassword(lockEntry, message))
            {
                logsUnlocked = true;
                lockMessage.clear();
            }
            else
            {
                lockMessage   = message;
                lockMessageOk = false;
            }

            Wipe(lockEntry, sizeof(lockEntry));
        }
    }

    if (!lockMessage.empty())
        ImGui::TextColored(lockMessageOk ? COLOR_GREEN : COLOR_RED,
            "%s", lockMessage.c_str());

    ImGui::EndGroup();

    ImGui::EndChild();
}

// true = unlocked, the page may draw its content
static bool LogsGate()
{
    const double now = ImGui::GetTime();
    ImGuiIO& io = ImGui::GetIO();

    const bool active =
        io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f ||
        io.MouseWheel != 0.0f || ImGui::IsMouseClicked(0) ||
        ImGui::IsAnyItemActive();

    if (active || !logsUnlocked)
        logsLastActive = now;

    if (logsUnlocked && now - logsLastActive > LOCK_IDLE_SECONDS)
    {
        logsUnlocked  = false;
        lockMessage   = "Locked after 5 minutes of inactivity.";
        lockMessageOk = false;
    }

    if (logsUnlocked)
        return true;

    DrawLockScreen();
    return false;
}

// ------------------------------------------------------------
// END TASK (shared by the Processes page and resource alerts)
// ------------------------------------------------------------

static const float HIGH_MEMORY_MB = 500.0f;

static bool        procAppsOnly     = false;
static bool        procHighMemOnly  = false;
static std::string procMessage;                 // result of the last End Task
static bool        procMessageOk    = true;
static int         pendingPid       = 0;
static std::string pendingName;
static float       pendingMem       = 0.0f;
static int         pendingAlertID   = 0;        // 0 = started from the Processes page
static bool        openEndTaskPopup = false;
static std::set<int> handledAlertIDs;           // alerts whose app was ended

static char        endPassword[64] = "";
static std::string endPopupMessage;

static void DrawEndTaskPopup()
{
    if (openEndTaskPopup)
    {
        ImGui::OpenPopup("Confirm End Task");
        openEndTaskPopup = false;

        Wipe(endPassword, sizeof(endPassword));
        endPopupMessage.clear();
        lockMessage.clear();
    }

    const ImVec2 screen = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos(
        ImVec2(screen.x * 0.5f, screen.y * 0.5f),
        ImGuiCond_Appearing,
        ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Confirm End Task", nullptr,
            ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("End %s (PID %d)?", pendingName.c_str(), pendingPid);
        ImGui::TextDisabled("It is using %.0f MB. Unsaved work in this app will be lost.",
            pendingMem);
        ImGui::Spacing();

        if (!backend.isLockConfigured())
        {
            ImGui::TextColored(COLOR_YELLOW,
                "Ending tasks is password protected. Create a password first.");
            ImGui::Spacing();

            DrawSetPasswordForm();

            if (!lockMessage.empty())
                ImGui::TextColored(lockMessageOk ? COLOR_GREEN : COLOR_RED, "%s", lockMessage.c_str());
        }
        else
        {
            const int wait = backend.lockoutSecondsLeft();

            ImGui::BeginDisabled(wait > 0);

            ImGui::SetNextItemWidth(260.0f);
            const bool enter = ImGui::InputTextWithHint("##endpw", "Password",
                endPassword, sizeof(endPassword),
                ImGuiInputTextFlags_Password | ImGuiInputTextFlags_EnterReturnsTrue);

            ImGui::EndDisabled();

            if (wait > 0)
                ImGui::TextColored(COLOR_YELLOW, "Locked. Try again in %d s.", wait);

            if (!endPopupMessage.empty())
                ImGui::TextColored(COLOR_RED, "%s", endPopupMessage.c_str());

            ImGui::Spacing();

            ImGui::BeginDisabled(wait > 0);
            const bool click = ImGui::Button("End Task", ImVec2(120.0f, 0.0f));
            ImGui::EndDisabled();

            if ((enter || click) && wait == 0)
            {
                std::string authMessage;
                const bool authorised = backend.verifyPassword(endPassword, authMessage);

                Wipe(endPassword, sizeof(endPassword));

                if (!authorised)
                {
                    endPopupMessage = authMessage;
                }
                else
                {
                    std::string result;

                    procMessageOk = backend.endTask(pendingPid, result, pendingName);
                    procMessage   = result;

                    if (procMessageOk && pendingAlertID != 0)
                        handledAlertIDs.insert(pendingAlertID);

                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::SameLine();
        }

        if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
        {
            Wipe(endPassword, sizeof(endPassword));
            Wipe(lockNew, sizeof(lockNew));
            Wipe(lockConfirm, sizeof(lockConfirm));
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

// ------------------------------------------------------------
// ALERT TABLE
// ------------------------------------------------------------

static void DrawAlerts(bool compact)
{
    BeginPanel("AlertsPanel", ImVec2(0.0f, compact ? 300.0f : 0.0f), COLOR_ORANGE);

    SectionHeading(
        compact ? "Recent security alerts" : "All security alerts"
    );

    // Severity filter (with counts) + search (full page only)
    if (!compact)
    {
        for (int i = 0; i < IM_ARRAYSIZE(severityFilterLabels); ++i)
        {
            const bool active = (severityFilter == i);
            const int  count  = (i == 0) ? (int)alerts.size()
                                         : CountSeverity(severityFilterLabels[i]);
            const ImVec4 tone = (i == 0) ? COLOR_BLUE
                                         : SeverityColor(severityFilterLabels[i]);

            char text[48];
            std::snprintf(text, sizeof(text), "%s  %d", severityFilterLabels[i], count);

            ImGui::PushStyleColor(ImGuiCol_Button,
                active ? WithAlpha(tone, 0.60f) : COLOR_FRAME);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, WithAlpha(tone, 0.45f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  WithAlpha(tone, 0.75f));

            if (ImGui::Button(text, ImVec2(112.0f, 0.0f)))
                severityFilter = i;

            ImGui::PopStyleColor(3);

            if (i + 1 < IM_ARRAYSIZE(severityFilterLabels))
                ImGui::SameLine();
        }

        ImGui::SameLine();
        ImGui::SetNextItemWidth(260.0f);
        ImGui::InputTextWithHint("##alertsearch", "Search alerts",
            alertSearch, IM_ARRAYSIZE(alertSearch));

        ImGui::Spacing();
    }

    if (!procMessage.empty())
        ImGui::TextColored(procMessageOk ? COLOR_GREEN : COLOR_RED,
            "%s", procMessage.c_str());

    DrawEndTaskPopup();

    // Rows that pass the filters
    std::vector<const AlertInfo*> rows;

    for (const AlertInfo& alert : alerts)
    {
        if (!compact && severityFilter != 0 &&
            alert.severity != severityFilterLabels[severityFilter])
            continue;

        if (!compact &&
            !ContainsNoCase(alert.description.c_str(), alertSearch) &&
            !ContainsNoCase(alert.source.c_str(), alertSearch))
            continue;

        rows.push_back(&alert);
    }

    if (rows.empty())
    {
        if (alerts.empty())
            EmptyState("All clear", "No security alerts have been raised.", COLOR_GREEN);
        else
            EmptyState("No matching alerts",
                "Try a different severity filter or search term.", COLOR_CYAN);

        ImGui::EndChild();
        return;
    }

    if (ImGui::BeginTable(
            "AlertsTable",
            5,
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_BordersInnerH |
            ImGuiTableFlags_Resizable |
            ImGuiTableFlags_SizingStretchProp |
            ImGuiTableFlags_ScrollY))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 110.0f);
        ImGui::TableSetupColumn("Severity", ImGuiTableColumnFlags_WidthFixed, 130.0f);
        ImGui::TableSetupColumn("Source", ImGuiTableColumnFlags_WidthFixed, 160.0f);
        ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableHeadersRow();

        for (const AlertInfo* ap : rows)
        {
            const AlertInfo& alert = *ap;

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            {
                const ImVec2 p = ImGui::GetCursorScreenPos();

                ImGui::GetWindowDrawList()->AddCircleFilled(
                    ImVec2(p.x + 4.0f, p.y + ImGui::GetTextLineHeight() * 0.5f),
                    4.0f,
                    ImGui::GetColorU32(SeverityColor(alert.severity.c_str())));

                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 16.0f);
            }
            ImGui::TextDisabled("%s", alert.time.c_str());

            ImGui::TableSetColumnIndex(1);
            SeverityBadge(alert.severity.c_str());

            ImGui::TableSetColumnIndex(2);
            ImGui::TextUnformatted(alert.source.c_str());

            ImGui::TableSetColumnIndex(3);
            ImGui::TextWrapped("%s", alert.description.c_str());

            // Resource alerts can offer End Task for the app causing them
            ImGui::TableSetColumnIndex(4);

            if (alert.canEndTask)
            {
                if (handledAlertIDs.count(alert.id))
                {
                    ImGui::TextColored(COLOR_GREEN, "Ended");
                }
                else
                {
                    ImGui::PushID(alert.id);

                    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.55f, 0.12f, 0.15f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.18f, 0.22f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  COLOR_RED);

                    if (ImGui::SmallButton("End Task"))
                    {
                        pendingAlertID   = alert.id;
                        pendingPid       = alert.culpritPid;
                        pendingName      = alert.culpritName;
                        pendingMem       = alert.culpritMem;
                        openEndTaskPopup = true;
                    }

                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                }
            }
            else
            {
                ImGui::TextDisabled("-");
            }
        }

        ImGui::EndTable();
    }

    ImGui::EndChild();
}

// ------------------------------------------------------------
// PROCESS TABLE
// ------------------------------------------------------------

static void DrawProcesses()
{
    BeginPanel("ProcessesPanel", ImVec2(0.0f, 0.0f), COLOR_CYAN);

    SectionHeading("Running processes",
        "apps can be ended here; system and background processes are protected");

    // Summary chips
    {
        int apps = 0;
        int high = 0;

        for (const ProcessInfo& p : processes)
        {
            if (p.isApp)                   ++apps;
            if (p.memory >= HIGH_MEMORY_MB) ++high;
        }

        char total[16], appText[16], highText[16];
        std::snprintf(total, sizeof(total), "%d", (int)processes.size());
        std::snprintf(appText, sizeof(appText), "%d", apps);
        std::snprintf(highText, sizeof(highText), "%d", high);

        StatChip("processes", total, COLOR_CYAN);
        ImGui::SameLine();
        StatChip("apps", appText, COLOR_BLUE);
        ImGui::SameLine();
        StatChip("high memory", highText, high > 0 ? COLOR_YELLOW : COLOR_GREEN);
    }

    ImGui::SetNextItemWidth(300.0f);

    ImGui::InputTextWithHint(
        "##ProcessSearch",
        "Search by name or PID",
        processSearch,
        IM_ARRAYSIZE(processSearch)
    );

    ImGui::SameLine();
    ImGui::Checkbox("Apps only", &procAppsOnly);

    ImGui::SameLine();

    char highLabel[64];
    std::snprintf(highLabel, sizeof(highLabel),
        "High memory only (%.0f MB+)", HIGH_MEMORY_MB);
    ImGui::Checkbox(highLabel, &procHighMemOnly);

    if (!procMessage.empty())
        ImGui::TextColored(procMessageOk ? COLOR_GREEN : COLOR_RED,
            "%s", procMessage.c_str());

    ImGui::Spacing();

    if (ImGui::BeginTable(
            "ProcessTable",
            6,
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_BordersInnerH |
            ImGuiTableFlags_Resizable |
            ImGuiTableFlags_Sortable |
            ImGuiTableFlags_SizingStretchProp |
            ImGuiTableFlags_ScrollY))
    {
        ImGui::TableSetupScrollFreeze(0, 1);

        ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed, 85.0f, 0);
        ImGui::TableSetupColumn("Process name", ImGuiTableColumnFlags_WidthStretch, 0.0f, 1);
        ImGui::TableSetupColumn(
            "CPU %",
            ImGuiTableColumnFlags_WidthFixed |
            ImGuiTableColumnFlags_PreferSortDescending,
            180.0f, 2);
        ImGui::TableSetupColumn(
            "Memory MB",
            ImGuiTableColumnFlags_WidthFixed |
            ImGuiTableColumnFlags_DefaultSort |
            ImGuiTableColumnFlags_PreferSortDescending,
            125.0f, 3);
        ImGui::TableSetupColumn("Running time", ImGuiTableColumnFlags_WidthFixed, 135.0f, 4);
        ImGui::TableSetupColumn("Action",
            ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoSort,
            100.0f, 5);

        ImGui::TableHeadersRow();

        std::vector<int> order(processes.size());

        for (size_t i = 0; i < order.size(); ++i)
            order[i] = (int)i;

        if (ImGuiTableSortSpecs* specs = ImGui::TableGetSortSpecs())
        {
            if (specs->SpecsCount > 0)
            {
                const ImGuiTableColumnSortSpecs& spec = specs->Specs[0];
                const bool asc =
                    spec.SortDirection == ImGuiSortDirection_Ascending;

                std::sort(order.begin(), order.end(),
                    [&](int a, int b)
                    {
                        const ProcessInfo& pa = processes[a];
                        const ProcessInfo& pb = processes[b];

                        int cmp = 0;

                        switch (spec.ColumnUserID)
                        {
                        case 0: cmp = (pa.pid > pb.pid) - (pa.pid < pb.pid); break;
                        case 1: cmp = pa.name.compare(pb.name); break;
                        case 2: cmp = (pa.cpu > pb.cpu) - (pa.cpu < pb.cpu); break;
                        case 3: cmp = (pa.memory > pb.memory) - (pa.memory < pb.memory); break;
                        case 4: cmp = (pa.runningSeconds > pb.runningSeconds) - (pa.runningSeconds < pb.runningSeconds); break;
                        }

                        return asc ? (cmp < 0) : (cmp > 0);
                    });
            }
        }

        int shown = 0;

        for (int index : order)
        {
            const ProcessInfo& process = processes[index];

            if (procAppsOnly && !process.isApp)
                continue;

            if (procHighMemOnly && process.memory < HIGH_MEMORY_MB)
                continue;

            char pidText[32];
            std::snprintf(pidText, sizeof(pidText), "%d", process.pid);

            const bool matches =
                ContainsNoCase(process.name.c_str(), processSearch) ||
                ContainsNoCase(pidText, processSearch);

            if (!matches)
                continue;

            ++shown;

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::TextDisabled("%d", process.pid);

            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(process.name.c_str());

            if (process.isApp && !process.title.empty())
            {
                ImGui::SameLine();
                ImGui::TextDisabled("- %s", process.title.c_str());
            }

            // CPU bar
            ImGui::TableSetColumnIndex(2);

            char cpuLabel[16];
            std::snprintf(cpuLabel, sizeof(cpuLabel), "%.1f", process.cpu);

            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, WithAlpha(LoadColor(process.cpu * 4.0f), 0.85f));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_FRAME);

            ImGui::ProgressBar(
                std::min(process.cpu / 25.0f, 1.0f),
                ImVec2(-FLT_MIN, 18.0f),
                cpuLabel
            );

            ImGui::PopStyleColor(2);

            // Memory: yellow when high, red when very high
            ImGui::TableSetColumnIndex(3);

            if (process.memory >= HIGH_MEMORY_MB * 2.0f)
                ImGui::TextColored(COLOR_RED, "%.1f", process.memory);
            else if (process.memory >= HIGH_MEMORY_MB)
                ImGui::TextColored(COLOR_YELLOW, "%.1f", process.memory);
            else
                ImGui::Text("%.1f", process.memory);

            ImGui::TableSetColumnIndex(4);
            ImGui::TextDisabled("%s", process.runningTime.c_str());

            // Only apps (windowed programs) get an End Task button
            ImGui::TableSetColumnIndex(5);

            if (process.isApp)
            {
                ImGui::PushID(process.pid);

                ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.55f, 0.12f, 0.15f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.18f, 0.22f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive,  COLOR_RED);

                if (ImGui::SmallButton("End Task"))
                {
                    pendingAlertID   = 0;
                    pendingPid       = process.pid;
                    pendingName      = process.name;
                    pendingMem       = process.memory;
                    openEndTaskPopup = true;
                }

                ImGui::PopStyleColor(3);
                ImGui::PopID();
            }
            else
            {
                ImGui::TextDisabled("-");
            }
        }

        if (shown == 0)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(1);
            ImGui::TextDisabled("No process matches the current filters.");
        }

        ImGui::EndTable();
    }

    DrawEndTaskPopup();

    ImGui::EndChild();
}

// ------------------------------------------------------------
// EVENT PAGE
// ------------------------------------------------------------

static ImVec4 EventColor(const std::string& type)
{
    if (ContainsNoCase(type.c_str(), "FAIL"))    return COLOR_RED;
    if (ContainsNoCase(type.c_str(), "DELETE"))  return COLOR_ORANGE;
    if (ContainsNoCase(type.c_str(), "MODIFY"))  return COLOR_YELLOW;
    if (ContainsNoCase(type.c_str(), "SUCCESS")) return COLOR_GREEN;

    return COLOR_CYAN;
}

static void DrawEventPage(
    const char* title,
    const std::vector<EventInfo>& events,
    const char* emptyText,
    const char* hint)
{
    BeginPanel(title, ImVec2(0.0f, 0.0f), COLOR_CYAN);

    SectionHeading(title, hint);

    {
        char count[16];
        std::snprintf(count, sizeof(count), "%d", (int)events.size());

        StatChip("events recorded", count,
            events.empty() ? COLOR_GREEN : COLOR_YELLOW);
        ImGui::Spacing();
    }

    if (events.empty())
    {
        EmptyState("Nothing to report", emptyText, COLOR_GREEN);
        ImGui::EndChild();
        return;
    }

    if (ImGui::BeginTable(
            "EventTable",
            3,
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_BordersInnerH |
            ImGuiTableFlags_Resizable |
            ImGuiTableFlags_SizingStretchProp |
            ImGuiTableFlags_ScrollY))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 190.0f);
        ImGui::TableSetupColumn("Event", ImGuiTableColumnFlags_WidthFixed, 190.0f);
        ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        for (const EventInfo& event : events)
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::TextDisabled("%s", event.time.c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(EventColor(event.type), "%s", event.type.c_str());

            ImGui::TableSetColumnIndex(2);
            ImGui::TextWrapped("%s", event.description.c_str());
        }

        ImGui::EndTable();
    }

    ImGui::EndChild();
}

static void DrawSystemAnalysis()
{
    BeginPanel("AnalysisPanel", ImVec2(0.0f, 0.0f), COLOR_BLUE);

    if (!analysisData.valid)
    {
        ImGui::TextDisabled("Collecting the first samples...");
        ImGui::EndChild();
        return;
    }

    const BackendAnalysis& a = analysisData;

    // ---- Resource usage ----
    SectionHeading("Resource usage");

    char cpuText[16];
    char ramText[16];
    char procText[16];
    char ramSub[96];
    char procSub[96];

    if (a.cpuAvailable)
        std::snprintf(cpuText, sizeof(cpuText), "%.1f%%", a.cpuPercent);
    else
        std::snprintf(cpuText, sizeof(cpuText), "n/a");

    if (a.memoryAvailable)
        std::snprintf(ramText, sizeof(ramText), "%.1f%%", a.memoryPercent);
    else
        std::snprintf(ramText, sizeof(ramText), "n/a");

    std::snprintf(ramSub, sizeof(ramSub), "%llu MB used of %llu MB",
        (unsigned long long)a.usedMemoryMB,
        (unsigned long long)a.totalMemoryMB);

    std::snprintf(procText, sizeof(procText), "%llu",
        (unsigned long long)a.processCount);
    std::snprintf(procSub, sizeof(procSub), "%llu high CPU, %llu high memory",
        (unsigned long long)a.highCpuProcessCount,
        (unsigned long long)a.highMemoryProcessCount);

    {
        const float sp = ImGui::GetStyle().ItemSpacing.x;
        const float w  = (ImGui::GetContentRegionAvail().x - sp * 2.0f) / 3.0f;

        MetricCard("CPU load", cpuText, "Current processor usage",
            LoadColor((float)a.cpuPercent), w, nullptr, -1,
            a.cpuAvailable ? (float)a.cpuPercent : 0.0f);
        ImGui::SameLine();

        MetricCard("Memory in use", ramText, ramSub,
            LoadColor((float)a.memoryPercent), w, nullptr, -1,
            a.memoryAvailable ? (float)a.memoryPercent : 0.0f);
        ImGui::SameLine();

        MetricCard("Processes", procText, procSub, COLOR_BLUE, w, nullptr, 1);
    }

    ImGui::Spacing();

    const float half =
        (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

    if (!a.cpuHistory.empty())
    {
        ImGui::BeginGroup();
        ImGui::TextDisabled("CPU usage (last 60 s)");
        DrawAreaChart("##cpuh",
            a.cpuHistory.data(), (int)a.cpuHistory.size(), 0,
            100.0f, ImVec2(half, 170.0f),
            LoadColor((float)a.cpuPercent));
        ImGui::EndGroup();
    }

    ImGui::SameLine();

    if (!a.ramHistory.empty())
    {
        ImGui::BeginGroup();
        ImGui::TextDisabled("Memory usage (last 60 s)");
        DrawAreaChart("##ramh",
            a.ramHistory.data(), (int)a.ramHistory.size(), 0,
            100.0f, ImVec2(half, 170.0f),
            LoadColor((float)a.memoryPercent));
        ImGui::EndGroup();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // ---- Findings ----
    SectionHeading("Findings", "A diagnostic signal, not proof of an intrusion");

    if (a.findings.empty())
    {
        ImGui::TextColored(COLOR_GREEN, "No resource concerns detected.");
    }
    else
    {
        for (const std::string& finding : a.findings)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, COLOR_YELLOW);
            ImGui::Bullet();
            ImGui::SameLine();
            ImGui::TextWrapped("%s", finding.c_str());
            ImGui::PopStyleColor();
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // ---- Top CPU processes ----
    SectionHeading("Top processes by CPU");

    if (ImGui::BeginTable(
            "TopCpuTable", 4,
            ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH |
            ImGuiTableFlags_SizingStretchProp))
    {
        ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed, 85.0f);
        ImGui::TableSetupColumn("Process name");
        ImGui::TableSetupColumn("CPU %", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Memory MB", ImGuiTableColumnFlags_WidthFixed, 125.0f);
        ImGui::TableHeadersRow();

        for (const BackendProcess& p : a.topCpuProcesses)
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::TextDisabled("%d", p.pid);

            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(p.name.c_str());

            ImGui::TableSetColumnIndex(2);
            ImGui::TextColored(LoadColor(p.cpuPercent * 4.0f), "%.1f", p.cpuPercent);

            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%.1f", p.memoryMB);
        }

        ImGui::EndTable();
    }

    ImGui::EndChild();
}

// ------------------------------------------------------------
// FILE SCANNER PAGE
// ------------------------------------------------------------

static std::string FormatBytes(unsigned long long b)
{
    char t[32];

    if (b >= (1ULL << 30))      std::snprintf(t, sizeof(t), "%.2f GB", b / 1073741824.0);
    else if (b >= (1ULL << 20)) std::snprintf(t, sizeof(t), "%.1f MB", b / 1048576.0);
    else if (b >= 1024ULL)      std::snprintf(t, sizeof(t), "%.1f KB", b / 1024.0);
    else                        std::snprintf(t, sizeof(t), "%llu B", b);

    return t;
}

// Rotating-arc spinner. Reserves a (2*radius) square at the cursor.
static void DrawSpinner(float radius, float thickness, const ImVec4& color, bool active)
{
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    const ImVec2 center(pos.x + radius, pos.y + radius);
    ImDrawList* draw = ImGui::GetWindowDrawList();
    const float t = (float)ImGui::GetTime();

    if (!active)
    {
        draw->AddCircle(center, radius, ImGui::GetColorU32(color), 48, thickness);
    }
    else
    {
        draw->AddCircle(center, radius,
            ImGui::GetColorU32(WithAlpha(color, 0.18f)), 48, thickness);

        const float start = t * 5.0f;

        draw->PathClear();
        draw->PathArcTo(center, radius, start, start + 2.4f, 32);
        draw->PathStroke(ImGui::GetColorU32(color), 0, thickness);

        const float pulse = 0.75f + 0.25f * std::sin(t * 4.0f);
        draw->AddCircleFilled(center, radius * 0.30f * pulse,
            ImGui::GetColorU32(WithAlpha(color, 0.85f)), 24);
    }

    ImGui::Dummy(ImVec2(radius * 2.0f, radius * 2.0f));
}

static void SplitPath(const std::string& full, std::string& dir, std::string& name)
{
    const std::size_t pos = full.find_last_of("\\/");

    if (pos == std::string::npos)
    {
        dir.clear();
        name = full;
    }
    else
    {
        dir  = full.substr(0, pos);
        name = full.substr(pos + 1);
    }
}

static void DrawFileScanner()
{
    static double lastRefresh = -1.0;
    const double now = ImGui::GetTime();

    if (lastRefresh < 0.0 || now - lastRefresh >= 0.25)
    {
        lastRefresh  = now;
        scanStatus   = backend.getScanStatus();
        scanFindings = backend.getScanFindings();
    }

    const bool running = scanStatus.running;

    BeginPanel("ScannerPanel", ImVec2(0.0f, 0.0f), COLOR_ORANGE);

    SectionHeading("File Scanner", "Signature + heuristic scan");

    // ---- Scan type ----
    ImGui::BeginDisabled(running);

    ImGui::RadioButton("Quick (user folders, temp, startup)", &scanTypeSel, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Full (all fixed drives)", &scanTypeSel, 1);
    ImGui::SameLine();
    ImGui::RadioButton("Custom", &scanTypeSel, 2);

    if (scanTypeSel == 2)
    {
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputTextWithHint("##scanpath",
            "Folder or file, e.g. C:\\Users\\you\\Downloads",
            scanPath, sizeof(scanPath));
    }

    ImGui::EndDisabled();

    // ---- Buttons ----
    if (!running)
    {
        PushGreenButton();

        if (ImGui::Button("Start Scan", ImVec2(130.0f, 0.0f)))
        {
            if (scanTypeSel == 2 && scanPath[0] == '\0')
                scanMessage = "Enter a folder or file path first.";
            else if (!backend.startScan((ScanType)scanTypeSel, scanPath))
                scanMessage = "A scan is already running.";
            else
                scanMessage.clear();
        }

        ImGui::PopStyleColor(3);
    }
    else
    {
        PushRedButton();

        if (ImGui::Button("Stop Scan", ImVec2(130.0f, 0.0f)))
            backend.cancelScan();

        ImGui::PopStyleColor(3);
    }

    if (!running && scanStatus.finished)
    {
        ImGui::SameLine();

        if (ImGui::Button("Export Report"))
        {
            std::string out;

            scanMessage = backend.exportScanReport(out)
                ? "Report saved: " + out
                : "Could not write the report file.";
        }
    }

    if (!scanMessage.empty())
    {
        ImGui::SameLine();
        ImGui::TextDisabled("%s", scanMessage.c_str());
    }

    ImGui::Separator();
    ImGui::Spacing();

    // Two panels side by side: live scan (left), threats (right)
    const float gap    = ImGui::GetStyle().ItemSpacing.x;
    const float leftW  = (ImGui::GetContentRegionAvail().x - gap) * 0.58f;
    const float panelH = 290.0f;

    ImGui::PushStyleColor(ImGuiCol_ChildBg, COLOR_SIDEBAR);

    // ---------------- LEFT: live scan ----------------
    ImVec4 stateColor = COLOR_MUTED;
    const char* stateText = "Ready to scan";

    if (running)
    {
        stateColor = COLOR_CYAN;
        stateText  = "Scanning";
    }
    else if (scanStatus.finished && scanStatus.cancelled)
    {
        stateColor = COLOR_YELLOW;
        stateText  = "Scan stopped";
    }
    else if (scanStatus.finished)
    {
        stateColor = (scanStatus.malicious + scanStatus.suspicious > 0)
            ? COLOR_ORANGE : COLOR_GREEN;
        stateText  = "Scan completed";
    }

    BeginPanel("ScanLive", ImVec2(leftW, panelH), stateColor);

    {
        DrawSpinner(26.0f, 4.0f, stateColor, running);
        ImGui::SameLine(0.0f, 16.0f);

        ImGui::BeginGroup();

        ImGui::SetWindowFontScale(1.3f);
        ImGui::PushStyleColor(ImGuiCol_Text, stateColor);

        if (running)
        {
            const int dots = (int)(ImGui::GetTime() * 2.0) % 4;
            ImGui::Text("%s%.*s", stateText, dots, "...");
        }
        else
        {
            ImGui::TextUnformatted(stateText);
        }

        ImGui::PopStyleColor();
        ImGui::SetWindowFontScale(1.0f);

        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() +
            std::max(120.0f, ImGui::GetContentRegionAvail().x - 4.0f));
        ImGui::TextDisabled("%s",
            scanStatus.target.empty() ? "Choose a scan type and press Start"
                                      : scanStatus.target.c_str());
        ImGui::PopTextWrapPos();

        ImGui::EndGroup();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextDisabled("%s", running ? "NOW SCANNING" : "LAST FILE SCANNED");

        std::string dir, name;
        SplitPath(scanStatus.currentFile, dir, name);

        if (name.empty())
        {
            ImGui::TextDisabled("-");
        }
        else
        {
            ImGui::SetWindowFontScale(1.15f);
            ImGui::PushStyleColor(ImGuiCol_Text, COLOR_TEXT);
            ImGui::TextUnformatted(name.c_str());
            ImGui::PopStyleColor();
            ImGui::SetWindowFontScale(1.0f);

            ImGui::PushStyleColor(ImGuiCol_Text, COLOR_MUTED);
            ImGui::TextWrapped("%s", dir.c_str());
            ImGui::PopStyleColor();
        }

        ImGui::Spacing();

        // Live counters as chips
        char files[24], skipped[24], data[24], timeText[24];
        std::snprintf(files, sizeof(files), "%llu", scanStatus.filesScanned);
        std::snprintf(skipped, sizeof(skipped), "%llu", scanStatus.filesSkipped);
        std::snprintf(data, sizeof(data), "%s", FormatBytes(scanStatus.bytesScanned).c_str());
        std::snprintf(timeText, sizeof(timeText), "%s",
            FormatDuration(scanStatus.elapsedSeconds).c_str());

        StatChip("files", files, COLOR_CYAN);
        ImGui::SameLine();
        StatChip("skipped", skipped, COLOR_MUTED);
        StatChip("scanned", data, COLOR_BLUE);
        ImGui::SameLine();
        StatChip("elapsed", timeText, COLOR_GREEN);
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // ---------------- RIGHT: threats found ----------------
    BeginPanel("ScanThreats", ImVec2(0.0f, panelH),
        (scanStatus.malicious > 0) ? COLOR_RED : COLOR_ORANGE);

    {
        ImGui::PushStyleColor(ImGuiCol_Text, COLOR_CYAN);
        ImGui::SetWindowFontScale(1.1f);
        ImGui::TextUnformatted("Suspicious & malicious files");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();

        ImGui::TextColored(COLOR_RED, "Malicious: %d", scanStatus.malicious);
        ImGui::SameLine(0.0f, 18.0f);
        ImGui::TextColored(COLOR_YELLOW, "Suspicious: %d", scanStatus.suspicious);

        ImGui::Separator();

        if (scanFindings.empty())
        {
            ImGui::Spacing();

            if (running)
                ImGui::TextDisabled("Nothing suspicious so far...");
            else if (scanStatus.finished)
                ImGui::TextColored(COLOR_GREEN, "No threats found.");
            else
                ImGui::TextDisabled("Findings will appear here.");
        }

        // Newest first
        for (std::size_t i = scanFindings.size(); i-- > 0; )
        {
            const ScanFinding& f = scanFindings[i];
            const ImVec4 c = (f.level == "Malicious") ? COLOR_RED : COLOR_YELLOW;

            std::string dir, name;
            SplitPath(f.path, dir, name);

            ImGui::PushID((int)i);

            ImGui::TextColored(c, "[%s]", f.level.c_str());
            ImGui::SameLine();
            ImGui::TextUnformatted(f.name.c_str());

            ImGui::TextDisabled("%s", name.c_str());

            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("%s", f.path.c_str());
                ImGui::Text("Reason: %s", f.reason.c_str());
                ImGui::Text("Score: %d", f.score);
                ImGui::EndTooltip();
            }

            ImGui::Separator();
            ImGui::PopID();
        }
    }

    ImGui::EndChild();

    ImGui::PopStyleColor();   // ChildBg

    ImGui::Spacing();

    // Full findings table
    SectionHeading("All findings", "hover a name for score and SHA-256");

    if (ImGui::BeginTable(
            "ScanFindings",
            5,
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_BordersInnerH |
            ImGuiTableFlags_Resizable |
            ImGuiTableFlags_SizingStretchProp |
            ImGuiTableFlags_ScrollY))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Time",   ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Level",  ImGuiTableColumnFlags_WidthFixed, 95.0f);
        ImGui::TableSetupColumn("Name",   ImGuiTableColumnFlags_WidthFixed, 190.0f);
        ImGui::TableSetupColumn("File");
        ImGui::TableSetupColumn("Reason");
        ImGui::TableHeadersRow();

        for (const ScanFinding& f : scanFindings)
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::TextDisabled("%s", f.time.c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(
                f.level == "Malicious" ? COLOR_RED : COLOR_YELLOW,
                "%s", f.level.c_str());

            ImGui::TableSetColumnIndex(2);
            ImGui::TextUnformatted(f.name.c_str());

            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Score: %d", f.score);
                ImGui::Text("Size:  %s", FormatBytes(f.size).c_str());

                if (!f.sha256.empty())
                    ImGui::Text("SHA-256: %s", f.sha256.c_str());

                ImGui::EndTooltip();
            }

            ImGui::TableSetColumnIndex(3);
            ImGui::TextWrapped("%s", f.path.c_str());

            ImGui::TableSetColumnIndex(4);
            ImGui::TextWrapped("%s", f.reason.c_str());
        }

        ImGui::EndTable();
    }

    ImGui::EndChild();
}

// ------------------------------------------------------------
// LOGS & REPORTS PAGE
// ------------------------------------------------------------

static void DrawLogsReports()
{
    if (!LogsGate())
        return;

    static std::vector<SignatureInfo> signatures;
    static bool loaded = false;
    static char search[128] = "";

    if (!loaded)
    {
        signatures = FileScanner::getSignatures();
        loaded = true;
    }

    int hashCount = 0, contentCount = 0, nameCount = 0;

    for (const SignatureInfo& s : signatures)
    {
        if (s.category == "Known hash")        ++hashCount;
        else if (s.category == "File content") ++contentCount;
        else                                   ++nameCount;
    }

    BeginPanel("LogsPanel", ImVec2(0.0f, 0.0f), COLOR_GREEN);

    SectionHeading("Detection signatures", "what the File Scanner checks for");

    if (ImGui::Button("Lock now"))
    {
        logsUnlocked = false;
        lockMessage.clear();
    }

    ImGui::SameLine();
    ImGui::TextDisabled("Locks again after 5 minutes idle or when you leave this page.");

    if (ImGui::CollapsingHeader("Change password"))
    {
        static std::string changeMessage;
        static bool        changeOk = false;

        ImGui::SetNextItemWidth(260.0f);
        ImGui::InputTextWithHint("##oldpw", "Current password",
            lockOld, sizeof(lockOld), ImGuiInputTextFlags_Password);

        ImGui::SetNextItemWidth(260.0f);
        ImGui::InputTextWithHint("##chnew", "New password",
            lockNew, sizeof(lockNew), ImGuiInputTextFlags_Password);

        ImGui::SetNextItemWidth(260.0f);
        ImGui::InputTextWithHint("##chconf", "Confirm new password",
            lockConfirm, sizeof(lockConfirm), ImGuiInputTextFlags_Password);

        if (ImGui::Button("Update password"))
        {
            if (std::strcmp(lockNew, lockConfirm) != 0)
            {
                changeMessage = "New passwords do not match.";
                changeOk      = false;
            }
            else
            {
                changeOk = backend.setPassword(lockNew, lockOld, changeMessage);

                if (changeOk)
                {
                    Wipe(lockOld, sizeof(lockOld));
                    Wipe(lockNew, sizeof(lockNew));
                    Wipe(lockConfirm, sizeof(lockConfirm));
                }
            }
        }

        if (!changeMessage.empty())
            ImGui::TextColored(changeOk ? COLOR_GREEN : COLOR_RED,
                "%s", changeMessage.c_str());
    }

    ImGui::Spacing();

    ImGui::PushTextWrapPos(0.0f);
    ImGui::TextColored(COLOR_MUTED,
        "A file is reported Malicious on an instant match (EICAR test string or a "
        "known-bad SHA-256). Otherwise every matching rule adds its weight to a "
        "score, and a file scoring %d or more is reported Suspicious.",
        FileScanner::suspiciousThreshold());
    ImGui::PopTextWrapPos();

    ImGui::Spacing();

    {
        char total[16], h[16], c[16], n[16];
        std::snprintf(total, sizeof(total), "%d", (int)signatures.size());
        std::snprintf(h, sizeof(h), "%d", hashCount);
        std::snprintf(c, sizeof(c), "%d", contentCount);
        std::snprintf(n, sizeof(n), "%d", nameCount);

        StatChip("rules", total, COLOR_GREEN);
        ImGui::SameLine();
        StatChip("known hashes", h, COLOR_RED);
        ImGui::SameLine();
        StatChip("file content", c, COLOR_CYAN);
        ImGui::SameLine();
        StatChip("name / location", n, COLOR_YELLOW);
    }

    ImGui::SetNextItemWidth(300.0f);
    ImGui::InputTextWithHint("##sigsearch", "Search signatures", search, IM_ARRAYSIZE(search));

    ImGui::SameLine();

    if (ImGui::Button("Reload"))
        signatures = FileScanner::getSignatures();   // re-reads hids_hashes.txt

    ImGui::Spacing();

    if (ImGui::BeginTable(
            "SignatureTable",
            4,
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_BordersInnerH |
            ImGuiTableFlags_Resizable |
            ImGuiTableFlags_SizingStretchProp |
            ImGuiTableFlags_ScrollY))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Type",      ImGuiTableColumnFlags_WidthFixed, 160.0f);
        ImGui::TableSetupColumn("Name",      ImGuiTableColumnFlags_WidthStretch, 1.0f);
        ImGui::TableSetupColumn("Pattern / condition", ImGuiTableColumnFlags_WidthStretch, 1.4f);
        ImGui::TableSetupColumn("Result",    ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableHeadersRow();

        int shown = 0;

        for (const SignatureInfo& s : signatures)
        {
            if (!ContainsNoCase(s.name.c_str(), search) &&
                !ContainsNoCase(s.pattern.c_str(), search) &&
                !ContainsNoCase(s.category.c_str(), search))
                continue;

            ++shown;

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(
                s.category == "Known hash"   ? COLOR_RED :
                s.category == "File content" ? COLOR_CYAN : COLOR_YELLOW,
                "%s", s.category.c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::TextWrapped("%s", s.name.c_str());

            ImGui::TableSetColumnIndex(2);
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
            ImGui::TextWrapped("%s", s.pattern.c_str());
            ImGui::PopStyleColor();

            ImGui::TableSetColumnIndex(3);

            if (s.instantMalicious)
                ImGui::TextColored(COLOR_RED, "Malicious");
            else
                ImGui::TextColored(
                    s.weight >= 4 ? COLOR_ORANGE :
                    s.weight >= 2 ? COLOR_YELLOW : COLOR_MUTED,
                    "+%d score", s.weight);
        }

        if (shown == 0)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(1);
            ImGui::TextDisabled("No signature matches \"%s\".", search);
        }

        ImGui::EndTable();
    }

    ImGui::EndChild();
}

// ------------------------------------------------------------
// NOTIFICATION SETTINGS (shown on the Settings page)
// ------------------------------------------------------------

static void DrawNotificationSettings()
{
    BeginPanel("NotificationSettings", ImVec2(0.0f, 220.0f), COLOR_YELLOW, true);

    SectionHeading("Desktop notifications",
        "pop-up toasts when a new alert is raised");

    bool enabled = backend.notificationsEnabled();

    if (ImGui::Checkbox("Show desktop notifications", &enabled))
        backend.setNotificationsEnabled(enabled);

    ImGui::BeginDisabled(!enabled);

    static const char* levels[] = {
        "Low and above", "Medium and above",
        "High and above", "Critical only"
    };

    int level = backend.notificationMinSeverity();

    ImGui::SetNextItemWidth(220.0f);

    if (ImGui::Combo("Notify for", &level, levels, IM_ARRAYSIZE(levels)))
        backend.setNotificationMinSeverity(level);

    if (ImGui::Button("Send test notification"))
        backend.sendTestNotification();

    ImGui::EndDisabled();

    ImGui::EndChild();
}

// ------------------------------------------------------------
// FEATURE PAGE: header with Back + only the selected feature
// ------------------------------------------------------------

static void DrawMainContent()
{
    RefreshFromBackend();

    DrawPageHeader();
    ImGui::Spacing();

    if (demoMode)
    {
        DrawPill("DEMO DATA", COLOR_YELLOW);
        ImGui::SameLine();
        ImGui::TextColored(COLOR_YELLOW,
            "Sample rows are labelled [DEMO] and are not real host findings.");
        ImGui::Spacing();
    }

    const float fade = ClampFloat(
        (float)((ImGui::GetTime() - screenChangedAt) / 0.18),
        0.0f, 1.0f
    );
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.25f + 0.75f * fade);

    switch (selectedPage)
    {
    case 0:
        DrawEngineBanner();     // only while protection is stopped
        DrawPostureBanner();    // only while protection is running
        DrawMetrics();
        ImGui::Spacing();
        DrawThreatPanel();
        ImGui::Spacing();
        DrawAlerts(true);
        break;

    case 1:
        DrawProcesses();
        break;

    case 2:
        DrawEventPage("Failed sign-in attempts", loginEvents,
            "No failed sign-ins recorded since monitoring started.",
            "Reads the Windows Security log - run as Administrator");
        break;

    case 3:
        DrawEventPage("File activity", fileEvents,
            "No changes detected in the watched folder yet.",
            "Watching the HIDS_Monitor folder");
        break;

    case 4:
        DrawAlerts(false);
        break;

    case 5:
        DrawLogsReports();
        break;

    case 6:
        DrawSystemAnalysis();
        break;

    case 7:
        DrawFileScanner();
        break;

    case 8:
        DrawNotificationSettings();
        ImGui::Spacing();
        DrawSettingsPage(monitoringActive);
        break;
    }

    ImGui::PopStyleVar();
}

int main()
{
    if (!glfwInit())
    {
        std::fprintf(stderr, "ERROR: Failed to initialize GLFW.\n");
        return 1;
    }

    // Request OpenGL 3.0
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(
        1400,
        900,
        "HIDS - Host Intrusion Detection System",
        nullptr,
        nullptr
    );

    if (window == nullptr)
    {
        std::fprintf(stderr, "ERROR: Failed to create dashboard window.\n");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Native window handle for the tray icon / desktop notifications
#ifdef _WIN32
    backend.attachWindow(glfwGetWin32Window(window));
#endif

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Don't write imgui.ini next to the executable
    io.IniFilename = nullptr;

    LoadFonts(io);
    LoadUiConfig();           // saved theme / interface size / demo flag
    ApplyTheme(themeIndex);   // also builds the ImGui style
    RefineStyle();            // rounded look, table + scrollbar colours
    InitUiScale(io);

    int styledTheme = themeIndex;

    // Seed sparkline history so the cards aren't empty on first frame
    for (int i = 0; i < HISTORY_SIZE; ++i)
    {
        cpuHistory[i] = cpuNow;
        ramHistory[i] = ramNow;
    }

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Monitoring starts only when the user presses Start HIDS / Start protection.
    monitoringActive = backend.isRunning();

    // Main application loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Send queued desktop notifications (must run on the UI thread)
        backend.flushNotifications();

        // Theme clicks are queued and applied here, never mid-frame
        ApplyPendingTheme();

        // A theme change rebuilds the style, so re-apply our refinements
        if (themeIndex != styledTheme)
        {
            RefineStyle();
            styledTheme = themeIndex;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        TrackScreenChange();   // fade + re-lock Logs & Reports when leaving it
        HandleBackKey();       // Esc: page -> domain -> hub -> welcome

        // Gradient backdrop + ambient glows; the root window is transparent
        DrawBackdrop();
        DrawAmbient();
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));

        // Full-window root
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 12.0f));

        ImGui::Begin(
            "HIDSRoot",
            nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoBringToFrontOnFocus
        );

        ImGui::PopStyleVar();

        if (appScreen == SCREEN_PAGE)
        {
            // A single feature page: header with Back, then only that feature.
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));

            ImGui::BeginChild(
                "MainContent",
                ImVec2(0.0f, 0.0f),
                false
            );

            DrawMainContent();

            ImGui::EndChild();
            ImGui::PopStyleColor();
        }
        else
        {
            // Welcome screen, home hub and domain screens
            DrawFullScreen();
        }

        ImGui::End();
        ImGui::PopStyleColor();   // WindowBg

        ImGui::Render();

        int displayWidth = 0;
        int displayHeight = 0;

        glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
        glViewport(0, 0, displayWidth, displayHeight);

        glClearColor(
            HIDS_COLOR_BACKGROUND.x,
            HIDS_COLOR_BACKGROUND.y,
            HIDS_COLOR_BACKGROUND.z,
            HIDS_COLOR_BACKGROUND.w
        );

        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    backend.shutdown();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}