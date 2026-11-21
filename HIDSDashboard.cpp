

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

// static const float HIDS_PI  = 3.14159265f;
// static const float HIDS_2PI = 6.28318531f;

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

// // In-app toast shown when a brand-new alert arrives
// static std::string toastText;
// static std::string toastSeverity;
// static double      toastAt = -100.0;

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
// // GRAPHICS HELPERS
// // Small building blocks shared by every screen: colour helpers,
// // animation easing, soft shadows, glass card chrome, usage bars.
// // ------------------------------------------------------------

// // Colour with alpha, already converted for the draw list
// static ImU32 Col(const ImVec4& c, float a = 1.0f)
// {
//     return ImGui::GetColorU32(WithAlpha(c, a));
// }

// static const ImVec4 FX_WHITE(1.0f, 1.0f, 1.0f, 1.0f);
// static const ImVec4 FX_BLACK(0.0f, 0.0f, 0.0f, 1.0f);

// static float Smooth01(float t)
// {
//     t = ClampFloat(t, 0.0f, 1.0f);
//     return t * t * (3.0f - 2.0f * t);
// }

// static ImVec2 Off(const ImVec2& p, float x, float y)
// {
//     return ImVec2(p.x + x, p.y + y);
// }

// // Eases a stored value toward a target (per-window state, keyed by id).
// // Used for hover glows and bar fills so nothing snaps.
// static float AnimTo(ImGuiID id, float target, float speed = 12.0f,
//                     float initial = -1.0e9f)
// {
//     ImGuiStorage* st = ImGui::GetStateStorage();
//     float v = st->GetFloat(id, initial < -1.0e8f ? target : initial);

//     v += (target - v) * ClampFloat(ImGui::GetIO().DeltaTime * speed, 0.0f, 1.0f);
//     st->SetFloat(id, v);
//     return v;
// }

// // Soft drop shadow made of stacked translucent rounded rects.
// // Must be drawn on the PARENT draw list, before the card's BeginChild.
// static void DrawSoftShadow(ImDrawList* d, const ImVec2& a, const ImVec2& b,
//                            float rounding, float spread, float alpha)
// {
//     const int layers = 6;

//     for (int i = 0; i < layers; ++i)
//     {
//         const float grow = spread * (1.0f - (float)i / (float)layers);
//         const float al   = alpha * (float)(i + 1) / (float)layers * 0.35f;

//         d->AddRectFilled(
//             ImVec2(a.x - grow, a.y - grow + 6.0f),
//             ImVec2(b.x + grow, b.y + grow + 6.0f),
//             Col(FX_BLACK, al), rounding + grow);
//     }
// }

// // Glass finish drawn inside a child: sheen, top highlight, tinted border
// static void DrawCardChrome(ImDrawList* d, const ImVec2& wp, const ImVec2& ws,
//                            const ImVec4& accent, float hover)
// {
//     const float r = ImGui::GetStyle().ChildRounding;

//     d->AddRectFilledMultiColor(
//         ImVec2(wp.x + 2.0f, wp.y + 2.0f),
//         ImVec2(wp.x + ws.x - 2.0f, wp.y + ws.y * 0.55f),
//         Col(FX_WHITE, 0.040f + 0.035f * hover),
//         Col(FX_WHITE, 0.012f),
//         Col(FX_WHITE, 0.0f),
//         Col(FX_WHITE, 0.0f));

//     d->AddLine(ImVec2(wp.x + r, wp.y + 1.5f),
//                ImVec2(wp.x + ws.x - r, wp.y + 1.5f),
//                Col(FX_WHITE, 0.07f + 0.05f * hover), 1.0f);

//     d->AddRect(wp, ImVec2(wp.x + ws.x, wp.y + ws.y),
//                Col(accent, 0.14f + 0.62f * hover), r, 0, 1.0f + 0.7f * hover);
// }

// // Rounded usage bar with a glossy highlight and centred label.
// // Fills the width available in the current cell / region.
// static void DrawUsageBar(float frac, const char* label, const ImVec4& col,
//                          float h = 18.0f)
// {
//     frac = ClampFloat(frac, 0.0f, 1.0f);

//     const float  w = std::max(20.0f, ImGui::GetContentRegionAvail().x);
//     const ImVec2 p = ImGui::GetCursorScreenPos();
//     ImDrawList*  d = ImGui::GetWindowDrawList();

//     d->AddRectFilled(p, ImVec2(p.x + w, p.y + h),
//         ImGui::GetColorU32(COLOR_FRAME), h * 0.5f);

//     if (frac > 0.004f)
//     {
//         const float fw = std::max(h, w * frac);

//         d->AddRectFilled(p, ImVec2(p.x + fw, p.y + h), Col(col, 0.88f), h * 0.5f);
//         d->AddRectFilled(ImVec2(p.x + 4.0f, p.y + 2.0f),
//                          ImVec2(p.x + fw - 4.0f, p.y + h * 0.46f),
//                          Col(FX_WHITE, 0.20f), h * 0.25f);
//     }

//     d->AddRect(p, ImVec2(p.x + w, p.y + h), Col(col, 0.25f), h * 0.5f);

//     if (label != nullptr && label[0] != '\0')
//     {
//         const ImVec2 ts = ImGui::CalcTextSize(label);
//         const ImVec2 tp(p.x + (w - ts.x) * 0.5f, p.y + (h - ts.y) * 0.5f);

//         d->AddText(Off(tp, 1.0f, 1.0f), Col(FX_BLACK, 0.55f), label);
//         d->AddText(tp, ImGui::GetColorU32(COLOR_TEXT), label);
//     }

//     ImGui::Dummy(ImVec2(w, h));
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
// // Rounded, flat look + tinted borders, table headers, scrollbars.
// // Applied after ApplyTheme(); colours still come from the theme.
// // ------------------------------------------------------------

// static void RefineStyle()
// {
//     ImGuiStyle& s = ImGui::GetStyle();

//     s.WindowRounding    = 0.0f;
//     s.ChildRounding     = 14.0f;
//     s.FrameRounding     = 8.0f;
//     s.PopupRounding     = 12.0f;
//     s.GrabRounding      = 7.0f;
//     s.ScrollbarRounding = 9.0f;
//     s.TabRounding       = 7.0f;

//     s.WindowBorderSize  = 0.0f;
//     s.ChildBorderSize   = 1.0f;
//     s.FrameBorderSize   = 0.0f;
//     s.PopupBorderSize   = 1.0f;

//     s.WindowPadding     = ImVec2(18.0f, 16.0f);
//     s.FramePadding      = ImVec2(13.0f, 7.0f);
//     s.ItemSpacing       = ImVec2(10.0f, 10.0f);
//     s.ItemInnerSpacing  = ImVec2(8.0f, 6.0f);
//     s.CellPadding       = ImVec2(10.0f, 8.0f);
//     s.ScrollbarSize     = 10.0f;
//     s.GrabMinSize       = 12.0f;

//     // Smoother circles / lines everywhere
//     s.AntiAliasedLines        = true;
//     s.AntiAliasedLinesUseTex  = true;
//     s.AntiAliasedFill         = true;
//     s.CircleTessellationMaxError = 0.20f;

//     ImVec4* c = s.Colors;
//     c[ImGuiCol_TableHeaderBg]     = WithAlpha(COLOR_CYAN, 0.14f);
//     c[ImGuiCol_TableBorderLight]  = WithAlpha(COLOR_CYAN, 0.07f);
//     c[ImGuiCol_TableBorderStrong] = WithAlpha(COLOR_CYAN, 0.18f);
//     c[ImGuiCol_TableRowBg]        = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
//     c[ImGuiCol_TableRowBgAlt]     = ImVec4(1.0f, 1.0f, 1.0f, 0.026f);
//     c[ImGuiCol_ScrollbarBg]       = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
//     c[ImGuiCol_ScrollbarGrab]        = WithAlpha(COLOR_CYAN, 0.30f);
//     c[ImGuiCol_ScrollbarGrabHovered] = WithAlpha(COLOR_CYAN, 0.55f);
//     c[ImGuiCol_ScrollbarGrabActive]  = WithAlpha(COLOR_CYAN, 0.80f);

//     c[ImGuiCol_Border]    = WithAlpha(COLOR_CYAN, 0.16f);
//     c[ImGuiCol_Separator] = WithAlpha(COLOR_CYAN, 0.14f);
//     c[ImGuiCol_CheckMark] = COLOR_CYAN;
//     c[ImGuiCol_SliderGrab]       = WithAlpha(COLOR_CYAN, 0.80f);
//     c[ImGuiCol_SliderGrabActive] = COLOR_CYAN;
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

// // Section title: glowing accent bar, title, hint, gradient underline
// static void SectionHeading(const char* title, const char* hint = nullptr)
// {
//     const float fullW = ImGui::GetContentRegionAvail().x;

//     {
//         const ImVec2 p = ImGui::GetCursorScreenPos();
//         const float  h = ImGui::GetTextLineHeight() * 1.1f;
//         ImDrawList*  d = ImGui::GetWindowDrawList();

//         d->AddRectFilled(ImVec2(p.x - 2.0f, p.y - 1.0f), ImVec2(p.x + 8.0f, p.y + h + 2.0f),
//             Col(COLOR_CYAN, 0.16f), 5.0f);
//         d->AddRectFilled(ImVec2(p.x, p.y + 1.0f), ImVec2(p.x + 4.0f, p.y + h),
//             ImGui::GetColorU32(COLOR_CYAN), 2.0f);

//         ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 14.0f);
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

//     {
//         const ImVec2 p = ImGui::GetCursorScreenPos();

//         ImGui::GetWindowDrawList()->AddRectFilledMultiColor(
//             ImVec2(p.x, p.y + 1.0f), ImVec2(p.x + fullW, p.y + 2.0f),
//             Col(COLOR_CYAN, 0.55f), Col(COLOR_CYAN, 0.0f),
//             Col(COLOR_CYAN, 0.0f),  Col(COLOR_CYAN, 0.55f));
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
//     const ImVec2 size(textSize.x + 12.0f + 9.0f + pad.x, textSize.y + pad.y * 2.0f);

//     ImDrawList* draw = ImGui::GetWindowDrawList();

//     draw->AddRectFilled(
//         pos,
//         ImVec2(pos.x + size.x, pos.y + size.y),
//         Col(color, 0.16f),
//         size.y * 0.5f
//     );

//     draw->AddRect(
//         pos,
//         ImVec2(pos.x + size.x, pos.y + size.y),
//         Col(color, 0.65f),
//         size.y * 0.5f
//     );

//     const ImVec2 dot(pos.x + 12.0f, pos.y + size.y * 0.5f);
//     draw->AddCircleFilled(dot, 6.5f, Col(color, 0.20f), 20);
//     draw->AddCircleFilled(dot, 3.5f, ImGui::GetColorU32(color), 16);

//     draw->AddText(
//         ImVec2(pos.x + 12.0f + 9.0f, pos.y + pad.y),
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
//         const float ring = std::fmod(t * 0.9f, 1.0f);

//         draw->AddCircleFilled(
//             center,
//             radius + 3.0f + wave * 3.0f,
//             Col(color, 0.18f * (1.0f - wave) + 0.05f)
//         );

//         draw->AddCircle(center, radius + 2.0f + ring * 9.0f,
//             Col(color, 0.45f * (1.0f - ring)), 24, 1.4f);
//     }

//     draw->AddCircleFilled(center, radius, ImGui::GetColorU32(color));
//     draw->AddCircleFilled(Off(center, -1.4f, -1.6f), radius * 0.35f, Col(FX_WHITE, 0.55f), 12);

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

//     // Weigh alerts that appeared since the last refresh (+ raise a toast)
//     {
//         static std::set<int> seenAlerts;
//         static bool seeded = false;

//         for (const AlertInfo& a : alerts)
//         {
//             if (seenAlerts.insert(a.id).second && seeded)
//             {
//                 threatAccum += SeverityWeight(a.severity);

//                 toastText     = a.description;
//                 toastSeverity = a.severity;
//                 toastAt       = now;
//             }
//         }

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
// // TOAST - slides in from the right when a new alert is raised
// // ------------------------------------------------------------

// static void DrawToast()
// {
//     const double life = 5.0;
//     const double age  = ImGui::GetTime() - toastAt;

//     if (age < 0.0 || age > life || toastText.empty())
//         return;

//     const float in  = Smooth01((float)(age / 0.35));
//     const float out = Smooth01((float)((life - age) / 0.45));
//     const float k   = std::min(in, out);

//     const ImVec2 disp = ImGui::GetIO().DisplaySize;
//     const float  w = 400.0f;
//     const float  h = 70.0f;
//     const ImVec4 sc = SeverityColor(toastSeverity.c_str());

//     const ImVec2 a(disp.x - w - 22.0f + (1.0f - k) * (w + 50.0f), 22.0f);
//     const ImVec2 b(a.x + w, a.y + h);

//     ImDrawList* d = ImGui::GetForegroundDrawList();

//     DrawSoftShadow(d, a, b, 14.0f, 16.0f, 0.55f * k);

//     d->AddRectFilled(a, b, Col(ImVec4(0.04f, 0.06f, 0.10f, 1.0f), 0.96f * k), 14.0f);
//     d->AddRectFilledMultiColor(
//         Off(a, 2.0f, 2.0f), ImVec2(a.x + w * 0.7f, b.y - 2.0f),
//         Col(sc, 0.18f * k), Col(sc, 0.0f), Col(sc, 0.0f), Col(sc, 0.18f * k));
//     d->AddRect(a, b, Col(sc, 0.75f * k), 14.0f, 0, 1.4f);
//     d->AddRectFilled(a, ImVec2(a.x + 5.0f, b.y), Col(sc, k), 14.0f,
//         ImDrawFlags_RoundCornersLeft);

//     // Icon
//     const ImVec2 ic(a.x + 38.0f, a.y + h * 0.5f);
//     d->AddCircleFilled(ic, 18.0f, Col(sc, 0.18f * k), 32);
//     d->AddCircle(ic, 18.0f, Col(sc, 0.85f * k), 32, 1.5f);
//     d->AddTriangleFilled(Off(ic, 0.0f, -9.0f), Off(ic, -9.0f, 7.0f), Off(ic, 9.0f, 7.0f),
//         Col(sc, k));
//     d->AddText(Off(ic, -2.5f, -5.0f), Col(FX_BLACK, 0.9f * k), "!");

//     // Text
//     char head[64];
//     std::snprintf(head, sizeof(head), "NEW ALERT  -  %s", toastSeverity.c_str());

//     std::string body = toastText;
//     if (body.size() > 54)
//         body = body.substr(0, 51) + "...";

//     d->AddText(Off(a, 68.0f, 14.0f), Col(sc, k), head);
//     d->AddText(Off(a, 68.0f, 38.0f), ImGui::GetColorU32(WithAlpha(COLOR_TEXT, k)),
//         body.c_str());

//     // Time-left bar
//     const float left = ClampFloat((float)((life - age) / life), 0.0f, 1.0f);
//     d->AddRectFilled(Off(a, 14.0f, h - 5.0f), ImVec2(a.x + 14.0f + (w - 28.0f) * left, b.y - 3.0f),
//         Col(sc, 0.65f * k), 2.0f);
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
//     const float  cardH = 150.0f;
//     const ImVec2 cp    = ImGui::GetCursorScreenPos();

//     DrawSoftShadow(ImGui::GetWindowDrawList(), cp, ImVec2(cp.x + width, cp.y + cardH),
//         ImGui::GetStyle().ChildRounding, 12.0f, 0.34f);

//     ImGui::BeginChild(
//         title,
//         ImVec2(width, cardH),
//         true,
//         ImGuiWindowFlags_NoScrollbar
//     );

//     const bool  hovered = linkPage >= 0 && ImGui::IsWindowHovered();
//     const float hv      = AnimTo(ImGui::GetID("##hv"), hovered ? 1.0f : 0.0f);
//     const float t       = (float)ImGui::GetTime();

//     {
//         const ImVec2 wp = ImGui::GetWindowPos();
//         const ImVec2 ws = ImGui::GetWindowSize();
//         ImDrawList*  d  = ImGui::GetWindowDrawList();

//         // soft tint from the left
//         d->AddRectFilledMultiColor(
//             ImVec2(wp.x + 1.0f, wp.y + 1.0f),
//             ImVec2(wp.x + ws.x * 0.70f, wp.y + ws.y - 1.0f),
//             Col(accent, 0.12f + 0.06f * hv),
//             Col(accent, 0.0f),
//             Col(accent, 0.0f),
//             Col(accent, 0.12f + 0.06f * hv));

//         DrawCardChrome(d, wp, ws, accent, hv);

//         // accent stripe
//         d->AddRectFilled(wp, ImVec2(wp.x + 4.0f, wp.y + ws.y),
//             ImGui::GetColorU32(accent),
//             ImGui::GetStyle().ChildRounding,
//             ImDrawFlags_RoundCornersLeft);

//         // glow + ring gauge (or page icon) on the right side
//         const ImVec2 gc(wp.x + ws.x - 52.0f, wp.y + 60.0f);

//         DrawGlow(d, gc, 48.0f + 8.0f * hv, accent);

//         if (gauge >= 0.0f)
//         {
//             // tick ring that lights up with the value
//             const int ticks = 30;
//             const float frac = ClampFloat(gauge / 100.0f, 0.0f, 1.0f);

//             for (int i = 0; i < ticks; ++i)
//             {
//                 const float a  = -HIDS_PI * 0.5f + (float)i * (HIDS_2PI / (float)ticks);
//                 const bool  on = ((float)i + 0.5f) / (float)ticks <= frac;
//                 const float r0 = 31.0f;
//                 const float r1 = on ? 36.0f : 34.0f;

//                 d->AddLine(
//                     ImVec2(gc.x + std::cos(a) * r0, gc.y + std::sin(a) * r0),
//                     ImVec2(gc.x + std::cos(a) * r1, gc.y + std::sin(a) * r1),
//                     Col(accent, on ? 0.85f : 0.14f), on ? 2.0f : 1.2f);
//             }

//             DrawGauge(d, gc, 24.0f, 6.0f, frac, accent);
//         }
//         else if (linkPage >= 0)
//         {
//             d->AddCircle(gc, 26.0f, Col(accent, 0.25f), 40, 1.2f);
//             d->AddCircleFilled(gc, 22.0f, Col(accent, 0.10f), 40);
//             DrawPageIcon(d, gc, 14.0f, linkPage,
//                 ImGui::GetColorU32(WithAlpha(accent, 0.85f)));
//         }
//     }

//     ImGui::TextDisabled("%s", title);

//     // Trend arrow next to the title (sparkline cards only)
//     if (history != nullptr)
//     {
//         const int   last  = (historyOffset + HISTORY_SIZE - 1) % HISTORY_SIZE;
//         const int   prev  = (historyOffset + HISTORY_SIZE - 9) % HISTORY_SIZE;
//         const float delta = history[last] - history[prev];
//         const bool  up    = delta >= 0.0f;
//         const bool  flat  = std::fabs(delta) < 1.0f;
//         const ImVec4 tc   = flat ? COLOR_MUTED : (up ? COLOR_ORANGE : COLOR_GREEN);

//         ImGui::SameLine(0.0f, 10.0f);

//         const ImVec2 q  = ImGui::GetCursorScreenPos();
//         const float  lh = ImGui::GetTextLineHeight();
//         ImDrawList*  d  = ImGui::GetWindowDrawList();

//         if (flat)
//             d->AddRectFilled(Off(q, 0.0f, lh * 0.5f - 1.0f), Off(q, 9.0f, lh * 0.5f + 1.0f),
//                 ImGui::GetColorU32(tc), 1.0f);
//         else if (up)
//             d->AddTriangleFilled(Off(q, 4.5f, lh * 0.5f - 4.0f), Off(q, 0.0f, lh * 0.5f + 3.0f),
//                 Off(q, 9.0f, lh * 0.5f + 3.0f), ImGui::GetColorU32(tc));
//         else
//             d->AddTriangleFilled(Off(q, 0.0f, lh * 0.5f - 3.0f), Off(q, 9.0f, lh * 0.5f - 3.0f),
//                 Off(q, 4.5f, lh * 0.5f + 4.0f), ImGui::GetColorU32(tc));

//         ImGui::Dummy(ImVec2(10.0f, lh));
//         ImGui::SameLine(0.0f, 4.0f);
//         ImGui::TextColored(tc, "%.0f", std::fabs(delta));
//     }

//     ImGui::PushStyleColor(ImGuiCol_Text, accent);
//     ImGui::SetWindowFontScale(1.65f);
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
//     if (hovered)
//     {
//         ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
//         ImGui::SetTooltip("Open %s", pages[linkPage]);

//         // sweeping shine on hover
//         {
//             const ImVec2 wp = ImGui::GetWindowPos();
//             const ImVec2 ws = ImGui::GetWindowSize();
//             const float  sx = wp.x + (ws.x + 60.0f) * std::fmod(t * 0.9f, 1.0f) - 30.0f;

//             ImDrawList* d = ImGui::GetWindowDrawList();
//             d->PushClipRect(Off(wp, 1.0f, 1.0f), ImVec2(wp.x + ws.x - 1.0f, wp.y + ws.y - 1.0f), true);
//             d->AddRectFilledMultiColor(
//                 ImVec2(sx - 30.0f, wp.y), ImVec2(sx + 30.0f, wp.y + ws.y),
//                 Col(FX_WHITE, 0.0f), Col(FX_WHITE, 0.07f), Col(FX_WHITE, 0.07f), Col(FX_WHITE, 0.0f));
//             d->PopClipRect();
//         }

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

// // Centred title with a drop shadow and a faint glow halo
// static void CenteredTitle(const char* text, float scale, const ImVec4& col)
// {
//     ImGui::SetWindowFontScale(scale);

//     const float w = ImGui::CalcTextSize(text).x;
//     ImGui::SetCursorPosX(std::max(0.0f, (ImGui::GetWindowWidth() - w) * 0.5f));

//     const ImVec2 p = ImGui::GetCursorScreenPos();
//     ImDrawList*  d = ImGui::GetWindowDrawList();

//     d->AddText(Off(p, 2.0f, 3.0f), Col(FX_BLACK, 0.55f), text);
//     d->AddText(Off(p, -1.0f, 0.0f), Col(col, 0.20f), text);
//     d->AddText(Off(p, 1.0f, 0.0f),  Col(col, 0.20f), text);
//     d->AddText(Off(p, 0.0f, -1.0f), Col(col, 0.20f), text);
//     d->AddText(Off(p, 0.0f, 1.0f),  Col(col, 0.20f), text);

//     ImGui::TextColored(col, "%s", text);
//     ImGui::SetWindowFontScale(1.0f);
// }

// // Centred inside the current content area (use inside panels)
// static void CenteredInContent(const char* text, float scale, const ImVec4& col)
// {
//     const float avail = ImGui::GetContentRegionAvail().x;

//     ImGui::SetWindowFontScale(scale);
//     const float w = ImGui::CalcTextSize(text).x;
//     ImGui::SetCursorPosX(ImGui::GetCursorPosX() + std::max(0.0f, (avail - w) * 0.5f));
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

//     // inner bevel for depth
//     ImVec2 inner[5] = {
//         ImVec2(c.x - 0.62f * s, c.y - 0.72f * s),
//         ImVec2(c.x + 0.62f * s, c.y - 0.72f * s),
//         ImVec2(c.x + 0.62f * s, c.y + 0.06f * s),
//         ImVec2(c.x,             c.y + 0.78f * s),
//         ImVec2(c.x - 0.62f * s, c.y + 0.06f * s)
//     };
//     d->AddPolyline(inner, 5, Col(FX_WHITE, 0.30f), ImDrawFlags_Closed, 1.2f);

//     ImVec2 tick[3] = {
//         ImVec2(c.x - 0.36f * s, c.y - 0.05f * s),
//         ImVec2(c.x - 0.08f * s, c.y + 0.25f * s),
//         ImVec2(c.x + 0.42f * s, c.y - 0.38f * s)
//     };
//     d->AddPolyline(tick, 3, IM_COL32(8, 16, 28, 255), 0, s * 0.16f);
// }

// static void DrawLockIcon(ImDrawList* d, const ImVec2& c, float s, const ImVec4& col)
// {
//     // shackle
//     d->PathClear();
//     d->PathArcTo(ImVec2(c.x, c.y - 0.05f * s), s * 0.60f, HIDS_PI, HIDS_PI * 2.0f, 24);
//     d->PathStroke(ImGui::GetColorU32(col), 0, s * 0.26f);

//     // body
//     d->AddRectFilled(ImVec2(c.x - s * 0.95f, c.y - 0.05f * s),
//                      ImVec2(c.x + s * 0.95f, c.y + s * 0.95f),
//                      ImGui::GetColorU32(col), s * 0.18f);

//     d->AddRectFilled(ImVec2(c.x - s * 0.85f, c.y + 0.02f * s),
//                      ImVec2(c.x + s * 0.85f, c.y + s * 0.30f),
//                      Col(FX_WHITE, 0.16f), s * 0.14f);

//     // keyhole
//     d->AddCircleFilled(ImVec2(c.x, c.y + s * 0.38f), s * 0.17f,
//                        IM_COL32(8, 16, 28, 255), 16);
// }

// // Clickable text link (breadcrumbs). Returns true when clicked.
// static bool CrumbLink(const char* label, const ImVec4& col)
// {
//     ImGui::PushID(label);

//     const ImVec2 sz = ImGui::CalcTextSize(label);
//     const ImVec2 p  = ImGui::GetCursorScreenPos();

//     const bool clicked = ImGui::InvisibleButton("##crumb", sz);
//     const bool hov     = ImGui::IsItemHovered();

//     ImDrawList*  d = ImGui::GetWindowDrawList();
//     const ImU32  u = ImGui::GetColorU32(hov ? col : WithAlpha(col, 0.78f));

//     d->AddText(p, u, label);

//     if (hov)
//     {
//         d->AddLine(ImVec2(p.x, p.y + sz.y), ImVec2(p.x + sz.x, p.y + sz.y), u, 1.0f);
//         ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
//     }

//     ImGui::PopID();
//     return clicked;
// }

// // Small "value + label" pill. Caller uses SameLine() between chips.
// static void StatChip(const char* label, const char* value, const ImVec4& col)
// {
//     const ImVec2 vs = ImGui::CalcTextSize(value);
//     const ImVec2 ls = ImGui::CalcTextSize(label);
//     const float  h  = 30.0f;
//     const float  w  = 14.0f + 10.0f + vs.x + 8.0f + ls.x + 14.0f;

//     const ImVec2 p = ImGui::GetCursorScreenPos();
//     ImDrawList*  d = ImGui::GetWindowDrawList();

//     d->AddRectFilled(p, ImVec2(p.x + w, p.y + h),
//         Col(col, 0.12f), h * 0.5f);
//     d->AddRectFilled(Off(p, 6.0f, 2.0f), ImVec2(p.x + w - 6.0f, p.y + h * 0.45f),
//         Col(FX_WHITE, 0.05f), h * 0.25f);
//     d->AddRect(p, ImVec2(p.x + w, p.y + h),
//         Col(col, 0.45f), h * 0.5f);

//     d->AddCircleFilled(ImVec2(p.x + 16.0f, p.y + h * 0.5f), 6.5f, Col(col, 0.20f), 20);
//     d->AddCircleFilled(ImVec2(p.x + 16.0f, p.y + h * 0.5f), 4.0f,
//         ImGui::GetColorU32(col));

//     const float ty = p.y + (h - vs.y) * 0.5f;

//     d->AddText(ImVec2(p.x + 28.0f, ty), ImGui::GetColorU32(col), value);
//     d->AddText(ImVec2(p.x + 28.0f + vs.x + 8.0f, ty),
//         ImGui::GetColorU32(COLOR_MUTED), label);

//     ImGui::Dummy(ImVec2(w, h));
// }

// // Same chip, but it wraps to the next row when it does not fit on the
// // current one (so a row of chips never runs out of the panel).
// static void StatChipFlow(const char* label, const char* value,
//                          const ImVec4& col, bool first)
// {
//     if (!first)
//     {
//         const float w = 14.0f + 10.0f + ImGui::CalcTextSize(value).x +
//                         8.0f + ImGui::CalcTextSize(label).x + 14.0f;

//         const float rightEdge = ImGui::GetCursorScreenPos().x +
//                                 ImGui::GetContentRegionAvail().x;
//         const float prevRight = ImGui::GetItemRectMax().x;
//         const float gap       = ImGui::GetStyle().ItemSpacing.x;

//         if (prevRight + gap + w <= rightEdge)
//             ImGui::SameLine();
//     }

//     StatChip(label, value, col);
// }

// // Friendly animated graphic for pages / tables with nothing to show
// static void EmptyState(const char* title, const char* hint, const ImVec4& col)
// {
//     const float avail = ImGui::GetContentRegionAvail().x;
//     const float t     = (float)ImGui::GetTime();

//     ImGui::Dummy(ImVec2(0.0f, 26.0f));

//     const ImVec2 p = ImGui::GetCursorScreenPos();
//     const ImVec2 c(p.x + avail * 0.5f, p.y + 46.0f);
//     ImDrawList*  d = ImGui::GetWindowDrawList();

//     const float phase  = std::fmod(t * 0.6f, 1.0f);
//     const float phase2 = std::fmod(t * 0.6f + 0.5f, 1.0f);

//     DrawGlow(d, c, 78.0f, col);
//     d->AddCircleFilled(c, 36.0f, Col(col, 0.12f), 48);
//     d->AddCircle(c, 36.0f, Col(col, 0.70f), 48, 1.8f);
//     d->AddCircle(c, 36.0f + phase * 22.0f,  Col(col, 0.35f * (1.0f - phase)),  48, 1.4f);
//     d->AddCircle(c, 36.0f + phase2 * 22.0f, Col(col, 0.25f * (1.0f - phase2)), 48, 1.2f);

//     // little orbiting satellites
//     for (int i = 0; i < 3; ++i)
//     {
//         const float a = t * 0.9f + (float)i * (HIDS_2PI / 3.0f);

//         d->AddCircleFilled(
//             ImVec2(c.x + std::cos(a) * 50.0f, c.y + std::sin(a) * 50.0f),
//             2.6f, Col(col, 0.75f), 12);
//     }

//     ImVec2 tick[3] = {
//         ImVec2(c.x - 12.0f, c.y + 1.0f),
//         ImVec2(c.x - 4.0f,  c.y + 9.0f),
//         ImVec2(c.x + 13.0f, c.y - 9.0f)
//     };
//     d->AddPolyline(tick, 3, ImGui::GetColorU32(col), 0, 3.2f);

//     ImGui::Dummy(ImVec2(avail, 108.0f));

//     CenteredInContent(title, 1.25f, COLOR_TEXT);
//     CenteredInContent(hint, 1.0f, COLOR_MUTED);
// }

// // Panel used by every inner page: glass card with shadow, a coloured top
// // line and a soft gradient. Always pair with ImGui::EndChild().
// static void BeginPanel(const char* id, const ImVec2& size,
//                        const ImVec4& accent, bool noScroll = false)
// {
//     // Resolve the real size so the shadow can be drawn first
//     ImVec2 sz = size;
//     const ImVec2 av = ImGui::GetContentRegionAvail();

//     if (sz.x <= 0.0f) sz.x = std::max(10.0f, av.x + sz.x);
//     if (sz.y <= 0.0f) sz.y = std::max(10.0f, av.y + sz.y);

//     const ImVec2 cp = ImGui::GetCursorScreenPos();

//     DrawSoftShadow(ImGui::GetWindowDrawList(), cp, ImVec2(cp.x + sz.x, cp.y + sz.y),
//         ImGui::GetStyle().ChildRounding, 14.0f, 0.30f);

//     ImGui::BeginChild(id, size, true,
//         noScroll ? ImGuiWindowFlags_NoScrollbar : 0);

//     const ImVec2 wp = ImGui::GetWindowPos();
//     const ImVec2 ws = ImGui::GetWindowSize();
//     ImDrawList*  d  = ImGui::GetWindowDrawList();

//     d->AddRectFilledMultiColor(
//         ImVec2(wp.x + 1.0f, wp.y + 3.0f),
//         ImVec2(wp.x + ws.x - 1.0f, wp.y + std::min(ws.y, 170.0f)),
//         Col(accent, 0.10f),
//         Col(accent, 0.10f),
//         Col(accent, 0.0f),
//         Col(accent, 0.0f));

//     DrawCardChrome(d, wp, ws, accent, 0.0f);

//     // glowing top line (fades toward the right)
//     d->AddRectFilledMultiColor(
//         ImVec2(wp.x + 16.0f, wp.y),
//         ImVec2(wp.x + ws.x - 16.0f, wp.y + 2.5f),
//         Col(accent, 0.95f), Col(accent, 0.25f), Col(accent, 0.25f), Col(accent, 0.95f));

//     d->AddRectFilledMultiColor(
//         ImVec2(wp.x + 16.0f, wp.y + 2.5f),
//         ImVec2(wp.x + ws.x * 0.55f, wp.y + 9.0f),
//         Col(accent, 0.22f), Col(accent, 0.0f), Col(accent, 0.0f), Col(accent, 0.0f));
// }

// // Logo tile + product name. Returns true when clicked (-> welcome screen).
// static bool DrawBrand(bool clickable = false)
// {
//     ImDrawList* d = ImGui::GetWindowDrawList();
//     const ImVec2 p = ImGui::GetCursorScreenPos();
//     const float  s = 40.0f;
//     const float  t = (float)ImGui::GetTime();

//     DrawGlow(d, ImVec2(p.x + s * 0.5f, p.y + s * 0.5f), 50.0f, COLOR_CYAN);

//     d->AddRectFilledMultiColor(p, ImVec2(p.x + s, p.y + s),
//         Col(COLOR_CYAN, 0.30f), Col(COLOR_CYAN, 0.10f),
//         Col(COLOR_CYAN, 0.06f), Col(COLOR_CYAN, 0.18f));
//     d->AddRect(p, ImVec2(p.x + s, p.y + s),
//         Col(COLOR_CYAN, 0.65f), 10.0f, 0, 1.2f);

//     const ImVec2 c(p.x + s * 0.5f, p.y + s * 0.5f + 0.5f);
//     DrawShieldLogo(d, c, 11.0f, COLOR_CYAN);

//     // slow scanning arc around the tile
//     d->PathClear();
//     d->PathArcTo(c, s * 0.62f, t * 1.4f, t * 1.4f + 0.9f, 16);
//     d->PathStroke(Col(COLOR_CYAN, 0.55f), 0, 1.6f);

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

// // Right-aligned engine status + Start/Stop button on a glass capsule
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

//     // capsule behind the status + button
//     {
//         const ImVec2 p = ImGui::GetCursorScreenPos();
//         const float  h = ImGui::GetFrameHeight();
//         ImDrawList*  d = ImGui::GetWindowDrawList();

//         const ImVec2 a(p.x - 14.0f, p.y - 6.0f);
//         const ImVec2 b(p.x + total + 8.0f, p.y + h + 6.0f);

//         d->AddRectFilled(a, b, Col(col, 0.07f), (b.y - a.y) * 0.5f);
//         d->AddRect(a, b, Col(col, 0.30f), (b.y - a.y) * 0.5f);
//     }

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

// // Drifting aurora glows, a reactive dot grid, a cursor halo, a vignette
// // and (while protection runs) a slow scan band behind every screen
// static void DrawAmbient()
// {
//     ImDrawList*  bg   = ImGui::GetBackgroundDrawList();
//     const ImVec2 disp = ImGui::GetIO().DisplaySize;
//     const float  t    = (float)ImGui::GetTime();

//     const ImVec4 a = monitoringActive ? COLOR_GREEN : COLOR_CYAN;

//     DrawGlow(bg,
//         ImVec2(disp.x * (0.12f + 0.04f * std::sin(t * 0.20f)),
//                disp.y * (0.05f + 0.03f * std::cos(t * 0.13f))),
//         380.0f, WithAlpha(a, 0.55f));

//     DrawGlow(bg,
//         ImVec2(disp.x * (0.90f + 0.03f * std::sin(t * 0.17f + 1.0f)), disp.y * 0.95f),
//         440.0f, WithAlpha(COLOR_BLUE, 0.50f));

//     DrawGlow(bg,
//         ImVec2(disp.x * (0.55f + 0.10f * std::sin(t * 0.11f + 2.0f)),
//                disp.y * (0.45f + 0.08f * std::cos(t * 0.09f))),
//         300.0f, WithAlpha(monitoringActive ? COLOR_CYAN : COLOR_ORANGE, 0.16f));

//     const bool  mouseOk = ImGui::IsMousePosValid();
//     const ImVec2 m      = ImGui::GetIO().MousePos;

//     if (mouseOk)
//         DrawGlow(bg, m, 150.0f, WithAlpha(COLOR_CYAN, 0.16f));

//     if (appScreen != SCREEN_WELCOME)
//     {
//         const float step = 46.0f;

//         for (float y = step * 0.5f; y < disp.y; y += step)
//         {
//             for (float x = step * 0.5f; x < disp.x; x += step)
//             {
//                 float lit = 0.0f;

//                 if (mouseOk)
//                 {
//                     const float dx = x - m.x;
//                     const float dy = y - m.y;
//                     const float dist = std::sqrt(dx * dx + dy * dy);
//                     lit = std::max(0.0f, 1.0f - dist / 230.0f);
//                 }

//                 const float sz = 2.0f + lit * 1.5f;

//                 bg->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz),
//                     Col(COLOR_CYAN, 0.045f + 0.20f * lit * lit));
//             }
//         }
//     }

//     // slow scan band while protection is running
//     if (monitoringActive)
//     {
//         const float ph   = std::fmod(t * 0.09f, 1.0f);
//         const float y    = -120.0f + (disp.y + 240.0f) * ph;
//         const float half = 60.0f;

//         bg->AddRectFilledMultiColor(ImVec2(0.0f, y - half), ImVec2(disp.x, y),
//             Col(COLOR_GREEN, 0.0f), Col(COLOR_GREEN, 0.0f),
//             Col(COLOR_GREEN, 0.045f), Col(COLOR_GREEN, 0.045f));
//         bg->AddRectFilledMultiColor(ImVec2(0.0f, y), ImVec2(disp.x, y + half),
//             Col(COLOR_GREEN, 0.045f), Col(COLOR_GREEN, 0.045f),
//             Col(COLOR_GREEN, 0.0f), Col(COLOR_GREEN, 0.0f));
//     }

//     // Vignette
//     {
//         const ImU32 dark = IM_COL32(0, 0, 0, 90);
//         const ImU32 none = IM_COL32(0, 0, 0, 0);
//         const float vh = disp.y * 0.16f;
//         const float vw = disp.x * 0.10f;

//         bg->AddRectFilledMultiColor(ImVec2(0, 0), ImVec2(disp.x, vh), dark, dark, none, none);
//         bg->AddRectFilledMultiColor(ImVec2(0, disp.y - vh), ImVec2(disp.x, disp.y), none, none, dark, dark);
//         bg->AddRectFilledMultiColor(ImVec2(0, 0), ImVec2(vw, disp.y), dark, none, none, dark);
//         bg->AddRectFilledMultiColor(ImVec2(disp.x - vw, 0), ImVec2(disp.x, disp.y), none, dark, dark, none);
//     }
// }

// // Header of a feature page: Back button, breadcrumb, icon, title.
// // No sidebar - just this feature and the way back.
// static void DrawPageHeader()
// {
//     const int    domain = DomainOfPage(selectedPage);
//     const ImVec4 accent = DomainAccent(domain);
//     const float  t      = (float)ImGui::GetTime();

//     DrawSoftShadow(ImGui::GetWindowDrawList(), ImGui::GetCursorScreenPos(),
//         ImVec2(ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x,
//                ImGui::GetCursorScreenPos().y + 134.0f),
//         ImGui::GetStyle().ChildRounding, 14.0f, 0.30f);

//     ImGui::BeginChild("PageHeader", ImVec2(0.0f, 134.0f), true,
//                       ImGuiWindowFlags_NoScrollbar);

//     const ImVec2 wp = ImGui::GetWindowPos();
//     const ImVec2 ws = ImGui::GetWindowSize();
//     ImDrawList*  d  = ImGui::GetWindowDrawList();

//     // Card background: gradient + accent line along the bottom
//     d->AddRectFilledMultiColor(
//         ImVec2(wp.x + 1.0f, wp.y + 1.0f), ImVec2(wp.x + ws.x - 1.0f, wp.y + ws.y - 1.0f),
//         Col(accent, 0.18f),
//         Col(accent, 0.03f),
//         Col(accent, 0.0f),
//         Col(accent, 0.06f));

//     // Big faint watermark of the page icon on the right
//     DrawPageIcon(d, ImVec2(wp.x + ws.x - 130.0f, wp.y + ws.y * 0.55f), 46.0f,
//         selectedPage, Col(accent, 0.07f));

//     DrawCardChrome(d, wp, ws, accent, 0.0f);

//     d->AddRectFilledMultiColor(
//         ImVec2(wp.x + 16.0f, wp.y + ws.y - 3.0f),
//         ImVec2(wp.x + ws.x - 16.0f, wp.y + ws.y - 1.0f),
//         Col(accent, 0.90f),
//         Col(accent, 0.0f),
//         Col(accent, 0.0f),
//         Col(accent, 0.90f));

//     // Light comet travelling along the bottom line
//     {
//         const float x0  = wp.x + 16.0f;
//         const float x1  = wp.x + ws.x - 16.0f;
//         const float pos = x0 + (x1 - x0) * std::fmod(t * 0.16f, 1.0f);
//         const float a   = std::max(x0, pos - 150.0f);

//         if (pos > a)
//             d->AddRectFilledMultiColor(
//                 ImVec2(a, wp.y + ws.y - 3.5f), ImVec2(pos, wp.y + ws.y - 0.5f),
//                 Col(FX_WHITE, 0.0f), Col(FX_WHITE, 0.90f),
//                 Col(FX_WHITE, 0.90f), Col(FX_WHITE, 0.0f));
//     }

//     const float padX = ImGui::GetCursorPosX();
//     const float topY = ImGui::GetCursorPosY();
//     const float lh   = ImGui::GetTextLineHeight();

//     // Row 1: Back + breadcrumb (all on one baseline)
//     if (ImGui::Button("<  Back", ImVec2(96.0f, 32.0f)))
//     {
//         selectedDomain = domain;
//         appScreen      = SCREEN_DOMAIN;
//     }

//     ImGui::SameLine(0.0f, 18.0f);
//     ImGui::SetCursorPosY(topY + (32.0f - lh) * 0.5f);

//     const ImVec4 link = COLOR_MUTED;

//     if (CrumbLink("Welcome", link))
//         appScreen = SCREEN_WELCOME;

//     ImGui::SameLine(0.0f, 8.0f);
//     ImGui::TextDisabled(">");
//     ImGui::SameLine(0.0f, 8.0f);

//     if (CrumbLink("Home", link))
//         appScreen = SCREEN_HUB;

//     ImGui::SameLine(0.0f, 8.0f);
//     ImGui::TextDisabled(">");
//     ImGui::SameLine(0.0f, 8.0f);

//     if (CrumbLink(hidsDomains[domain].title, link))
//     {
//         selectedDomain = domain;
//         appScreen      = SCREEN_DOMAIN;
//     }

//     ImGui::SameLine(0.0f, 8.0f);
//     ImGui::TextDisabled(">");
//     ImGui::SameLine(0.0f, 8.0f);
//     ImGui::TextColored(accent, "%s", pages[selectedPage]);

//     DrawHeaderStatus(topY);

//     // Row 2: icon + title + subtitle
//     {
//         const ImVec2 ic(wp.x + padX + 30.0f, wp.y + topY + 46.0f + 31.0f);

//         DrawGlow(d, ic, 56.0f, accent);
//         d->AddCircleFilled(ic, 28.0f, Col(accent, 0.18f), 48);
//         d->AddCircle(ic, 28.0f, Col(accent, 0.80f), 48, 1.6f);

//         // two rotating arcs around the icon
//         d->PathClear();
//         d->PathArcTo(ic, 34.0f, t * 1.2f, t * 1.2f + 1.3f, 20);
//         d->PathStroke(Col(accent, 0.85f), 0, 2.2f);

//         d->PathClear();
//         d->PathArcTo(ic, 34.0f, t * 1.2f + HIDS_PI, t * 1.2f + HIDS_PI + 0.7f, 12);
//         d->PathStroke(Col(accent, 0.45f), 0, 2.2f);

//         DrawPageIcon(d, ic, 13.0f, selectedPage, ImGui::GetColorU32(accent));
//     }

//     ImGui::SetCursorPos(ImVec2(padX + 82.0f, topY + 46.0f));
//     ImGui::BeginGroup();
//     ImGui::SetWindowFontScale(1.6f);
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
//     const float t    = (float)ImGui::GetTime();

//     BeginPanel("EngineBanner", ImVec2(0.0f, 104.0f), COLOR_YELLOW, true);

//     const ImVec2 wp = ImGui::GetWindowPos();
//     ImDrawList*  d  = ImGui::GetWindowDrawList();

//     // warning beacon
//     {
//         const ImVec2 c(wp.x + 48.0f, wp.y + 52.0f);
//         const float  ph = std::fmod(t * 0.8f, 1.0f);

//         DrawGlow(d, c, 58.0f, COLOR_YELLOW);
//         d->AddCircleFilled(c, 28.0f, Col(COLOR_YELLOW, 0.14f), 40);
//         d->AddCircle(c, 28.0f, Col(COLOR_YELLOW, 0.70f), 40, 1.6f);
//         d->AddCircle(c, 28.0f + ph * 16.0f, Col(COLOR_YELLOW, 0.40f * (1.0f - ph)), 40, 1.4f);

//         d->AddTriangleFilled(Off(c, 0.0f, -12.0f), Off(c, -12.0f, 10.0f), Off(c, 12.0f, 10.0f),
//             ImGui::GetColorU32(COLOR_YELLOW));
//         d->AddText(Off(c, -2.5f, -6.0f), IM_COL32(8, 16, 28, 255), "!");
//     }

//     const float wrapX = ImGui::GetWindowWidth() - btnW - 60.0f;

//     ImGui::SetCursorPos(ImVec2(96.0f, 16.0f));
//     ImGui::BeginGroup();
//     ImGui::PushTextWrapPos(wrapX);
//     ImGui::SetWindowFontScale(1.2f);
//     ImGui::TextUnformatted("Protection is stopped");
//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::TextDisabled("Start the engine to collect host events, watch processes "
//                         "and the monitored folder, and raise alerts.");
//     ImGui::PopTextWrapPos();
//     ImGui::EndGroup();

//     ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - btnW - 20.0f, 32.0f));

//     PushGreenButton();
//     if (ImGui::Button("Start protection", ImVec2(btnW, 40.0f)))
//         StartEngine();
//     ImGui::PopStyleColor(3);

//     ImGui::EndChild();
//     ImGui::Spacing();
// }

// // Shown on the dashboard while protection runs: overall threat level
// static void DrawPostureBanner()
// {
//     if (!monitoringActive)
//         return;

//     const int crit = CountSeverity("Critical");
//     const int high = CountSeverity("High");
//     const int med  = CountSeverity("Medium");

//     const char* level = "Low";
//     ImVec4      col   = COLOR_GREEN;

//     if (crit > 0)      { level = "Critical"; col = COLOR_RED;    }
//     else if (high > 0) { level = "Elevated"; col = COLOR_ORANGE; }
//     else if (med > 0)  { level = "Guarded";  col = COLOR_YELLOW; }

//     const double now = ImGui::GetTime();

//     if (engineStartedAt < 0.0)
//         engineStartedAt = now;

//     BeginPanel("PostureBanner", ImVec2(0.0f, 98.0f), col, true);

//     const ImVec2 wp = ImGui::GetWindowPos();
//     const ImVec2 ws = ImGui::GetWindowSize();
//     ImDrawList*  d  = ImGui::GetWindowDrawList();
//     const float  t  = (float)now;

//     const ImVec2 c(wp.x + 56.0f, wp.y + 50.0f);

//     DrawGlow(d, c, 68.0f, col);
//     d->AddCircleFilled(c, 34.0f, Col(col, 0.14f), 48);
//     d->AddCircle(c, 34.0f, Col(col, 0.55f), 48, 1.4f);

//     {
//         const float ph = std::fmod(t * 0.7f, 1.0f);
//         d->AddCircle(c, 34.0f + ph * 12.0f, Col(col, 0.35f * (1.0f - ph)), 48, 1.2f);
//     }

//     d->PathClear();
//     d->PathArcTo(c, 40.0f, t * 1.6f, t * 1.6f + 1.6f, 24);
//     d->PathStroke(ImGui::GetColorU32(col), 0, 2.5f);

//     DrawShieldLogo(d, c, 16.0f, col);

//     ImGui::SetCursorPos(ImVec2(112.0f, 18.0f));
//     ImGui::BeginGroup();
//     ImGui::TextDisabled("THREAT LEVEL");
//     ImGui::SetWindowFontScale(1.6f);
//     ImGui::TextColored(col, "%s", level);
//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::EndGroup();

//     char l1[64];
//     char l2[96];

//     std::snprintf(l1, sizeof(l1), "Monitoring for %s",
//         FormatDuration(now - engineStartedAt).c_str());
//     std::snprintf(l2, sizeof(l2), "%d alerts   -   %d processes watched",
//         (int)alerts.size(), processCount);

//     const float rightX1 = std::max(240.0f, ws.x - ImGui::CalcTextSize(l1).x - 24.0f);
//     const float rightX2 = std::max(240.0f, ws.x - ImGui::CalcTextSize(l2).x - 24.0f);

//     // Heartbeat line between the level text and the right-hand stats
//     {
//         const float x0 = wp.x + 290.0f;
//         const float x1 = wp.x + std::min(rightX1, rightX2) - 30.0f;

//         if (x1 - x0 > 120.0f)
//         {
//             const int   n  = 70;
//             const float cy = wp.y + 50.0f;
//             ImVec2      prev(x0, cy);

//             for (int i = 1; i <= n; ++i)
//             {
//                 const float u  = (float)i / (float)n;
//                 const float ph = std::fmod(u * 2.4f - t * 0.55f + 10.0f, 1.0f);

//                 const float y =
//                     -26.0f * std::exp(-std::pow((ph - 0.50f) * 22.0f, 2.0f)) +
//                      10.0f * std::exp(-std::pow((ph - 0.56f) * 26.0f, 2.0f)) +
//                       4.0f * std::exp(-std::pow((ph - 0.30f) * 10.0f, 2.0f));

//                 const ImVec2 cur(x0 + (x1 - x0) * u, cy + y);
//                 const float  fade = std::sin(u * HIDS_PI);

//                 d->AddLine(prev, cur, Col(col, 0.75f * fade), 1.8f);
//                 prev = cur;
//             }
//         }
//     }

//     ImGui::SetCursorPos(ImVec2(rightX1, 26.0f));
//     ImGui::TextColored(COLOR_GREEN, "%s", l1);

//     ImGui::SetCursorPos(ImVec2(rightX2, 52.0f));
//     ImGui::TextDisabled("%s", l2);

//     ImGui::EndChild();
//     ImGui::Spacing();
// }

// // ------------------------------------------------------------
// // WELCOME SCREEN
// // Reachable at any time. Shows whether HIDS is working.
// // ------------------------------------------------------------

// // Glass stat tile used on the welcome screen
// static void DrawMiniStat(const char* label, const char* value,
//                          const ImVec4& col, float w)
// {
//     const float  h = 60.0f;
//     const ImVec2 p = ImGui::GetCursorScreenPos();
//     ImDrawList*  d = ImGui::GetWindowDrawList();

//     d->AddRectFilled(p, ImVec2(p.x + w, p.y + h), Col(col, 0.08f), 12.0f);
//     d->AddRectFilledMultiColor(Off(p, 2.0f, 2.0f), ImVec2(p.x + w - 2.0f, p.y + h * 0.5f),
//         Col(FX_WHITE, 0.05f), Col(FX_WHITE, 0.02f), Col(FX_WHITE, 0.0f), Col(FX_WHITE, 0.0f));
//     d->AddRect(p, ImVec2(p.x + w, p.y + h), Col(col, 0.40f), 12.0f);
//     d->AddRectFilled(Off(p, 14.0f, 0.0f), ImVec2(p.x + w - 14.0f, p.y + 2.0f),
//         Col(col, 0.85f), 1.0f);

//     ImGui::SetCursorScreenPos(Off(p, 14.0f, 8.0f));
//     ImGui::TextDisabled("%s", label);

//     ImGui::SetWindowFontScale(1.3f);
//     ImGui::SetCursorScreenPos(Off(p, 14.0f, 26.0f));
//     ImGui::TextColored(col, "%s", value);
//     ImGui::SetWindowFontScale(1.0f);

//     ImGui::SetCursorScreenPos(p);
//     ImGui::Dummy(ImVec2(w, h));
// }

// // Row of the five security areas at the bottom of the welcome screen
// static void DrawDomainChips()
// {
//     const int   n      = IM_ARRAYSIZE(hidsDomains);
//     const float itemW  = std::min(150.0f, (ImGui::GetWindowWidth() - 40.0f) / (float)n);
//     const float total  = itemW * (float)n;

//     ImGui::SetCursorPosX(std::max(0.0f, (ImGui::GetWindowWidth() - total) * 0.5f));

//     const ImVec2 p0 = ImGui::GetCursorScreenPos();
//     ImDrawList*  d  = ImGui::GetWindowDrawList();

//     for (int i = 0; i < n; ++i)
//     {
//         const ImVec4 acc = DomainAccent(i);
//         const ImVec2 c(p0.x + itemW * ((float)i + 0.5f), p0.y + 20.0f);

//         if (i + 1 < n)
//             d->AddLine(Off(c, 24.0f, 0.0f), Off(c, itemW - 24.0f, 0.0f), Col(COLOR_CYAN, 0.14f), 1.0f);

//         d->AddCircleFilled(c, 18.0f, Col(acc, 0.14f), 32);
//         d->AddCircle(c, 18.0f, Col(acc, 0.60f), 32, 1.3f);
//         DrawPageIcon(d, c, 8.0f, domainIconPage[i], ImGui::GetColorU32(acc));

//         const ImVec2 ts = ImGui::CalcTextSize(hidsDomains[i].title);
//         d->AddText(ImVec2(c.x - ts.x * 0.5f, p0.y + 44.0f),
//             ImGui::GetColorU32(WithAlpha(COLOR_MUTED, 0.9f)), hidsDomains[i].title);
//     }

//     ImGui::Dummy(ImVec2(total, 66.0f));
// }

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
//     for (int i = 0; i < 56; ++i)
//     {
//         const float fx = std::fmod(i * 0.6180339f, 1.0f);
//         const float fy = std::fmod(i * 0.3819660f + t * (0.006f + 0.004f * (i % 4)), 1.0f);
//         const float a  = 0.08f + 0.08f * (0.5f + 0.5f * std::sin(t * 0.8f + i));

//         bg->AddCircleFilled(ImVec2(fx * disp.x, fy * disp.y), 1.2f + (i % 3),
//             Col(acc, a));
//     }

//     // Soft glow
//     DrawGlow(bg, c, 190.0f, acc);

//     // Radar grid
//     for (int k = 1; k <= 4; ++k)
//         bg->AddCircle(c, 70.0f + (float)k * 62.0f, Col(acc, 0.065f), 120, 1.0f);

//     bg->AddLine(ImVec2(c.x - 320.0f, c.y), ImVec2(c.x + 320.0f, c.y), Col(acc, 0.05f), 1.0f);
//     bg->AddLine(ImVec2(c.x, c.y - 320.0f), ImVec2(c.x, c.y + 320.0f), Col(acc, 0.05f), 1.0f);

//     // Radar sweep with a fading trail
//     {
//         const float sweep = t * 0.9f;
//         const float R     = 318.0f;
//         const float step  = 0.04f;

//         for (int k = 0; k < 30; ++k)
//         {
//             const float a0 = sweep - (float)k * step;
//             const float a1 = a0 - step;
//             const float f  = 1.0f - (float)k / 30.0f;

//             bg->AddTriangleFilled(c,
//                 ImVec2(c.x + std::cos(a0) * R, c.y + std::sin(a0) * R),
//                 ImVec2(c.x + std::cos(a1) * R, c.y + std::sin(a1) * R),
//                 Col(acc, 0.17f * f * f));
//         }

//         bg->AddLine(c, ImVec2(c.x + std::cos(sweep) * R, c.y + std::sin(sweep) * R),
//             Col(acc, 0.40f), 1.5f);

//         // Blips that flash as the sweep passes
//         for (int i = 0; i < 7; ++i)
//         {
//             const float ba = (float)i * 2.399f;
//             const float br = 96.0f + std::fmod((float)i * 53.0f, 210.0f);

//             float diff = std::fmod(sweep - ba, HIDS_2PI);
//             if (diff < 0.0f)
//                 diff += HIDS_2PI;

//             const float lit = std::max(0.0f, 1.0f - diff / 2.6f);

//             if (lit > 0.02f)
//             {
//                 const ImVec2 bp(c.x + std::cos(ba) * br, c.y + std::sin(ba) * br);

//                 bg->AddCircleFilled(bp, 2.5f + 2.0f * lit, Col(acc, 0.75f * lit), 16);
//                 bg->AddCircle(bp, 6.0f + 12.0f * (1.0f - lit), Col(acc, 0.35f * lit), 24, 1.2f);
//             }
//         }
//     }

//     // Expanding radar rings
//     for (int k = 0; k < 3; ++k)
//     {
//         const float phase  = std::fmod(t * 0.30f + k / 3.0f, 1.0f);
//         const float radius = 66.0f + phase * 190.0f;

//         bg->AddCircle(c, radius,
//             Col(acc, 0.28f * (1.0f - phase)), 96, 1.6f);
//     }

//     // Logo badge + rotating arcs
//     bg->AddCircleFilled(c, 64.0f, Col(acc, 0.12f), 64);
//     bg->AddCircle(c, 64.0f, Col(acc, 0.55f), 64, 1.5f);

//     bg->PathClear();
//     bg->PathArcTo(c, 72.0f, t * 1.6f, t * 1.6f + 1.9f, 32);
//     bg->PathStroke(ImGui::GetColorU32(acc), 0, 3.0f);

//     bg->PathClear();
//     bg->PathArcTo(c, 72.0f, t * 1.6f + HIDS_PI, t * 1.6f + HIDS_PI + 0.8f, 20);
//     bg->PathStroke(Col(acc, 0.45f), 0, 3.0f);

//     DrawShieldLogo(bg, c, 30.0f, acc);

//     // Text block
//     ImGui::SetCursorPosY(c.y + 100.0f);

//     CenteredTitle("HIDS", 3.0f, COLOR_TEXT);
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
//             Col(COLOR_GREEN, 0.14f), pillH * 0.5f);
//         d->AddRect(p, ImVec2(p.x + pillW, p.y + pillH),
//             Col(COLOR_GREEN, 0.70f), pillH * 0.5f, 0, 1.4f);
//         d->AddCircleFilled(ImVec2(p.x + 20.0f, p.y + pillH * 0.5f),
//             9.0f + wave * 2.0f,
//             Col(COLOR_GREEN, 0.20f * (1.0f - wave) + 0.05f));
//         d->AddCircleFilled(ImVec2(p.x + 20.0f, p.y + pillH * 0.5f), 5.0f,
//             ImGui::GetColorU32(COLOR_GREEN));
//         d->AddText(ImVec2(p.x + 36.0f, p.y + (pillH - ImGui::GetTextLineHeight()) * 0.5f),
//             ImGui::GetColorU32(COLOR_GREEN), label);

//         ImGui::Dummy(ImVec2(pillW, pillH));
//         ImGui::Dummy(ImVec2(0.0f, 6.0f));

//         // Live stat tiles
//         {
//             const float gap   = 12.0f;
//             const int   count = 5;
//             const float tileW = std::min(150.0f,
//                 (ImGui::GetWindowWidth() - 60.0f - gap * (count - 1)) / (float)count);
//             const float rowW  = tileW * count + gap * (count - 1);

//             char cpu[16], ram[16], proc[16], alr[16];
//             std::snprintf(cpu,  sizeof(cpu),  "%.0f%%", cpuNow);
//             std::snprintf(ram,  sizeof(ram),  "%.0f%%", ramNow);
//             std::snprintf(proc, sizeof(proc), "%d", processCount);
//             std::snprintf(alr,  sizeof(alr),  "%d", (int)alerts.size());

//             const std::string up = FormatDuration(now - engineStartedAt);

//             ImGui::SetCursorPosX(std::max(0.0f, (ImGui::GetWindowWidth() - rowW) * 0.5f));

//             ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(gap, gap));

//             DrawMiniStat("CPU", cpu, LoadColor(cpuNow), tileW);
//             ImGui::SameLine();
//             DrawMiniStat("RAM", ram, LoadColor(ramNow), tileW);
//             ImGui::SameLine();
//             DrawMiniStat("PROCESSES", proc, COLOR_BLUE, tileW);
//             ImGui::SameLine();
//             DrawMiniStat("ALERTS", alr,
//                 CountSeverity("Critical") > 0 ? COLOR_RED : COLOR_YELLOW, tileW);
//             ImGui::SameLine();
//             DrawMiniStat("UPTIME", up.c_str(), COLOR_GREEN, tileW);

//             ImGui::PopStyleVar();
//         }
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
//             Col(COLOR_GREEN, 0.40f * (1.0f - w) + 0.05f),
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
//         {
//             ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

//             const ImVec2 a = ImGui::GetItemRectMin();
//             const ImVec2 b = ImGui::GetItemRectMax();

//             ImGui::GetWindowDrawList()->AddLine(
//                 ImVec2(a.x, b.y), ImVec2(b.x, b.y), Col(COLOR_MUTED, 0.8f), 1.0f);
//         }

//         if (ImGui::IsItemClicked())
//             openConsole = true;
//     }

//     if (openConsole)
//         appScreen = SCREEN_HUB;

//     // Footer: the five areas + version
//     {
//         const float y = ImGui::GetWindowHeight() - 100.0f;

//         if (ImGui::GetCursorPosY() < y)
//             ImGui::SetCursorPosY(y);

//         DrawDomainChips();
//         CenteredText("HIDS Prototype v2.0", 0.9f, WithAlpha(COLOR_MUTED, 0.6f));
//     }
// }

// // ------------------------------------------------------------
// // Shared card used by the hub and the domain screens.
// // ------------------------------------------------------------
// static bool HubCard(
//     const char* id, const char* title, const char* desc, const char* footer,
//     int iconPage, const ImVec4& accent, const ImVec2& size,
//     const char* badge = nullptr, ImVec4 badgeCol = COLOR_CYAN)
// {
//     bool clicked = false;

//     ImGui::PushID(id);

//     const ImVec2 cp = ImGui::GetCursorScreenPos();
//     DrawSoftShadow(ImGui::GetWindowDrawList(), cp, ImVec2(cp.x + size.x, cp.y + size.y),
//         ImGui::GetStyle().ChildRounding, 14.0f, 0.38f);

//     ImGui::BeginChild("card", size, true, ImGuiWindowFlags_NoScrollbar);

//     const ImVec2 wp = ImGui::GetWindowPos();
//     const ImVec2 ws = ImGui::GetWindowSize();
//     ImDrawList*  d  = ImGui::GetWindowDrawList();
//     const bool   isHov = ImGui::IsWindowHovered();
//     const float  hov   = AnimTo(ImGui::GetID("##hv"), isHov ? 1.0f : 0.0f, 10.0f);
//     const float  rounding = ImGui::GetStyle().ChildRounding;
//     const float  t = (float)ImGui::GetTime();

//     // Soft tint fading down from the top
//     d->AddRectFilledMultiColor(
//         ImVec2(wp.x + 1.0f, wp.y + 4.0f),
//         ImVec2(wp.x + ws.x - 1.0f, wp.y + ws.y * 0.62f),
//         Col(accent, 0.11f + 0.10f * hov),
//         Col(accent, 0.11f + 0.10f * hov),
//         Col(accent, 0.0f),
//         Col(accent, 0.0f));

//     // Decorative rings in the top-right corner (breathe on hover)
//     for (int k = 0; k < 3; ++k)
//         d->AddCircle(ImVec2(wp.x + ws.x - 26.0f, wp.y + 30.0f),
//             34.0f + k * 22.0f + hov * 6.0f * (float)(k + 1),
//             Col(accent, 0.10f - k * 0.025f + 0.04f * hov), 64, 1.2f);

//     DrawCardChrome(d, wp, ws, accent, hov);

//     // Accent bar across the top
//     d->AddRectFilled(wp, ImVec2(wp.x + ws.x, wp.y + 4.0f),
//         ImGui::GetColorU32(accent), rounding, ImDrawFlags_RoundCornersTop);

//     // Icon badge
//     const ImVec2 ic(wp.x + 22.0f + 32.0f, wp.y + 26.0f + 32.0f);

//     DrawGlow(d, ic, 62.0f + 22.0f * hov, accent);
//     d->AddCircleFilled(ic, 32.0f, Col(accent, 0.16f + 0.16f * hov), 48);
//     d->AddCircle(ic, 32.0f, Col(accent, 0.75f), 48, 1.6f);

//     // orbiting spark on the icon ring
//     {
//         const float a = t * (0.8f + 1.4f * hov) + (float)(iconPage * 1.3f);
//         d->AddCircleFilled(ImVec2(ic.x + std::cos(a) * 32.0f, ic.y + std::sin(a) * 32.0f),
//             2.8f, Col(accent, 0.55f + 0.4f * hov), 12);
//     }

//     DrawPageIcon(d, ic, 15.0f, iconPage, ImGui::GetColorU32(accent));

//     // Live badge (top-right)
//     if (badge != nullptr)
//     {
//         const ImVec2 bs = ImGui::CalcTextSize(badge);
//         const float  bw = bs.x + 26.0f;
//         const float  bh = 22.0f;
//         const ImVec2 bp(wp.x + ws.x - bw - 14.0f, wp.y + 14.0f);

//         d->AddRectFilled(bp, ImVec2(bp.x + bw, bp.y + bh), Col(badgeCol, 0.20f), bh * 0.5f);
//         d->AddRect(bp, ImVec2(bp.x + bw, bp.y + bh), Col(badgeCol, 0.70f), bh * 0.5f);
//         d->AddCircleFilled(ImVec2(bp.x + 11.0f, bp.y + bh * 0.5f), 3.2f,
//             ImGui::GetColorU32(badgeCol), 12);
//         d->AddText(ImVec2(bp.x + 20.0f, bp.y + (bh - bs.y) * 0.5f),
//             ImGui::GetColorU32(badgeCol), badge);
//     }

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

//         d->AddCircleFilled(ac, 13.0f + hov * 1.5f, Col(accent, 0.18f + 0.24f * hov), 32);
//         d->AddCircle(ac, 13.0f + hov * 1.5f, Col(accent, 0.70f), 32, 1.2f);

//         const float nx = hov * 2.0f;
//         ImVec2 arrow[3] = {
//             ImVec2(ac.x - 2.5f + nx, ac.y - 5.0f),
//             ImVec2(ac.x + 3.0f + nx, ac.y),
//             ImVec2(ac.x - 2.5f + nx, ac.y + 5.0f)
//         };
//         d->AddPolyline(arrow, 3, ImGui::GetColorU32(accent), 0, 2.0f);
//     }

//     // Click + shine sweep
//     if (isHov)
//     {
//         ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

//         const float sx = wp.x + (ws.x + 80.0f) * std::fmod(t * 0.8f, 1.0f) - 40.0f;

//         d->PushClipRect(Off(wp, 1.0f, 1.0f), ImVec2(wp.x + ws.x - 1.0f, wp.y + ws.y - 1.0f), true);
//         d->AddRectFilledMultiColor(
//             ImVec2(sx - 40.0f, wp.y), ImVec2(sx + 40.0f, wp.y + ws.y),
//             Col(FX_WHITE, 0.0f), Col(FX_WHITE, 0.06f), Col(FX_WHITE, 0.06f), Col(FX_WHITE, 0.0f));
//         d->PopClipRect();

//         if (ImGui::IsMouseClicked(0))
//             clicked = true;
//     }

//     ImGui::EndChild();
//     ImGui::PopID();

//     return clicked;
// }

// // "HIDS IS WORKING / STOPPED" pill (hub)
// static void DrawStatusPill()
// {
//     const ImVec4 col   = monitoringActive ? COLOR_GREEN : COLOR_YELLOW;
//     const char*  label = monitoringActive ? "HIDS IS WORKING" : "HIDS IS STOPPED";
//     const float  pillH = 30.0f;
//     const float  pillW = ImGui::CalcTextSize(label).x + 56.0f;

//     ImGui::SetCursorPosX(std::max(0.0f, (ImGui::GetWindowWidth() - pillW) * 0.5f));

//     const ImVec2 p    = ImGui::GetCursorScreenPos();
//     ImDrawList*  d    = ImGui::GetWindowDrawList();
//     const float  wave = 0.5f + 0.5f * std::sin((float)ImGui::GetTime() * 3.0f);
//     const ImVec2 dot(p.x + 18.0f, p.y + pillH * 0.5f);

//     d->AddRectFilled(p, ImVec2(p.x + pillW, p.y + pillH),
//         Col(col, 0.14f), pillH * 0.5f);
//     d->AddRect(p, ImVec2(p.x + pillW, p.y + pillH),
//         Col(col, 0.70f), pillH * 0.5f, 0, 1.4f);

//     if (monitoringActive)
//         d->AddCircleFilled(dot, 8.0f + wave * 3.0f,
//             Col(col, 0.20f * (1.0f - wave) + 0.05f));

//     d->AddCircleFilled(dot, 5.0f, ImGui::GetColorU32(col));
//     d->AddText(ImVec2(p.x + 34.0f, p.y + (pillH - ImGui::GetTextLineHeight()) * 0.5f),
//         ImGui::GetColorU32(col), label);

//     ImGui::Dummy(ImVec2(pillW, pillH));
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

//     ImGui::SameLine(0.0f, 28.0f);
//     ImGui::SetCursorPosY(topY + 4.0f);

//     if (ImGui::Button("<  Welcome screen", ImVec2(180.0f, 34.0f)))
//         appScreen = SCREEN_WELCOME;

//     DrawHeaderStatus(topY + 4.0f);

//     ImGui::SetCursorPosY(topY + 64.0f);
//     ImGui::Separator();
//     ImGui::Dummy(ImVec2(0.0f, 12.0f));

//     CenteredTitle("Security Console", 2.0f, COLOR_TEXT);
//     CenteredText("Choose an area to explore", 1.1f, COLOR_MUTED);
//     ImGui::Dummy(ImVec2(0.0f, 6.0f));
//     DrawStatusPill();
//     ImGui::Dummy(ImVec2(0.0f, 14.0f));

//     // Card grid
//     const int   count = IM_ARRAYSIZE(hidsDomains);
//     const float gap   = 16.0f;
//     const float avail = ImGui::GetContentRegionAvail().x;
//     const int   cols  = (avail >= 1100.0f) ? 5 : 3;
//     const float cardW = (avail - gap * (cols - 1)) / cols;
//     const float cardH = (cols == 5) ? 280.0f : 215.0f;

//     ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(gap, gap));

//     for (int i = 0; i < count; ++i)
//     {
//         if (i % cols != 0)
//             ImGui::SameLine();

//         const HidsDomainRoute& dom = hidsDomains[i];

//         char footer[48];
//         std::snprintf(footer, sizeof(footer), "%d feature%s",
//             dom.featureCount, dom.featureCount == 1 ? "" : "s");

//         // Live badge: engine state on Monitoring, alert count on Threat Detection
//         char        badgeText[32] = "";
//         const char* badge         = nullptr;
//         ImVec4      badgeCol      = COLOR_CYAN;

//         if (i == 0)
//         {
//             std::snprintf(badgeText, sizeof(badgeText), "%s", monitoringActive ? "Live" : "Idle");
//             badge    = badgeText;
//             badgeCol = monitoringActive ? COLOR_GREEN : COLOR_YELLOW;
//         }
//         else if (i == 1 && !alerts.empty())
//         {
//             std::snprintf(badgeText, sizeof(badgeText), "%d alert%s",
//                 (int)alerts.size(), alerts.size() == 1 ? "" : "s");
//             badge    = badgeText;
//             badgeCol = CountSeverity("Critical") > 0 ? COLOR_RED : COLOR_ORANGE;
//         }

//         if (HubCard(dom.title, dom.title, dom.description, footer,
//                 domainIconPage[i], DomainAccent(i), ImVec2(cardW, cardH),
//                 badge, badgeCol))
//         {
//             selectedDomain = i;
//             appScreen      = SCREEN_DOMAIN;
//         }
//     }

//     ImGui::PopStyleVar();

//     ImGui::Dummy(ImVec2(0.0f, 4.0f));

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

//     FooterHint("Click an area to open it   -   Esc goes back to the welcome screen");
// }

// // ------------------------------------------------------------
// // DOMAIN SCREEN - the features inside one domain
// // ------------------------------------------------------------
// static void DrawDomainScreen()
// {
//     RefreshFromBackend();

//     const HidsDomainRoute& dom = hidsDomains[selectedDomain];
//     const ImVec4 accent = DomainAccent(selectedDomain);
//     const float  t      = (float)ImGui::GetTime();

//     const float topY = ImGui::GetCursorPosY();
//     const float lh   = ImGui::GetTextLineHeight();

//     if (ImGui::Button("<  Home", ImVec2(100.0f, 34.0f)))
//         appScreen = SCREEN_HUB;

//     ImGui::SameLine(0.0f, 18.0f);
//     ImGui::SetCursorPosY(topY + (34.0f - lh) * 0.5f);

//     if (CrumbLink("Welcome", COLOR_MUTED))
//         appScreen = SCREEN_WELCOME;

//     ImGui::SameLine(0.0f, 8.0f);
//     ImGui::TextDisabled(">");
//     ImGui::SameLine(0.0f, 8.0f);

//     if (CrumbLink("Home", COLOR_MUTED))
//         appScreen = SCREEN_HUB;

//     ImGui::SameLine(0.0f, 8.0f);
//     ImGui::TextDisabled(">");
//     ImGui::SameLine(0.0f, 8.0f);
//     ImGui::TextColored(accent, "%s", dom.title);

//     DrawHeaderStatus(topY + 1.0f);

//     ImGui::SetCursorPosY(topY + 48.0f);
//     ImGui::Separator();
//     ImGui::Dummy(ImVec2(0.0f, 16.0f));

//     // Domain banner
//     {
//         const ImVec2 p = ImGui::GetCursorScreenPos();
//         const ImVec2 ic(p.x + 36.0f, p.y + 36.0f);
//         ImDrawList*  d = ImGui::GetWindowDrawList();

//         DrawGlow(d, ic, 76.0f, accent);
//         d->AddCircleFilled(ic, 34.0f, Col(accent, 0.18f), 48);
//         d->AddCircle(ic, 34.0f, Col(accent, 0.75f), 48, 1.6f);

//         d->PathClear();
//         d->PathArcTo(ic, 41.0f, t * 1.3f, t * 1.3f + 1.5f, 20);
//         d->PathStroke(Col(accent, 0.85f), 0, 2.4f);

//         d->PathClear();
//         d->PathArcTo(ic, 41.0f, t * 1.3f + HIDS_PI, t * 1.3f + HIDS_PI + 0.8f, 12);
//         d->PathStroke(Col(accent, 0.40f), 0, 2.4f);

//         DrawPageIcon(d, ic, 16.0f, domainIconPage[selectedDomain], ImGui::GetColorU32(accent));

//         ImGui::Dummy(ImVec2(72.0f, 72.0f));
//         ImGui::SameLine(0.0f, 20.0f);

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
//     BeginPanel("ThreatPanel", ImVec2(0.0f, 222.0f), COLOR_CYAN, true);

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

//     ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 8.0f));

//     for (int i = 0; i < 4; ++i)
//     {
//         const int    count = CountSeverity(names[i]);
//         const ImVec4 sc    = SeverityColor(names[i]);

//         ImGui::PushID(i);

//         const float  w = ImGui::GetContentRegionAvail().x;
//         const float  h = 24.0f;
//         const ImVec2 p = ImGui::GetCursorScreenPos();

//         const bool clicked = ImGui::InvisibleButton("##sev", ImVec2(w, h));
//         const bool hovered = ImGui::IsItemHovered();

//         const float hv    = AnimTo(ImGui::GetID("##hv"), hovered ? 1.0f : 0.0f);
//         const float shown = AnimTo(ImGui::GetID("##fill"),
//             (float)count / (float)total, 6.0f, 0.0f);

//         ImDrawList* d = ImGui::GetWindowDrawList();

//         d->AddRectFilled(p, ImVec2(p.x + w, p.y + h),
//             ImGui::GetColorU32(COLOR_FRAME), h * 0.5f);

//         if (shown > 0.004f)
//         {
//             const float fw = std::max(h, w * shown);

//             d->AddRectFilled(p, ImVec2(p.x + fw, p.y + h),
//                 Col(sc, 0.55f + 0.30f * hv), h * 0.5f);
//             d->AddRectFilled(Off(p, 4.0f, 2.0f), ImVec2(p.x + fw - 4.0f, p.y + h * 0.45f),
//                 Col(FX_WHITE, 0.16f), h * 0.25f);
//         }

//         d->AddRect(p, ImVec2(p.x + w, p.y + h), Col(sc, 0.30f + 0.5f * hv), h * 0.5f);

//         const float ty = p.y + (h - ImGui::GetTextLineHeight()) * 0.5f;
//         char num[16];
//         std::snprintf(num, sizeof(num), "%d", count);

//         d->AddText(ImVec2(p.x + 12.0f, ty), ImGui::GetColorU32(COLOR_TEXT), names[i]);
//         d->AddText(ImVec2(p.x + w - 12.0f - ImGui::CalcTextSize(num).x, ty),
//             ImGui::GetColorU32(COLOR_TEXT), num);

//         if (hovered)
//         {
//             ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
//             ImGui::SetTooltip("Show %s alerts", names[i]);
//         }

//         if (clicked)
//         {
//             severityFilter = i + 1;
//             OpenPage(4);               // Alerts
//         }

//         ImGui::PopID();
//     }

//     ImGui::PopStyleVar();

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

// // Esc: page -> domain -> hub -> welcome
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
//     else if (appScreen == SCREEN_HUB)
//     {
//         appScreen = SCREEN_WELCOME;
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
//     BeginPanel("LockScreen", ImVec2(0.0f, 0.0f), COLOR_GREEN);

//     ImGui::Dummy(ImVec2(0.0f, 8.0f));

//     // Padlock graphic on the left, form on the right
//     {
//         const ImVec2 p = ImGui::GetCursorScreenPos();
//         const ImVec2 c(p.x + 48.0f, p.y + 46.0f);
//         ImDrawList*  d = ImGui::GetWindowDrawList();
//         const float  t = (float)ImGui::GetTime();
//         const float  ph = std::fmod(t * 0.5f, 1.0f);

//         DrawGlow(d, c, 86.0f, COLOR_GREEN);
//         d->AddCircleFilled(c, 44.0f, Col(COLOR_GREEN, 0.12f), 48);
//         d->AddCircle(c, 44.0f, Col(COLOR_GREEN, 0.60f), 48, 1.6f);
//         d->AddCircle(c, 44.0f + ph * 18.0f, Col(COLOR_GREEN, 0.35f * (1.0f - ph)), 48, 1.3f);

//         d->PathClear();
//         d->PathArcTo(c, 51.0f, t * 1.1f, t * 1.1f + 1.4f, 20);
//         d->PathStroke(Col(COLOR_GREEN, 0.8f), 0, 2.2f);

//         DrawLockIcon(d, ImVec2(c.x, c.y - 4.0f), 18.0f, COLOR_GREEN);

//         ImGui::Dummy(ImVec2(104.0f, 96.0f));
//         ImGui::SameLine(0.0f, 18.0f);
//     }

//     ImGui::BeginGroup();

//     SectionHeading("Logs & Reports are locked",
//         "enter the password to see the detection signatures");

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

//     ImGui::EndGroup();

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
//         // Red warning strip across the top of the dialog
//         {
//             const ImVec2 wp = ImGui::GetWindowPos();
//             const ImVec2 ws = ImGui::GetWindowSize();

//             ImDrawList* d = ImGui::GetWindowDrawList();

//             d->AddRectFilledMultiColor(
//                 ImVec2(wp.x + 1.0f, wp.y + 1.0f), ImVec2(wp.x + ws.x - 1.0f, wp.y + 90.0f),
//                 Col(COLOR_RED, 0.16f), Col(COLOR_RED, 0.16f), Col(COLOR_RED, 0.0f), Col(COLOR_RED, 0.0f));
//             d->AddRectFilled(ImVec2(wp.x + 14.0f, wp.y), ImVec2(wp.x + ws.x - 14.0f, wp.y + 3.0f),
//                 ImGui::GetColorU32(COLOR_RED), 2.0f);
//         }

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
//     BeginPanel("AlertsPanel", ImVec2(0.0f, compact ? 300.0f : 0.0f), COLOR_ORANGE);

//     SectionHeading(
//         compact ? "Recent security alerts" : "All security alerts"
//     );

//     // Severity filter (with counts) + search (full page only)
//     if (!compact)
//     {
//         for (int i = 0; i < IM_ARRAYSIZE(severityFilterLabels); ++i)
//         {
//             const bool active = (severityFilter == i);
//             const int  count  = (i == 0) ? (int)alerts.size()
//                                          : CountSeverity(severityFilterLabels[i]);
//             const ImVec4 tone = (i == 0) ? COLOR_BLUE
//                                          : SeverityColor(severityFilterLabels[i]);

//             char text[48];
//             std::snprintf(text, sizeof(text), "%s  %d", severityFilterLabels[i], count);

//             ImGui::PushStyleColor(ImGuiCol_Button,
//                 active ? WithAlpha(tone, 0.60f) : COLOR_FRAME);
//             ImGui::PushStyleColor(ImGuiCol_ButtonHovered, WithAlpha(tone, 0.45f));
//             ImGui::PushStyleColor(ImGuiCol_ButtonActive,  WithAlpha(tone, 0.75f));

//             if (ImGui::Button(text, ImVec2(112.0f, 0.0f)))
//                 severityFilter = i;

//             ImGui::PopStyleColor(3);

//             // underline glow under the active filter
//             if (active)
//             {
//                 const ImVec2 a = ImGui::GetItemRectMin();
//                 const ImVec2 b = ImGui::GetItemRectMax();

//                 ImGui::GetWindowDrawList()->AddRectFilled(
//                     ImVec2(a.x + 10.0f, b.y + 1.0f), ImVec2(b.x - 10.0f, b.y + 3.0f),
//                     ImGui::GetColorU32(tone), 1.0f);
//             }

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

//     // Rows that pass the filters
//     std::vector<const AlertInfo*> rows;

//     for (const AlertInfo& alert : alerts)
//     {
//         if (!compact && severityFilter != 0 &&
//             alert.severity != severityFilterLabels[severityFilter])
//             continue;

//         if (!compact &&
//             !ContainsNoCase(alert.description.c_str(), alertSearch) &&
//             !ContainsNoCase(alert.source.c_str(), alertSearch))
//             continue;

//         rows.push_back(&alert);
//     }

//     if (rows.empty())
//     {
//         if (alerts.empty())
//             EmptyState("All clear", "No security alerts have been raised.", COLOR_GREEN);
//         else
//             EmptyState("No matching alerts",
//                 "Try a different severity filter or search term.", COLOR_CYAN);

//         ImGui::EndChild();
//         return;
//     }

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
//         ImGui::TableSetupColumn("Severity", ImGuiTableColumnFlags_WidthFixed, 140.0f);
//         ImGui::TableSetupColumn("Source", ImGuiTableColumnFlags_WidthFixed, 160.0f);
//         ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthStretch);
//         ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 100.0f);
//         ImGui::TableHeadersRow();

//         for (const AlertInfo* ap : rows)
//         {
//             const AlertInfo& alert = *ap;
//             const ImVec4 sc = SeverityColor(alert.severity.c_str());

//             ImGui::TableNextRow();

//             // Critical / High rows get a faint severity wash
//             if (alert.severity == "Critical")
//                 ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, Col(sc, 0.11f));
//             else if (alert.severity == "High")
//                 ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, Col(sc, 0.06f));

//             ImGui::TableSetColumnIndex(0);
//             {
//                 const ImVec2 p = ImGui::GetCursorScreenPos();
//                 const float  cy = p.y + ImGui::GetTextLineHeight() * 0.5f;
//                 ImDrawList*  d  = ImGui::GetWindowDrawList();

//                 // coloured stripe on the left edge of the row
//                 d->AddRectFilled(
//                     ImVec2(p.x - ImGui::GetStyle().CellPadding.x, p.y - ImGui::GetStyle().CellPadding.y),
//                     ImVec2(p.x - ImGui::GetStyle().CellPadding.x + 3.0f,
//                            p.y + ImGui::GetTextLineHeight() + ImGui::GetStyle().CellPadding.y),
//                     ImGui::GetColorU32(sc));

//                 d->AddCircleFilled(ImVec2(p.x + 4.0f, cy), 6.5f, Col(sc, 0.18f), 20);
//                 d->AddCircleFilled(ImVec2(p.x + 4.0f, cy), 3.8f, ImGui::GetColorU32(sc), 16);

//                 ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18.0f);
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

//         ImGui::EndTable();
//     }

//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // PROCESS TABLE
// // ------------------------------------------------------------

// static void DrawProcesses()
// {
//     BeginPanel("ProcessesPanel", ImVec2(0.0f, 0.0f), COLOR_CYAN);

//     SectionHeading("Running processes",
//         "apps can be ended here; system and background processes are protected");

//     // Summary chips
//     {
//         int apps = 0;
//         int high = 0;

//         for (const ProcessInfo& p : processes)
//         {
//             if (p.isApp)                   ++apps;
//             if (p.memory >= HIGH_MEMORY_MB) ++high;
//         }

//         char total[16], appText[16], highText[16];
//         std::snprintf(total, sizeof(total), "%d", (int)processes.size());
//         std::snprintf(appText, sizeof(appText), "%d", apps);
//         std::snprintf(highText, sizeof(highText), "%d", high);

//         StatChip("processes", total, COLOR_CYAN);
//         ImGui::SameLine();
//         StatChip("apps", appText, COLOR_BLUE);
//         ImGui::SameLine();
//         StatChip("high memory", highText, high > 0 ? COLOR_YELLOW : COLOR_GREEN);
//     }

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

//             // Name with a small coloured initial tile
//             ImGui::TableSetColumnIndex(1);
//             {
//                 static const ImVec4* palette[5] = {
//                     &COLOR_CYAN, &COLOR_BLUE, &COLOR_GREEN, &COLOR_ORANGE, &COLOR_YELLOW
//                 };

//                 unsigned hash = 0;
//                 for (char ch : process.name)
//                     hash = hash * 31u + (unsigned char)ch;

//                 const ImVec4 tone = process.isApp ? *palette[hash % 5u] : COLOR_MUTED;
//                 const float  s    = ImGui::GetTextLineHeight() + 2.0f;
//                 const ImVec2 p    = ImGui::GetCursorScreenPos();
//                 ImDrawList*  d    = ImGui::GetWindowDrawList();

//                 d->AddRectFilled(p, ImVec2(p.x + s, p.y + s), Col(tone, 0.18f), 6.0f);
//                 d->AddRect(p, ImVec2(p.x + s, p.y + s), Col(tone, 0.55f), 6.0f);

//                 char letter[2] = {
//                     process.name.empty() ? '?' : (char)std::toupper((unsigned char)process.name[0]),
//                     '\0'
//                 };
//                 const ImVec2 ts = ImGui::CalcTextSize(letter);

//                 d->AddText(ImVec2(p.x + (s - ts.x) * 0.5f, p.y + (s - ts.y) * 0.5f),
//                     ImGui::GetColorU32(tone), letter);

//                 ImGui::Dummy(ImVec2(s, s));
//                 ImGui::SameLine(0.0f, 8.0f);
//             }

//             ImGui::TextUnformatted(process.name.c_str());

//             if (process.isApp && !process.title.empty())
//             {
//                 ImGui::SameLine();
//                 ImGui::TextDisabled("- %s", process.title.c_str());
//             }

//             // CPU bar
//             ImGui::TableSetColumnIndex(2);

//             {
//                 char cpuLabel[16];
//                 std::snprintf(cpuLabel, sizeof(cpuLabel), "%.1f", process.cpu);

//                 DrawUsageBar(std::min(process.cpu / 25.0f, 1.0f), cpuLabel,
//                     LoadColor(process.cpu * 4.0f), 18.0f);
//             }

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

// static ImVec4 EventColor(const std::string& type)
// {
//     if (ContainsNoCase(type.c_str(), "FAIL"))    return COLOR_RED;
//     if (ContainsNoCase(type.c_str(), "DELETE"))  return COLOR_ORANGE;
//     if (ContainsNoCase(type.c_str(), "MODIFY"))  return COLOR_YELLOW;
//     if (ContainsNoCase(type.c_str(), "SUCCESS")) return COLOR_GREEN;

//     return COLOR_CYAN;
// }

// static void DrawEventPage(
//     const char* title,
//     const std::vector<EventInfo>& events,
//     const char* emptyText,
//     const char* hint)
// {
//     BeginPanel(title, ImVec2(0.0f, 0.0f), COLOR_CYAN);

//     SectionHeading(title, hint);

//     {
//         char count[16];
//         std::snprintf(count, sizeof(count), "%d", (int)events.size());

//         StatChip("events recorded", count,
//             events.empty() ? COLOR_GREEN : COLOR_YELLOW);
//         ImGui::Spacing();
//     }

//     if (events.empty())
//     {
//         EmptyState("Nothing to report", emptyText, COLOR_GREEN);
//         ImGui::EndChild();
//         return;
//     }

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
//         ImGui::TableSetupColumn("Event", ImGuiTableColumnFlags_WidthFixed, 210.0f);
//         ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthStretch);
//         ImGui::TableHeadersRow();

//         for (const EventInfo& event : events)
//         {
//             const ImVec4 ec = EventColor(event.type);

//             ImGui::TableNextRow();

//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextDisabled("%s", event.time.c_str());

//             // Event type as a coloured chip
//             ImGui::TableSetColumnIndex(1);
//             {
//                 const ImVec2 ts = ImGui::CalcTextSize(event.type.c_str());
//                 const ImVec2 p  = ImGui::GetCursorScreenPos();
//                 const float  w  = ts.x + 30.0f;
//                 const float  h  = ts.y + 4.0f;
//                 ImDrawList*  d  = ImGui::GetWindowDrawList();

//                 d->AddRectFilled(p, ImVec2(p.x + w, p.y + h), Col(ec, 0.14f), h * 0.5f);
//                 d->AddRect(p, ImVec2(p.x + w, p.y + h), Col(ec, 0.50f), h * 0.5f);
//                 d->AddCircleFilled(ImVec2(p.x + 12.0f, p.y + h * 0.5f), 3.4f,
//                     ImGui::GetColorU32(ec), 12);
//                 d->AddText(ImVec2(p.x + 22.0f, p.y + 2.0f), ImGui::GetColorU32(ec),
//                     event.type.c_str());

//                 ImGui::Dummy(ImVec2(w, h));
//             }

//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextWrapped("%s", event.description.c_str());
//         }

//         ImGui::EndTable();
//     }

//     ImGui::EndChild();
// }

// static void DrawSystemAnalysis()
// {
//     BeginPanel("AnalysisPanel", ImVec2(0.0f, 0.0f), COLOR_BLUE);

//     if (!analysisData.valid)
//     {
//         ImGui::TextDisabled("Collecting the first samples...");

//         // little loading dots
//         {
//             const ImVec2 p = ImGui::GetCursorScreenPos();
//             const float  t = (float)ImGui::GetTime();

//             for (int i = 0; i < 3; ++i)
//             {
//                 const float k = 0.5f + 0.5f * std::sin(t * 5.0f - (float)i * 0.9f);

//                 ImGui::GetWindowDrawList()->AddCircleFilled(
//                     ImVec2(p.x + 8.0f + (float)i * 18.0f, p.y + 12.0f),
//                     3.0f + 3.0f * k, Col(COLOR_BLUE, 0.4f + 0.5f * k), 16);
//             }

//             ImGui::Dummy(ImVec2(70.0f, 24.0f));
//         }

//         ImGui::EndChild();
//         return;
//     }

//     const BackendAnalysis& a = analysisData;

//     // ---- Resource usage ----
//     SectionHeading("Resource usage");

//     char cpuText[16];
//     char ramText[16];
//     char procText[16];
//     char ramSub[96];
//     char procSub[96];

//     if (a.cpuAvailable)
//         std::snprintf(cpuText, sizeof(cpuText), "%.1f%%", a.cpuPercent);
//     else
//         std::snprintf(cpuText, sizeof(cpuText), "n/a");

//     if (a.memoryAvailable)
//         std::snprintf(ramText, sizeof(ramText), "%.1f%%", a.memoryPercent);
//     else
//         std::snprintf(ramText, sizeof(ramText), "n/a");

//     std::snprintf(ramSub, sizeof(ramSub), "%llu MB used of %llu MB",
//         (unsigned long long)a.usedMemoryMB,
//         (unsigned long long)a.totalMemoryMB);

//     std::snprintf(procText, sizeof(procText), "%llu",
//         (unsigned long long)a.processCount);
//     std::snprintf(procSub, sizeof(procSub), "%llu high CPU, %llu high memory",
//         (unsigned long long)a.highCpuProcessCount,
//         (unsigned long long)a.highMemoryProcessCount);

//     {
//         const float sp = ImGui::GetStyle().ItemSpacing.x;
//         const float w  = (ImGui::GetContentRegionAvail().x - sp * 2.0f) / 3.0f;

//         MetricCard("CPU load", cpuText, "Current processor usage",
//             LoadColor((float)a.cpuPercent), w, nullptr, -1,
//             a.cpuAvailable ? (float)a.cpuPercent : 0.0f);
//         ImGui::SameLine();

//         MetricCard("Memory in use", ramText, ramSub,
//             LoadColor((float)a.memoryPercent), w, nullptr, -1,
//             a.memoryAvailable ? (float)a.memoryPercent : 0.0f);
//         ImGui::SameLine();

//         MetricCard("Processes", procText, procSub, COLOR_BLUE, w, nullptr, 1);
//     }

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

//     if (a.findings.empty())
//     {
//         ImGui::TextColored(COLOR_GREEN, "No resource concerns detected.");
//     }
//     else
//     {
//         for (const std::string& finding : a.findings)
//         {
//             const ImVec2 p0 = ImGui::GetCursorScreenPos();
//             const float  w  = ImGui::GetContentRegionAvail().x;

//             ImGui::PushStyleColor(ImGuiCol_Text, COLOR_YELLOW);
//             ImGui::Bullet();
//             ImGui::SameLine();
//             ImGui::TextWrapped("%s", finding.c_str());
//             ImGui::PopStyleColor();

//             // faint highlight card + stripe behind each finding
//             const ImVec2 p1 = ImGui::GetCursorScreenPos();

//             ImDrawList* d = ImGui::GetWindowDrawList();
//             d->AddRectFilled(Off(p0, -6.0f, -2.0f),
//                 ImVec2(p0.x + w, p1.y - ImGui::GetStyle().ItemSpacing.y + 2.0f),
//                 Col(COLOR_YELLOW, 0.05f), 8.0f);
//             d->AddRectFilled(Off(p0, -6.0f, -2.0f),
//                 ImVec2(p0.x - 3.0f, p1.y - ImGui::GetStyle().ItemSpacing.y + 2.0f),
//                 Col(COLOR_YELLOW, 0.85f), 2.0f);
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
//         ImGui::TableSetupColumn("CPU %", ImGuiTableColumnFlags_WidthFixed, 150.0f);
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
//             {
//                 char label[16];
//                 std::snprintf(label, sizeof(label), "%.1f", p.cpuPercent);

//                 DrawUsageBar(std::min(p.cpuPercent / 25.0f, 1.0f), label,
//                     LoadColor(p.cpuPercent * 4.0f), 16.0f);
//             }

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

// // Radar widget for the scan state. Reserves a (2*radius) square at the cursor.
// //   active -> sweeping beam, done -> tick, otherwise idle crosshair.
// static void DrawRadar(float radius, float thickness, const ImVec4& color,
//                       bool active, bool done)
// {
//     const ImVec2 pos = ImGui::GetCursorScreenPos();
//     const ImVec2 c(pos.x + radius, pos.y + radius);
//     ImDrawList*  d = ImGui::GetWindowDrawList();
//     const float  t = (float)ImGui::GetTime();

//     d->AddCircleFilled(c, radius, Col(color, 0.08f), 48);

//     for (int k = 1; k <= 3; ++k)
//         d->AddCircle(c, radius * (float)k / 3.0f,
//             Col(color, k == 3 ? 0.75f : 0.22f), 48, k == 3 ? thickness * 0.6f : 1.0f);

//     d->AddLine(Off(c, -radius, 0.0f), Off(c, radius, 0.0f), Col(color, 0.18f), 1.0f);
//     d->AddLine(Off(c, 0.0f, -radius), Off(c, 0.0f, radius), Col(color, 0.18f), 1.0f);

//     if (active)
//     {
//         const float sweep = t * 4.0f;

//         for (int k = 0; k < 14; ++k)
//         {
//             const float a0 = sweep - (float)k * 0.10f;
//             const float a1 = a0 - 0.10f;
//             const float f  = 1.0f - (float)k / 14.0f;

//             d->AddTriangleFilled(c,
//                 ImVec2(c.x + std::cos(a0) * radius, c.y + std::sin(a0) * radius),
//                 ImVec2(c.x + std::cos(a1) * radius, c.y + std::sin(a1) * radius),
//                 Col(color, 0.45f * f * f));
//         }

//         d->AddLine(c, ImVec2(c.x + std::cos(sweep) * radius, c.y + std::sin(sweep) * radius),
//             ImGui::GetColorU32(color), 1.8f);

//         const float pulse = 0.75f + 0.25f * std::sin(t * 4.0f);
//         d->AddCircleFilled(c, radius * 0.14f * pulse, Col(color, 0.95f), 16);
//     }
//     else if (done)
//     {
//         const ImVec2 tick[3] = {
//             ImVec2(c.x - radius * 0.32f, c.y + radius * 0.02f),
//             ImVec2(c.x - radius * 0.08f, c.y + radius * 0.28f),
//             ImVec2(c.x + radius * 0.36f, c.y - radius * 0.26f)
//         };
//         d->AddPolyline(tick, 3, ImGui::GetColorU32(color), 0, thickness * 0.8f);
//     }
//     else
//     {
//         d->AddCircleFilled(c, radius * 0.12f, Col(color, 0.8f), 16);
//     }

//     ImGui::Dummy(ImVec2(radius * 2.0f, radius * 2.0f));
// }

// // Thin progress beam: sliding highlight while scanning, solid when finished
// static void DrawBeamBar(const ImVec4& color, bool running, bool finished)
// {
//     const float  w = std::max(40.0f, ImGui::GetContentRegionAvail().x);
//     const float  h = 6.0f;
//     const ImVec2 p = ImGui::GetCursorScreenPos();
//     ImDrawList*  d = ImGui::GetWindowDrawList();
//     const float  t = (float)ImGui::GetTime();

//     d->AddRectFilled(p, ImVec2(p.x + w, p.y + h), ImGui::GetColorU32(COLOR_FRAME), h * 0.5f);

//     if (running)
//     {
//         const float ph  = std::fmod(t * 0.55f, 1.0f);
//         const float len = w * 0.32f;
//         const float x0  = p.x - len + (w + len) * ph;

//         d->PushClipRect(p, ImVec2(p.x + w, p.y + h), true);
//         d->AddRectFilledMultiColor(ImVec2(x0, p.y), ImVec2(x0 + len, p.y + h),
//             Col(color, 0.0f), Col(color, 0.95f), Col(color, 0.95f), Col(color, 0.0f));
//         d->PopClipRect();
//     }
//     else if (finished)
//     {
//         d->AddRectFilled(p, ImVec2(p.x + w, p.y + h), Col(color, 0.75f), h * 0.5f);
//     }

//     ImGui::Dummy(ImVec2(w, h));
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
//     static char   findingSearch[128] = "";
//     const double now = ImGui::GetTime();

//     if (lastRefresh < 0.0 || now - lastRefresh >= 0.25)
//     {
//         lastRefresh  = now;
//         scanStatus   = backend.getScanStatus();
//         scanFindings = backend.getScanFindings();
//     }

//     const bool running = scanStatus.running;

//     BeginPanel("ScannerPanel", ImVec2(0.0f, 0.0f), COLOR_ORANGE);

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
//         PushGreenButton();

//         if (ImGui::Button("Start Scan", ImVec2(130.0f, 0.0f)))
//         {
//             if (scanTypeSel == 2 && scanPath[0] == '\0')
//                 scanMessage = "Enter a folder or file path first.";
//             else if (!backend.startScan((ScanType)scanTypeSel, scanPath))
//                 scanMessage = "A scan is already running.";
//             else
//                 scanMessage.clear();
//         }

//         ImGui::PopStyleColor(3);
//     }
//     else
//     {
//         PushRedButton();

//         if (ImGui::Button("Stop Scan", ImVec2(130.0f, 0.0f)))
//             backend.cancelScan();

//         ImGui::PopStyleColor(3);
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

//     // What the scanner reports and what it trusts
//     ImGui::PushTextWrapPos(0.0f);
//     ImGui::TextDisabled(
//         "Programs and scripts are checked against all detection rules; text files only "
//         "for ransom-note wording. Validly signed programs and anything listed in "
//         "hids_allowlist.txt are not reported unless a known-bad hash matches.");
//     ImGui::PopTextWrapPos();

//     ImGui::Separator();
//     ImGui::Spacing();

//     // Two panels side by side: live scan (left), threats (right)
//     const float gap    = ImGui::GetStyle().ItemSpacing.x;
//     const float leftW  = (ImGui::GetContentRegionAvail().x - gap) * 0.58f;
//     const float panelH = 290.0f;

//     ImGui::PushStyleColor(ImGuiCol_ChildBg, COLOR_SIDEBAR);

//     // ---------------- LEFT: live scan ----------------
//     ImVec4 stateColor = COLOR_MUTED;
//     const char* stateText = "Ready to scan";

//     if (running)
//     {
//         stateColor = COLOR_CYAN;
//         stateText  = "Scanning";
//     }
//     else if (scanStatus.finished && scanStatus.cancelled)
//     {
//         stateColor = COLOR_YELLOW;
//         stateText  = "Scan stopped";
//     }
//     else if (scanStatus.finished)
//     {
//         stateColor = (scanStatus.malicious + scanStatus.suspicious > 0)
//             ? COLOR_ORANGE : COLOR_GREEN;
//         stateText  = "Scan completed";
//     }

//     BeginPanel("ScanLive", ImVec2(leftW, panelH), stateColor);

//     {
//         DrawRadar(30.0f, 4.0f, stateColor, running,
//             scanStatus.finished && !scanStatus.cancelled);
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
//         DrawBeamBar(stateColor, running, scanStatus.finished && !scanStatus.cancelled);
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

//         // Live counters as chips (wrap instead of running out of the panel)
//         char files[24], skipped[24], data[24], timeText[24];
//         std::snprintf(files, sizeof(files), "%llu", scanStatus.filesScanned);
//         std::snprintf(skipped, sizeof(skipped), "%llu", scanStatus.filesSkipped);
//         std::snprintf(data, sizeof(data), "%s", FormatBytes(scanStatus.bytesScanned).c_str());
//         std::snprintf(timeText, sizeof(timeText), "%s",
//             FormatDuration(scanStatus.elapsedSeconds).c_str());

//         StatChipFlow("files",   files,    COLOR_CYAN,  true);
//         StatChipFlow("skipped", skipped,  COLOR_MUTED, false);
//         StatChipFlow("scanned", data,     COLOR_BLUE,  false);
//         StatChipFlow("elapsed", timeText, COLOR_GREEN, false);
//     }

//     ImGui::EndChild();

//     ImGui::SameLine();

//     // ---------------- RIGHT: threats found ----------------
//     BeginPanel("ScanThreats", ImVec2(0.0f, panelH),
//         (scanStatus.malicious > 0) ? COLOR_RED : COLOR_ORANGE);

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

//         // Newest first; only the latest few are drawn here so a big scan
//         // stays smooth. The table below lists everything.
//         const std::size_t maxCards = 40;
//         std::size_t       drawn    = 0;

//         for (std::size_t i = scanFindings.size(); i-- > 0 && drawn < maxCards; )
//         {
//             const ScanFinding& f = scanFindings[i];
//             const ImVec4 c = (f.level == "Malicious") ? COLOR_RED : COLOR_YELLOW;

//             std::string dir, name;
//             SplitPath(f.path, dir, name);

//             ++drawn;

//             ImGui::PushID((int)i);

//             const ImVec2 cardTop = ImGui::GetCursorScreenPos();

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

//             // severity stripe + faint wash for the finding
//             {
//                 const ImVec2 cardBot = ImGui::GetCursorScreenPos();
//                 const float  bottom  = cardBot.y - ImGui::GetStyle().ItemSpacing.y * 0.5f;
//                 ImDrawList*  dl      = ImGui::GetWindowDrawList();

//                 dl->AddRectFilled(Off(cardTop, -8.0f, -2.0f),
//                     ImVec2(cardTop.x - 5.0f, bottom), ImGui::GetColorU32(c), 2.0f);
//                 dl->AddRectFilled(Off(cardTop, -8.0f, -2.0f),
//                     ImVec2(cardTop.x + ImGui::GetContentRegionAvail().x, bottom),
//                     Col(c, 0.045f), 6.0f);
//             }

//             ImGui::Separator();
//             ImGui::PopID();
//         }

//         if (scanFindings.size() > maxCards)
//             ImGui::TextDisabled("+ %d more in the table below",
//                 (int)(scanFindings.size() - maxCards));
//     }

//     ImGui::EndChild();

//     ImGui::PopStyleColor();   // ChildBg

//     ImGui::Spacing();

//     // Full findings table
//     SectionHeading("All findings", "hover a name for score and SHA-256");

//     ImGui::SetNextItemWidth(300.0f);
//     ImGui::InputTextWithHint("##findingsearch",
//         "Filter by name, file, level or reason",
//         findingSearch, IM_ARRAYSIZE(findingSearch));

//     ImGui::Spacing();

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
//         ImGui::TableSetupColumn("Level",  ImGuiTableColumnFlags_WidthFixed, 110.0f);
//         ImGui::TableSetupColumn("Name",   ImGuiTableColumnFlags_WidthFixed, 190.0f);
//         ImGui::TableSetupColumn("File");
//         ImGui::TableSetupColumn("Reason");
//         ImGui::TableHeadersRow();

//         for (const ScanFinding& f : scanFindings)
//         {
//             if (!ContainsNoCase(f.name.c_str(),   findingSearch) &&
//                 !ContainsNoCase(f.path.c_str(),   findingSearch) &&
//                 !ContainsNoCase(f.level.c_str(),  findingSearch) &&
//                 !ContainsNoCase(f.reason.c_str(), findingSearch))
//                 continue;

//             const bool   bad = (f.level == "Malicious");
//             const ImVec4 lc  = bad ? COLOR_RED : COLOR_YELLOW;

//             ImGui::TableNextRow();

//             if (bad)
//                 ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, Col(lc, 0.08f));

//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextDisabled("%s", f.time.c_str());

//             // level as a pill
//             ImGui::TableSetColumnIndex(1);
//             {
//                 const ImVec2 ts = ImGui::CalcTextSize(f.level.c_str());
//                 const ImVec2 p  = ImGui::GetCursorScreenPos();
//                 const float  w  = ts.x + 24.0f;
//                 const float  h  = ts.y + 4.0f;
//                 ImDrawList*  d  = ImGui::GetWindowDrawList();

//                 d->AddRectFilled(p, ImVec2(p.x + w, p.y + h), Col(lc, 0.16f), h * 0.5f);
//                 d->AddRect(p, ImVec2(p.x + w, p.y + h), Col(lc, 0.60f), h * 0.5f);
//                 d->AddText(ImVec2(p.x + 12.0f, p.y + 2.0f), ImGui::GetColorU32(lc),
//                     f.level.c_str());

//                 ImGui::Dummy(ImVec2(w, h));
//             }

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
//     static int  catFilter = 0;     // 0 All, 1 Known hash, 2 File content, 3 Name / location

//     if (!loaded)
//     {
//         signatures = FileScanner::getSignatures();
//         loaded = true;
//     }

//     const int threshold = FileScanner::suspiciousThreshold();

//     // 1 = known hash, 2 = file content, 3 = name / location
//     auto categoryIndex = [](const std::string& category)
//     {
//         if (category == "Known hash")   return 1;
//         if (category == "File content") return 2;
//         return 3;
//     };

//     int hashCount = 0, contentCount = 0, nameCount = 0, aloneCount = 0;

//     for (const SignatureInfo& s : signatures)
//     {
//         const int cat = categoryIndex(s.category);

//         if (cat == 1)      ++hashCount;
//         else if (cat == 2) ++contentCount;
//         else               ++nameCount;

//         if (!s.instantMalicious && s.weight >= threshold)
//             ++aloneCount;
//     }

//     BeginPanel("LogsPanel", ImVec2(0.0f, 0.0f), COLOR_GREEN);

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

//     // How a file ends up in the results
//     ImGui::PushTextWrapPos(0.0f);
//     ImGui::TextColored(COLOR_MUTED,
//         "A file is reported Malicious on an instant match (EICAR test string or a "
//         "known-bad SHA-256). Otherwise every matching rule adds its weight to a "
//         "score, and a file scoring %d or more is reported Suspicious.",
//         threshold);
//     ImGui::Spacing();
//     ImGui::TextColored(COLOR_MUTED,
//         "Rules that describe the same behaviour (for example the different ways to "
//         "delete shadow copies) count once, using the highest weight. Command rules "
//         "run on programs and scripts only; ransom-note wording is also checked in "
//         "text files. Validly signed programs and files listed in hids_allowlist.txt "
//         "are not reported, unless a known-bad hash matches.");
//     ImGui::PopTextWrapPos();

//     ImGui::Spacing();

//     {
//         char total[16], h[16], c[16], n[16], a[16];
//         std::snprintf(total, sizeof(total), "%d", (int)signatures.size());
//         std::snprintf(h, sizeof(h), "%d", hashCount);
//         std::snprintf(c, sizeof(c), "%d", contentCount);
//         std::snprintf(n, sizeof(n), "%d", nameCount);
//         std::snprintf(a, sizeof(a), "%d", aloneCount);

//         StatChipFlow("rules",           total, COLOR_GREEN,  true);
//         StatChipFlow("known hashes",    h,     COLOR_RED,    false);
//         StatChipFlow("file content",    c,     COLOR_CYAN,   false);
//         StatChipFlow("name / location", n,     COLOR_YELLOW, false);
//         StatChipFlow("flag a file alone", a,   COLOR_ORANGE, false);
//     }

//     ImGui::Spacing();

//     // Category filter (with counts)
//     {
//         static const char* catLabels[4] =
//             { "All", "Known hashes", "File content", "Name / location" };
//         const int    catCounts[4] =
//             { (int)signatures.size(), hashCount, contentCount, nameCount };
//         const ImVec4 catTones[4] =
//             { COLOR_BLUE, COLOR_RED, COLOR_CYAN, COLOR_YELLOW };

//         for (int i = 0; i < 4; ++i)
//         {
//             const bool active = (catFilter == i);

//             char text[48];
//             std::snprintf(text, sizeof(text), "%s  %d", catLabels[i], catCounts[i]);

//             ImGui::PushStyleColor(ImGuiCol_Button,
//                 active ? WithAlpha(catTones[i], 0.60f) : COLOR_FRAME);
//             ImGui::PushStyleColor(ImGuiCol_ButtonHovered, WithAlpha(catTones[i], 0.45f));
//             ImGui::PushStyleColor(ImGuiCol_ButtonActive,  WithAlpha(catTones[i], 0.75f));

//             if (ImGui::Button(text, ImVec2(160.0f, 0.0f)))
//                 catFilter = i;

//             ImGui::PopStyleColor(3);

//             if (active)
//             {
//                 const ImVec2 a = ImGui::GetItemRectMin();
//                 const ImVec2 b = ImGui::GetItemRectMax();

//                 ImGui::GetWindowDrawList()->AddRectFilled(
//                     ImVec2(a.x + 10.0f, b.y + 1.0f), ImVec2(b.x - 10.0f, b.y + 3.0f),
//                     ImGui::GetColorU32(catTones[i]), 1.0f);
//             }

//             if (i + 1 < 4)
//             {
//                 // Wrap to the next row if the next button would not fit
//                 const float rightEdge = ImGui::GetCursorScreenPos().x +
//                                         ImGui::GetContentRegionAvail().x;

//                 if (ImGui::GetItemRectMax().x +
//                     ImGui::GetStyle().ItemSpacing.x + 160.0f <= rightEdge)
//                     ImGui::SameLine();
//             }
//         }
//     }

//     ImGui::SetNextItemWidth(300.0f);
//     ImGui::InputTextWithHint("##sigsearch", "Search signatures", search, IM_ARRAYSIZE(search));

//     ImGui::SameLine();

//     if (ImGui::Button("Reload"))
//         signatures = FileScanner::getSignatures();   // re-reads hids_hashes.txt

//     ImGui::SameLine();
//     ImGui::TextDisabled("Rules marked \"flags alone\" reach the threshold by themselves; "
//                         "the others need a second indicator.");

//     ImGui::Spacing();

//     if (ImGui::BeginTable(
//             "SignatureTable",
//             4,
//             ImGuiTableFlags_RowBg |
//             ImGuiTableFlags_BordersInnerH |
//             ImGuiTableFlags_Resizable |
//             ImGuiTableFlags_Sortable |
//             ImGuiTableFlags_SizingStretchProp |
//             ImGuiTableFlags_ScrollY))
//     {
//         ImGui::TableSetupScrollFreeze(0, 1);
//         ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 160.0f, 0);
//         ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 1.0f, 1);
//         ImGui::TableSetupColumn("Pattern / condition",
//             ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoSort, 1.4f, 2);
//         ImGui::TableSetupColumn("Result",
//             ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_PreferSortDescending,
//             150.0f, 3);
//         ImGui::TableHeadersRow();

//         // Optional sorting (by type, name or weight)
//         std::vector<int> order(signatures.size());

//         for (std::size_t i = 0; i < order.size(); ++i)
//             order[i] = (int)i;

//         if (ImGuiTableSortSpecs* specs = ImGui::TableGetSortSpecs())
//         {
//             if (specs->SpecsCount > 0)
//             {
//                 const ImGuiTableColumnSortSpecs& spec = specs->Specs[0];
//                 const bool asc =
//                     spec.SortDirection == ImGuiSortDirection_Ascending;

//                 std::stable_sort(order.begin(), order.end(),
//                     [&](int a, int b)
//                     {
//                         const SignatureInfo& sa = signatures[a];
//                         const SignatureInfo& sb = signatures[b];

//                         int cmp = 0;

//                         switch (spec.ColumnUserID)
//                         {
//                         case 0: cmp = sa.category.compare(sb.category); break;
//                         case 1: cmp = sa.name.compare(sb.name); break;
//                         case 3:
//                         {
//                             const int wa = sa.instantMalicious ? 100 : sa.weight;
//                             const int wb = sb.instantMalicious ? 100 : sb.weight;
//                             cmp = (wa > wb) - (wa < wb);
//                             break;
//                         }
//                         }

//                         if (cmp == 0)
//                             return false;

//                         return asc ? (cmp < 0) : (cmp > 0);
//                     });
//             }
//         }

//         int shown = 0;

//         for (int index : order)
//         {
//             const SignatureInfo& s = signatures[index];

//             if (catFilter != 0 && categoryIndex(s.category) != catFilter)
//                 continue;

//             if (!ContainsNoCase(s.name.c_str(), search) &&
//                 !ContainsNoCase(s.pattern.c_str(), search) &&
//                 !ContainsNoCase(s.category.c_str(), search))
//                 continue;

//             ++shown;

//             ImGui::TableNextRow();

//             ImGui::TableSetColumnIndex(0);
//             {
//                 const ImVec4 tc =
//                     s.category == "Known hash"   ? COLOR_RED :
//                     s.category == "File content" ? COLOR_CYAN : COLOR_YELLOW;

//                 const ImVec2 p = ImGui::GetCursorScreenPos();
//                 ImGui::GetWindowDrawList()->AddCircleFilled(
//                     ImVec2(p.x + 4.0f, p.y + ImGui::GetTextLineHeight() * 0.5f),
//                     3.4f, ImGui::GetColorU32(tc), 12);

//                 ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 14.0f);
//                 ImGui::TextColored(tc, "%s", s.category.c_str());
//             }

//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextWrapped("%s", s.name.c_str());

//             ImGui::TableSetColumnIndex(2);
//             ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
//             ImGui::TextWrapped("%s", s.pattern.c_str());
//             ImGui::PopStyleColor();

//             ImGui::TableSetColumnIndex(3);

//             if (s.instantMalicious)
//             {
//                 ImGui::TextColored(COLOR_RED, "Malicious");
//             }
//             else
//             {
//                 const ImVec4 tone =
//                     s.weight >= 5 ? COLOR_RED :
//                     s.weight >= 4 ? COLOR_ORANGE :
//                     s.weight >= 2 ? COLOR_YELLOW : COLOR_MUTED;

//                 if (s.weight >= threshold)
//                     ImGui::TextColored(tone, "+%d  flags alone", s.weight);
//                 else
//                     ImGui::TextColored(tone, "+%d score", s.weight);
//             }
//         }

//         if (shown == 0)
//         {
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextDisabled("No signature matches the current filter.");
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
//     BeginPanel("NotificationSettings", ImVec2(0.0f, 220.0f), COLOR_YELLOW, true);

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

//     // Page content eases up into place when the page opens
//     {
//         const float k = Smooth01((float)((ImGui::GetTime() - screenChangedAt) / 0.30));
//         const float slide = (1.0f - k) * 16.0f;

//         if (slide > 0.5f)
//             ImGui::SetCursorPosY(ImGui::GetCursorPosY() + slide);
//     }

//     DrawPageHeader();
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
//         DrawEngineBanner();     // only while protection is stopped
//         DrawPostureBanner();    // only while protection is running
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

//     // Keep the layout from being squeezed below what the cards need
//     glfwSetWindowSizeLimits(window, 1100, 720, GLFW_DONT_CARE, GLFW_DONT_CARE);

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
//     RefineStyle();            // rounded look, table + scrollbar colours
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
//         HandleBackKey();       // Esc: page -> domain -> hub -> welcome

//         // Gradient backdrop + ambient glows; the root window is transparent
//         DrawBackdrop();
//         DrawAmbient();
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

//         // In-app toast for brand-new alerts (drawn above everything)
//         DrawToast();

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

// static const float HIDS_PI  = 3.14159265f;
// static const float HIDS_2PI = 6.28318531f;

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

// // In-app toast shown when a brand-new alert arrives
// static std::string toastText;
// static std::string toastSeverity;
// static double      toastAt = -100.0;

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
// // GRAPHICS HELPERS
// // Small building blocks shared by every screen: colour helpers,
// // animation easing, soft shadows, glass card chrome, usage bars.
// // ------------------------------------------------------------

// // Colour with alpha, already converted for the draw list
// static ImU32 Col(const ImVec4& c, float a = 1.0f)
// {
//     return ImGui::GetColorU32(WithAlpha(c, a));
// }

// static const ImVec4 FX_WHITE(1.0f, 1.0f, 1.0f, 1.0f);
// static const ImVec4 FX_BLACK(0.0f, 0.0f, 0.0f, 1.0f);

// static float Smooth01(float t)
// {
//     t = ClampFloat(t, 0.0f, 1.0f);
//     return t * t * (3.0f - 2.0f * t);
// }

// static ImVec2 Off(const ImVec2& p, float x, float y)
// {
//     return ImVec2(p.x + x, p.y + y);
// }

// // Eases a stored value toward a target (per-window state, keyed by id).
// // Used for hover glows and bar fills so nothing snaps.
// static float AnimTo(ImGuiID id, float target, float speed = 12.0f,
//                     float initial = -1.0e9f)
// {
//     ImGuiStorage* st = ImGui::GetStateStorage();
//     float v = st->GetFloat(id, initial < -1.0e8f ? target : initial);

//     v += (target - v) * ClampFloat(ImGui::GetIO().DeltaTime * speed, 0.0f, 1.0f);
//     st->SetFloat(id, v);
//     return v;
// }

// // Soft drop shadow made of stacked translucent rounded rects.
// // Must be drawn on the PARENT draw list, before the card's BeginChild.
// static void DrawSoftShadow(ImDrawList* d, const ImVec2& a, const ImVec2& b,
//                            float rounding, float spread, float alpha)
// {
//     const int layers = 6;

//     for (int i = 0; i < layers; ++i)
//     {
//         const float grow = spread * (1.0f - (float)i / (float)layers);
//         const float al   = alpha * (float)(i + 1) / (float)layers * 0.35f;

//         d->AddRectFilled(
//             ImVec2(a.x - grow, a.y - grow + 6.0f),
//             ImVec2(b.x + grow, b.y + grow + 6.0f),
//             Col(FX_BLACK, al), rounding + grow);
//     }
// }

// // Glass finish drawn inside a child: sheen, top highlight, tinted border
// static void DrawCardChrome(ImDrawList* d, const ImVec2& wp, const ImVec2& ws,
//                            const ImVec4& accent, float hover)
// {
//     const float r = ImGui::GetStyle().ChildRounding;

//     d->AddRectFilledMultiColor(
//         ImVec2(wp.x + 2.0f, wp.y + 2.0f),
//         ImVec2(wp.x + ws.x - 2.0f, wp.y + ws.y * 0.55f),
//         Col(FX_WHITE, 0.040f + 0.035f * hover),
//         Col(FX_WHITE, 0.012f),
//         Col(FX_WHITE, 0.0f),
//         Col(FX_WHITE, 0.0f));

//     d->AddLine(ImVec2(wp.x + r, wp.y + 1.5f),
//                ImVec2(wp.x + ws.x - r, wp.y + 1.5f),
//                Col(FX_WHITE, 0.07f + 0.05f * hover), 1.0f);

//     d->AddRect(wp, ImVec2(wp.x + ws.x, wp.y + ws.y),
//                Col(accent, 0.14f + 0.62f * hover), r, 0, 1.0f + 0.7f * hover);
// }

// // Rounded usage bar with a glossy highlight and centred label.
// // Fills the width available in the current cell / region.
// static void DrawUsageBar(float frac, const char* label, const ImVec4& col,
//                          float h = 18.0f)
// {
//     frac = ClampFloat(frac, 0.0f, 1.0f);

//     const float  w = std::max(20.0f, ImGui::GetContentRegionAvail().x);
//     const ImVec2 p = ImGui::GetCursorScreenPos();
//     ImDrawList*  d = ImGui::GetWindowDrawList();

//     d->AddRectFilled(p, ImVec2(p.x + w, p.y + h),
//         ImGui::GetColorU32(COLOR_FRAME), h * 0.5f);

//     if (frac > 0.004f)
//     {
//         const float fw = std::max(h, w * frac);

//         d->AddRectFilled(p, ImVec2(p.x + fw, p.y + h), Col(col, 0.88f), h * 0.5f);
//         d->AddRectFilled(ImVec2(p.x + 4.0f, p.y + 2.0f),
//                          ImVec2(p.x + fw - 4.0f, p.y + h * 0.46f),
//                          Col(FX_WHITE, 0.20f), h * 0.25f);
//     }

//     d->AddRect(p, ImVec2(p.x + w, p.y + h), Col(col, 0.25f), h * 0.5f);

//     if (label != nullptr && label[0] != '\0')
//     {
//         const ImVec2 ts = ImGui::CalcTextSize(label);
//         const ImVec2 tp(p.x + (w - ts.x) * 0.5f, p.y + (h - ts.y) * 0.5f);

//         d->AddText(Off(tp, 1.0f, 1.0f), Col(FX_BLACK, 0.55f), label);
//         d->AddText(tp, ImGui::GetColorU32(COLOR_TEXT), label);
//     }

//     ImGui::Dummy(ImVec2(w, h));
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
// // Rounded, flat look + tinted borders, table headers, scrollbars.
// // Applied after ApplyTheme(); colours still come from the theme.
// // ------------------------------------------------------------

// static void RefineStyle()
// {
//     ImGuiStyle& s = ImGui::GetStyle();

//     s.WindowRounding    = 0.0f;
//     s.ChildRounding     = 14.0f;
//     s.FrameRounding     = 8.0f;
//     s.PopupRounding     = 12.0f;
//     s.GrabRounding      = 7.0f;
//     s.ScrollbarRounding = 9.0f;
//     s.TabRounding       = 7.0f;

//     s.WindowBorderSize  = 0.0f;
//     s.ChildBorderSize   = 1.0f;
//     s.FrameBorderSize   = 0.0f;
//     s.PopupBorderSize   = 1.0f;

//     s.WindowPadding     = ImVec2(18.0f, 16.0f);
//     s.FramePadding      = ImVec2(13.0f, 7.0f);
//     s.ItemSpacing       = ImVec2(10.0f, 10.0f);
//     s.ItemInnerSpacing  = ImVec2(8.0f, 6.0f);
//     s.CellPadding       = ImVec2(10.0f, 8.0f);
//     s.ScrollbarSize     = 10.0f;
//     s.GrabMinSize       = 12.0f;

//     // Smoother circles / lines everywhere
//     s.AntiAliasedLines        = true;
//     s.AntiAliasedLinesUseTex  = true;
//     s.AntiAliasedFill         = true;
//     s.CircleTessellationMaxError = 0.20f;

//     ImVec4* c = s.Colors;
//     c[ImGuiCol_TableHeaderBg]     = WithAlpha(COLOR_CYAN, 0.14f);
//     c[ImGuiCol_TableBorderLight]  = WithAlpha(COLOR_CYAN, 0.07f);
//     c[ImGuiCol_TableBorderStrong] = WithAlpha(COLOR_CYAN, 0.18f);
//     c[ImGuiCol_TableRowBg]        = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
//     c[ImGuiCol_TableRowBgAlt]     = ImVec4(1.0f, 1.0f, 1.0f, 0.026f);
//     c[ImGuiCol_ScrollbarBg]       = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
//     c[ImGuiCol_ScrollbarGrab]        = WithAlpha(COLOR_CYAN, 0.30f);
//     c[ImGuiCol_ScrollbarGrabHovered] = WithAlpha(COLOR_CYAN, 0.55f);
//     c[ImGuiCol_ScrollbarGrabActive]  = WithAlpha(COLOR_CYAN, 0.80f);

//     c[ImGuiCol_Border]    = WithAlpha(COLOR_CYAN, 0.16f);
//     c[ImGuiCol_Separator] = WithAlpha(COLOR_CYAN, 0.14f);
//     c[ImGuiCol_CheckMark] = COLOR_CYAN;
//     c[ImGuiCol_SliderGrab]       = WithAlpha(COLOR_CYAN, 0.80f);
//     c[ImGuiCol_SliderGrabActive] = COLOR_CYAN;
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

// // Section title: glowing accent bar, title, hint, gradient underline
// static void SectionHeading(const char* title, const char* hint = nullptr)
// {
//     const float fullW = ImGui::GetContentRegionAvail().x;

//     {
//         const ImVec2 p = ImGui::GetCursorScreenPos();
//         const float  h = ImGui::GetTextLineHeight() * 1.1f;
//         ImDrawList*  d = ImGui::GetWindowDrawList();

//         d->AddRectFilled(ImVec2(p.x - 2.0f, p.y - 1.0f), ImVec2(p.x + 8.0f, p.y + h + 2.0f),
//             Col(COLOR_CYAN, 0.16f), 5.0f);
//         d->AddRectFilled(ImVec2(p.x, p.y + 1.0f), ImVec2(p.x + 4.0f, p.y + h),
//             ImGui::GetColorU32(COLOR_CYAN), 2.0f);

//         ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 14.0f);
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

//     {
//         const ImVec2 p = ImGui::GetCursorScreenPos();

//         ImGui::GetWindowDrawList()->AddRectFilledMultiColor(
//             ImVec2(p.x, p.y + 1.0f), ImVec2(p.x + fullW, p.y + 2.0f),
//             Col(COLOR_CYAN, 0.55f), Col(COLOR_CYAN, 0.0f),
//             Col(COLOR_CYAN, 0.0f),  Col(COLOR_CYAN, 0.55f));
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
//     const ImVec2 size(textSize.x + 12.0f + 9.0f + pad.x, textSize.y + pad.y * 2.0f);

//     ImDrawList* draw = ImGui::GetWindowDrawList();

//     draw->AddRectFilled(
//         pos,
//         ImVec2(pos.x + size.x, pos.y + size.y),
//         Col(color, 0.16f),
//         size.y * 0.5f
//     );

//     draw->AddRect(
//         pos,
//         ImVec2(pos.x + size.x, pos.y + size.y),
//         Col(color, 0.65f),
//         size.y * 0.5f
//     );

//     const ImVec2 dot(pos.x + 12.0f, pos.y + size.y * 0.5f);
//     draw->AddCircleFilled(dot, 6.5f, Col(color, 0.20f), 20);
//     draw->AddCircleFilled(dot, 3.5f, ImGui::GetColorU32(color), 16);

//     draw->AddText(
//         ImVec2(pos.x + 12.0f + 9.0f, pos.y + pad.y),
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
//         const float ring = std::fmod(t * 0.9f, 1.0f);

//         draw->AddCircleFilled(
//             center,
//             radius + 3.0f + wave * 3.0f,
//             Col(color, 0.18f * (1.0f - wave) + 0.05f)
//         );

//         draw->AddCircle(center, radius + 2.0f + ring * 9.0f,
//             Col(color, 0.45f * (1.0f - ring)), 24, 1.4f);
//     }

//     draw->AddCircleFilled(center, radius, ImGui::GetColorU32(color));
//     draw->AddCircleFilled(Off(center, -1.4f, -1.6f), radius * 0.35f, Col(FX_WHITE, 0.55f), 12);

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

//     // Weigh alerts that appeared since the last refresh (+ raise a toast)
//     {
//         static std::set<int> seenAlerts;
//         static bool seeded = false;

//         for (const AlertInfo& a : alerts)
//         {
//             if (seenAlerts.insert(a.id).second && seeded)
//             {
//                 threatAccum += SeverityWeight(a.severity);

//                 toastText     = a.description;
//                 toastSeverity = a.severity;
//                 toastAt       = now;
//             }
//         }

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



//     ImGui::EndChild();
// }

// // ------------------------------------------------------------
// // FEATURE PAGE: header with Back + only the selected feature
// // ------------------------------------------------------------

// static void DrawMainContent()
// {
//     RefreshFromBackend();

//     // Page content eases up into place when the page opens
//     {
//         const float k = Smooth01((float)((ImGui::GetTime() - screenChangedAt) / 0.30));
//         const float slide = (1.0f - k) * 16.0f;

//         if (slide > 0.5f)
//             ImGui::SetCursorPosY(ImGui::GetCursorPosY() + slide);
//     }

//     DrawPageHeader();
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
//         DrawEngineBanner();     // only while protection is stopped
//         DrawPostureBanner();    // only while protection is running
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

//     // Keep the layout from being squeezed below what the cards need
//     glfwSetWindowSizeLimits(window, 1100, 720, GLFW_DONT_CARE, GLFW_DONT_CARE);

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
//     RefineStyle();            // rounded look, table + scrollbar colours
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
//         HandleBackKey();       // Esc: page -> domain -> hub -> welcome

//         // Gradient backdrop + ambient glows; the root window is transparent
//         DrawBackdrop();
//         DrawAmbient();
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

//         // In-app toast for brand-new alerts (drawn above everything)
//         DrawToast();

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
#include <ctime>
#include <set>
#include <string>
#include <vector>
#include "HIDSBackend.h"
#include "DashboardTheme.h"   // themes, colours, icons, Settings page

static const float HIDS_PI  = 3.14159265f;
static const float HIDS_2PI = 6.28318531f;

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

// In-app toast shown when a brand-new alert arrives
static std::string toastText;
static std::string toastSeverity;
static double      toastAt = -100.0;

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
// GRAPHICS HELPERS
// Small building blocks shared by every screen: colour helpers,
// animation easing, soft shadows, glass card chrome, usage bars.
// ------------------------------------------------------------

// Colour with alpha, already converted for the draw list
static ImU32 Col(const ImVec4& c, float a = 1.0f)
{
    return ImGui::GetColorU32(WithAlpha(c, a));
}

static const ImVec4 FX_WHITE(1.0f, 1.0f, 1.0f, 1.0f);
static const ImVec4 FX_BLACK(0.0f, 0.0f, 0.0f, 1.0f);

static float Smooth01(float t)
{
    t = ClampFloat(t, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

static ImVec2 Off(const ImVec2& p, float x, float y)
{
    return ImVec2(p.x + x, p.y + y);
}

// Eases a stored value toward a target (per-window state, keyed by id).
// Used for hover glows and bar fills so nothing snaps.
static float AnimTo(ImGuiID id, float target, float speed = 12.0f,
                    float initial = -1.0e9f)
{
    ImGuiStorage* st = ImGui::GetStateStorage();
    float v = st->GetFloat(id, initial < -1.0e8f ? target : initial);

    v += (target - v) * ClampFloat(ImGui::GetIO().DeltaTime * speed, 0.0f, 1.0f);
    st->SetFloat(id, v);
    return v;
}

// Soft drop shadow made of stacked translucent rounded rects.
// Must be drawn on the PARENT draw list, before the card's BeginChild.
static void DrawSoftShadow(ImDrawList* d, const ImVec2& a, const ImVec2& b,
                           float rounding, float spread, float alpha)
{
    const int layers = 6;

    for (int i = 0; i < layers; ++i)
    {
        const float grow = spread * (1.0f - (float)i / (float)layers);
        const float al   = alpha * (float)(i + 1) / (float)layers * 0.35f;

        d->AddRectFilled(
            ImVec2(a.x - grow, a.y - grow + 6.0f),
            ImVec2(b.x + grow, b.y + grow + 6.0f),
            Col(FX_BLACK, al), rounding + grow);
    }
}

// Glass finish drawn inside a child: sheen, top highlight, tinted border
static void DrawCardChrome(ImDrawList* d, const ImVec2& wp, const ImVec2& ws,
                           const ImVec4& accent, float hover)
{
    const float r = ImGui::GetStyle().ChildRounding;

    d->AddRectFilledMultiColor(
        ImVec2(wp.x + 2.0f, wp.y + 2.0f),
        ImVec2(wp.x + ws.x - 2.0f, wp.y + ws.y * 0.55f),
        Col(FX_WHITE, 0.040f + 0.035f * hover),
        Col(FX_WHITE, 0.012f),
        Col(FX_WHITE, 0.0f),
        Col(FX_WHITE, 0.0f));

    d->AddLine(ImVec2(wp.x + r, wp.y + 1.5f),
               ImVec2(wp.x + ws.x - r, wp.y + 1.5f),
               Col(FX_WHITE, 0.07f + 0.05f * hover), 1.0f);

    d->AddRect(wp, ImVec2(wp.x + ws.x, wp.y + ws.y),
               Col(accent, 0.14f + 0.62f * hover), r, 0, 1.0f + 0.7f * hover);
}

// Rounded usage bar with a glossy highlight and centred label.
// Fills the width available in the current cell / region.
static void DrawUsageBar(float frac, const char* label, const ImVec4& col,
                         float h = 18.0f)
{
    frac = ClampFloat(frac, 0.0f, 1.0f);

    const float  w = std::max(20.0f, ImGui::GetContentRegionAvail().x);
    const ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList*  d = ImGui::GetWindowDrawList();

    d->AddRectFilled(p, ImVec2(p.x + w, p.y + h),
        ImGui::GetColorU32(COLOR_FRAME), h * 0.5f);

    if (frac > 0.004f)
    {
        const float fw = std::max(h, w * frac);

        d->AddRectFilled(p, ImVec2(p.x + fw, p.y + h), Col(col, 0.88f), h * 0.5f);
        d->AddRectFilled(ImVec2(p.x + 4.0f, p.y + 2.0f),
                         ImVec2(p.x + fw - 4.0f, p.y + h * 0.46f),
                         Col(FX_WHITE, 0.20f), h * 0.25f);
    }

    d->AddRect(p, ImVec2(p.x + w, p.y + h), Col(col, 0.25f), h * 0.5f);

    if (label != nullptr && label[0] != '\0')
    {
        const ImVec2 ts = ImGui::CalcTextSize(label);
        const ImVec2 tp(p.x + (w - ts.x) * 0.5f, p.y + (h - ts.y) * 0.5f);

        d->AddText(Off(tp, 1.0f, 1.0f), Col(FX_BLACK, 0.55f), label);
        d->AddText(tp, ImGui::GetColorU32(COLOR_TEXT), label);
    }

    ImGui::Dummy(ImVec2(w, h));
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
// Rounded, flat look + tinted borders, table headers, scrollbars.
// Applied after ApplyTheme(); colours still come from the theme.
// ------------------------------------------------------------

static void RefineStyle()
{
    ImGuiStyle& s = ImGui::GetStyle();

    s.WindowRounding    = 0.0f;
    s.ChildRounding     = 14.0f;
    s.FrameRounding     = 8.0f;
    s.PopupRounding     = 12.0f;
    s.GrabRounding      = 7.0f;
    s.ScrollbarRounding = 9.0f;
    s.TabRounding       = 7.0f;

    s.WindowBorderSize  = 0.0f;
    s.ChildBorderSize   = 1.0f;
    s.FrameBorderSize   = 0.0f;
    s.PopupBorderSize   = 1.0f;

    s.WindowPadding     = ImVec2(18.0f, 16.0f);
    s.FramePadding      = ImVec2(13.0f, 7.0f);
    s.ItemSpacing       = ImVec2(10.0f, 10.0f);
    s.ItemInnerSpacing  = ImVec2(8.0f, 6.0f);
    s.CellPadding       = ImVec2(10.0f, 8.0f);
    s.ScrollbarSize     = 10.0f;
    s.GrabMinSize       = 12.0f;

    // Smoother circles / lines everywhere
    s.AntiAliasedLines        = true;
    s.AntiAliasedLinesUseTex  = true;
    s.AntiAliasedFill         = true;
    s.CircleTessellationMaxError = 0.20f;

    ImVec4* c = s.Colors;
    c[ImGuiCol_TableHeaderBg]     = WithAlpha(COLOR_CYAN, 0.14f);
    c[ImGuiCol_TableBorderLight]  = WithAlpha(COLOR_CYAN, 0.07f);
    c[ImGuiCol_TableBorderStrong] = WithAlpha(COLOR_CYAN, 0.18f);
    c[ImGuiCol_TableRowBg]        = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    c[ImGuiCol_TableRowBgAlt]     = ImVec4(1.0f, 1.0f, 1.0f, 0.026f);
    c[ImGuiCol_ScrollbarBg]       = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    c[ImGuiCol_ScrollbarGrab]        = WithAlpha(COLOR_CYAN, 0.30f);
    c[ImGuiCol_ScrollbarGrabHovered] = WithAlpha(COLOR_CYAN, 0.55f);
    c[ImGuiCol_ScrollbarGrabActive]  = WithAlpha(COLOR_CYAN, 0.80f);

    c[ImGuiCol_Border]    = WithAlpha(COLOR_CYAN, 0.16f);
    c[ImGuiCol_Separator] = WithAlpha(COLOR_CYAN, 0.14f);
    c[ImGuiCol_CheckMark] = COLOR_CYAN;
    c[ImGuiCol_SliderGrab]       = WithAlpha(COLOR_CYAN, 0.80f);
    c[ImGuiCol_SliderGrabActive] = COLOR_CYAN;

    // Inputs glow softly when hovered / focused
    c[ImGuiCol_FrameBgHovered]   = WithAlpha(COLOR_CYAN, 0.14f);
    c[ImGuiCol_FrameBgActive]    = WithAlpha(COLOR_CYAN, 0.22f);
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

// Section title: glowing accent bar, title, hint, gradient underline
static void SectionHeading(const char* title, const char* hint = nullptr)
{
    const float fullW = ImGui::GetContentRegionAvail().x;

    {
        const ImVec2 p = ImGui::GetCursorScreenPos();
        const float  h = ImGui::GetTextLineHeight() * 1.1f;
        ImDrawList*  d = ImGui::GetWindowDrawList();

        d->AddRectFilled(ImVec2(p.x - 2.0f, p.y - 1.0f), ImVec2(p.x + 8.0f, p.y + h + 2.0f),
            Col(COLOR_CYAN, 0.16f), 5.0f);
        d->AddRectFilled(ImVec2(p.x, p.y + 1.0f), ImVec2(p.x + 4.0f, p.y + h),
            ImGui::GetColorU32(COLOR_CYAN), 2.0f);

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 14.0f);
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

    {
        const ImVec2 p = ImGui::GetCursorScreenPos();

        ImGui::GetWindowDrawList()->AddRectFilledMultiColor(
            ImVec2(p.x, p.y + 1.0f), ImVec2(p.x + fullW, p.y + 2.0f),
            Col(COLOR_CYAN, 0.55f), Col(COLOR_CYAN, 0.0f),
            Col(COLOR_CYAN, 0.0f),  Col(COLOR_CYAN, 0.55f));
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
    const ImVec2 size(textSize.x + 12.0f + 9.0f + pad.x, textSize.y + pad.y * 2.0f);

    ImDrawList* draw = ImGui::GetWindowDrawList();

    draw->AddRectFilled(
        pos,
        ImVec2(pos.x + size.x, pos.y + size.y),
        Col(color, 0.16f),
        size.y * 0.5f
    );

    draw->AddRect(
        pos,
        ImVec2(pos.x + size.x, pos.y + size.y),
        Col(color, 0.65f),
        size.y * 0.5f
    );

    const ImVec2 dot(pos.x + 12.0f, pos.y + size.y * 0.5f);
    draw->AddCircleFilled(dot, 6.5f, Col(color, 0.20f), 20);
    draw->AddCircleFilled(dot, 3.5f, ImGui::GetColorU32(color), 16);

    draw->AddText(
        ImVec2(pos.x + 12.0f + 9.0f, pos.y + pad.y),
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
        const float ring = std::fmod(t * 0.9f, 1.0f);

        draw->AddCircleFilled(
            center,
            radius + 3.0f + wave * 3.0f,
            Col(color, 0.18f * (1.0f - wave) + 0.05f)
        );

        draw->AddCircle(center, radius + 2.0f + ring * 9.0f,
            Col(color, 0.45f * (1.0f - ring)), 24, 1.4f);
    }

    draw->AddCircleFilled(center, radius, ImGui::GetColorU32(color));
    draw->AddCircleFilled(Off(center, -1.4f, -1.6f), radius * 0.35f, Col(FX_WHITE, 0.55f), 12);

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

    // Weigh alerts that appeared since the last refresh (+ raise a toast)
    {
        static std::set<int> seenAlerts;
        static bool seeded = false;

        for (const AlertInfo& a : alerts)
        {
            if (seenAlerts.insert(a.id).second && seeded)
            {
                threatAccum += SeverityWeight(a.severity);

                toastText     = a.description;
                toastSeverity = a.severity;
                toastAt       = now;
            }
        }

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
// TOAST - slides in from the right when a new alert is raised
// ------------------------------------------------------------

static void DrawToast()
{
    const double life = 5.0;
    const double age  = ImGui::GetTime() - toastAt;

    if (age < 0.0 || age > life || toastText.empty())
        return;

    const float in  = Smooth01((float)(age / 0.35));
    const float out = Smooth01((float)((life - age) / 0.45));
    const float k   = std::min(in, out);

    const ImVec2 disp = ImGui::GetIO().DisplaySize;
    const float  w = 400.0f;
    const float  h = 70.0f;
    const ImVec4 sc = SeverityColor(toastSeverity.c_str());

    const ImVec2 a(disp.x - w - 22.0f + (1.0f - k) * (w + 50.0f), 22.0f);
    const ImVec2 b(a.x + w, a.y + h);

    ImDrawList* d = ImGui::GetForegroundDrawList();

    DrawSoftShadow(d, a, b, 14.0f, 16.0f, 0.55f * k);

    d->AddRectFilled(a, b, Col(ImVec4(0.04f, 0.06f, 0.10f, 1.0f), 0.96f * k), 14.0f);
    d->AddRectFilledMultiColor(
        Off(a, 2.0f, 2.0f), ImVec2(a.x + w * 0.7f, b.y - 2.0f),
        Col(sc, 0.18f * k), Col(sc, 0.0f), Col(sc, 0.0f), Col(sc, 0.18f * k));
    d->AddRect(a, b, Col(sc, 0.75f * k), 14.0f, 0, 1.4f);
    d->AddRectFilled(a, ImVec2(a.x + 5.0f, b.y), Col(sc, k), 14.0f,
        ImDrawFlags_RoundCornersLeft);

    // Icon
    const ImVec2 ic(a.x + 38.0f, a.y + h * 0.5f);
    d->AddCircleFilled(ic, 18.0f, Col(sc, 0.18f * k), 32);
    d->AddCircle(ic, 18.0f, Col(sc, 0.85f * k), 32, 1.5f);
    d->AddTriangleFilled(Off(ic, 0.0f, -9.0f), Off(ic, -9.0f, 7.0f), Off(ic, 9.0f, 7.0f),
        Col(sc, k));
    d->AddText(Off(ic, -2.5f, -5.0f), Col(FX_BLACK, 0.9f * k), "!");

    // Text
    char head[64];
    std::snprintf(head, sizeof(head), "NEW ALERT  -  %s", toastSeverity.c_str());

    std::string body = toastText;
    if (body.size() > 54)
        body = body.substr(0, 51) + "...";

    d->AddText(Off(a, 68.0f, 14.0f), Col(sc, k), head);
    d->AddText(Off(a, 68.0f, 38.0f), ImGui::GetColorU32(WithAlpha(COLOR_TEXT, k)),
        body.c_str());

    // Time-left bar
    const float left = ClampFloat((float)((life - age) / life), 0.0f, 1.0f);
    d->AddRectFilled(Off(a, 14.0f, h - 5.0f), ImVec2(a.x + 14.0f + (w - 28.0f) * left, b.y - 3.0f),
        Col(sc, 0.65f * k), 2.0f);
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
    const float  cardH = 150.0f;
    const ImVec2 cp    = ImGui::GetCursorScreenPos();

    DrawSoftShadow(ImGui::GetWindowDrawList(), cp, ImVec2(cp.x + width, cp.y + cardH),
        ImGui::GetStyle().ChildRounding, 12.0f, 0.34f);

    ImGui::BeginChild(
        title,
        ImVec2(width, cardH),
        true,
        ImGuiWindowFlags_NoScrollbar
    );

    const bool  hovered = linkPage >= 0 && ImGui::IsWindowHovered();
    const float hv      = AnimTo(ImGui::GetID("##hv"), hovered ? 1.0f : 0.0f);
    const float t       = (float)ImGui::GetTime();

    {
        const ImVec2 wp = ImGui::GetWindowPos();
        const ImVec2 ws = ImGui::GetWindowSize();
        ImDrawList*  d  = ImGui::GetWindowDrawList();

        // soft tint from the left
        d->AddRectFilledMultiColor(
            ImVec2(wp.x + 1.0f, wp.y + 1.0f),
            ImVec2(wp.x + ws.x * 0.70f, wp.y + ws.y - 1.0f),
            Col(accent, 0.12f + 0.06f * hv),
            Col(accent, 0.0f),
            Col(accent, 0.0f),
            Col(accent, 0.12f + 0.06f * hv));

        DrawCardChrome(d, wp, ws, accent, hv);

        // accent stripe
        d->AddRectFilled(wp, ImVec2(wp.x + 4.0f, wp.y + ws.y),
            ImGui::GetColorU32(accent),
            ImGui::GetStyle().ChildRounding,
            ImDrawFlags_RoundCornersLeft);

        // glow + ring gauge (or page icon) on the right side
        const ImVec2 gc(wp.x + ws.x - 52.0f, wp.y + 60.0f);

        DrawGlow(d, gc, 48.0f + 8.0f * hv, accent);

        if (gauge >= 0.0f)
        {
            // tick ring that lights up with the value
            const int ticks = 30;
            const float frac = ClampFloat(gauge / 100.0f, 0.0f, 1.0f);

            for (int i = 0; i < ticks; ++i)
            {
                const float a  = -HIDS_PI * 0.5f + (float)i * (HIDS_2PI / (float)ticks);
                const bool  on = ((float)i + 0.5f) / (float)ticks <= frac;
                const float r0 = 31.0f;
                const float r1 = on ? 36.0f : 34.0f;

                d->AddLine(
                    ImVec2(gc.x + std::cos(a) * r0, gc.y + std::sin(a) * r0),
                    ImVec2(gc.x + std::cos(a) * r1, gc.y + std::sin(a) * r1),
                    Col(accent, on ? 0.85f : 0.14f), on ? 2.0f : 1.2f);
            }

            DrawGauge(d, gc, 24.0f, 6.0f, frac, accent);
        }
        else if (linkPage >= 0)
        {
            d->AddCircle(gc, 26.0f, Col(accent, 0.25f), 40, 1.2f);
            d->AddCircleFilled(gc, 22.0f, Col(accent, 0.10f), 40);
            DrawPageIcon(d, gc, 14.0f, linkPage,
                ImGui::GetColorU32(WithAlpha(accent, 0.85f)));
        }
    }

    ImGui::TextDisabled("%s", title);

    // Trend arrow next to the title (sparkline cards only)
    if (history != nullptr)
    {
        const int   last  = (historyOffset + HISTORY_SIZE - 1) % HISTORY_SIZE;
        const int   prev  = (historyOffset + HISTORY_SIZE - 9) % HISTORY_SIZE;
        const float delta = history[last] - history[prev];
        const bool  up    = delta >= 0.0f;
        const bool  flat  = std::fabs(delta) < 1.0f;
        const ImVec4 tc   = flat ? COLOR_MUTED : (up ? COLOR_ORANGE : COLOR_GREEN);

        ImGui::SameLine(0.0f, 10.0f);

        const ImVec2 q  = ImGui::GetCursorScreenPos();
        const float  lh = ImGui::GetTextLineHeight();
        ImDrawList*  d  = ImGui::GetWindowDrawList();

        if (flat)
            d->AddRectFilled(Off(q, 0.0f, lh * 0.5f - 1.0f), Off(q, 9.0f, lh * 0.5f + 1.0f),
                ImGui::GetColorU32(tc), 1.0f);
        else if (up)
            d->AddTriangleFilled(Off(q, 4.5f, lh * 0.5f - 4.0f), Off(q, 0.0f, lh * 0.5f + 3.0f),
                Off(q, 9.0f, lh * 0.5f + 3.0f), ImGui::GetColorU32(tc));
        else
            d->AddTriangleFilled(Off(q, 0.0f, lh * 0.5f - 3.0f), Off(q, 9.0f, lh * 0.5f - 3.0f),
                Off(q, 4.5f, lh * 0.5f + 4.0f), ImGui::GetColorU32(tc));

        ImGui::Dummy(ImVec2(10.0f, lh));
        ImGui::SameLine(0.0f, 4.0f);
        ImGui::TextColored(tc, "%.0f", std::fabs(delta));
    }

    ImGui::PushStyleColor(ImGuiCol_Text, accent);
    ImGui::SetWindowFontScale(1.65f);
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
    if (hovered)
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        ImGui::SetTooltip("Open %s", pages[linkPage]);

        // sweeping shine on hover
        {
            const ImVec2 wp = ImGui::GetWindowPos();
            const ImVec2 ws = ImGui::GetWindowSize();
            const float  sx = wp.x + (ws.x + 60.0f) * std::fmod(t * 0.9f, 1.0f) - 30.0f;

            ImDrawList* d = ImGui::GetWindowDrawList();
            d->PushClipRect(Off(wp, 1.0f, 1.0f), ImVec2(wp.x + ws.x - 1.0f, wp.y + ws.y - 1.0f), true);
            d->AddRectFilledMultiColor(
                ImVec2(sx - 30.0f, wp.y), ImVec2(sx + 30.0f, wp.y + ws.y),
                Col(FX_WHITE, 0.0f), Col(FX_WHITE, 0.07f), Col(FX_WHITE, 0.07f), Col(FX_WHITE, 0.0f));
            d->PopClipRect();
        }

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

// Centred title with a drop shadow and a faint glow halo
static void CenteredTitle(const char* text, float scale, const ImVec4& col)
{
    ImGui::SetWindowFontScale(scale);

    const float w = ImGui::CalcTextSize(text).x;
    ImGui::SetCursorPosX(std::max(0.0f, (ImGui::GetWindowWidth() - w) * 0.5f));

    const ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList*  d = ImGui::GetWindowDrawList();

    d->AddText(Off(p, 2.0f, 3.0f), Col(FX_BLACK, 0.55f), text);
    d->AddText(Off(p, -1.0f, 0.0f), Col(col, 0.20f), text);
    d->AddText(Off(p, 1.0f, 0.0f),  Col(col, 0.20f), text);
    d->AddText(Off(p, 0.0f, -1.0f), Col(col, 0.20f), text);
    d->AddText(Off(p, 0.0f, 1.0f),  Col(col, 0.20f), text);

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
// (leaves room for the status bar along the very bottom edge)
static void FooterHint(const char* text, float scale = 0.95f)
{
    const float y = ImGui::GetWindowHeight() - 62.0f;

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

    // inner bevel for depth
    ImVec2 inner[5] = {
        ImVec2(c.x - 0.62f * s, c.y - 0.72f * s),
        ImVec2(c.x + 0.62f * s, c.y - 0.72f * s),
        ImVec2(c.x + 0.62f * s, c.y + 0.06f * s),
        ImVec2(c.x,             c.y + 0.78f * s),
        ImVec2(c.x - 0.62f * s, c.y + 0.06f * s)
    };
    d->AddPolyline(inner, 5, Col(FX_WHITE, 0.30f), ImDrawFlags_Closed, 1.2f);

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

    d->AddRectFilled(ImVec2(c.x - s * 0.85f, c.y + 0.02f * s),
                     ImVec2(c.x + s * 0.85f, c.y + s * 0.30f),
                     Col(FX_WHITE, 0.16f), s * 0.14f);

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
        Col(col, 0.12f), h * 0.5f);
    d->AddRectFilled(Off(p, 6.0f, 2.0f), ImVec2(p.x + w - 6.0f, p.y + h * 0.45f),
        Col(FX_WHITE, 0.05f), h * 0.25f);
    d->AddRect(p, ImVec2(p.x + w, p.y + h),
        Col(col, 0.45f), h * 0.5f);

    d->AddCircleFilled(ImVec2(p.x + 16.0f, p.y + h * 0.5f), 6.5f, Col(col, 0.20f), 20);
    d->AddCircleFilled(ImVec2(p.x + 16.0f, p.y + h * 0.5f), 4.0f,
        ImGui::GetColorU32(col));

    const float ty = p.y + (h - vs.y) * 0.5f;

    d->AddText(ImVec2(p.x + 28.0f, ty), ImGui::GetColorU32(col), value);
    d->AddText(ImVec2(p.x + 28.0f + vs.x + 8.0f, ty),
        ImGui::GetColorU32(COLOR_MUTED), label);

    ImGui::Dummy(ImVec2(w, h));
}

// Same chip, but it wraps to the next row when it does not fit on the
// current one (so a row of chips never runs out of the panel).
static void StatChipFlow(const char* label, const char* value,
                         const ImVec4& col, bool first)
{
    if (!first)
    {
        const float w = 14.0f + 10.0f + ImGui::CalcTextSize(value).x +
                        8.0f + ImGui::CalcTextSize(label).x + 14.0f;

        const float rightEdge = ImGui::GetCursorScreenPos().x +
                                ImGui::GetContentRegionAvail().x;
        const float prevRight = ImGui::GetItemRectMax().x;
        const float gap       = ImGui::GetStyle().ItemSpacing.x;

        if (prevRight + gap + w <= rightEdge)
            ImGui::SameLine();
    }

    StatChip(label, value, col);
}

// ############################################################################
// SECTION 1 - SHARED GRAPHICS HELPERS  (paste after StatChipFlow)
// ############################################################################

#include <functional>   // (std::hash comes with <string>; harmless)

struct DonutSlice
{
    const char* label;
    float       value;
    ImVec4      color;
};

// Animated segmented ring. Returns the index of the hovered slice, or -1.
// Reserves a square of (2*radius + 12) at the cursor.
static int DrawDonut(const char* id, const DonutSlice* slices, int n,
                     float radius, float thickness,
                     const char* centerValue, const char* centerLabel)
{
    ImGui::PushID(id);

    const float  box = radius * 2.0f + 12.0f;
    const ImVec2 p   = ImGui::GetCursorScreenPos();
    const ImVec2 c(p.x + box * 0.5f, p.y + box * 0.5f);

    ImGui::InvisibleButton("##donut", ImVec2(box, box));
    const bool hovered = ImGui::IsItemHovered();

    ImDrawList* d = ImGui::GetWindowDrawList();
    const float sweepK = Smooth01(AnimTo(ImGui::GetID("##sweep"), 1.0f, 2.2f, 0.0f));

    float total = 0.0f;
    for (int i = 0; i < n; ++i)
        total += std::max(0.0f, slices[i].value);

    // Which slice is under the mouse? (angle 0 = top, clockwise)
    int hot = -1;

    if (hovered && total > 0.0f)
    {
        const ImVec2 m  = ImGui::GetIO().MousePos;
        const float  dx = m.x - c.x;
        const float  dy = m.y - c.y;
        const float  dist = std::sqrt(dx * dx + dy * dy);

        if (dist >= radius - thickness - 4.0f && dist <= radius + 6.0f)
        {
            float ang = std::atan2(dy, dx) + HIDS_PI * 0.5f;
            if (ang < 0.0f) ang += HIDS_2PI;

            float acc = 0.0f;

            for (int i = 0; i < n; ++i)
            {
                const float span = std::max(0.0f, slices[i].value) / total * HIDS_2PI;

                if (ang >= acc && ang < acc + span) { hot = i; break; }
                acc += span;
            }
        }
    }

    const float ringR = radius - thickness * 0.5f;

    // track
    d->AddCircle(c, ringR, Col(FX_WHITE, 0.06f), 64, thickness);

    int nonZero = 0;
    for (int i = 0; i < n; ++i)
        if (slices[i].value > 0.0f) ++nonZero;

    float start = -HIDS_PI * 0.5f;

    for (int i = 0; i < n; ++i)
    {
        if (slices[i].value <= 0.0f || total <= 0.0f)
            continue;

        const float span = slices[i].value / total * HIDS_2PI * sweepK;
        const float gap  = nonZero > 1 ? 0.05f : 0.0f;
        const float a0   = start + gap * 0.5f;
        const float a1   = start + span - gap * 0.5f;
        const bool  h    = (hot == i);

        if (a1 > a0)
        {
            d->PathClear();
            d->PathArcTo(c, ringR + (h ? 2.0f : 0.0f), a0, a1,
                std::max(6, (int)(span * 14.0f)));
            d->PathStroke(Col(slices[i].color, h ? 1.0f : 0.88f), 0,
                thickness + (h ? 4.0f : 0.0f));
        }

        start += span;
    }

    // centre text (scaled draw-list text, so it works inside any child)
    if (centerValue != nullptr)
    {
        const float  k  = 1.55f;
        const ImVec2 ts = ImGui::CalcTextSize(centerValue);
        d->AddText(ImGui::GetFont(), ImGui::GetFontSize() * k,
            ImVec2(c.x - ts.x * k * 0.5f, c.y - ts.y * k * 0.5f - (centerLabel ? 6.0f : 0.0f)),
            ImGui::GetColorU32(COLOR_TEXT), centerValue);
    }

    if (centerLabel != nullptr)
    {
        const ImVec2 ls = ImGui::CalcTextSize(centerLabel);
        d->AddText(ImVec2(c.x - ls.x * 0.5f, c.y + 12.0f),
            ImGui::GetColorU32(COLOR_MUTED), centerLabel);
    }

    if (hot >= 0)
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        ImGui::SetTooltip("%s: %.0f", slices[hot].label, slices[hot].value);
    }

    ImGui::PopID();
    return hot;
}

// Coloured dot + label + value (used next to donuts)
static void LegendRow(const char* label, const char* value, const ImVec4& col)
{
    const ImVec2 p  = ImGui::GetCursorScreenPos();
    const float  lh = ImGui::GetTextLineHeight();
    ImDrawList*  d  = ImGui::GetWindowDrawList();

    d->AddCircleFilled(ImVec2(p.x + 6.0f, p.y + lh * 0.5f), 7.0f, Col(col, 0.20f), 16);
    d->AddCircleFilled(ImVec2(p.x + 6.0f, p.y + lh * 0.5f), 3.8f, ImGui::GetColorU32(col), 12);

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.0f);
    ImGui::TextUnformatted(label);
    ImGui::SameLine(0.0f, 10.0f);
    ImGui::TextColored(col, "%s", value);
}

// Circular countdown (frac 1 = full, 0 = empty)
static void DrawCountdownRing(const ImVec2& c, float r, float frac,
                              const ImVec4& col, float thick = 4.0f)
{
    ImDrawList* d = ImGui::GetWindowDrawList();

    frac = ClampFloat(frac, 0.0f, 1.0f);
    d->AddCircle(c, r, Col(col, 0.16f), 56, thick);

    if (frac > 0.003f)
    {
        d->PathClear();
        d->PathArcTo(c, r, -HIDS_PI * 0.5f, -HIDS_PI * 0.5f + HIDS_2PI * frac,
            std::max(8, (int)(frac * 56.0f)));
        d->PathStroke(ImGui::GetColorU32(col), 0, thick);
    }
}

// 0 = too short / empty, 1 = weak ... 4 = strong
static int PasswordStrength(const char* pw)
{
    const size_t len = std::strlen(pw);
    bool lo = false, up = false, dg = false, sy = false;

    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char ch = (unsigned char)pw[i];

        if (std::islower(ch))      lo = true;
        else if (std::isupper(ch)) up = true;
        else if (std::isdigit(ch)) dg = true;
        else                       sy = true;
    }

    const int classes = (int)lo + (int)up + (int)dg + (int)sy;

    if (len < 6)                       return 0;
    if (len >= 12 && classes >= 4)     return 4;
    if (len >= 10 && classes >= 3)     return 3;
    if (len >= 8  && classes >= 2)     return 2;
    return 1;
}

// Four-segment strength bar with a label underneath
static void PasswordStrengthMeter(const char* pw, float width)
{
    const int    s    = PasswordStrength(pw);
    const bool   any  = pw[0] != '\0';
    const ImVec4 tones[5] = { COLOR_RED, COLOR_RED, COLOR_ORANGE, COLOR_YELLOW, COLOR_GREEN };
    const char*  names[5] = { "Too short", "Weak", "Fair", "Good", "Strong" };
    const ImVec4 tone = any ? tones[s] : COLOR_MUTED;

    const ImVec2 p = ImGui::GetCursorScreenPos();
    const float  seg = (width - 3.0f * 5.0f) / 4.0f;
    ImDrawList*  d = ImGui::GetWindowDrawList();

    for (int i = 0; i < 4; ++i)
    {
        const float x = p.x + (float)i * (seg + 5.0f);
        const bool  lit = any && (i < s || (s == 0 && i == 0));

        d->AddRectFilled(ImVec2(x, p.y), ImVec2(x + seg, p.y + 5.0f),
            lit ? Col(tone, 0.95f) : Col(FX_WHITE, 0.08f), 2.5f);
    }

    ImGui::Dummy(ImVec2(width, 6.0f));

    if (any)
        ImGui::TextColored(tone, "%s", names[s]);
    else
        ImGui::TextDisabled("Strength");
}

// "...tail of a very long path" / "start of a long name..."
static std::string ElideFront(const std::string& s, float maxW)
{
    if (ImGui::CalcTextSize(s.c_str()).x <= maxW)
        return s;

    std::string t = s;

    while (t.size() > 4 && ImGui::CalcTextSize(("..." + t).c_str()).x > maxW)
        t.erase(0, 1);

    return "..." + t;
}

static std::string ElideBack(const std::string& s, float maxW)
{
    if (ImGui::CalcTextSize(s.c_str()).x <= maxW)
        return s;

    std::string t = s;

    while (t.size() > 4 && ImGui::CalcTextSize((t + "...").c_str()).x > maxW)
        t.pop_back();

    return t + "...";
}

// Big selectable card for the scan type (0 quick, 1 full, 2 custom)
static bool ScanModeCard(int kind, const char* title, const char* desc,
                         bool selected, const ImVec4& accent, const ImVec2& size)
{
    ImGui::PushID(kind);

    const ImVec2 p = ImGui::GetCursorScreenPos();
    const ImVec2 b(p.x + size.x, p.y + size.y);

    const bool clicked = ImGui::InvisibleButton("##mode", size);
    const bool hov     = ImGui::IsItemHovered();

    const float hv  = AnimTo(ImGui::GetID("##hv"),  hov ? 1.0f : 0.0f, 10.0f);
    const float sel = AnimTo(ImGui::GetID("##sel"), selected ? 1.0f : 0.0f, 10.0f);

    ImDrawList* d = ImGui::GetWindowDrawList();
    const float t = (float)ImGui::GetTime();

    d->AddRectFilled(p, b, Col(accent, 0.05f + 0.10f * sel + 0.04f * hv), 12.0f);
    d->AddRectFilledMultiColor(Off(p, 2.0f, 2.0f), ImVec2(b.x - 2.0f, p.y + size.y * 0.5f),
        Col(FX_WHITE, 0.04f), Col(FX_WHITE, 0.01f), Col(FX_WHITE, 0.0f), Col(FX_WHITE, 0.0f));
    d->AddRect(p, b, Col(accent, 0.20f + 0.65f * sel + 0.20f * hv), 12.0f, 0, 1.0f + sel);

    const ImVec2 ic(p.x + 38.0f, p.y + size.y * 0.5f);

    if (sel > 0.02f)
        DrawGlow(d, ic, 46.0f * sel, accent);

    d->AddCircleFilled(ic, 21.0f, Col(accent, 0.14f + 0.14f * sel), 40);
    d->AddCircle(ic, 21.0f, Col(accent, 0.45f + 0.45f * sel), 40, 1.5f);

    if (selected)
    {
        const float a = t * 1.5f;
        d->AddCircleFilled(ImVec2(ic.x + std::cos(a) * 21.0f, ic.y + std::sin(a) * 21.0f),
            2.6f, Col(accent, 0.9f), 12);
    }

    const ImU32 iu = ImGui::GetColorU32(accent);

    if (kind == 0)          // lightning bolt
    {
        const ImVec2 bolt[4] = { Off(ic, 3.0f, -10.0f), Off(ic, -5.0f, 1.0f),
                                 Off(ic, 2.0f, 1.0f),   Off(ic, -3.0f, 11.0f) };
        d->AddPolyline(bolt, 4, iu, 0, 2.6f);
    }
    else if (kind == 1)     // drive / disc
    {
        d->AddCircle(ic, 10.0f, iu, 24, 2.0f);
        d->AddCircleFilled(ic, 3.0f, iu, 12);
        d->AddLine(ic, Off(ic, 7.0f, -6.0f), iu, 2.0f);
    }
    else                    // folder
    {
        d->AddRectFilled(Off(ic, -10.0f, -8.0f), Off(ic, -1.0f, -3.0f), Col(accent, 0.5f), 2.0f);
        d->AddRect(Off(ic, -10.0f, -5.0f), Off(ic, 10.0f, 8.0f), iu, 3.0f, 0, 2.0f);
    }

    // texts (clipped to the card)
    const float tx = p.x + 74.0f;
    const float tw = size.x - 74.0f - 36.0f;

    d->PushClipRect(p, b, true);
    d->AddText(ImGui::GetFont(), ImGui::GetFontSize() * 1.12f, ImVec2(tx, p.y + 13.0f),
        ImGui::GetColorU32(COLOR_TEXT), title);
    d->AddText(ImGui::GetFont(), ImGui::GetFontSize() * 0.92f,
        ImVec2(tx, p.y + 13.0f + ImGui::GetTextLineHeight() * 1.25f + 3.0f),
        ImGui::GetColorU32(COLOR_MUTED), desc, nullptr, tw);
    d->PopClipRect();

    // selected check mark
    if (sel > 0.02f)
    {
        const ImVec2 ck(b.x - 22.0f, p.y + 22.0f);

        d->AddCircleFilled(ck, 10.0f, Col(accent, 0.90f * sel), 24);

        const ImVec2 tick[3] = { Off(ck, -4.0f, 0.0f), Off(ck, -1.0f, 3.5f), Off(ck, 4.5f, -3.5f) };
        d->AddPolyline(tick, 3, IM_COL32(8, 16, 28, 255), 0, 2.0f);
    }

    if (hov && !ImGui::IsItemActive())
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    ImGui::PopID();
    return clicked;
}

// Scan radar with live blips for every finding. Reserves 2*(radius+12) square.
static void DrawScanRadar(float radius, const ImVec4& color, bool active, bool done,
                          const std::vector<ScanFinding>& findings)
{
    const float  pad = 12.0f;
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    const ImVec2 c(pos.x + radius + pad, pos.y + radius + pad);
    ImDrawList*  d = ImGui::GetWindowDrawList();
    const float  t = (float)ImGui::GetTime();

    DrawGlow(d, c, radius * 1.9f, color);
    d->AddCircleFilled(c, radius, Col(color, 0.08f), 64);

    for (int k = 1; k <= 4; ++k)
        d->AddCircle(c, radius * (float)k / 4.0f, Col(color, k == 4 ? 0.75f : 0.20f), 64,
            k == 4 ? 2.0f : 1.0f);

    d->AddLine(Off(c, -radius, 0.0f), Off(c, radius, 0.0f), Col(color, 0.16f), 1.0f);
    d->AddLine(Off(c, 0.0f, -radius), Off(c, 0.0f, radius), Col(color, 0.16f), 1.0f);

    // outer tick ring (turns while scanning)
    for (int i = 0; i < 60; ++i)
    {
        const float a  = (float)i * HIDS_2PI / 60.0f + (active ? t * 0.25f : 0.0f);
        const bool  mj = (i % 5 == 0);
        const float r0 = radius + 5.0f;
        const float r1 = radius + (mj ? 11.0f : 8.0f);

        d->AddLine(ImVec2(c.x + std::cos(a) * r0, c.y + std::sin(a) * r0),
                   ImVec2(c.x + std::cos(a) * r1, c.y + std::sin(a) * r1),
                   Col(color, mj ? 0.55f : 0.22f), 1.0f);
    }

    if (active)
    {
        const float sweep = t * 3.6f;

        for (int k = 0; k < 18; ++k)
        {
            const float a0 = sweep - (float)k * 0.09f;
            const float a1 = a0 - 0.09f;
            const float f  = 1.0f - (float)k / 18.0f;

            d->AddTriangleFilled(c,
                ImVec2(c.x + std::cos(a0) * radius, c.y + std::sin(a0) * radius),
                ImVec2(c.x + std::cos(a1) * radius, c.y + std::sin(a1) * radius),
                Col(color, 0.42f * f * f));
        }

        d->AddLine(c, ImVec2(c.x + std::cos(sweep) * radius, c.y + std::sin(sweep) * radius),
            ImGui::GetColorU32(color), 1.8f);

        const float pulse = 0.75f + 0.25f * std::sin(t * 4.0f);
        d->AddCircleFilled(c, radius * 0.10f * pulse, Col(color, 0.95f), 16);
    }
    else if (done && findings.empty())
    {
        const ImVec2 tick[3] = {
            ImVec2(c.x - radius * 0.30f, c.y + radius * 0.02f),
            ImVec2(c.x - radius * 0.08f, c.y + radius * 0.26f),
            ImVec2(c.x + radius * 0.34f, c.y - radius * 0.26f)
        };
        d->AddPolyline(tick, 3, ImGui::GetColorU32(color), 0, 3.4f);
    }
    else if (!done)
    {
        d->AddCircleFilled(c, radius * 0.09f, Col(color, 0.8f), 16);
    }

    // one blip per finding (stable position from the file path)
    const std::size_t n = std::min<std::size_t>(findings.size(), 60);

    for (std::size_t i = 0; i < n; ++i)
    {
        const std::size_t h   = std::hash<std::string>()(findings[i].path);
        const float       ang = (float)(h % 3600u) / 3600.0f * HIDS_2PI;
        const float       rad = radius * (0.28f + 0.64f * (float)((h / 3600u) % 1000u) / 1000.0f);
        const ImVec2      bp(c.x + std::cos(ang) * rad, c.y + std::sin(ang) * rad);
        const ImVec4      bc = (findings[i].level == "Malicious") ? COLOR_RED : COLOR_YELLOW;
        const float       pl = 0.5f + 0.5f * std::sin(t * 3.0f + (float)i);

        d->AddCircleFilled(bp, 3.2f, Col(bc, 0.95f), 12);
        d->AddCircle(bp, 5.0f + 5.0f * pl, Col(bc, 0.45f * (1.0f - pl)), 20, 1.3f);
    }

    ImGui::Dummy(ImVec2((radius + pad) * 2.0f, (radius + pad) * 2.0f));
}


// Friendly animated graphic for pages / tables with nothing to show
static void EmptyState(const char* title, const char* hint, const ImVec4& col)
{
    const float avail = ImGui::GetContentRegionAvail().x;
    const float t     = (float)ImGui::GetTime();

    ImGui::Dummy(ImVec2(0.0f, 26.0f));

    const ImVec2 p = ImGui::GetCursorScreenPos();
    const ImVec2 c(p.x + avail * 0.5f, p.y + 46.0f);
    ImDrawList*  d = ImGui::GetWindowDrawList();

    const float phase  = std::fmod(t * 0.6f, 1.0f);
    const float phase2 = std::fmod(t * 0.6f + 0.5f, 1.0f);

    DrawGlow(d, c, 78.0f, col);
    d->AddCircleFilled(c, 36.0f, Col(col, 0.12f), 48);
    d->AddCircle(c, 36.0f, Col(col, 0.70f), 48, 1.8f);
    d->AddCircle(c, 36.0f + phase * 22.0f,  Col(col, 0.35f * (1.0f - phase)),  48, 1.4f);
    d->AddCircle(c, 36.0f + phase2 * 22.0f, Col(col, 0.25f * (1.0f - phase2)), 48, 1.2f);

    // little orbiting satellites
    for (int i = 0; i < 3; ++i)
    {
        const float a = t * 0.9f + (float)i * (HIDS_2PI / 3.0f);

        d->AddCircleFilled(
            ImVec2(c.x + std::cos(a) * 50.0f, c.y + std::sin(a) * 50.0f),
            2.6f, Col(col, 0.75f), 12);
    }

    ImVec2 tick[3] = {
        ImVec2(c.x - 12.0f, c.y + 1.0f),
        ImVec2(c.x - 4.0f,  c.y + 9.0f),
        ImVec2(c.x + 13.0f, c.y - 9.0f)
    };
    d->AddPolyline(tick, 3, ImGui::GetColorU32(col), 0, 3.2f);

    ImGui::Dummy(ImVec2(avail, 108.0f));

    CenteredInContent(title, 1.25f, COLOR_TEXT);
    CenteredInContent(hint, 1.0f, COLOR_MUTED);
}

// Panel used by every inner page: glass card with shadow, a coloured top
// line and a soft gradient. Always pair with ImGui::EndChild().
static void BeginPanel(const char* id, const ImVec2& size,
                       const ImVec4& accent, bool noScroll = false)
{
    // Resolve the real size so the shadow can be drawn first
    ImVec2 sz = size;
    const ImVec2 av = ImGui::GetContentRegionAvail();

    if (sz.x <= 0.0f) sz.x = std::max(10.0f, av.x + sz.x);
    if (sz.y <= 0.0f) sz.y = std::max(10.0f, av.y + sz.y);

    const ImVec2 cp = ImGui::GetCursorScreenPos();

    DrawSoftShadow(ImGui::GetWindowDrawList(), cp, ImVec2(cp.x + sz.x, cp.y + sz.y),
        ImGui::GetStyle().ChildRounding, 14.0f, 0.30f);

    ImGui::BeginChild(id, size, true,
        noScroll ? ImGuiWindowFlags_NoScrollbar : 0);

    const ImVec2 wp = ImGui::GetWindowPos();
    const ImVec2 ws = ImGui::GetWindowSize();
    ImDrawList*  d  = ImGui::GetWindowDrawList();

    d->AddRectFilledMultiColor(
        ImVec2(wp.x + 1.0f, wp.y + 3.0f),
        ImVec2(wp.x + ws.x - 1.0f, wp.y + std::min(ws.y, 170.0f)),
        Col(accent, 0.10f),
        Col(accent, 0.10f),
        Col(accent, 0.0f),
        Col(accent, 0.0f));

    DrawCardChrome(d, wp, ws, accent, 0.0f);

    // glowing top line (fades toward the right)
    d->AddRectFilledMultiColor(
        ImVec2(wp.x + 16.0f, wp.y),
        ImVec2(wp.x + ws.x - 16.0f, wp.y + 2.5f),
        Col(accent, 0.95f), Col(accent, 0.25f), Col(accent, 0.25f), Col(accent, 0.95f));

    d->AddRectFilledMultiColor(
        ImVec2(wp.x + 16.0f, wp.y + 2.5f),
        ImVec2(wp.x + ws.x * 0.55f, wp.y + 9.0f),
        Col(accent, 0.22f), Col(accent, 0.0f), Col(accent, 0.0f), Col(accent, 0.0f));
}

// Logo tile + product name. Returns true when clicked (-> welcome screen).
static bool DrawBrand(bool clickable = false)
{
    ImDrawList* d = ImGui::GetWindowDrawList();
    const ImVec2 p = ImGui::GetCursorScreenPos();
    const float  s = 40.0f;
    const float  t = (float)ImGui::GetTime();

    DrawGlow(d, ImVec2(p.x + s * 0.5f, p.y + s * 0.5f), 50.0f, COLOR_CYAN);

    d->AddRectFilledMultiColor(p, ImVec2(p.x + s, p.y + s),
        Col(COLOR_CYAN, 0.30f), Col(COLOR_CYAN, 0.10f),
        Col(COLOR_CYAN, 0.06f), Col(COLOR_CYAN, 0.18f));
    d->AddRect(p, ImVec2(p.x + s, p.y + s),
        Col(COLOR_CYAN, 0.65f), 10.0f, 0, 1.2f);

    const ImVec2 c(p.x + s * 0.5f, p.y + s * 0.5f + 0.5f);
    DrawShieldLogo(d, c, 11.0f, COLOR_CYAN);

    // slow scanning arc around the tile
    d->PathClear();
    d->PathArcTo(c, s * 0.62f, t * 1.4f, t * 1.4f + 0.9f, 16);
    d->PathStroke(Col(COLOR_CYAN, 0.55f), 0, 1.6f);

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

// Right-aligned engine status + Start/Stop button on a glass capsule
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

    // capsule behind the status + button
    {
        const ImVec2 p = ImGui::GetCursorScreenPos();
        const float  h = ImGui::GetFrameHeight();
        ImDrawList*  d = ImGui::GetWindowDrawList();

        const ImVec2 a(p.x - 14.0f, p.y - 6.0f);
        const ImVec2 b(p.x + total + 8.0f, p.y + h + 6.0f);

        d->AddRectFilled(a, b, Col(col, 0.07f), (b.y - a.y) * 0.5f);
        d->AddRect(a, b, Col(col, 0.30f), (b.y - a.y) * 0.5f);
    }

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

// Drifting aurora glows, a reactive dot grid, a cursor halo, a vignette
// and (while protection runs) a slow scan band behind every screen
static void DrawAmbient()
{
    ImDrawList*  bg   = ImGui::GetBackgroundDrawList();
    const ImVec2 disp = ImGui::GetIO().DisplaySize;
    const float  t    = (float)ImGui::GetTime();

    const ImVec4 a = monitoringActive ? COLOR_GREEN : COLOR_CYAN;

    DrawGlow(bg,
        ImVec2(disp.x * (0.12f + 0.04f * std::sin(t * 0.20f)),
               disp.y * (0.05f + 0.03f * std::cos(t * 0.13f))),
        380.0f, WithAlpha(a, 0.55f));

    DrawGlow(bg,
        ImVec2(disp.x * (0.90f + 0.03f * std::sin(t * 0.17f + 1.0f)), disp.y * 0.95f),
        440.0f, WithAlpha(COLOR_BLUE, 0.50f));

    DrawGlow(bg,
        ImVec2(disp.x * (0.55f + 0.10f * std::sin(t * 0.11f + 2.0f)),
               disp.y * (0.45f + 0.08f * std::cos(t * 0.09f))),
        300.0f, WithAlpha(monitoringActive ? COLOR_CYAN : COLOR_ORANGE, 0.16f));

    const bool  mouseOk = ImGui::IsMousePosValid();
    const ImVec2 m      = ImGui::GetIO().MousePos;

    if (mouseOk)
        DrawGlow(bg, m, 150.0f, WithAlpha(COLOR_CYAN, 0.16f));

    if (appScreen != SCREEN_WELCOME)
    {
        const float step = 46.0f;

        for (float y = step * 0.5f; y < disp.y; y += step)
        {
            for (float x = step * 0.5f; x < disp.x; x += step)
            {
                float lit = 0.0f;

                if (mouseOk)
                {
                    const float dx = x - m.x;
                    const float dy = y - m.y;
                    const float dist = std::sqrt(dx * dx + dy * dy);
                    lit = std::max(0.0f, 1.0f - dist / 230.0f);
                }

                const float sz = 2.0f + lit * 1.5f;

                bg->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz),
                    Col(COLOR_CYAN, 0.045f + 0.20f * lit * lit));
            }
        }
    }

    // slow scan band while protection is running
    if (monitoringActive)
    {
        const float ph   = std::fmod(t * 0.09f, 1.0f);
        const float y    = -120.0f + (disp.y + 240.0f) * ph;
        const float half = 60.0f;

        bg->AddRectFilledMultiColor(ImVec2(0.0f, y - half), ImVec2(disp.x, y),
            Col(COLOR_GREEN, 0.0f), Col(COLOR_GREEN, 0.0f),
            Col(COLOR_GREEN, 0.045f), Col(COLOR_GREEN, 0.045f));
        bg->AddRectFilledMultiColor(ImVec2(0.0f, y), ImVec2(disp.x, y + half),
            Col(COLOR_GREEN, 0.045f), Col(COLOR_GREEN, 0.045f),
            Col(COLOR_GREEN, 0.0f), Col(COLOR_GREEN, 0.0f));
    }

    // Vignette
    {
        const ImU32 dark = IM_COL32(0, 0, 0, 90);
        const ImU32 none = IM_COL32(0, 0, 0, 0);
        const float vh = disp.y * 0.16f;
        const float vw = disp.x * 0.10f;

        bg->AddRectFilledMultiColor(ImVec2(0, 0), ImVec2(disp.x, vh), dark, dark, none, none);
        bg->AddRectFilledMultiColor(ImVec2(0, disp.y - vh), ImVec2(disp.x, disp.y), none, none, dark, dark);
        bg->AddRectFilledMultiColor(ImVec2(0, 0), ImVec2(vw, disp.y), dark, none, none, dark);
        bg->AddRectFilledMultiColor(ImVec2(disp.x - vw, 0), ImVec2(disp.x, disp.y), none, dark, dark, none);
    }
}

// Header of a feature page: Back button, breadcrumb, icon, title.
// No sidebar - just this feature and the way back.
static void DrawPageHeader()
{
    const int    domain = DomainOfPage(selectedPage);
    const ImVec4 accent = DomainAccent(domain);
    const float  t      = (float)ImGui::GetTime();

    DrawSoftShadow(ImGui::GetWindowDrawList(), ImGui::GetCursorScreenPos(),
        ImVec2(ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x,
               ImGui::GetCursorScreenPos().y + 134.0f),
        ImGui::GetStyle().ChildRounding, 14.0f, 0.30f);

    ImGui::BeginChild("PageHeader", ImVec2(0.0f, 134.0f), true,
                      ImGuiWindowFlags_NoScrollbar);

    const ImVec2 wp = ImGui::GetWindowPos();
    const ImVec2 ws = ImGui::GetWindowSize();
    ImDrawList*  d  = ImGui::GetWindowDrawList();

    // Card background: gradient + accent line along the bottom
    d->AddRectFilledMultiColor(
        ImVec2(wp.x + 1.0f, wp.y + 1.0f), ImVec2(wp.x + ws.x - 1.0f, wp.y + ws.y - 1.0f),
        Col(accent, 0.18f),
        Col(accent, 0.03f),
        Col(accent, 0.0f),
        Col(accent, 0.06f));

    // Big faint watermark of the page icon on the right
    DrawPageIcon(d, ImVec2(wp.x + ws.x - 130.0f, wp.y + ws.y * 0.55f), 46.0f,
        selectedPage, Col(accent, 0.07f));

    DrawCardChrome(d, wp, ws, accent, 0.0f);

    d->AddRectFilledMultiColor(
        ImVec2(wp.x + 16.0f, wp.y + ws.y - 3.0f),
        ImVec2(wp.x + ws.x - 16.0f, wp.y + ws.y - 1.0f),
        Col(accent, 0.90f),
        Col(accent, 0.0f),
        Col(accent, 0.0f),
        Col(accent, 0.90f));

    // Light comet travelling along the bottom line
    {
        const float x0  = wp.x + 16.0f;
        const float x1  = wp.x + ws.x - 16.0f;
        const float pos = x0 + (x1 - x0) * std::fmod(t * 0.16f, 1.0f);
        const float a   = std::max(x0, pos - 150.0f);

        if (pos > a)
            d->AddRectFilledMultiColor(
                ImVec2(a, wp.y + ws.y - 3.5f), ImVec2(pos, wp.y + ws.y - 0.5f),
                Col(FX_WHITE, 0.0f), Col(FX_WHITE, 0.90f),
                Col(FX_WHITE, 0.90f), Col(FX_WHITE, 0.0f));
    }

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

        DrawGlow(d, ic, 56.0f, accent);
        d->AddCircleFilled(ic, 28.0f, Col(accent, 0.18f), 48);
        d->AddCircle(ic, 28.0f, Col(accent, 0.80f), 48, 1.6f);

        // two rotating arcs around the icon
        d->PathClear();
        d->PathArcTo(ic, 34.0f, t * 1.2f, t * 1.2f + 1.3f, 20);
        d->PathStroke(Col(accent, 0.85f), 0, 2.2f);

        d->PathClear();
        d->PathArcTo(ic, 34.0f, t * 1.2f + HIDS_PI, t * 1.2f + HIDS_PI + 0.7f, 12);
        d->PathStroke(Col(accent, 0.45f), 0, 2.2f);

        DrawPageIcon(d, ic, 13.0f, selectedPage, ImGui::GetColorU32(accent));
    }

    ImGui::SetCursorPos(ImVec2(padX + 82.0f, topY + 46.0f));
    ImGui::BeginGroup();
    ImGui::SetWindowFontScale(1.6f);
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
    const float t    = (float)ImGui::GetTime();

    BeginPanel("EngineBanner", ImVec2(0.0f, 104.0f), COLOR_YELLOW, true);

    const ImVec2 wp = ImGui::GetWindowPos();
    ImDrawList*  d  = ImGui::GetWindowDrawList();

    // warning beacon
    {
        const ImVec2 c(wp.x + 48.0f, wp.y + 52.0f);
        const float  ph = std::fmod(t * 0.8f, 1.0f);

        DrawGlow(d, c, 58.0f, COLOR_YELLOW);
        d->AddCircleFilled(c, 28.0f, Col(COLOR_YELLOW, 0.14f), 40);
        d->AddCircle(c, 28.0f, Col(COLOR_YELLOW, 0.70f), 40, 1.6f);
        d->AddCircle(c, 28.0f + ph * 16.0f, Col(COLOR_YELLOW, 0.40f * (1.0f - ph)), 40, 1.4f);

        d->AddTriangleFilled(Off(c, 0.0f, -12.0f), Off(c, -12.0f, 10.0f), Off(c, 12.0f, 10.0f),
            ImGui::GetColorU32(COLOR_YELLOW));
        d->AddText(Off(c, -2.5f, -6.0f), IM_COL32(8, 16, 28, 255), "!");
    }

    const float wrapX = ImGui::GetWindowWidth() - btnW - 60.0f;

    ImGui::SetCursorPos(ImVec2(96.0f, 16.0f));
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

    DrawGlow(d, c, 68.0f, col);
    d->AddCircleFilled(c, 34.0f, Col(col, 0.14f), 48);
    d->AddCircle(c, 34.0f, Col(col, 0.55f), 48, 1.4f);

    {
        const float ph = std::fmod(t * 0.7f, 1.0f);
        d->AddCircle(c, 34.0f + ph * 12.0f, Col(col, 0.35f * (1.0f - ph)), 48, 1.2f);
    }

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

    const float rightX1 = std::max(240.0f, ws.x - ImGui::CalcTextSize(l1).x - 24.0f);
    const float rightX2 = std::max(240.0f, ws.x - ImGui::CalcTextSize(l2).x - 24.0f);

    // Heartbeat line between the level text and the right-hand stats
    {
        const float x0 = wp.x + 290.0f;
        const float x1 = wp.x + std::min(rightX1, rightX2) - 30.0f;

        if (x1 - x0 > 120.0f)
        {
            const int   n  = 70;
            const float cy = wp.y + 50.0f;
            ImVec2      prev(x0, cy);

            for (int i = 1; i <= n; ++i)
            {
                const float u  = (float)i / (float)n;
                const float ph = std::fmod(u * 2.4f - t * 0.55f + 10.0f, 1.0f);

                const float y =
                    -26.0f * std::exp(-std::pow((ph - 0.50f) * 22.0f, 2.0f)) +
                     10.0f * std::exp(-std::pow((ph - 0.56f) * 26.0f, 2.0f)) +
                      4.0f * std::exp(-std::pow((ph - 0.30f) * 10.0f, 2.0f));

                const ImVec2 cur(x0 + (x1 - x0) * u, cy + y);
                const float  fade = std::sin(u * HIDS_PI);

                d->AddLine(prev, cur, Col(col, 0.75f * fade), 1.8f);
                prev = cur;
            }
        }
    }

    ImGui::SetCursorPos(ImVec2(rightX1, 26.0f));
    ImGui::TextColored(COLOR_GREEN, "%s", l1);

    ImGui::SetCursorPos(ImVec2(rightX2, 52.0f));
    ImGui::TextDisabled("%s", l2);

    ImGui::EndChild();
    ImGui::Spacing();
}

// ------------------------------------------------------------
// WELCOME SCREEN
// Reachable at any time. Shows whether HIDS is working.
// ------------------------------------------------------------

// Glass stat tile used on the welcome screen
static void DrawMiniStat(const char* label, const char* value,
                         const ImVec4& col, float w)
{
    const float  h = 60.0f;
    const ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList*  d = ImGui::GetWindowDrawList();

    d->AddRectFilled(p, ImVec2(p.x + w, p.y + h), Col(col, 0.08f), 12.0f);
    d->AddRectFilledMultiColor(Off(p, 2.0f, 2.0f), ImVec2(p.x + w - 2.0f, p.y + h * 0.5f),
        Col(FX_WHITE, 0.05f), Col(FX_WHITE, 0.02f), Col(FX_WHITE, 0.0f), Col(FX_WHITE, 0.0f));
    d->AddRect(p, ImVec2(p.x + w, p.y + h), Col(col, 0.40f), 12.0f);
    d->AddRectFilled(Off(p, 14.0f, 0.0f), ImVec2(p.x + w - 14.0f, p.y + 2.0f),
        Col(col, 0.85f), 1.0f);

    ImGui::SetCursorScreenPos(Off(p, 14.0f, 8.0f));
    ImGui::TextDisabled("%s", label);

    ImGui::SetWindowFontScale(1.3f);
    ImGui::SetCursorScreenPos(Off(p, 14.0f, 26.0f));
    ImGui::TextColored(col, "%s", value);
    ImGui::SetWindowFontScale(1.0f);

    ImGui::SetCursorScreenPos(p);
    ImGui::Dummy(ImVec2(w, h));
}

// Row of the five security areas at the bottom of the welcome screen
static void DrawDomainChips()
{
    const int   n      = IM_ARRAYSIZE(hidsDomains);
    const float itemW  = std::min(150.0f, (ImGui::GetWindowWidth() - 40.0f) / (float)n);
    const float total  = itemW * (float)n;

    ImGui::SetCursorPosX(std::max(0.0f, (ImGui::GetWindowWidth() - total) * 0.5f));

    const ImVec2 p0 = ImGui::GetCursorScreenPos();
    ImDrawList*  d  = ImGui::GetWindowDrawList();

    for (int i = 0; i < n; ++i)
    {
        const ImVec4 acc = DomainAccent(i);
        const ImVec2 c(p0.x + itemW * ((float)i + 0.5f), p0.y + 20.0f);

        if (i + 1 < n)
            d->AddLine(Off(c, 24.0f, 0.0f), Off(c, itemW - 24.0f, 0.0f), Col(COLOR_CYAN, 0.14f), 1.0f);

        d->AddCircleFilled(c, 18.0f, Col(acc, 0.14f), 32);
        d->AddCircle(c, 18.0f, Col(acc, 0.60f), 32, 1.3f);
        DrawPageIcon(d, c, 8.0f, domainIconPage[i], ImGui::GetColorU32(acc));

        const ImVec2 ts = ImGui::CalcTextSize(hidsDomains[i].title);
        d->AddText(ImVec2(c.x - ts.x * 0.5f, p0.y + 44.0f),
            ImGui::GetColorU32(WithAlpha(COLOR_MUTED, 0.9f)), hidsDomains[i].title);
    }

    ImGui::Dummy(ImVec2(total, 66.0f));
}

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
    for (int i = 0; i < 56; ++i)
    {
        const float fx = std::fmod(i * 0.6180339f, 1.0f);
        const float fy = std::fmod(i * 0.3819660f + t * (0.006f + 0.004f * (i % 4)), 1.0f);
        const float a  = 0.08f + 0.08f * (0.5f + 0.5f * std::sin(t * 0.8f + i));

        bg->AddCircleFilled(ImVec2(fx * disp.x, fy * disp.y), 1.2f + (i % 3),
            Col(acc, a));
    }

    // Soft glow
    DrawGlow(bg, c, 190.0f, acc);

    // Radar grid
    for (int k = 1; k <= 4; ++k)
        bg->AddCircle(c, 70.0f + (float)k * 62.0f, Col(acc, 0.065f), 120, 1.0f);

    bg->AddLine(ImVec2(c.x - 320.0f, c.y), ImVec2(c.x + 320.0f, c.y), Col(acc, 0.05f), 1.0f);
    bg->AddLine(ImVec2(c.x, c.y - 320.0f), ImVec2(c.x, c.y + 320.0f), Col(acc, 0.05f), 1.0f);

    // Radar sweep with a fading trail
    {
        const float sweep = t * 0.9f;
        const float R     = 318.0f;
        const float step  = 0.04f;

        for (int k = 0; k < 30; ++k)
        {
            const float a0 = sweep - (float)k * step;
            const float a1 = a0 - step;
            const float f  = 1.0f - (float)k / 30.0f;

            bg->AddTriangleFilled(c,
                ImVec2(c.x + std::cos(a0) * R, c.y + std::sin(a0) * R),
                ImVec2(c.x + std::cos(a1) * R, c.y + std::sin(a1) * R),
                Col(acc, 0.17f * f * f));
        }

        bg->AddLine(c, ImVec2(c.x + std::cos(sweep) * R, c.y + std::sin(sweep) * R),
            Col(acc, 0.40f), 1.5f);

        // Blips that flash as the sweep passes
        for (int i = 0; i < 7; ++i)
        {
            const float ba = (float)i * 2.399f;
            const float br = 96.0f + std::fmod((float)i * 53.0f, 210.0f);

            float diff = std::fmod(sweep - ba, HIDS_2PI);
            if (diff < 0.0f)
                diff += HIDS_2PI;

            const float lit = std::max(0.0f, 1.0f - diff / 2.6f);

            if (lit > 0.02f)
            {
                const ImVec2 bp(c.x + std::cos(ba) * br, c.y + std::sin(ba) * br);

                bg->AddCircleFilled(bp, 2.5f + 2.0f * lit, Col(acc, 0.75f * lit), 16);
                bg->AddCircle(bp, 6.0f + 12.0f * (1.0f - lit), Col(acc, 0.35f * lit), 24, 1.2f);
            }
        }
    }

    // Expanding radar rings
    for (int k = 0; k < 3; ++k)
    {
        const float phase  = std::fmod(t * 0.30f + k / 3.0f, 1.0f);
        const float radius = 66.0f + phase * 190.0f;

        bg->AddCircle(c, radius,
            Col(acc, 0.28f * (1.0f - phase)), 96, 1.6f);
    }

    // Logo badge + rotating arcs
    bg->AddCircleFilled(c, 64.0f, Col(acc, 0.12f), 64);
    bg->AddCircle(c, 64.0f, Col(acc, 0.55f), 64, 1.5f);

    bg->PathClear();
    bg->PathArcTo(c, 72.0f, t * 1.6f, t * 1.6f + 1.9f, 32);
    bg->PathStroke(ImGui::GetColorU32(acc), 0, 3.0f);

    bg->PathClear();
    bg->PathArcTo(c, 72.0f, t * 1.6f + HIDS_PI, t * 1.6f + HIDS_PI + 0.8f, 20);
    bg->PathStroke(Col(acc, 0.45f), 0, 3.0f);

    DrawShieldLogo(bg, c, 30.0f, acc);

    // Text block
    ImGui::SetCursorPosY(c.y + 100.0f);

    CenteredTitle("HIDS", 3.0f, COLOR_TEXT);
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
            Col(COLOR_GREEN, 0.14f), pillH * 0.5f);
        d->AddRect(p, ImVec2(p.x + pillW, p.y + pillH),
            Col(COLOR_GREEN, 0.70f), pillH * 0.5f, 0, 1.4f);
        d->AddCircleFilled(ImVec2(p.x + 20.0f, p.y + pillH * 0.5f),
            9.0f + wave * 2.0f,
            Col(COLOR_GREEN, 0.20f * (1.0f - wave) + 0.05f));
        d->AddCircleFilled(ImVec2(p.x + 20.0f, p.y + pillH * 0.5f), 5.0f,
            ImGui::GetColorU32(COLOR_GREEN));
        d->AddText(ImVec2(p.x + 36.0f, p.y + (pillH - ImGui::GetTextLineHeight()) * 0.5f),
            ImGui::GetColorU32(COLOR_GREEN), label);

        ImGui::Dummy(ImVec2(pillW, pillH));
        ImGui::Dummy(ImVec2(0.0f, 6.0f));

        // Live stat tiles
        {
            const float gap   = 12.0f;
            const int   count = 5;
            const float tileW = std::min(150.0f,
                (ImGui::GetWindowWidth() - 60.0f - gap * (count - 1)) / (float)count);
            const float rowW  = tileW * count + gap * (count - 1);

            char cpu[16], ram[16], proc[16], alr[16];
            std::snprintf(cpu,  sizeof(cpu),  "%.0f%%", cpuNow);
            std::snprintf(ram,  sizeof(ram),  "%.0f%%", ramNow);
            std::snprintf(proc, sizeof(proc), "%d", processCount);
            std::snprintf(alr,  sizeof(alr),  "%d", (int)alerts.size());

            const std::string up = FormatDuration(now - engineStartedAt);

            ImGui::SetCursorPosX(std::max(0.0f, (ImGui::GetWindowWidth() - rowW) * 0.5f));

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(gap, gap));

            DrawMiniStat("CPU", cpu, LoadColor(cpuNow), tileW);
            ImGui::SameLine();
            DrawMiniStat("RAM", ram, LoadColor(ramNow), tileW);
            ImGui::SameLine();
            DrawMiniStat("PROCESSES", proc, COLOR_BLUE, tileW);
            ImGui::SameLine();
            DrawMiniStat("ALERTS", alr,
                CountSeverity("Critical") > 0 ? COLOR_RED : COLOR_YELLOW, tileW);
            ImGui::SameLine();
            DrawMiniStat("UPTIME", up.c_str(), COLOR_GREEN, tileW);

            ImGui::PopStyleVar();
        }
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
            Col(COLOR_GREEN, 0.40f * (1.0f - w) + 0.05f),
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
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

            const ImVec2 a = ImGui::GetItemRectMin();
            const ImVec2 b = ImGui::GetItemRectMax();

            ImGui::GetWindowDrawList()->AddLine(
                ImVec2(a.x, b.y), ImVec2(b.x, b.y), Col(COLOR_MUTED, 0.8f), 1.0f);
        }

        if (ImGui::IsItemClicked())
            openConsole = true;
    }

    if (openConsole)
        appScreen = SCREEN_HUB;

    // Footer: the five areas + version
    {
        const float y = ImGui::GetWindowHeight() - 100.0f;

        if (ImGui::GetCursorPosY() < y)
            ImGui::SetCursorPosY(y);

        DrawDomainChips();
        CenteredText("HIDS Prototype v2.0", 0.9f, WithAlpha(COLOR_MUTED, 0.6f));
    }
}

// ------------------------------------------------------------
// Shared card used by the hub and the domain screens.
// ------------------------------------------------------------
static bool HubCard(
    const char* id, const char* title, const char* desc, const char* footer,
    int iconPage, const ImVec4& accent, const ImVec2& size,
    const char* badge = nullptr, ImVec4 badgeCol = COLOR_CYAN)
{
    bool clicked = false;

    ImGui::PushID(id);

    const ImVec2 cp = ImGui::GetCursorScreenPos();
    DrawSoftShadow(ImGui::GetWindowDrawList(), cp, ImVec2(cp.x + size.x, cp.y + size.y),
        ImGui::GetStyle().ChildRounding, 14.0f, 0.38f);

    ImGui::BeginChild("card", size, true, ImGuiWindowFlags_NoScrollbar);

    const ImVec2 wp = ImGui::GetWindowPos();
    const ImVec2 ws = ImGui::GetWindowSize();
    ImDrawList*  d  = ImGui::GetWindowDrawList();
    const bool   isHov = ImGui::IsWindowHovered();
    const float  hov   = AnimTo(ImGui::GetID("##hv"), isHov ? 1.0f : 0.0f, 10.0f);
    const float  rounding = ImGui::GetStyle().ChildRounding;
    const float  t = (float)ImGui::GetTime();

    // Soft tint fading down from the top
    d->AddRectFilledMultiColor(
        ImVec2(wp.x + 1.0f, wp.y + 4.0f),
        ImVec2(wp.x + ws.x - 1.0f, wp.y + ws.y * 0.62f),
        Col(accent, 0.11f + 0.10f * hov),
        Col(accent, 0.11f + 0.10f * hov),
        Col(accent, 0.0f),
        Col(accent, 0.0f));

    // Decorative rings in the top-right corner (breathe on hover)
    for (int k = 0; k < 3; ++k)
        d->AddCircle(ImVec2(wp.x + ws.x - 26.0f, wp.y + 30.0f),
            34.0f + k * 22.0f + hov * 6.0f * (float)(k + 1),
            Col(accent, 0.10f - k * 0.025f + 0.04f * hov), 64, 1.2f);

    DrawCardChrome(d, wp, ws, accent, hov);

    // Accent bar across the top
    d->AddRectFilled(wp, ImVec2(wp.x + ws.x, wp.y + 4.0f),
        ImGui::GetColorU32(accent), rounding, ImDrawFlags_RoundCornersTop);

    // Icon badge
    const ImVec2 ic(wp.x + 22.0f + 32.0f, wp.y + 26.0f + 32.0f);

    DrawGlow(d, ic, 62.0f + 22.0f * hov, accent);
    d->AddCircleFilled(ic, 32.0f, Col(accent, 0.16f + 0.16f * hov), 48);
    d->AddCircle(ic, 32.0f, Col(accent, 0.75f), 48, 1.6f);

    // orbiting spark on the icon ring
    {
        const float a = t * (0.8f + 1.4f * hov) + (float)(iconPage * 1.3f);
        d->AddCircleFilled(ImVec2(ic.x + std::cos(a) * 32.0f, ic.y + std::sin(a) * 32.0f),
            2.8f, Col(accent, 0.55f + 0.4f * hov), 12);
    }

    DrawPageIcon(d, ic, 15.0f, iconPage, ImGui::GetColorU32(accent));

    // Live badge (top-right)
    if (badge != nullptr)
    {
        const ImVec2 bs = ImGui::CalcTextSize(badge);
        const float  bw = bs.x + 26.0f;
        const float  bh = 22.0f;
        const ImVec2 bp(wp.x + ws.x - bw - 14.0f, wp.y + 14.0f);

        d->AddRectFilled(bp, ImVec2(bp.x + bw, bp.y + bh), Col(badgeCol, 0.20f), bh * 0.5f);
        d->AddRect(bp, ImVec2(bp.x + bw, bp.y + bh), Col(badgeCol, 0.70f), bh * 0.5f);
        d->AddCircleFilled(ImVec2(bp.x + 11.0f, bp.y + bh * 0.5f), 3.2f,
            ImGui::GetColorU32(badgeCol), 12);
        d->AddText(ImVec2(bp.x + 20.0f, bp.y + (bh - bs.y) * 0.5f),
            ImGui::GetColorU32(badgeCol), badge);
    }

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

        d->AddCircleFilled(ac, 13.0f + hov * 1.5f, Col(accent, 0.18f + 0.24f * hov), 32);
        d->AddCircle(ac, 13.0f + hov * 1.5f, Col(accent, 0.70f), 32, 1.2f);

        const float nx = hov * 2.0f;
        ImVec2 arrow[3] = {
            ImVec2(ac.x - 2.5f + nx, ac.y - 5.0f),
            ImVec2(ac.x + 3.0f + nx, ac.y),
            ImVec2(ac.x - 2.5f + nx, ac.y + 5.0f)
        };
        d->AddPolyline(arrow, 3, ImGui::GetColorU32(accent), 0, 2.0f);
    }

    // Click + shine sweep
    if (isHov)
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        const float sx = wp.x + (ws.x + 80.0f) * std::fmod(t * 0.8f, 1.0f) - 40.0f;

        d->PushClipRect(Off(wp, 1.0f, 1.0f), ImVec2(wp.x + ws.x - 1.0f, wp.y + ws.y - 1.0f), true);
        d->AddRectFilledMultiColor(
            ImVec2(sx - 40.0f, wp.y), ImVec2(sx + 40.0f, wp.y + ws.y),
            Col(FX_WHITE, 0.0f), Col(FX_WHITE, 0.06f), Col(FX_WHITE, 0.06f), Col(FX_WHITE, 0.0f));
        d->PopClipRect();

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
        Col(col, 0.14f), pillH * 0.5f);
    d->AddRect(p, ImVec2(p.x + pillW, p.y + pillH),
        Col(col, 0.70f), pillH * 0.5f, 0, 1.4f);

    if (monitoringActive)
        d->AddCircleFilled(dot, 8.0f + wave * 3.0f,
            Col(col, 0.20f * (1.0f - wave) + 0.05f));

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

    CenteredTitle("Security Console", 2.0f, COLOR_TEXT);
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

        // Live badge: engine state on Monitoring, alert count on Threat Detection
        char        badgeText[32] = "";
        const char* badge         = nullptr;
        ImVec4      badgeCol      = COLOR_CYAN;

        if (i == 0)
        {
            std::snprintf(badgeText, sizeof(badgeText), "%s", monitoringActive ? "Live" : "Idle");
            badge    = badgeText;
            badgeCol = monitoringActive ? COLOR_GREEN : COLOR_YELLOW;
        }
        else if (i == 1 && !alerts.empty())
        {
            std::snprintf(badgeText, sizeof(badgeText), "%d alert%s",
                (int)alerts.size(), alerts.size() == 1 ? "" : "s");
            badge    = badgeText;
            badgeCol = CountSeverity("Critical") > 0 ? COLOR_RED : COLOR_ORANGE;
        }

        if (HubCard(dom.title, dom.title, dom.description, footer,
                domainIconPage[i], DomainAccent(i), ImVec2(cardW, cardH),
                badge, badgeCol))
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
    const float  t      = (float)ImGui::GetTime();

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

        DrawGlow(d, ic, 76.0f, accent);
        d->AddCircleFilled(ic, 34.0f, Col(accent, 0.18f), 48);
        d->AddCircle(ic, 34.0f, Col(accent, 0.75f), 48, 1.6f);

        d->PathClear();
        d->PathArcTo(ic, 41.0f, t * 1.3f, t * 1.3f + 1.5f, 20);
        d->PathStroke(Col(accent, 0.85f), 0, 2.4f);

        d->PathClear();
        d->PathArcTo(ic, 41.0f, t * 1.3f + HIDS_PI, t * 1.3f + HIDS_PI + 0.8f, 12);
        d->PathStroke(Col(accent, 0.40f), 0, 2.4f);

        DrawPageIcon(d, ic, 16.0f, domainIconPage[selectedDomain], ImGui::GetColorU32(accent));

        ImGui::Dummy(ImVec2(72.0f, 72.0f));
        ImGui::SameLine(0.0f, 20.0f);

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

// ############################################################################
// SECTION 2 - THREAT PANEL  (replaces DrawThreatPanel)
// Bar chart  +  severity donut (click a slice)  +  clickable severity bars
// ############################################################################

static void DrawThreatPanel()
{
    BeginPanel("ThreatPanel", ImVec2(0.0f, 238.0f), COLOR_CYAN, true);

    SectionHeading("Threat activity",
        "new alerts per second, weighted by severity (last 60 s)");

    float peak = 6.0f;
    for (float v : threatHistory)
        peak = std::max(peak, v);

    static const char* names[] = { "Critical", "High", "Medium", "Low" };

    const float availW = ImGui::GetContentRegionAvail().x;

    DrawBarChart("##threat", threatHistory, HISTORY_SIZE, historyOffset,
        peak, ImVec2(availW * 0.46f, 140.0f));

    // ---- donut of the current alert mix ----
    ImGui::SameLine(0.0f, 18.0f);

    int counts[4];
    for (int i = 0; i < 4; ++i)
        counts[i] = CountSeverity(names[i]);

    const DonutSlice slices[4] = {
        { names[0], (float)counts[0], SeverityColor(names[0]) },
        { names[1], (float)counts[1], SeverityColor(names[1]) },
        { names[2], (float)counts[2], SeverityColor(names[2]) },
        { names[3], (float)counts[3], SeverityColor(names[3]) }
    };

    char totalText[16];
    std::snprintf(totalText, sizeof(totalText), "%d", (int)alerts.size());

    const int hot = DrawDonut("sevdonut", slices, 4, 62.0f, 15.0f, totalText, "alerts");

    if (hot >= 0 && ImGui::IsMouseClicked(0))
    {
        severityFilter = hot + 1;
        OpenPage(4);
    }

    // ---- clickable severity bars ----
    ImGui::SameLine(0.0f, 18.0f);
    ImGui::BeginGroup();

    const int total = std::max(1, (int)alerts.size());

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 8.0f));

    for (int i = 0; i < 4; ++i)
    {
        const ImVec4 sc = SeverityColor(names[i]);

        ImGui::PushID(i);

        const float  w = std::max(120.0f, ImGui::GetContentRegionAvail().x);
        const float  h = 28.0f;
        const ImVec2 p = ImGui::GetCursorScreenPos();

        const bool clicked = ImGui::InvisibleButton("##sev", ImVec2(w, h));
        const bool hovered = ImGui::IsItemHovered();

        const float hv    = AnimTo(ImGui::GetID("##hv"), hovered ? 1.0f : 0.0f);
        const float shown = AnimTo(ImGui::GetID("##fill"),
            (float)counts[i] / (float)total, 6.0f, 0.0f);

        ImDrawList* d = ImGui::GetWindowDrawList();

        d->AddRectFilled(p, ImVec2(p.x + w, p.y + h), ImGui::GetColorU32(COLOR_FRAME), h * 0.5f);

        if (shown > 0.004f)
        {
            const float fw = std::max(h, w * shown);

            d->AddRectFilled(p, ImVec2(p.x + fw, p.y + h), Col(sc, 0.55f + 0.30f * hv), h * 0.5f);
            d->AddRectFilled(Off(p, 4.0f, 2.0f), ImVec2(p.x + fw - 4.0f, p.y + h * 0.45f),
                Col(FX_WHITE, 0.16f), h * 0.25f);
        }

        d->AddRect(p, ImVec2(p.x + w, p.y + h), Col(sc, 0.30f + 0.5f * hv), h * 0.5f);

        const float ty = p.y + (h - ImGui::GetTextLineHeight()) * 0.5f;
        char num[16];
        std::snprintf(num, sizeof(num), "%d", counts[i]);

        d->AddText(ImVec2(p.x + 14.0f, ty), ImGui::GetColorU32(COLOR_TEXT), names[i]);
        d->AddText(ImVec2(p.x + w - 14.0f - ImGui::CalcTextSize(num).x, ty),
            ImGui::GetColorU32(COLOR_TEXT), num);

        if (hovered)
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            ImGui::SetTooltip("Show %s alerts", names[i]);
        }

        if (clicked)
        {
            severityFilter = i + 1;
            OpenPage(4);
        }

        ImGui::PopID();
    }

    ImGui::PopStyleVar();
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

// ############################################################################
// SECTION 3 - PASSWORD LOCK
// ############################################################################

// (3a) add these three lines right after:  static bool lockMessageOk = false;
static double lockShakeAt = -100.0;   // last wrong password / error (shake)
static double lockOkAt    = -100.0;   // last successful unlock (flash)
static bool   lockShowPw  = false;    // "show password" toggle

// (3b) replaces DrawSetPasswordForm - now with show toggle, strength meter and
//      live "passwords match" hint. Still returns true on the frame it is created.
static bool DrawSetPasswordForm()
{
    bool created = false;

    const ImGuiInputTextFlags pwFlags = lockShowPw ? 0 : ImGuiInputTextFlags_Password;

    ImGui::TextDisabled("Create a password (at least 6 characters)");

    ImGui::SetNextItemWidth(260.0f);
    ImGui::InputTextWithHint("##newpw", "New password", lockNew, sizeof(lockNew), pwFlags);
    ImGui::SameLine();
    ImGui::Checkbox("Show##setpw", &lockShowPw);

    PasswordStrengthMeter(lockNew, 260.0f);

    ImGui::SetNextItemWidth(260.0f);
    ImGui::InputTextWithHint("##confpw", "Confirm password", lockConfirm, sizeof(lockConfirm), pwFlags);

    if (lockConfirm[0] != '\0')
    {
        const bool match = std::strcmp(lockNew, lockConfirm) == 0;
        ImGui::SameLine();
        ImGui::TextColored(match ? COLOR_GREEN : COLOR_ORANGE, match ? "Match" : "No match");
    }

    PushGreenButton();

    if (ImGui::Button("Set Password", ImVec2(150.0f, 0.0f)))
    {
        if (std::strcmp(lockNew, lockConfirm) != 0)
        {
            lockMessage   = "Passwords do not match.";
            lockMessageOk = false;
            lockShakeAt   = ImGui::GetTime();
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
            else
            {
                lockShakeAt = ImGui::GetTime();
            }

            lockMessage   = message;
            lockMessageOk = created;
        }
    }

    ImGui::PopStyleColor(3);

    return created;
}

// (3c) replaces DrawLockScreen - a "vault" screen
static void DrawLockScreen()
{
    const double now         = ImGui::GetTime();
    const float  t           = (float)now;
    const bool   firstTime   = !backend.isLockConfigured();
    const int    wait        = firstTime ? 0 : backend.lockoutSecondsLeft();
    const float  shakeAge    = (float)(now - lockShakeAt);
    const bool   shaking     = shakeAge >= 0.0f && shakeAge < 0.8f;

    const ImVec4 accent = (wait > 0 || shaking) ? COLOR_RED
                        : (firstTime ? COLOR_CYAN : COLOR_GREEN);

    BeginPanel("LockScreen", ImVec2(0.0f, 0.0f), accent);

    const ImVec2 wp = ImGui::GetWindowPos();
    const ImVec2 ws = ImGui::GetWindowSize();
    ImDrawList*  d  = ImGui::GetWindowDrawList();

    // drifting particles behind everything
    d->PushClipRect(wp, ImVec2(wp.x + ws.x, wp.y + ws.y), true);

    for (int i = 0; i < 30; ++i)
    {
        const float fx = std::fmod((float)i * 0.6180339f, 1.0f);
        const float fy = 1.0f - std::fmod((float)i * 0.3819660f + t * (0.012f + 0.006f * (float)(i % 4)), 1.0f);
        const float a  = 0.07f + 0.08f * (0.5f + 0.5f * std::sin(t * 0.9f + (float)i));

        d->AddCircleFilled(ImVec2(wp.x + fx * ws.x, wp.y + fy * ws.y),
            1.2f + (float)(i % 3), Col(accent, a), 10);
    }

    d->PopClipRect();

    ImGui::Dummy(ImVec2(0.0f, 14.0f));

    // ---- vault graphic ----
    {
        const ImVec2 p = ImGui::GetCursorScreenPos();
        const ImVec2 c(wp.x + ws.x * 0.5f, p.y + 90.0f);

        DrawGlow(d, c, 140.0f, accent);

        for (int k = 0; k < 2; ++k)
        {
            const float ph = std::fmod(t * 0.45f + (float)k * 0.5f, 1.0f);
            d->AddCircle(c, 56.0f + ph * 26.0f, Col(accent, 0.30f * (1.0f - ph)), 64, 1.4f);
        }

        // rotating tick ring (turns faster when locked out)
        const float spin = t * (wait > 0 ? 1.2f : 0.35f);

        for (int i = 0; i < 48; ++i)
        {
            const float a  = (float)i * HIDS_2PI / 48.0f + spin;
            const bool  mj = (i % 4 == 0);
            const float r0 = 66.0f;
            const float r1 = mj ? 76.0f : 71.0f;

            d->AddLine(ImVec2(c.x + std::cos(a) * r0, c.y + std::sin(a) * r0),
                       ImVec2(c.x + std::cos(a) * r1, c.y + std::sin(a) * r1),
                       Col(accent, mj ? 0.60f : 0.22f), mj ? 1.6f : 1.0f);
        }

        d->AddCircleFilled(c, 54.0f, Col(accent, 0.11f), 56);
        d->AddCircle(c, 54.0f, Col(accent, 0.65f), 56, 1.6f);

        d->PathClear();
        d->PathArcTo(c, 60.0f, -t * 1.3f, -t * 1.3f + 1.4f, 24);
        d->PathStroke(Col(accent, 0.85f), 0, 2.4f);

        float shakeX = 0.0f;
        if (shaking)
            shakeX = std::exp(-shakeAge * 5.0f) * std::sin(shakeAge * 48.0f) * 10.0f;

        DrawLockIcon(d, Off(c, shakeX, 2.0f), 24.0f, accent);

        ImGui::Dummy(ImVec2(0.0f, 186.0f));
    }

    // ---- title ----
    CenteredTitle(firstTime ? "Set up your vault password" : "Logs & Reports are locked",
        1.7f, COLOR_TEXT);

    CenteredText(firstTime
        ? "This password protects detection signatures and the End Task action."
        : "Detection signatures stay hidden until you authenticate.",
        1.0f, COLOR_MUTED);

    ImGui::Dummy(ImVec2(0.0f, 16.0f));

    // ---- form (centred with Indent) ----
    const float formW = 372.0f;
    const float x0    = std::max(0.0f, (ImGui::GetContentRegionAvail().x - formW) * 0.5f);

    ImGui::Indent(x0);

    if (firstTime)
    {
        if (DrawSetPasswordForm())
        {
            logsUnlocked = true;          // whoever creates it is let in
            lockOkAt     = now;
        }
    }
    else
    {
        const ImGuiInputTextFlags pwFlags =
            (lockShowPw ? 0 : ImGuiInputTextFlags_Password) |
            ImGuiInputTextFlags_EnterReturnsTrue;

        ImGui::BeginDisabled(wait > 0);

        if (ImGui::IsWindowAppearing() && wait == 0)
            ImGui::SetKeyboardFocusHere();

        ImGui::SetNextItemWidth(270.0f);
        const bool enter = ImGui::InputTextWithHint("##unlockpw", "Enter password",
            lockEntry, sizeof(lockEntry), pwFlags);

        ImGui::SameLine(0.0f, 8.0f);

        PushGreenButton();
        const bool click = ImGui::Button("Unlock", ImVec2(90.0f, 0.0f));
        ImGui::PopStyleColor(3);

        ImGui::Checkbox("Show password##unlock", &lockShowPw);

        ImGui::EndDisabled();

        if ((enter || click) && wait == 0)
        {
            std::string message;

            if (backend.verifyPassword(lockEntry, message))
            {
                logsUnlocked = true;
                lockMessage.clear();
                lockOkAt = now;
            }
            else
            {
                lockMessage   = message;
                lockMessageOk = false;
                lockShakeAt   = now;
            }

            Wipe(lockEntry, sizeof(lockEntry));
        }
    }

    ImGui::Unindent(x0);

    // ---- messages ----
    if (wait > 0)
    {
        char line[96];
        std::snprintf(line, sizeof(line), "Too many attempts. Try again in %d s.", wait);
        ImGui::Dummy(ImVec2(0.0f, 4.0f));
        CenteredText(line, 1.0f, COLOR_YELLOW);
    }

    if (!lockMessage.empty())
    {
        ImGui::Dummy(ImVec2(0.0f, 4.0f));
        CenteredText(lockMessage.c_str(), 1.0f, lockMessageOk ? COLOR_GREEN : COLOR_RED);
    }

    ImGui::Dummy(ImVec2(0.0f, 14.0f));
    CenteredText("Auto-locks after 5 minutes idle   -   wrong attempts trigger a lockout   -   also protects End Task",
        0.92f, WithAlpha(COLOR_MUTED, 0.75f));

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
        // Red warning strip across the top of the dialog
        {
            const ImVec2 wp = ImGui::GetWindowPos();
            const ImVec2 ws = ImGui::GetWindowSize();

            ImDrawList* d = ImGui::GetWindowDrawList();

            d->AddRectFilledMultiColor(
                ImVec2(wp.x + 1.0f, wp.y + 1.0f), ImVec2(wp.x + ws.x - 1.0f, wp.y + 90.0f),
                Col(COLOR_RED, 0.16f), Col(COLOR_RED, 0.16f), Col(COLOR_RED, 0.0f), Col(COLOR_RED, 0.0f));
            d->AddRectFilled(ImVec2(wp.x + 14.0f, wp.y), ImVec2(wp.x + ws.x - 14.0f, wp.y + 3.0f),
                ImGui::GetColorU32(COLOR_RED), 2.0f);
        }

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

            // underline glow under the active filter
            if (active)
            {
                const ImVec2 a = ImGui::GetItemRectMin();
                const ImVec2 b = ImGui::GetItemRectMax();

                ImGui::GetWindowDrawList()->AddRectFilled(
                    ImVec2(a.x + 10.0f, b.y + 1.0f), ImVec2(b.x - 10.0f, b.y + 3.0f),
                    ImGui::GetColorU32(tone), 1.0f);
            }

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
        ImGui::TableSetupColumn("Severity", ImGuiTableColumnFlags_WidthFixed, 140.0f);
        ImGui::TableSetupColumn("Source", ImGuiTableColumnFlags_WidthFixed, 160.0f);
        ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableHeadersRow();

        for (const AlertInfo* ap : rows)
        {
            const AlertInfo& alert = *ap;
            const ImVec4 sc = SeverityColor(alert.severity.c_str());

            ImGui::TableNextRow();

            // Critical / High rows get a faint severity wash
            if (alert.severity == "Critical")
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, Col(sc, 0.11f));
            else if (alert.severity == "High")
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, Col(sc, 0.06f));

            ImGui::TableSetColumnIndex(0);
            {
                const ImVec2 p = ImGui::GetCursorScreenPos();
                const float  cy = p.y + ImGui::GetTextLineHeight() * 0.5f;
                ImDrawList*  d  = ImGui::GetWindowDrawList();

                // coloured stripe on the left edge of the row
                d->AddRectFilled(
                    ImVec2(p.x - ImGui::GetStyle().CellPadding.x, p.y - ImGui::GetStyle().CellPadding.y),
                    ImVec2(p.x - ImGui::GetStyle().CellPadding.x + 3.0f,
                           p.y + ImGui::GetTextLineHeight() + ImGui::GetStyle().CellPadding.y),
                    ImGui::GetColorU32(sc));

                d->AddCircleFilled(ImVec2(p.x + 4.0f, cy), 6.5f, Col(sc, 0.18f), 20);
                d->AddCircleFilled(ImVec2(p.x + 4.0f, cy), 3.8f, ImGui::GetColorU32(sc), 16);

                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18.0f);
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

            // Name with a small coloured initial tile
            ImGui::TableSetColumnIndex(1);
            {
                static const ImVec4* palette[5] = {
                    &COLOR_CYAN, &COLOR_BLUE, &COLOR_GREEN, &COLOR_ORANGE, &COLOR_YELLOW
                };

                unsigned hash = 0;
                for (char ch : process.name)
                    hash = hash * 31u + (unsigned char)ch;

                const ImVec4 tone = process.isApp ? *palette[hash % 5u] : COLOR_MUTED;
                const float  s    = ImGui::GetTextLineHeight() + 2.0f;
                const ImVec2 p    = ImGui::GetCursorScreenPos();
                ImDrawList*  d    = ImGui::GetWindowDrawList();

                d->AddRectFilled(p, ImVec2(p.x + s, p.y + s), Col(tone, 0.18f), 6.0f);
                d->AddRect(p, ImVec2(p.x + s, p.y + s), Col(tone, 0.55f), 6.0f);

                char letter[2] = {
                    process.name.empty() ? '?' : (char)std::toupper((unsigned char)process.name[0]),
                    '\0'
                };
                const ImVec2 ts = ImGui::CalcTextSize(letter);

                d->AddText(ImVec2(p.x + (s - ts.x) * 0.5f, p.y + (s - ts.y) * 0.5f),
                    ImGui::GetColorU32(tone), letter);

                ImGui::Dummy(ImVec2(s, s));
                ImGui::SameLine(0.0f, 8.0f);
            }

            ImGui::TextUnformatted(process.name.c_str());

            if (process.isApp && !process.title.empty())
            {
                ImGui::SameLine();
                ImGui::TextDisabled("- %s", process.title.c_str());
            }

            // CPU bar
            ImGui::TableSetColumnIndex(2);

            {
                char cpuLabel[16];
                std::snprintf(cpuLabel, sizeof(cpuLabel), "%.1f", process.cpu);

                DrawUsageBar(std::min(process.cpu / 25.0f, 1.0f), cpuLabel,
                    LoadColor(process.cpu * 4.0f), 18.0f);
            }

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
        ImGui::TableSetupColumn("Event", ImGuiTableColumnFlags_WidthFixed, 210.0f);
        ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        for (const EventInfo& event : events)
        {
            const ImVec4 ec = EventColor(event.type);

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::TextDisabled("%s", event.time.c_str());

            // Event type as a coloured chip
            ImGui::TableSetColumnIndex(1);
            {
                const ImVec2 ts = ImGui::CalcTextSize(event.type.c_str());
                const ImVec2 p  = ImGui::GetCursorScreenPos();
                const float  w  = ts.x + 30.0f;
                const float  h  = ts.y + 4.0f;
                ImDrawList*  d  = ImGui::GetWindowDrawList();

                d->AddRectFilled(p, ImVec2(p.x + w, p.y + h), Col(ec, 0.14f), h * 0.5f);
                d->AddRect(p, ImVec2(p.x + w, p.y + h), Col(ec, 0.50f), h * 0.5f);
                d->AddCircleFilled(ImVec2(p.x + 12.0f, p.y + h * 0.5f), 3.4f,
                    ImGui::GetColorU32(ec), 12);
                d->AddText(ImVec2(p.x + 22.0f, p.y + 2.0f), ImGui::GetColorU32(ec),
                    event.type.c_str());

                ImGui::Dummy(ImVec2(w, h));
            }

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

        // little loading dots
        {
            const ImVec2 p = ImGui::GetCursorScreenPos();
            const float  t = (float)ImGui::GetTime();

            for (int i = 0; i < 3; ++i)
            {
                const float k = 0.5f + 0.5f * std::sin(t * 5.0f - (float)i * 0.9f);

                ImGui::GetWindowDrawList()->AddCircleFilled(
                    ImVec2(p.x + 8.0f + (float)i * 18.0f, p.y + 12.0f),
                    3.0f + 3.0f * k, Col(COLOR_BLUE, 0.4f + 0.5f * k), 16);
            }

            ImGui::Dummy(ImVec2(70.0f, 24.0f));
        }

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
            const ImVec2 p0 = ImGui::GetCursorScreenPos();
            const float  w  = ImGui::GetContentRegionAvail().x;

            ImGui::PushStyleColor(ImGuiCol_Text, COLOR_YELLOW);
            ImGui::Bullet();
            ImGui::SameLine();
            ImGui::TextWrapped("%s", finding.c_str());
            ImGui::PopStyleColor();

            // faint highlight card + stripe behind each finding
            const ImVec2 p1 = ImGui::GetCursorScreenPos();

            ImDrawList* d = ImGui::GetWindowDrawList();
            d->AddRectFilled(Off(p0, -6.0f, -2.0f),
                ImVec2(p0.x + w, p1.y - ImGui::GetStyle().ItemSpacing.y + 2.0f),
                Col(COLOR_YELLOW, 0.05f), 8.0f);
            d->AddRectFilled(Off(p0, -6.0f, -2.0f),
                ImVec2(p0.x - 3.0f, p1.y - ImGui::GetStyle().ItemSpacing.y + 2.0f),
                Col(COLOR_YELLOW, 0.85f), 2.0f);
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
        ImGui::TableSetupColumn("CPU %", ImGuiTableColumnFlags_WidthFixed, 150.0f);
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
            {
                char label[16];
                std::snprintf(label, sizeof(label), "%.1f", p.cpuPercent);

                DrawUsageBar(std::min(p.cpuPercent / 25.0f, 1.0f), label,
                    LoadColor(p.cpuPercent * 4.0f), 16.0f);
            }

            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%.1f", p.memoryMB);
        }

        ImGui::EndTable();
    }

    ImGui::EndChild();
}

// ------------------------------------------------------------
// FILE SCANNER HELPERS (kept; the new scanner page reuses them)
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

// Thin progress beam: sliding highlight while scanning, solid when finished
static void DrawBeamBar(const ImVec4& color, bool running, bool finished)
{
    const float  w = std::max(40.0f, ImGui::GetContentRegionAvail().x);
    const float  h = 6.0f;
    const ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList*  d = ImGui::GetWindowDrawList();
    const float  t = (float)ImGui::GetTime();

    d->AddRectFilled(p, ImVec2(p.x + w, p.y + h), ImGui::GetColorU32(COLOR_FRAME), h * 0.5f);

    if (running)
    {
        const float ph  = std::fmod(t * 0.55f, 1.0f);
        const float len = w * 0.32f;
        const float x0  = p.x - len + (w + len) * ph;

        d->PushClipRect(p, ImVec2(p.x + w, p.y + h), true);
        d->AddRectFilledMultiColor(ImVec2(x0, p.y), ImVec2(x0 + len, p.y + h),
            Col(color, 0.0f), Col(color, 0.95f), Col(color, 0.95f), Col(color, 0.0f));
        d->PopClipRect();
    }
    else if (finished)
    {
        d->AddRectFilled(p, ImVec2(p.x + w, p.y + h), Col(color, 0.75f), h * 0.5f);
    }

    ImGui::Dummy(ImVec2(w, h));
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

// ############################################################################
// SECTION 4 - FILE SCANNER  (replaces DrawFileScanner)
// Mode cards, live radar with finding blips, speed graph, threat donut,
// finding cards (click = copy path), filterable findings table.
// ############################################################################

static void DrawFileScanner()
{
    static double lastRefresh = -1.0;
    static char   findingSearch[128] = "";
    static int    levelFilter = 0;                 // 0 all, 1 malicious, 2 suspicious

    static float  rateHist[60] = {};
    static int    rateOff   = 0;
    static float  rateNow   = 0.0f;
    static float  ratePeak  = 20.0f;
    static bool   wasRunning = false;
    static double lastFiles  = 0.0;

    const double now = ImGui::GetTime();

    // ---- poll the backend 4x per second and sample scan speed ----
    if (lastRefresh < 0.0 || now - lastRefresh >= 0.25)
    {
        const double dt = lastRefresh < 0.0 ? 0.25 : now - lastRefresh;

        lastRefresh  = now;
        scanStatus   = backend.getScanStatus();
        scanFindings = backend.getScanFindings();

        const double files = (double)scanStatus.filesScanned;

        if (scanStatus.running)
        {
            if (!wasRunning)
            {
                std::memset(rateHist, 0, sizeof(rateHist));
                rateOff  = 0;
                rateNow  = 0.0f;
                ratePeak = 20.0f;
                lastFiles = files;
            }

            const float r = (files >= lastFiles && dt > 0.0)
                ? (float)((files - lastFiles) / dt) : 0.0f;

            rateNow = rateNow * 0.6f + r * 0.4f;
            rateHist[rateOff] = rateNow;
            rateOff  = (rateOff + 1) % 60;
            ratePeak = std::max(ratePeak, rateNow * 1.15f);
            lastFiles = files;
        }

        wasRunning = scanStatus.running;
    }

    const bool running  = scanStatus.running;
    const bool finished = scanStatus.finished;
    const bool cleanEnd = finished && !scanStatus.cancelled;
    const int  threats  = scanStatus.malicious + scanStatus.suspicious;

    BeginPanel("ScannerPanel", ImVec2(0.0f, 0.0f), COLOR_ORANGE);

    SectionHeading("File Scanner", "Signature + heuristic scan");

    // ---------------------------------------------------------------- modes
    {
        const float gap = ImGui::GetStyle().ItemSpacing.x;
        const float cw  = (ImGui::GetContentRegionAvail().x - gap * 2.0f) / 3.0f;

        static const char* titles[3] = { "Quick scan", "Full scan", "Custom scan" };
        static const char* descs[3]  = {
            "User folders, temp files and startup items",
            "Every file on all fixed drives",
            "Choose one folder or file"
        };
        const ImVec4 accents[3] = { COLOR_GREEN, COLOR_BLUE, COLOR_ORANGE };

        ImGui::BeginDisabled(running);

        for (int i = 0; i < 3; ++i)
        {
            if (i > 0)
                ImGui::SameLine();

            if (ScanModeCard(i, titles[i], descs[i], scanTypeSel == i, accents[i],
                    ImVec2(cw, 76.0f)))
                scanTypeSel = i;
        }

        if (scanTypeSel == 2)
        {
            ImGui::SetNextItemWidth(-1.0f);
            ImGui::InputTextWithHint("##scanpath",
                "Folder or file, e.g. C:\\Users\\you\\Downloads",
                scanPath, sizeof(scanPath));

            const char* home = std::getenv("USERPROFILE");

            if (home != nullptr)
            {
                static const char* presets[3] = { "Downloads", "Desktop", "Documents" };

                ImGui::TextDisabled("Quick pick:");

                for (int i = 0; i < 3; ++i)
                {
                    ImGui::SameLine();

                    if (ImGui::SmallButton(presets[i]))
                        std::snprintf(scanPath, sizeof(scanPath), "%s\\%s", home, presets[i]);
                }
            }
        }

        ImGui::EndDisabled();
    }

    // -------------------------------------------------------------- actions
    ImGui::Spacing();

    if (!running)
    {
        PushGreenButton();

        if (ImGui::Button("Start scan", ImVec2(160.0f, 38.0f)))
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

        if (ImGui::Button("Stop scan", ImVec2(160.0f, 38.0f)))
            backend.cancelScan();

        ImGui::PopStyleColor(3);
    }

    if (!running && finished)
    {
        ImGui::SameLine();

        if (ImGui::Button("Export report", ImVec2(130.0f, 38.0f)))
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
        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("%s", scanMessage.c_str());
    }

    if (ImGui::CollapsingHeader("How scanning works"))
    {
        ImGui::PushTextWrapPos(0.0f);
        ImGui::TextDisabled(
            "Programs and scripts are checked against all detection rules; text files only "
            "for ransom-note wording. Validly signed programs and anything listed in "
            "hids_allowlist.txt are not reported unless a known-bad hash matches.");
        ImGui::PopTextWrapPos();
    }

    ImGui::Spacing();

    // ----------------------------------------------------------- state text
    ImVec4      stateColor = COLOR_MUTED;
    const char* stateText  = "Ready to scan";

    if (running)
    {
        stateColor = COLOR_CYAN;
        stateText  = "Scanning";
    }
    else if (finished && scanStatus.cancelled)
    {
        stateColor = COLOR_YELLOW;
        stateText  = "Scan stopped";
    }
    else if (finished)
    {
        stateColor = threats > 0 ? COLOR_ORANGE : COLOR_GREEN;
        stateText  = "Scan completed";
    }

    const float gap    = ImGui::GetStyle().ItemSpacing.x;
    const float leftW  = (ImGui::GetContentRegionAvail().x - gap) * 0.60f;
    const float panelH = 372.0f;

    ImGui::PushStyleColor(ImGuiCol_ChildBg, COLOR_SIDEBAR);

    // ================================================================ LEFT
    BeginPanel("ScanLive", ImVec2(leftW, panelH), stateColor, true);

    {
        DrawScanRadar(54.0f, stateColor, running, cleanEnd, scanFindings);
        ImGui::SameLine(0.0f, 12.0f);

        ImGui::BeginGroup();

        ImGui::SetWindowFontScale(1.4f);
        ImGui::PushStyleColor(ImGuiCol_Text, stateColor);

        if (running)
        {
            const int dots = (int)(now * 2.0) % 4;
            ImGui::Text("%s%.*s", stateText, dots, "...");
        }
        else
        {
            ImGui::TextUnformatted(stateText);
        }

        ImGui::PopStyleColor();
        ImGui::SetWindowFontScale(1.0f);

        const float gw = std::max(120.0f, ImGui::GetContentRegionAvail().x - 6.0f);

        ImGui::TextDisabled("%s",
            ElideBack(scanStatus.target.empty() ? std::string("Choose a scan type and press Start")
                                                : scanStatus.target, gw).c_str());

        if (cleanEnd)
        {
            if (threats > 0)
                ImGui::TextColored(COLOR_ORANGE, "%d threat%s need attention",
                    threats, threats == 1 ? "" : "s");
            else
                ImGui::TextColored(COLOR_GREEN, "No threats found");
        }
        else
        {
            ImGui::Dummy(ImVec2(0.0f, ImGui::GetTextLineHeight()));
        }

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
            ImGui::SetWindowFontScale(1.12f);
            ImGui::TextUnformatted(ElideBack(name, gw).c_str());
            ImGui::SetWindowFontScale(1.0f);

            ImGui::PushStyleColor(ImGuiCol_Text, COLOR_MUTED);
            ImGui::TextUnformatted(ElideFront(dir, gw).c_str());
            ImGui::PopStyleColor();
        }

        ImGui::EndGroup();

        ImGui::Spacing();
        DrawBeamBar(stateColor, running, cleanEnd);
        ImGui::Spacing();

        // live counters as tiles
        {
            char files[24], skipped[24], data[24], speed[24];
            std::snprintf(files,   sizeof(files),   "%llu", (unsigned long long)scanStatus.filesScanned);
            std::snprintf(skipped, sizeof(skipped), "%llu", (unsigned long long)scanStatus.filesSkipped);
            std::snprintf(data,    sizeof(data),    "%s", FormatBytes(scanStatus.bytesScanned).c_str());

            const double avg = scanStatus.elapsedSeconds > 0.5
                ? (double)scanStatus.filesScanned / scanStatus.elapsedSeconds : 0.0;
            std::snprintf(speed, sizeof(speed), "%.0f/s", running ? (double)rateNow : avg);

            const std::string elapsed = FormatDuration(scanStatus.elapsedSeconds);

            const float g  = 10.0f;
            const float tw = (ImGui::GetContentRegionAvail().x - g * 4.0f) / 5.0f;

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(g, g));

            DrawMiniStat("FILES",   files,   COLOR_CYAN,  tw); ImGui::SameLine();
            DrawMiniStat("SKIPPED", skipped, COLOR_MUTED, tw); ImGui::SameLine();
            DrawMiniStat("DATA",    data,    COLOR_BLUE,  tw); ImGui::SameLine();
            DrawMiniStat("SPEED",   speed,   COLOR_CYAN,  tw); ImGui::SameLine();
            DrawMiniStat("ELAPSED", elapsed.c_str(), COLOR_GREEN, tw);

            ImGui::PopStyleVar();
        }

        ImGui::TextDisabled("SCAN SPEED  (files per second)");

        DrawAreaChart("##scanrate", rateHist, 60, rateOff, ratePeak,
            ImVec2(ImGui::GetContentRegionAvail().x, 50.0f), COLOR_CYAN);
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // =============================================================== RIGHT
    BeginPanel("ScanThreats", ImVec2(0.0f, panelH),
        scanStatus.malicious > 0 ? COLOR_RED : COLOR_ORANGE, true);

    {
        ImGui::PushStyleColor(ImGuiCol_Text, COLOR_CYAN);
        ImGui::SetWindowFontScale(1.1f);
        ImGui::TextUnformatted("Suspicious & malicious files");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();

        const DonutSlice slices[2] = {
            { "Malicious",  (float)scanStatus.malicious,  COLOR_RED },
            { "Suspicious", (float)scanStatus.suspicious, COLOR_YELLOW }
        };

        char totalText[16];
        std::snprintf(totalText, sizeof(totalText), "%d", threats);

        DrawDonut("scandonut", slices, 2, 44.0f, 12.0f, totalText, "threats");

        ImGui::SameLine(0.0f, 16.0f);
        ImGui::BeginGroup();
        ImGui::Dummy(ImVec2(0.0f, 6.0f));

        char a[24], b[24], c[24];
        std::snprintf(a, sizeof(a), "%d", scanStatus.malicious);
        std::snprintf(b, sizeof(b), "%d", scanStatus.suspicious);

        const unsigned long long scanned = (unsigned long long)scanStatus.filesScanned;
        const unsigned long long clean   =
            scanned > (unsigned long long)threats ? scanned - (unsigned long long)threats : 0ULL;
        std::snprintf(c, sizeof(c), "%llu", clean);

        LegendRow("Malicious",   a, COLOR_RED);
        LegendRow("Suspicious",  b, COLOR_YELLOW);
        LegendRow("Clean files", c, COLOR_GREEN);

        ImGui::EndGroup();

        ImGui::Separator();

        // scrolling list of the latest findings (click a card = copy path)
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
        ImGui::BeginChild("##findcards", ImVec2(0.0f, 0.0f), false);

        if (scanFindings.empty())
        {
            ImGui::Spacing();

            if (running)
                ImGui::TextDisabled("Nothing suspicious so far...");
            else if (finished)
                ImGui::TextColored(COLOR_GREEN, "No threats found.");
            else
                ImGui::TextDisabled("Findings will appear here.");
        }

        const std::size_t maxCards = 40;
        std::size_t       drawn    = 0;

        for (std::size_t i = scanFindings.size(); i-- > 0 && drawn < maxCards; )
        {
            const ScanFinding& f = scanFindings[i];
            const ImVec4 c4 = (f.level == "Malicious") ? COLOR_RED : COLOR_YELLOW;

            std::string fdir, fname;
            SplitPath(f.path, fdir, fname);

            ++drawn;
            ImGui::PushID((int)i);

            const float  cardH = 50.0f;
            const float  w     = ImGui::GetContentRegionAvail().x;
            const ImVec2 p     = ImGui::GetCursorScreenPos();

            const bool clicked = ImGui::Selectable("##card", false, 0, ImVec2(0.0f, cardH - 4.0f));
            const bool hov     = ImGui::IsItemHovered();

            ImDrawList* dl = ImGui::GetWindowDrawList();

            dl->AddRectFilled(p, ImVec2(p.x + w, p.y + cardH - 4.0f),
                Col(c4, 0.06f + 0.08f * (hov ? 1.0f : 0.0f)), 8.0f);
            dl->AddRectFilled(p, ImVec2(p.x + 4.0f, p.y + cardH - 4.0f),
                ImGui::GetColorU32(c4), 3.0f, ImDrawFlags_RoundCornersLeft);

            const ImVec2 ls = ImGui::CalcTextSize(f.level.c_str());

            dl->AddText(Off(p, 14.0f, 6.0f), ImGui::GetColorU32(c4), f.level.c_str());
            dl->AddText(Off(p, 14.0f + ls.x + 10.0f, 6.0f), ImGui::GetColorU32(COLOR_TEXT),
                ElideBack(f.name, w - ls.x - 34.0f).c_str());
            dl->AddText(Off(p, 14.0f, 6.0f + ImGui::GetTextLineHeight() + 3.0f),
                ImGui::GetColorU32(COLOR_MUTED), ElideBack(fname, w - 28.0f).c_str());

            if (hov)
            {
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                ImGui::BeginTooltip();
                ImGui::Text("%s", f.path.c_str());
                ImGui::Text("Reason: %s", f.reason.c_str());
                ImGui::Text("Score: %d", f.score);
                ImGui::TextDisabled("Click to copy the path");
                ImGui::EndTooltip();
            }

            if (clicked)
            {
                ImGui::SetClipboardText(f.path.c_str());
                scanMessage = "Path copied to clipboard.";
            }

            ImGui::PopID();
        }

        if (scanFindings.size() > maxCards)
            ImGui::TextDisabled("+ %d more in the table below",
                (int)(scanFindings.size() - maxCards));

        ImGui::EndChild();
        ImGui::PopStyleColor();   // inner ChildBg
    }

    ImGui::EndChild();

    ImGui::PopStyleColor();       // COLOR_SIDEBAR

    ImGui::Spacing();

    // ============================================================== TABLE
    SectionHeading("All findings", "hover a name for score and SHA-256");

    int nBad = 0, nSus = 0;

    for (const ScanFinding& f : scanFindings)
    {
        if (f.level == "Malicious") ++nBad;
        else                        ++nSus;
    }

    {
        static const char* lv[3] = { "All", "Malicious", "Suspicious" };
        const int    cnt[3]  = { (int)scanFindings.size(), nBad, nSus };
        const ImVec4 tone[3] = { COLOR_BLUE, COLOR_RED, COLOR_YELLOW };

        for (int i = 0; i < 3; ++i)
        {
            const bool active = (levelFilter == i);

            char text[48];
            std::snprintf(text, sizeof(text), "%s  %d", lv[i], cnt[i]);

            ImGui::PushStyleColor(ImGuiCol_Button,
                active ? WithAlpha(tone[i], 0.60f) : COLOR_FRAME);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, WithAlpha(tone[i], 0.45f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  WithAlpha(tone[i], 0.75f));

            if (ImGui::Button(text, ImVec2(128.0f, 0.0f)))
                levelFilter = i;

            ImGui::PopStyleColor(3);

            if (active)
            {
                const ImVec2 a = ImGui::GetItemRectMin();
                const ImVec2 b = ImGui::GetItemRectMax();

                ImGui::GetWindowDrawList()->AddRectFilled(
                    ImVec2(a.x + 10.0f, b.y + 1.0f), ImVec2(b.x - 10.0f, b.y + 3.0f),
                    ImGui::GetColorU32(tone[i]), 1.0f);
            }

            ImGui::SameLine();
        }

        ImGui::SetNextItemWidth(280.0f);
        ImGui::InputTextWithHint("##findingsearch",
            "Filter by name, file or reason", findingSearch, IM_ARRAYSIZE(findingSearch));
    }

    ImGui::Spacing();

    if (scanFindings.empty())
    {
        EmptyState(finished ? "No threats found" : "No findings yet",
            finished ? "Everything scanned looked clean."
                     : "Start a scan - anything suspicious will be listed here.",
            finished ? COLOR_GREEN : COLOR_CYAN);

        ImGui::EndChild();
        return;
    }

    if (ImGui::BeginTable(
            "ScanFindings", 6,
            ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH |
            ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp |
            ImGuiTableFlags_ScrollY,
            ImVec2(0.0f, std::max(260.0f, ImGui::GetContentRegionAvail().y))))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Time",   ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Level",  ImGuiTableColumnFlags_WidthFixed, 110.0f);
        ImGui::TableSetupColumn("Name",   ImGuiTableColumnFlags_WidthFixed, 190.0f);
        ImGui::TableSetupColumn("File");
        ImGui::TableSetupColumn("Reason");
        ImGui::TableSetupColumn("Copy",   ImGuiTableColumnFlags_WidthFixed, 70.0f);
        ImGui::TableHeadersRow();

        for (std::size_t idx = scanFindings.size(); idx-- > 0; )      // newest first
        {
            const ScanFinding& f = scanFindings[idx];

            const bool bad = (f.level == "Malicious");

            if (levelFilter == 1 && !bad) continue;
            if (levelFilter == 2 && bad)  continue;

            if (!ContainsNoCase(f.name.c_str(),   findingSearch) &&
                !ContainsNoCase(f.path.c_str(),   findingSearch) &&
                !ContainsNoCase(f.reason.c_str(), findingSearch))
                continue;

            const ImVec4 lc = bad ? COLOR_RED : COLOR_YELLOW;

            ImGui::PushID((int)idx);
            ImGui::TableNextRow();

            if (bad)
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, Col(lc, 0.08f));

            ImGui::TableSetColumnIndex(0);
            ImGui::TextDisabled("%s", f.time.c_str());

            ImGui::TableSetColumnIndex(1);
            {
                const ImVec2 ts = ImGui::CalcTextSize(f.level.c_str());
                const ImVec2 p  = ImGui::GetCursorScreenPos();
                const float  w  = ts.x + 30.0f;
                const float  h  = ts.y + 4.0f;
                ImDrawList*  d  = ImGui::GetWindowDrawList();

                d->AddRectFilled(p, ImVec2(p.x + w, p.y + h), Col(lc, 0.16f), h * 0.5f);
                d->AddRect(p, ImVec2(p.x + w, p.y + h), Col(lc, 0.60f), h * 0.5f);
                d->AddCircleFilled(ImVec2(p.x + 12.0f, p.y + h * 0.5f), 3.4f,
                    ImGui::GetColorU32(lc), 12);
                d->AddText(ImVec2(p.x + 22.0f, p.y + 2.0f), ImGui::GetColorU32(lc),
                    f.level.c_str());

                ImGui::Dummy(ImVec2(w, h));
            }

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

            ImGui::TableSetColumnIndex(5);

            if (ImGui::SmallButton("Path"))
                ImGui::SetClipboardText(f.path.c_str());

            ImGui::SameLine();

            if (!f.sha256.empty() && ImGui::SmallButton("Hash"))
                ImGui::SetClipboardText(f.sha256.c_str());

            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    ImGui::EndChild();
}


// ############################################################################
// SECTION 5 - LOGS & REPORTS  (replaces DrawLogsReports)
// Overview cards: rule-type donut, rule-weight histogram, vault timer ring.
// ############################################################################

static void DrawLogsReports()
{
    if (!LogsGate())
        return;

    static std::vector<SignatureInfo> signatures;
    static bool loaded = false;
    static char search[128] = "";
    static int  catFilter = 0;     // 0 All, 1 Known hash, 2 File content, 3 Name / location

    if (!loaded)
    {
        signatures = FileScanner::getSignatures();
        loaded = true;
    }

    const int    threshold = FileScanner::suspiciousThreshold();
    const double now       = ImGui::GetTime();

    auto categoryIndex = [](const std::string& category)
    {
        if (category == "Known hash")   return 1;
        if (category == "File content") return 2;
        return 3;
    };

    int hashCount = 0, contentCount = 0, nameCount = 0, instantCount = 0;
    int wHist[7] = {};                       // weights 1..6 (index 1..6)

    for (const SignatureInfo& s : signatures)
    {
        const int cat = categoryIndex(s.category);

        if (cat == 1)      ++hashCount;
        else if (cat == 2) ++contentCount;
        else               ++nameCount;

        if (s.instantMalicious)
            ++instantCount;
        else
            ++wHist[std::min(6, std::max(1, s.weight))];
    }

    BeginPanel("LogsPanel", ImVec2(0.0f, 0.0f), COLOR_GREEN);

    SectionHeading("Detection signatures", "what the File Scanner checks for");

    // ============================================================ overview
    {
        const float sp    = ImGui::GetStyle().ItemSpacing.x;
        const float avail = ImGui::GetContentRegionAvail().x;
        const float w1    = (avail - sp * 2.0f) * 0.30f;
        const float w2    = (avail - sp * 2.0f) * 0.42f;
        const float cardH = 184.0f;

        // ---- card 1: rules by type (donut) ----
        BeginPanel("RuleTypeCard", ImVec2(w1, cardH), COLOR_CYAN, true);
        {
            ImGui::TextDisabled("RULES BY TYPE");

            const DonutSlice slices[3] = {
                { "Known hashes",    (float)hashCount,    COLOR_RED },
                { "File content",    (float)contentCount, COLOR_CYAN },
                { "Name / location", (float)nameCount,    COLOR_YELLOW }
            };

            char total[16];
            std::snprintf(total, sizeof(total), "%d", (int)signatures.size());

            DrawDonut("ruledonut", slices, 3, 54.0f, 14.0f, total, "rules");

            ImGui::SameLine(0.0f, 12.0f);
            ImGui::BeginGroup();
            ImGui::Dummy(ImVec2(0.0f, 14.0f));

            char a[16], b[16], c[16];
            std::snprintf(a, sizeof(a), "%d", hashCount);
            std::snprintf(b, sizeof(b), "%d", contentCount);
            std::snprintf(c, sizeof(c), "%d", nameCount);

            LegendRow("Hashes",   a, COLOR_RED);
            LegendRow("Content",  b, COLOR_CYAN);
            LegendRow("Name/loc", c, COLOR_YELLOW);

            ImGui::EndGroup();
        }
        ImGui::EndChild();

        ImGui::SameLine();

        // ---- card 2: weight histogram ----
        BeginPanel("WeightCard", ImVec2(w2, cardH), COLOR_ORANGE, true);
        {
            ImGui::TextDisabled("RULE WEIGHTS  (orange = flags a file alone)");

            const ImVec2 p     = ImGui::GetCursorScreenPos();
            const float  w     = ImGui::GetContentRegionAvail().x;
            const float  areaH = 112.0f;
            const float  colW  = w / 7.0f;
            ImDrawList*  d     = ImGui::GetWindowDrawList();

            int maxCount = std::max(1, instantCount);
            for (int i = 1; i <= 6; ++i)
                maxCount = std::max(maxCount, wHist[i]);

            const float baseY = p.y + areaH - 16.0f;

            d->AddLine(ImVec2(p.x, baseY), ImVec2(p.x + w, baseY), Col(FX_WHITE, 0.10f), 1.0f);

            for (int i = 0; i < 7; ++i)
            {
                const bool instant = (i == 6);
                const int  weight  = i + 1;
                const int  count   = instant ? instantCount : wHist[weight];
                const bool alone   = instant || weight >= threshold;
                const ImVec4 col   = instant ? COLOR_RED : (alone ? COLOR_ORANGE : COLOR_CYAN);

                ImGui::PushID(i);
                const float target = (areaH - 36.0f) * (float)count / (float)maxCount;
                const float shown  = AnimTo(ImGui::GetID("##bar"), target, 8.0f, 0.0f);
                ImGui::PopID();

                const float x0 = p.x + (float)i * colW + colW * 0.18f;
                const float x1 = p.x + (float)(i + 1) * colW - colW * 0.18f;
                const float y0 = baseY - shown;

                if (shown > 1.0f)
                {
                    d->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, baseY), Col(col, 0.80f), 5.0f,
                        ImDrawFlags_RoundCornersTop);
                    d->AddRectFilled(ImVec2(x0 + 3.0f, y0 + 2.0f), ImVec2(x1 - 3.0f, y0 + std::min(shown, 10.0f)),
                        Col(FX_WHITE, 0.16f), 3.0f);
                }

                char num[16];
                std::snprintf(num, sizeof(num), "%d", count);
                const ImVec2 ns = ImGui::CalcTextSize(num);

                if (count > 0)
                    d->AddText(ImVec2((x0 + x1 - ns.x) * 0.5f, y0 - ns.y - 2.0f),
                        ImGui::GetColorU32(COLOR_TEXT), num);

                char lab[16];
                if (instant) std::snprintf(lab, sizeof(lab), "Hash");
                else         std::snprintf(lab, sizeof(lab), "+%d", weight);
                const ImVec2 lsz = ImGui::CalcTextSize(lab);

                d->AddText(ImVec2((x0 + x1 - lsz.x) * 0.5f, baseY + 3.0f),
                    ImGui::GetColorU32(COLOR_MUTED), lab);

                if (ImGui::IsMouseHoveringRect(ImVec2(x0, p.y), ImVec2(x1, baseY + 18.0f)))
                {
                    if (instant)
                        ImGui::SetTooltip("%d rule(s) report Malicious instantly", count);
                    else
                        ImGui::SetTooltip("%d rule(s) with weight +%d%s", count, weight,
                            alone ? " (flag a file alone)" : "");
                }
            }

            ImGui::Dummy(ImVec2(w, areaH));
            ImGui::TextDisabled("Suspicious threshold: score %d", threshold);
        }
        ImGui::EndChild();

        ImGui::SameLine();

        // ---- card 3: vault status + auto-lock timer ----
        BeginPanel("VaultCard", ImVec2(0.0f, cardH), COLOR_GREEN, true);
        {
            ImGui::TextDisabled("VAULT");

            const double left = std::max(0.0, LOCK_IDLE_SECONDS - (now - logsLastActive));
            const float  frac = (float)(left / LOCK_IDLE_SECONDS);
            const ImVec4 rc   = left < 20.0 ? COLOR_RED : (left < 60.0 ? COLOR_YELLOW : COLOR_GREEN);

            const ImVec2 p = ImGui::GetCursorScreenPos();
            const ImVec2 c(p.x + 50.0f, p.y + 52.0f);
            ImDrawList*  d = ImGui::GetWindowDrawList();

            DrawGlow(d, c, 70.0f, rc);
            DrawCountdownRing(c, 38.0f, frac, rc, 5.0f);

            const float flash = (float)(now - lockOkAt);

            if (flash >= 0.0f && flash < 1.3f)
            {
                const float ph = flash / 1.3f;
                d->AddCircle(c, 38.0f + ph * 34.0f, Col(COLOR_GREEN, 0.65f * (1.0f - ph)), 48, 2.2f);
            }

            char tm[16];
            std::snprintf(tm, sizeof(tm), "%d:%02d", (int)left / 60, (int)left % 60);
            const ImVec2 ts = ImGui::CalcTextSize(tm);
            d->AddText(ImVec2(c.x - ts.x * 0.5f, c.y - ts.y * 0.5f), ImGui::GetColorU32(COLOR_TEXT), tm);

            ImGui::Dummy(ImVec2(100.0f, 104.0f));
            ImGui::SameLine(0.0f, 10.0f);

            ImGui::BeginGroup();
            ImGui::Dummy(ImVec2(0.0f, 8.0f));
            ImGui::TextColored(COLOR_GREEN, "Unlocked");
            ImGui::TextDisabled("Auto-locks when idle");
            ImGui::Dummy(ImVec2(0.0f, 4.0f));

            PushRedButton();

            if (ImGui::Button("Lock now", ImVec2(110.0f, 0.0f)))
            {
                logsUnlocked = false;
                lockMessage.clear();
            }

            ImGui::PopStyleColor(3);
            ImGui::EndGroup();
        }
        ImGui::EndChild();
    }

    ImGui::Spacing();

    // ===================================================== change password
    if (ImGui::CollapsingHeader("Change password"))
    {
        static std::string changeMessage;
        static bool        changeOk = false;

        const ImGuiInputTextFlags pwFlags = lockShowPw ? 0 : ImGuiInputTextFlags_Password;

        ImGui::SetNextItemWidth(260.0f);
        ImGui::InputTextWithHint("##oldpw", "Current password",
            lockOld, sizeof(lockOld), pwFlags);

        ImGui::SetNextItemWidth(260.0f);
        ImGui::InputTextWithHint("##chnew", "New password",
            lockNew, sizeof(lockNew), pwFlags);

        ImGui::SameLine();
        ImGui::Checkbox("Show##change", &lockShowPw);

        PasswordStrengthMeter(lockNew, 260.0f);

        ImGui::SetNextItemWidth(260.0f);
        ImGui::InputTextWithHint("##chconf", "Confirm new password",
            lockConfirm, sizeof(lockConfirm), pwFlags);

        if (ImGui::Button("Update password", ImVec2(160.0f, 0.0f)))
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
            ImGui::TextColored(changeOk ? COLOR_GREEN : COLOR_RED, "%s", changeMessage.c_str());

        ImGui::Spacing();
    }

    // ===================================================== how scoring works
    if (ImGui::CollapsingHeader("How a file is scored"))
    {
        ImGui::PushTextWrapPos(0.0f);
        ImGui::TextColored(COLOR_MUTED,
            "A file is reported Malicious on an instant match (EICAR test string or a "
            "known-bad SHA-256). Otherwise every matching rule adds its weight to a "
            "score, and a file scoring %d or more is reported Suspicious.",
            threshold);
        ImGui::Spacing();
        ImGui::TextColored(COLOR_MUTED,
            "Rules that describe the same behaviour (for example the different ways to "
            "delete shadow copies) count once, using the highest weight. Command rules "
            "run on programs and scripts only; ransom-note wording is also checked in "
            "text files. Validly signed programs and files listed in hids_allowlist.txt "
            "are not reported, unless a known-bad hash matches.");
        ImGui::PopTextWrapPos();
        ImGui::Spacing();
    }

    // ============================================================== filters
    {
        static const char* catLabels[4] =
            { "All", "Known hashes", "File content", "Name / location" };
        const int    catCounts[4] =
            { (int)signatures.size(), hashCount, contentCount, nameCount };
        const ImVec4 catTones[4] =
            { COLOR_BLUE, COLOR_RED, COLOR_CYAN, COLOR_YELLOW };

        for (int i = 0; i < 4; ++i)
        {
            const bool active = (catFilter == i);

            char text[48];
            std::snprintf(text, sizeof(text), "%s  %d", catLabels[i], catCounts[i]);

            ImGui::PushStyleColor(ImGuiCol_Button,
                active ? WithAlpha(catTones[i], 0.60f) : COLOR_FRAME);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, WithAlpha(catTones[i], 0.45f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  WithAlpha(catTones[i], 0.75f));

            if (ImGui::Button(text, ImVec2(160.0f, 0.0f)))
                catFilter = i;

            ImGui::PopStyleColor(3);

            if (active)
            {
                const ImVec2 a = ImGui::GetItemRectMin();
                const ImVec2 b = ImGui::GetItemRectMax();

                ImGui::GetWindowDrawList()->AddRectFilled(
                    ImVec2(a.x + 10.0f, b.y + 1.0f), ImVec2(b.x - 10.0f, b.y + 3.0f),
                    ImGui::GetColorU32(catTones[i]), 1.0f);
            }

            if (i + 1 < 4)
            {
                const float rightEdge = ImGui::GetCursorScreenPos().x +
                                        ImGui::GetContentRegionAvail().x;

                if (ImGui::GetItemRectMax().x +
                    ImGui::GetStyle().ItemSpacing.x + 160.0f <= rightEdge)
                    ImGui::SameLine();
            }
        }
    }

    ImGui::SetNextItemWidth(300.0f);
    ImGui::InputTextWithHint("##sigsearch", "Search signatures", search, IM_ARRAYSIZE(search));

    ImGui::SameLine();

    if (ImGui::Button("Reload"))
        signatures = FileScanner::getSignatures();   // re-reads hids_hashes.txt

    ImGui::Spacing();

    // ================================================================ table
    if (ImGui::BeginTable(
            "SignatureTable", 4,
            ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH |
            ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable |
            ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_ScrollY,
            ImVec2(0.0f, std::max(260.0f, ImGui::GetContentRegionAvail().y))))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 160.0f, 0);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 1.0f, 1);
        ImGui::TableSetupColumn("Pattern / condition",
            ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoSort, 1.4f, 2);
        ImGui::TableSetupColumn("Result",
            ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_PreferSortDescending,
            190.0f, 3);
        ImGui::TableHeadersRow();

        std::vector<int> order(signatures.size());

        for (std::size_t i = 0; i < order.size(); ++i)
            order[i] = (int)i;

        if (ImGuiTableSortSpecs* specs = ImGui::TableGetSortSpecs())
        {
            if (specs->SpecsCount > 0)
            {
                const ImGuiTableColumnSortSpecs& spec = specs->Specs[0];
                const bool asc = spec.SortDirection == ImGuiSortDirection_Ascending;

                std::stable_sort(order.begin(), order.end(),
                    [&](int a, int b)
                    {
                        const SignatureInfo& sa = signatures[a];
                        const SignatureInfo& sb = signatures[b];

                        int cmp = 0;

                        switch (spec.ColumnUserID)
                        {
                        case 0: cmp = sa.category.compare(sb.category); break;
                        case 1: cmp = sa.name.compare(sb.name); break;
                        case 3:
                        {
                            const int wa = sa.instantMalicious ? 100 : sa.weight;
                            const int wb = sb.instantMalicious ? 100 : sb.weight;
                            cmp = (wa > wb) - (wa < wb);
                            break;
                        }
                        }

                        if (cmp == 0)
                            return false;

                        return asc ? (cmp < 0) : (cmp > 0);
                    });
            }
        }

        int shown = 0;

        for (int index : order)
        {
            const SignatureInfo& s = signatures[index];

            if (catFilter != 0 && categoryIndex(s.category) != catFilter)
                continue;

            if (!ContainsNoCase(s.name.c_str(), search) &&
                !ContainsNoCase(s.pattern.c_str(), search) &&
                !ContainsNoCase(s.category.c_str(), search))
                continue;

            ++shown;

            ImGui::PushID(index);
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            {
                const ImVec4 tc =
                    s.category == "Known hash"   ? COLOR_RED :
                    s.category == "File content" ? COLOR_CYAN : COLOR_YELLOW;

                const ImVec2 p = ImGui::GetCursorScreenPos();
                ImGui::GetWindowDrawList()->AddCircleFilled(
                    ImVec2(p.x + 4.0f, p.y + ImGui::GetTextLineHeight() * 0.5f),
                    3.4f, ImGui::GetColorU32(tc), 12);

                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 14.0f);
                ImGui::TextColored(tc, "%s", s.category.c_str());
            }

            ImGui::TableSetColumnIndex(1);
            ImGui::TextWrapped("%s", s.name.c_str());

            ImGui::TableSetColumnIndex(2);
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
            ImGui::TextWrapped("%s", s.pattern.c_str());
            ImGui::PopStyleColor();

            // Result as a weight bar
            ImGui::TableSetColumnIndex(3);

            if (s.instantMalicious)
            {
                DrawUsageBar(1.0f, "Malicious", COLOR_RED, 18.0f);
            }
            else
            {
                const ImVec4 tone =
                    s.weight >= 5 ? COLOR_RED :
                    s.weight >= 4 ? COLOR_ORANGE :
                    s.weight >= 2 ? COLOR_YELLOW : COLOR_CYAN;

                char lab[32];

                if (s.weight >= threshold)
                    std::snprintf(lab, sizeof(lab), "+%d  flags alone", s.weight);
                else
                    std::snprintf(lab, sizeof(lab), "+%d  score", s.weight);

                DrawUsageBar(ClampFloat((float)s.weight / 6.0f, 0.08f, 1.0f), lab, tone, 18.0f);
            }

            ImGui::PopID();
        }

        if (shown == 0)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(1);
            ImGui::TextDisabled("No signature matches the current filter.");
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

    // Page content eases up into place when the page opens
    {
        const float k = Smooth01((float)((ImGui::GetTime() - screenChangedAt) / 0.30));
        const float slide = (1.0f - k) * 16.0f;

        if (slide > 0.5f)
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + slide);
    }

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

// ------------------------------------------------------------
// STATUS BAR - thin live strip along the bottom of every screen
// (protection state, CPU, RAM, alerts, clock)
// ------------------------------------------------------------

static void DrawStatusBar()
{
    if (appScreen == SCREEN_WELCOME)
        return;

    ImDrawList*  d    = ImGui::GetForegroundDrawList();
    const ImVec2 disp = ImGui::GetIO().DisplaySize;
    const float  h    = 26.0f;
    const ImVec2 a(0.0f, disp.y - h);
    const float  lh   = ImGui::GetTextLineHeight();
    const float  ty   = a.y + (h - lh) * 0.5f;
    const float  cy   = a.y + h * 0.5f;

    const ImVec4 pc = monitoringActive ? COLOR_GREEN : COLOR_YELLOW;

    d->AddRectFilled(a, disp, IM_COL32(6, 10, 18, 235));
    d->AddLine(a, ImVec2(disp.x, a.y), Col(COLOR_CYAN, 0.25f), 1.0f);

    float x = 16.0f;

    // Protection state
    {
        const float wave = 0.5f + 0.5f * std::sin((float)ImGui::GetTime() * 3.0f);

        if (monitoringActive)
            d->AddCircleFilled(ImVec2(x + 4.0f, cy), 4.0f + wave * 3.0f,
                Col(pc, 0.20f * (1.0f - wave) + 0.05f));

        d->AddCircleFilled(ImVec2(x + 4.0f, cy), 4.0f, ImGui::GetColorU32(pc));
        x += 18.0f;

        const char* state = monitoringActive ? "Protection active" : "Protection stopped";
        d->AddText(ImVec2(x, ty), ImGui::GetColorU32(pc), state);
        x += ImGui::CalcTextSize(state).x + 28.0f;
    }

    // CPU / RAM mini bars
    auto metric = [&](const char* label, float pct, const ImVec4& mc)
    {
        d->AddText(ImVec2(x, ty), ImGui::GetColorU32(COLOR_MUTED), label);
        x += ImGui::CalcTextSize(label).x + 8.0f;

        const float bw = 56.0f;
        const float by = cy - 3.0f;

        d->AddRectFilled(ImVec2(x, by), ImVec2(x + bw, by + 6.0f),
            Col(FX_WHITE, 0.10f), 3.0f);
        d->AddRectFilled(ImVec2(x, by),
            ImVec2(x + std::max(4.0f, bw * ClampFloat(pct / 100.0f, 0.0f, 1.0f)), by + 6.0f),
            ImGui::GetColorU32(mc), 3.0f);
        x += bw + 8.0f;

        char v[16];
        std::snprintf(v, sizeof(v), "%.0f%%", pct);
        d->AddText(ImVec2(x, ty), ImGui::GetColorU32(COLOR_TEXT), v);
        x += ImGui::CalcTextSize(v).x + 28.0f;
    };

    metric("CPU", cpuNow, LoadColor(cpuNow));
    metric("RAM", ramNow, LoadColor(ramNow));

    // Alerts
    {
        char v[48];
        std::snprintf(v, sizeof(v), "Alerts  %d", (int)alerts.size());

        const ImVec4 ac = CountSeverity("Critical") > 0 ? COLOR_RED
                        : (alerts.empty() ? COLOR_GREEN : COLOR_YELLOW);

        d->AddCircleFilled(ImVec2(x + 4.0f, cy), 3.5f, ImGui::GetColorU32(ac));
        d->AddText(ImVec2(x + 14.0f, ty), ImGui::GetColorU32(ac), v);
    }

    // Clock + hint on the right
    {
        std::time_t raw = std::time(nullptr);
        std::tm     tmv = {};

#ifdef _WIN32
        localtime_s(&tmv, &raw);
#else
        localtime_r(&raw, &tmv);
#endif

        char clock[32];
        std::strftime(clock, sizeof(clock), "%H:%M:%S", &tmv);

        const char* hint = "Esc = back";
        const float cw   = ImGui::CalcTextSize(clock).x;
        const float hw   = ImGui::CalcTextSize(hint).x;

        d->AddText(ImVec2(disp.x - 16.0f - cw, ty), ImGui::GetColorU32(COLOR_TEXT), clock);
        d->AddText(ImVec2(disp.x - 16.0f - cw - 24.0f - hw, ty),
            ImGui::GetColorU32(COLOR_MUTED), hint);
    }
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

    // Keep the layout from being squeezed below what the cards need
    glfwSetWindowSizeLimits(window, 1100, 720, GLFW_DONT_CARE, GLFW_DONT_CARE);

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
            // The last 30 px stay free for the status bar.
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));

            ImGui::BeginChild(
                "MainContent",
                ImVec2(0.0f, -30.0f),
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

        // In-app toast for brand-new alerts (drawn above everything)
        DrawToast();

        // Live status strip along the bottom edge
        DrawStatusBar();

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
