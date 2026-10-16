


// #include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"
 
// #include <GLFW/glfw3.h>
 
// #include <algorithm>
// #include <cctype>
// #include <cfloat>
// #include <cmath>
// #include <cstdio>
// #include <cstdlib>
// #include <cstring>
// #include <string>
// #include <vector>

// #include "HIDSBackend.h"
 
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
// };

// struct AlertInfo {
//     std::string time;
//     std::string severity;
//     std::string source;
//     std::string description;
// };

// struct EventInfo {
//     std::string time;
//     std::string type;
//     std::string description;
// };

// // ------------------------------------------------------------
// // LIVE DATA
// // The backend runs the monitors on its own threads.
// // RefreshFromBackend() copies its latest state into these.
// // ------------------------------------------------------------

// static HIDSBackend backend;

// static std::vector<ProcessInfo> processes;
// static std::vector<AlertInfo>   alerts;
// static std::vector<EventInfo>   loginEvents;
// static std::vector<EventInfo>   fileEvents;
// static int processCount = 0;

// // ------------------------------------------------------------
// // GLOBAL STATE
// // ------------------------------------------------------------
 
// static int  selectedPage = 0;
// static char processSearch[128] = "";
// static bool monitoringActive = true;
 
// static int  severityFilter = 0;   // 0 = All, 1..4 = Critical..Low
// static const char* severityFilterLabels[] =
//     { "All", "Critical", "High", "Medium", "Low" };
 
// static const char* pages[] = {
//     "Dashboard",
//     "Processes",
//     "Login Monitoring",
//     "File Monitoring",
//     "Alerts",
//     "Logs & Reports",
//     "System Analysis",
//     "Settings"
// };
 
// static const char* pageSubtitles[] = {
//     "Overview of this machine's security status",
//     "Everything currently running on this host",
//     "Sign-in activity on this host",
//     "Changes to monitored files and folders",
//     "Events raised by the HIDS monitors",
//     "Saved logs and generated reports",
//     "Deeper inspection of system behaviour",
//     "Configure how HIDS monitors this host"
// };
 
// // Live metrics (ring buffers feed the sparklines)
// static const int HISTORY_SIZE = 60;
// static float cpuHistory[HISTORY_SIZE] = {};
// static float ramHistory[HISTORY_SIZE] = {};
// static int   historyOffset = 0;
// static float cpuNow = 0.0f;
// static float ramNow = 0.0f;
 
// // ------------------------------------------------------------
// // COLORS AND STYLE
// // ------------------------------------------------------------
 
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
 
// static float ClampFloat(float v, float lo, float hi)
// {
//     return v < lo ? lo : (v > hi ? hi : v);
// }
 
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
// // HELPERS
// // ------------------------------------------------------------
 
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
//     const ImVec2 size(textSize.x + pad.x * 2.0f,
//                       textSize.y + pad.y * 2.0f);
 
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
//     const ImVec2 center(pos.x + radius + 2.0f,
//                         pos.y + ImGui::GetFrameHeight() * 0.5f);
 
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
 
// // ------------------------------------------------------------
// // BACKEND REFRESH
// // Copies the latest snapshots from the backend threads into the
// // vectors the UI draws from. Runs twice per second on the UI
// // thread and only handles copies, so it never waits on a monitor.
// // ------------------------------------------------------------

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

//     // The sparklines only get a new point when the backend has a new sample
//     if (stats.sample != lastSample)
//     {
//         lastSample = stats.sample;

//         cpuHistory[historyOffset] = cpuNow;
//         ramHistory[historyOffset] = ramNow;
//         historyOffset = (historyOffset + 1) % HISTORY_SIZE;
//     }

//     // Processes
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

//         processes.push_back(row);
//     }

//     // Alerts (already newest first)
//     alerts.clear();

//     for (const BackendAlert& a : backend.getAlerts())
//     {
//         AlertInfo row;

//         row.time        = a.time;
//         row.severity    = a.severity;
//         row.source      = a.source;
//         row.description = a.description;

//         alerts.push_back(row);
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
// }
 
// // ------------------------------------------------------------
// // METRIC CARD
// // Accent stripe on the left, big value, optional sparkline.
// // ------------------------------------------------------------
 
// static void MetricCard(
//     const char* title,
//     const char* value,
//     const char* subtitle,
//     const ImVec4& accent,
//     float width,
//     const float* history = nullptr)
// {
//     ImGui::BeginChild(
//         title,
//         ImVec2(width, 132.0f),
//         true,
//         ImGuiWindowFlags_NoScrollbar
//     );
 
//     // Accent stripe
//     {
//         const ImVec2 pos = ImGui::GetWindowPos();
//         const ImVec2 size = ImGui::GetWindowSize();
 
//         ImGui::GetWindowDrawList()->AddRectFilled(
//             pos,
//             ImVec2(pos.x + 4.0f, pos.y + size.y),
//             ImGui::GetColorU32(accent),
//             ImGui::GetStyle().ChildRounding,
//             ImDrawFlags_RoundCornersLeft
//         );
//     }
 
//     ImGui::TextDisabled("%s", title);
 
//     ImGui::PushStyleColor(ImGuiCol_Text, accent);
//     ImGui::SetWindowFontScale(1.55f);
//     ImGui::TextUnformatted(value);
//     ImGui::SetWindowFontScale(1.0f);
//     ImGui::PopStyleColor();
 
//     if (history != nullptr)
//     {
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
//     }
//     else
//     {
//         ImGui::Spacing();
//         ImGui::TextDisabled("%s", subtitle);
//     }
 
//     ImGui::EndChild();
// }
 
// // ------------------------------------------------------------
// // SIDEBAR
// // ------------------------------------------------------------
 
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
//             ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.65f, 0.74f, 0.82f, 1.0f));
 
//         // Leading spaces leave room for the accent bar
//         char label[64];
//         std::snprintf(label, sizeof(label), "   %s", pages[i]);
 
//         if (ImGui::Selectable(label, selected, 0, ImVec2(0.0f, 38.0f)))
//             selectedPage = i;
 
//         if (!selected)
//             ImGui::PopStyleColor();
 
//         const ImVec2 itemMin = ImGui::GetItemRectMin();
//         const ImVec2 itemMax = ImGui::GetItemRectMax();
//         ImDrawList* draw = ImGui::GetWindowDrawList();
 
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
 
//     ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 52.0f);
 
//     ImGui::Separator();
//     ImGui::TextDisabled("HIDS Prototype v1.1");
 
//     ImGui::EndChild();
 
//     ImGui::PopStyleColor();
// }
 
// // ------------------------------------------------------------
// // TOP BAR
// // ------------------------------------------------------------
 
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
 
// // ------------------------------------------------------------
// // DASHBOARD METRICS
// // ------------------------------------------------------------
 
// static void DrawMetrics()
// {
//     const float spacing = ImGui::GetStyle().ItemSpacing.x;
 
//     const float cardWidth =
//         (ImGui::GetContentRegionAvail().x - spacing * 3.0f) / 4.0f;
 
//     char cpuText[16];
//     char ramText[16];
//     char alertText[16];
 
//     std::snprintf(cpuText, sizeof(cpuText), "%.0f%%", cpuNow);
//     std::snprintf(ramText, sizeof(ramText), "%.0f%%", ramNow);
//     std::snprintf(alertText, sizeof(alertText), "%d", (int)alerts.size());
 
//     // Alerts card turns red when anything critical is present
//     const ImVec4 alertColor =
//         CountSeverity("Critical") > 0 ? COLOR_RED : COLOR_YELLOW;
 
//     char alertSubtitle[64];
//     std::snprintf(
//         alertSubtitle, sizeof(alertSubtitle),
//         "%d critical, %d high",
//         CountSeverity("Critical"), CountSeverity("High")
//     );
 
//     MetricCard("CPU usage", cpuText, "", LoadColor(cpuNow), cardWidth, cpuHistory);
//     ImGui::SameLine();
 
//     MetricCard("RAM usage", ramText, "", LoadColor(ramNow), cardWidth, ramHistory);
//     ImGui::SameLine();
 
//     char processText[16];
//     std::snprintf(processText, sizeof(processText), "%d", processCount);

//     MetricCard("Running processes", processText, "On this host", COLOR_BLUE, cardWidth);
//     ImGui::SameLine();
 
//     MetricCard("Total alerts", alertText, alertSubtitle, alertColor, cardWidth);
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
 
//     // Severity filter (full page only)
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
//                 ImGui::PushStyleColor(
//                     ImGuiCol_Button,
//                     ImVec4(0.07f, 0.12f, 0.17f, 1.0f)
//                 );
//             }
 
//             if (ImGui::Button(severityFilterLabels[i], ImVec2(96.0f, 0.0f)))
//                 severityFilter = i;
 
//             ImGui::PopStyleColor();
 
//             if (i + 1 < IM_ARRAYSIZE(severityFilterLabels))
//                 ImGui::SameLine();
//         }
 
//         ImGui::Spacing();
//     }
 
//     if (ImGui::BeginTable(
//             "AlertsTable",
//             4,
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
//         ImGui::TableHeadersRow();
 
//         int shown = 0;
 
//         for (const auto& alert : alerts)
//         {
//             if (!compact && severityFilter != 0 &&
//                 alert.severity != severityFilterLabels[severityFilter])
//             {
//                 continue;
//             }
 
//             ++shown;
 
//             ImGui::TableNextRow();
 
//             ImGui::TableSetColumnIndex(0);
//             ImGui::TextDisabled("%s", alert.time.c_str());
 
//             ImGui::TableSetColumnIndex(1);
//             SeverityBadge(alert.severity.c_str());
 
//             ImGui::TableSetColumnIndex(2);
//             ImGui::TextUnformatted(alert.source.c_str());
 
//             ImGui::TableSetColumnIndex(3);
//             ImGui::TextUnformatted(alert.description.c_str());
//         }
 
//         if (shown == 0)
//         {
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(3);
//             ImGui::TextDisabled("No alerts with this severity.");
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
 
//     SectionHeading("Running processes");
 
//     ImGui::SetNextItemWidth(300.0f);
 
//     ImGui::InputTextWithHint(
//         "##ProcessSearch",
//         "Search by name or PID",
//         processSearch,
//         IM_ARRAYSIZE(processSearch)
//     );
 
//     ImGui::Spacing();
 
//     if (ImGui::BeginTable(
//             "ProcessTable",
//             5,
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
//             ImGuiTableColumnFlags_DefaultSort |
//             ImGuiTableColumnFlags_PreferSortDescending,
//             180.0f, 2);
//         ImGui::TableSetupColumn(
//             "Memory MB",
//             ImGuiTableColumnFlags_WidthFixed |
//             ImGuiTableColumnFlags_PreferSortDescending,
//             125.0f, 3);
//         ImGui::TableSetupColumn("Running time", ImGuiTableColumnFlags_WidthFixed, 135.0f, 4);
 
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
 
//             // CPU bar, scaled so the busiest realistic process fills it
//             ImGui::TableSetColumnIndex(2);
 
//             char cpuLabel[16];
//             std::snprintf(cpuLabel, sizeof(cpuLabel), "%.1f", process.cpu);
 
//             ImGui::PushStyleColor(ImGuiCol_PlotHistogram, WithAlpha(LoadColor(process.cpu * 4.0f), 0.85f));
//             ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.07f, 0.12f, 0.17f, 1.0f));
 
//             ImGui::ProgressBar(
//                 std::min(process.cpu / 25.0f, 1.0f),
//                 ImVec2(-FLT_MIN, 18.0f),
//                 cpuLabel
//             );
 
//             ImGui::PopStyleColor(2);
 
//             ImGui::TableSetColumnIndex(3);
//             ImGui::Text("%.1f", process.memory);
 
//             ImGui::TableSetColumnIndex(4);
//             ImGui::TextDisabled("%s", process.runningTime.c_str());
//         }
 
//         if (shown == 0)
//         {
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(1);
//             ImGui::TextDisabled("No process matches \"%s\".", processSearch);
//         }
 
//         ImGui::EndTable();
//     }
 
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

// // ------------------------------------------------------------
// // PLACEHOLDER PAGES
// // Centered message explaining what the page will show.
// // ------------------------------------------------------------
 
// static void DrawPlaceholderPage(const char* title, const char* willShow)
// {
//     ImGui::BeginChild(
//         "PagePanel",
//         ImVec2(0.0f, 0.0f),
//         true
//     );
 
//     const float availWidth = ImGui::GetContentRegionAvail().x;
//     const float availHeight = ImGui::GetContentRegionAvail().y;
//     const float wrapWidth = std::min(availWidth - 40.0f, 460.0f);
 
//     ImGui::SetCursorPosY(ImGui::GetCursorPosY() + availHeight * 0.30f);
 
//     // Title
//     ImGui::SetWindowFontScale(1.35f);
//     {
//         const char* heading = "Not connected yet";
//         const float w = ImGui::CalcTextSize(heading).x;
 
//         ImGui::SetCursorPosX((ImGui::GetWindowWidth() - w) * 0.5f);
//         ImGui::PushStyleColor(ImGuiCol_Text, COLOR_CYAN);
//         ImGui::TextUnformatted(heading);
//         ImGui::PopStyleColor();
//     }
//     ImGui::SetWindowFontScale(1.0f);
 
//     ImGui::Spacing();
 
//     // Body
//     char body[256];
//     std::snprintf(
//         body, sizeof(body),
//         "%s will show %s once it is linked to its HIDS monitoring module.",
//         title, willShow
//     );
 
//     ImGui::SetCursorPosX((ImGui::GetWindowWidth() - wrapWidth) * 0.5f);
//     ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + wrapWidth);
//     ImGui::PushStyleColor(ImGuiCol_Text, COLOR_MUTED);
//     ImGui::TextWrapped("%s", body);
//     ImGui::PopStyleColor();
//     ImGui::PopTextWrapPos();
 
//     ImGui::EndChild();
// }
 
// // ------------------------------------------------------------
// // MAIN DASHBOARD CONTENT
// // ------------------------------------------------------------
 
// static void DrawMainContent()
// {
//     RefreshFromBackend();
 
//     DrawTopBar();
//     ImGui::Separator();
//     ImGui::Spacing();
 
//     switch (selectedPage)
//     {
//     case 0:
//         DrawMetrics();
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
//         DrawPlaceholderPage("Logs & Reports",
//             "saved event logs and exportable reports");
//         break;
 
//     case 6:
//         DrawPlaceholderPage("System Analysis",
//             "detailed system and process analysis");
//         break;
 
//     case 7:
//         DrawPlaceholderPage("Settings",
//             "monitoring rules, paths and notification options");
//         break;
//     }
// }
 
// // ------------------------------------------------------------
// // MAIN FUNCTION
// // ------------------------------------------------------------
 
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
 
//     IMGUI_CHECKVERSION();
//     ImGui::CreateContext();
 
//     ImGuiIO& io = ImGui::GetIO();
 
//     io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
 
//     // Don't write imgui.ini next to the executable
//     io.IniFilename = nullptr;
 
//     LoadFonts(io);
//     SetupStyle();
 
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
 
//         ImGui_ImplOpenGL3_NewFrame();
//         ImGui_ImplGlfw_NewFrame();
//         ImGui::NewFrame();
 
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
 
//         ImGui::Render();
 
//         int displayWidth = 0;
//         int displayHeight = 0;
 
//         glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
//         glViewport(0, 0, displayWidth, displayHeight);
 
//         glClearColor(
//             COLOR_BACKGROUND.x,
//             COLOR_BACKGROUND.y,
//             COLOR_BACKGROUND.z,
//             COLOR_BACKGROUND.w
//         );
 
//         glClear(GL_COLOR_BUFFER_BIT);
 
//         ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
 
//         glfwSwapBuffers(window);
//     }
 
//     // Clean shutdown (monitors first, so no thread outlives the UI)
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
 
#include <algorithm>
#include <cctype>
#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
 
#include "HIDSBackend.h"
 
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
};
 
struct AlertInfo {
    std::string time;
    std::string severity;
    std::string source;
    std::string description;
};
 
struct EventInfo {
    std::string time;
    std::string type;
    std::string description;
};
 
// ------------------------------------------------------------
// LIVE DATA
// The backend runs the monitors on its own threads.
// RefreshFromBackend() copies its latest state into these.
// ------------------------------------------------------------
 
static HIDSBackend backend;
 
static std::vector<ProcessInfo> processes;
static std::vector<AlertInfo>   alerts;
static std::vector<EventInfo>   loginEvents;
static std::vector<EventInfo>   fileEvents;
static BackendAnalysis          analysisData;
static int processCount = 0;
 
// ------------------------------------------------------------
// GLOBAL STATE
// ------------------------------------------------------------
 
static int  selectedPage = 0;
static char processSearch[128] = "";
static bool monitoringActive = true;
 
static int  severityFilter = 0;   // 0 = All, 1..4 = Critical..Low
static const char* severityFilterLabels[] =
    { "All", "Critical", "High", "Medium", "Low" };
 
static const char* pages[] = {
    "Dashboard",
    "Processes",
    "Login Monitoring",
    "File Monitoring",
    "Alerts",
    "Logs & Reports",
    "System Analysis",
    "Settings"
};
 
static const char* pageSubtitles[] = {
    "Overview of this machine's security status",
    "Everything currently running on this host",
    "Sign-in activity on this host",
    "Changes to monitored files and folders",
    "Events raised by the HIDS monitors",
    "Saved logs and generated reports",
    "Deeper inspection of system behaviour",
    "Configure how HIDS monitors this host"
};
 
// Live metrics (ring buffers feed the sparklines)
static const int HISTORY_SIZE = 60;
static float cpuHistory[HISTORY_SIZE] = {};
static float ramHistory[HISTORY_SIZE] = {};
static int   historyOffset = 0;
static float cpuNow = 0.0f;
static float ramNow = 0.0f;
 
// ------------------------------------------------------------
// COLORS AND STYLE
// ------------------------------------------------------------
 
static const ImVec4 COLOR_BACKGROUND = ImVec4(0.025f, 0.045f, 0.065f, 1.0f);
static const ImVec4 COLOR_SIDEBAR    = ImVec4(0.020f, 0.036f, 0.054f, 1.0f);
static const ImVec4 COLOR_PANEL      = ImVec4(0.035f, 0.065f, 0.090f, 1.0f);
static const ImVec4 COLOR_BORDER     = ImVec4(0.09f, 0.20f, 0.28f, 1.0f);
static const ImVec4 COLOR_BLUE       = ImVec4(0.10f, 0.55f, 0.95f, 1.0f);
static const ImVec4 COLOR_CYAN       = ImVec4(0.0f, 0.75f, 1.0f, 1.0f);
static const ImVec4 COLOR_GREEN      = ImVec4(0.0f, 0.85f, 0.55f, 1.0f);
static const ImVec4 COLOR_RED        = ImVec4(1.0f, 0.25f, 0.30f, 1.0f);
static const ImVec4 COLOR_ORANGE     = ImVec4(1.0f, 0.50f, 0.15f, 1.0f);
static const ImVec4 COLOR_YELLOW     = ImVec4(1.0f, 0.78f, 0.15f, 1.0f);
static const ImVec4 COLOR_TEXT       = ImVec4(0.88f, 0.93f, 0.97f, 1.0f);
static const ImVec4 COLOR_MUTED      = ImVec4(0.50f, 0.59f, 0.66f, 1.0f);
 
static float ClampFloat(float v, float lo, float hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}
 
static ImVec4 WithAlpha(const ImVec4& c, float a)
{
    return ImVec4(c.x, c.y, c.z, a);
}
 
static void SetupStyle()
{
    ImGui::StyleColorsDark();
 
    ImGuiStyle& style = ImGui::GetStyle();
 
    style.WindowPadding    = ImVec2(16.0f, 16.0f);
    style.FramePadding     = ImVec2(10.0f, 7.0f);
    style.ItemSpacing      = ImVec2(10.0f, 10.0f);
    style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);
    style.CellPadding      = ImVec2(10.0f, 8.0f);
    style.ScrollbarSize    = 12.0f;
 
    style.WindowRounding    = 0.0f;
    style.ChildRounding     = 10.0f;
    style.FrameRounding     = 6.0f;
    style.PopupRounding     = 6.0f;
    style.ScrollbarRounding = 8.0f;
    style.GrabRounding      = 6.0f;
    style.TabRounding       = 6.0f;
 
    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize  = 1.0f;
    style.FrameBorderSize  = 0.0f;
 
    ImVec4* colors = style.Colors;
 
    colors[ImGuiCol_WindowBg] = COLOR_BACKGROUND;
    colors[ImGuiCol_ChildBg]  = COLOR_PANEL;
    colors[ImGuiCol_PopupBg]  = COLOR_PANEL;
    colors[ImGuiCol_Border]   = COLOR_BORDER;
 
    colors[ImGuiCol_Text]         = COLOR_TEXT;
    colors[ImGuiCol_TextDisabled] = COLOR_MUTED;
 
    colors[ImGuiCol_FrameBg]        = ImVec4(0.07f, 0.12f, 0.17f, 1.0f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.10f, 0.22f, 0.31f, 1.0f);
    colors[ImGuiCol_FrameBgActive]  = ImVec4(0.10f, 0.28f, 0.40f, 1.0f);
 
    colors[ImGuiCol_Button]        = ImVec4(0.06f, 0.28f, 0.43f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.08f, 0.42f, 0.62f, 1.0f);
    colors[ImGuiCol_ButtonActive]  = COLOR_BLUE;
 
    colors[ImGuiCol_Header]        = ImVec4(0.05f, 0.23f, 0.35f, 0.70f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.08f, 0.34f, 0.48f, 0.80f);
    colors[ImGuiCol_HeaderActive]  = COLOR_BLUE;
 
    colors[ImGuiCol_TableHeaderBg]      = ImVec4(0.06f, 0.12f, 0.18f, 1.0f);
    colors[ImGuiCol_TableBorderStrong]  = COLOR_BORDER;
    colors[ImGuiCol_TableBorderLight]   = ImVec4(0.08f, 0.14f, 0.19f, 1.0f);
    colors[ImGuiCol_TableRowBg]         = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_TableRowBgAlt]      = ImVec4(1.0f, 1.0f, 1.0f, 0.025f);
 
    colors[ImGuiCol_Separator]       = COLOR_BORDER;
    colors[ImGuiCol_ScrollbarBg]     = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_ScrollbarGrab]   = ImVec4(0.15f, 0.28f, 0.38f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.20f, 0.38f, 0.50f, 1.0f);
 
    colors[ImGuiCol_CheckMark]        = COLOR_GREEN;
    colors[ImGuiCol_SliderGrab]       = COLOR_BLUE;
    colors[ImGuiCol_SliderGrabActive] = COLOR_CYAN;
 
    colors[ImGuiCol_PlotLines]     = COLOR_CYAN;
    colors[ImGuiCol_PlotHistogram] = COLOR_CYAN;
}
 
// ------------------------------------------------------------
// FONT LOADING
// Uses a real TTF if one is found; otherwise scales the default
// bitmap font so the dashboard is still readable.
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
// HELPERS
// ------------------------------------------------------------
 
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
    const ImVec2 size(textSize.x + pad.x * 2.0f,
                      textSize.y + pad.y * 2.0f);
 
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
    const ImVec2 center(pos.x + radius + 2.0f,
                        pos.y + ImGui::GetFrameHeight() * 0.5f);
 
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
 
// ------------------------------------------------------------
// BACKEND REFRESH
// Copies the latest snapshots from the backend threads into the
// vectors the UI draws from. Runs twice per second on the UI
// thread and only handles copies, so it never waits on a monitor.
// ------------------------------------------------------------
 
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
 
    // The sparklines only get a new point when the backend has a new sample
    if (stats.sample != lastSample)
    {
        lastSample = stats.sample;
 
        cpuHistory[historyOffset] = cpuNow;
        ramHistory[historyOffset] = ramNow;
        historyOffset = (historyOffset + 1) % HISTORY_SIZE;
    }
 
    // System Analysis results
    analysisData = backend.getAnalysis();
 
    // Processes
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
 
        processes.push_back(row);
    }
 
    // Alerts (already newest first)
    alerts.clear();
 
    for (const BackendAlert& a : backend.getAlerts())
    {
        AlertInfo row;
 
        row.time        = a.time;
        row.severity    = a.severity;
        row.source      = a.source;
        row.description = a.description;
 
        alerts.push_back(row);
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
}
 
// ------------------------------------------------------------
// METRIC CARD
// Accent stripe on the left, big value, optional sparkline.
// ------------------------------------------------------------
 
static void MetricCard(
    const char* title,
    const char* value,
    const char* subtitle,
    const ImVec4& accent,
    float width,
    const float* history = nullptr)
{
    ImGui::BeginChild(
        title,
        ImVec2(width, 132.0f),
        true,
        ImGuiWindowFlags_NoScrollbar
    );
 
    // Accent stripe
    {
        const ImVec2 pos = ImGui::GetWindowPos();
        const ImVec2 size = ImGui::GetWindowSize();
 
        ImGui::GetWindowDrawList()->AddRectFilled(
            pos,
            ImVec2(pos.x + 4.0f, pos.y + size.y),
            ImGui::GetColorU32(accent),
            ImGui::GetStyle().ChildRounding,
            ImDrawFlags_RoundCornersLeft
        );
    }
 
    ImGui::TextDisabled("%s", title);
 
    ImGui::PushStyleColor(ImGuiCol_Text, accent);
    ImGui::SetWindowFontScale(1.55f);
    ImGui::TextUnformatted(value);
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();
 
    if (history != nullptr)
    {
        ImGui::PushStyleColor(ImGuiCol_PlotLines, accent);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
 
        ImGui::PlotLines(
            "##spark",
            history,
            HISTORY_SIZE,
            historyOffset,
            nullptr,
            0.0f,
            100.0f,
            ImVec2(ImGui::GetContentRegionAvail().x, 26.0f)
        );
 
        ImGui::PopStyleColor(2);
    }
    else
    {
        ImGui::Spacing();
        ImGui::TextDisabled("%s", subtitle);
    }
 
    ImGui::EndChild();
}
 
// ------------------------------------------------------------
// SIDEBAR
// ------------------------------------------------------------
 
static void DrawSidebar()
{
    ImGui::PushStyleColor(ImGuiCol_ChildBg, COLOR_SIDEBAR);
 
    ImGui::BeginChild(
        "Sidebar",
        ImVec2(220.0f, 0.0f),
        true
    );
 
    ImGui::PushStyleColor(ImGuiCol_Text, COLOR_CYAN);
    ImGui::SetWindowFontScale(1.45f);
    ImGui::TextUnformatted("HIDS");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();
 
    ImGui::TextDisabled("Host Intrusion Detection");
 
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
 
    const int alertCount = (int)alerts.size();
 
    for (int i = 0; i < IM_ARRAYSIZE(pages); ++i)
    {
        ImGui::PushID(i);
 
        const bool selected = (selectedPage == i);
 
        if (!selected)
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.65f, 0.74f, 0.82f, 1.0f));
 
        // Leading spaces leave room for the accent bar
        char label[64];
        std::snprintf(label, sizeof(label), "   %s", pages[i]);
 
        if (ImGui::Selectable(label, selected, 0, ImVec2(0.0f, 38.0f)))
            selectedPage = i;
 
        if (!selected)
            ImGui::PopStyleColor();
 
        const ImVec2 itemMin = ImGui::GetItemRectMin();
        const ImVec2 itemMax = ImGui::GetItemRectMax();
        ImDrawList* draw = ImGui::GetWindowDrawList();
 
        if (selected)
        {
            draw->AddRectFilled(
                itemMin,
                ImVec2(itemMin.x + 4.0f, itemMax.y),
                ImGui::GetColorU32(COLOR_CYAN),
                2.0f
            );
        }
 
        // Alert count badge next to "Alerts"
        if (std::strcmp(pages[i], "Alerts") == 0 && alertCount > 0)
        {
            char count[16];
            std::snprintf(count, sizeof(count), "%d", alertCount);
 
            const ImVec2 ts = ImGui::CalcTextSize(count);
            const float w = ts.x + 14.0f;
            const float h = ts.y + 2.0f;
            const float x = itemMax.x - w - 8.0f;
            const float y = itemMin.y + (itemMax.y - itemMin.y - h) * 0.5f;
 
            draw->AddRectFilled(
                ImVec2(x, y), ImVec2(x + w, y + h),
                ImGui::GetColorU32(WithAlpha(COLOR_RED, 0.9f)),
                h * 0.5f
            );
 
            draw->AddText(
                ImVec2(x + 7.0f, y + 1.0f),
                IM_COL32(255, 255, 255, 255),
                count
            );
        }
 
        ImGui::PopID();
    }
 
    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 52.0f);
 
    ImGui::Separator();
    ImGui::TextDisabled("HIDS Prototype v1.1");
 
    ImGui::EndChild();
 
    ImGui::PopStyleColor();
}
 
// ------------------------------------------------------------
// TOP BAR
// ------------------------------------------------------------
 
static void DrawTopBar()
{
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
 
    ImGui::BeginChild(
        "TopBar",
        ImVec2(0.0f, 58.0f),
        false,
        ImGuiWindowFlags_NoScrollbar
    );
 
    ImGui::BeginGroup();
 
    ImGui::SetWindowFontScale(1.30f);
    ImGui::TextUnformatted(pages[selectedPage]);
    ImGui::SetWindowFontScale(1.0f);
 
    ImGui::TextDisabled("%s", pageSubtitles[selectedPage]);
 
    ImGui::EndGroup();
 
    // Right-aligned status + button
    const char* statusText =
        monitoringActive ? "Monitoring active" : "Monitoring stopped";
 
    const ImVec4 statusColor =
        monitoringActive ? COLOR_GREEN : COLOR_RED;
 
    const float statusWidth =
        ImGui::CalcTextSize(statusText).x + 24.0f;
 
    const float buttonWidth = 120.0f;
 
    ImGui::SameLine(
        ImGui::GetWindowWidth() - statusWidth - buttonWidth - 28.0f
    );
 
    ImGui::AlignTextToFramePadding();
    StatusDot(statusColor, monitoringActive);
 
    ImGui::SameLine(0.0f, 4.0f);
 
    ImGui::PushStyleColor(ImGuiCol_Text, statusColor);
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(statusText);
    ImGui::PopStyleColor();
 
    ImGui::SameLine();
 
    if (monitoringActive)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.70f, 0.14f, 0.18f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COLOR_RED);
 
        if (ImGui::Button("Stop HIDS", ImVec2(buttonWidth, 34.0f)))
        {
            backend.stop();
            monitoringActive = false;
        }
 
        ImGui::PopStyleColor(2);
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.55f, 0.36f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COLOR_GREEN);
 
        if (ImGui::Button("Start HIDS", ImVec2(buttonWidth, 34.0f)))
        {
            backend.start();
            monitoringActive = true;
        }
 
        ImGui::PopStyleColor(2);
    }
 
    ImGui::EndChild();
 
    ImGui::PopStyleColor();
}
 
// ------------------------------------------------------------
// DASHBOARD METRICS
// ------------------------------------------------------------
 
static void DrawMetrics()
{
    const float spacing = ImGui::GetStyle().ItemSpacing.x;
 
    const float cardWidth =
        (ImGui::GetContentRegionAvail().x - spacing * 3.0f) / 4.0f;
 
    char cpuText[16];
    char ramText[16];
    char alertText[16];
 
    std::snprintf(cpuText, sizeof(cpuText), "%.0f%%", cpuNow);
    std::snprintf(ramText, sizeof(ramText), "%.0f%%", ramNow);
    std::snprintf(alertText, sizeof(alertText), "%d", (int)alerts.size());
 
    // Alerts card turns red when anything critical is present
    const ImVec4 alertColor =
        CountSeverity("Critical") > 0 ? COLOR_RED : COLOR_YELLOW;
 
    char alertSubtitle[64];
    std::snprintf(
        alertSubtitle, sizeof(alertSubtitle),
        "%d critical, %d high",
        CountSeverity("Critical"), CountSeverity("High")
    );
 
    MetricCard("CPU usage", cpuText, "", LoadColor(cpuNow), cardWidth, cpuHistory);
    ImGui::SameLine();
 
    MetricCard("RAM usage", ramText, "", LoadColor(ramNow), cardWidth, ramHistory);
    ImGui::SameLine();
 
    char processText[16];
    std::snprintf(processText, sizeof(processText), "%d", processCount);
 
    MetricCard("Running processes", processText, "On this host", COLOR_BLUE, cardWidth);
    ImGui::SameLine();
 
    MetricCard("Total alerts", alertText, alertSubtitle, alertColor, cardWidth);
}
 
// ------------------------------------------------------------
// ALERT TABLE
// compact = true: dashboard preview. false: full Alerts page.
// ------------------------------------------------------------
 
static void DrawAlerts(bool compact)
{
    ImGui::BeginChild(
        "AlertsPanel",
        ImVec2(0.0f, compact ? 270.0f : 0.0f),
        true
    );
 
    SectionHeading(
        compact ? "Recent security alerts" : "All security alerts"
    );
 
    // Severity filter (full page only)
    if (!compact)
    {
        for (int i = 0; i < IM_ARRAYSIZE(severityFilterLabels); ++i)
        {
            const bool active = (severityFilter == i);
 
            if (active)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, COLOR_BLUE);
            }
            else
            {
                ImGui::PushStyleColor(
                    ImGuiCol_Button,
                    ImVec4(0.07f, 0.12f, 0.17f, 1.0f)
                );
            }
 
            if (ImGui::Button(severityFilterLabels[i], ImVec2(96.0f, 0.0f)))
                severityFilter = i;
 
            ImGui::PopStyleColor();
 
            if (i + 1 < IM_ARRAYSIZE(severityFilterLabels))
                ImGui::SameLine();
        }
 
        ImGui::Spacing();
    }
 
    if (ImGui::BeginTable(
            "AlertsTable",
            4,
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_BordersInnerH |
            ImGuiTableFlags_Resizable |
            ImGuiTableFlags_SizingStretchProp |
            ImGuiTableFlags_ScrollY))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 110.0f);
        ImGui::TableSetupColumn("Severity", ImGuiTableColumnFlags_WidthFixed, 130.0f);
        ImGui::TableSetupColumn("Source", ImGuiTableColumnFlags_WidthFixed, 200.0f);
        ImGui::TableSetupColumn("Description");
        ImGui::TableHeadersRow();
 
        int shown = 0;
 
        for (const auto& alert : alerts)
        {
            if (!compact && severityFilter != 0 &&
                alert.severity != severityFilterLabels[severityFilter])
            {
                continue;
            }
 
            ++shown;
 
            ImGui::TableNextRow();
 
            ImGui::TableSetColumnIndex(0);
            ImGui::TextDisabled("%s", alert.time.c_str());
 
            ImGui::TableSetColumnIndex(1);
            SeverityBadge(alert.severity.c_str());
 
            ImGui::TableSetColumnIndex(2);
            ImGui::TextUnformatted(alert.source.c_str());
 
            ImGui::TableSetColumnIndex(3);
            ImGui::TextUnformatted(alert.description.c_str());
        }
 
        if (shown == 0)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(3);
            ImGui::TextDisabled("No alerts with this severity.");
        }
 
        ImGui::EndTable();
    }
 
    ImGui::EndChild();
}
 
// ------------------------------------------------------------
// PROCESS TABLE
// Sortable columns, case-insensitive search, CPU usage bars.
// ------------------------------------------------------------
 
static void DrawProcesses()
{
    ImGui::BeginChild(
        "ProcessesPanel",
        ImVec2(0.0f, 0.0f),
        true
    );
 
    SectionHeading("Running processes");
 
    ImGui::SetNextItemWidth(300.0f);
 
    ImGui::InputTextWithHint(
        "##ProcessSearch",
        "Search by name or PID",
        processSearch,
        IM_ARRAYSIZE(processSearch)
    );
 
    ImGui::Spacing();
 
    if (ImGui::BeginTable(
            "ProcessTable",
            5,
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
            ImGuiTableColumnFlags_DefaultSort |
            ImGuiTableColumnFlags_PreferSortDescending,
            180.0f, 2);
        ImGui::TableSetupColumn(
            "Memory MB",
            ImGuiTableColumnFlags_WidthFixed |
            ImGuiTableColumnFlags_PreferSortDescending,
            125.0f, 3);
        ImGui::TableSetupColumn("Running time", ImGuiTableColumnFlags_WidthFixed, 135.0f, 4);
 
        ImGui::TableHeadersRow();
 
        // Build a sorted list of indices
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
 
            // CPU bar, scaled so the busiest realistic process fills it
            ImGui::TableSetColumnIndex(2);
 
            char cpuLabel[16];
            std::snprintf(cpuLabel, sizeof(cpuLabel), "%.1f", process.cpu);
 
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, WithAlpha(LoadColor(process.cpu * 4.0f), 0.85f));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.07f, 0.12f, 0.17f, 1.0f));
 
            ImGui::ProgressBar(
                std::min(process.cpu / 25.0f, 1.0f),
                ImVec2(-FLT_MIN, 18.0f),
                cpuLabel
            );
 
            ImGui::PopStyleColor(2);
 
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%.1f", process.memory);
 
            ImGui::TableSetColumnIndex(4);
            ImGui::TextDisabled("%s", process.runningTime.c_str());
        }
 
        if (shown == 0)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(1);
            ImGui::TextDisabled("No process matches \"%s\".", processSearch);
        }
 
        ImGui::EndTable();
    }
 
    ImGui::EndChild();
}
 
// ------------------------------------------------------------
// EVENT PAGE
// Newest-first activity list for Login and File monitoring.
// ------------------------------------------------------------
 
static void DrawEventPage(
    const char* title,
    const std::vector<EventInfo>& events,
    const char* emptyText,
    const char* hint)
{
    ImGui::BeginChild(
        title,
        ImVec2(0.0f, 0.0f),
        true
    );
 
    SectionHeading(title, hint);
 
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
        ImGui::TableSetupColumn("Event", ImGuiTableColumnFlags_WidthFixed, 170.0f);
        ImGui::TableSetupColumn("Description");
        ImGui::TableHeadersRow();
 
        for (const EventInfo& event : events)
        {
            ImGui::TableNextRow();
 
            ImGui::TableSetColumnIndex(0);
            ImGui::TextDisabled("%s", event.time.c_str());
 
            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(event.type.c_str());
 
            ImGui::TableSetColumnIndex(2);
            ImGui::TextUnformatted(event.description.c_str());
        }
 
        if (events.empty())
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(2);
            ImGui::TextDisabled("%s", emptyText);
        }
 
        ImGui::EndTable();
    }
 
    ImGui::EndChild();
}
 
// ------------------------------------------------------------
// SYSTEM ANALYSIS PAGE
// Resource usage, history graphs, findings and top processes,
// all produced by the System Analysis modules in the backend.
// ------------------------------------------------------------
 
static void DrawSystemAnalysis()
{
    ImGui::BeginChild("AnalysisPanel", ImVec2(0.0f, 0.0f), true);
 
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
 
    if (a.cpuAvailable)
        std::snprintf(cpuText, sizeof(cpuText), "%.1f%%", a.cpuPercent);
    else
        std::snprintf(cpuText, sizeof(cpuText), "n/a");
 
    if (a.memoryAvailable)
        std::snprintf(ramText, sizeof(ramText), "%.1f%%", a.memoryPercent);
    else
        std::snprintf(ramText, sizeof(ramText), "n/a");
 
    ImGui::Text("CPU:");
    ImGui::SameLine();
    ImGui::TextColored(LoadColor((float)a.cpuPercent), "%s", cpuText);
 
    ImGui::Text("Memory:");
    ImGui::SameLine();
    ImGui::TextColored(LoadColor((float)a.memoryPercent), "%s", ramText);
    ImGui::SameLine();
    ImGui::TextDisabled("(%llu MB used of %llu MB, %llu MB free)",
        (unsigned long long)a.usedMemoryMB,
        (unsigned long long)a.totalMemoryMB,
        (unsigned long long)a.availableMemoryMB);
 
    ImGui::Spacing();
 
    const float half =
        (ImGui::GetContentRegionAvail().x -
         ImGui::GetStyle().ItemSpacing.x) * 0.5f;
 
    if (!a.cpuHistory.empty())
    {
        ImGui::PlotLines(
            "##cpuh",
            a.cpuHistory.data(), (int)a.cpuHistory.size(),
            0, "CPU % (last 60 s)", 0.0f, 100.0f,
            ImVec2(half, 70.0f));
    }
 
    ImGui::SameLine();
 
    if (!a.ramHistory.empty())
    {
        ImGui::PlotLines(
            "##ramh",
            a.ramHistory.data(), (int)a.ramHistory.size(),
            0, "RAM % (last 60 s)", 0.0f, 100.0f,
            ImVec2(half, 70.0f));
    }
 
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
 
    // ---- Findings ----
    SectionHeading("Findings", "A diagnostic signal, not proof of an intrusion");
 
    ImGui::Text(
        "Processes: %llu    High CPU (>=80%%): %llu    High memory (>=1 GB): %llu",
        (unsigned long long)a.processCount,
        (unsigned long long)a.highCpuProcessCount,
        (unsigned long long)a.highMemoryProcessCount);
 
    if (a.findings.empty())
    {
        ImGui::TextColored(COLOR_GREEN, "No resource concerns detected.");
    }
    else
    {
        for (const std::string& finding : a.findings)
            ImGui::TextColored(COLOR_YELLOW, "- %s", finding.c_str());
    }
 
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
 
    // ---- Top CPU processes ----
    SectionHeading("Top processes by CPU");
 
    if (ImGui::BeginTable(
            "TopCpuTable",
            4,
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_BordersInnerH |
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
            ImGui::Text("%.1f", p.cpuPercent);
 
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%.1f", p.memoryMB);
        }
 
        ImGui::EndTable();
    }
 
    ImGui::EndChild();
}
 
// ------------------------------------------------------------
// PLACEHOLDER PAGES
// Centered message explaining what the page will show.
// ------------------------------------------------------------
 
static void DrawPlaceholderPage(const char* title, const char* willShow)
{
    ImGui::BeginChild(
        "PagePanel",
        ImVec2(0.0f, 0.0f),
        true
    );
 
    const float availWidth = ImGui::GetContentRegionAvail().x;
    const float availHeight = ImGui::GetContentRegionAvail().y;
    const float wrapWidth = std::min(availWidth - 40.0f, 460.0f);
 
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + availHeight * 0.30f);
 
    // Title
    ImGui::SetWindowFontScale(1.35f);
    {
        const char* heading = "Not connected yet";
        const float w = ImGui::CalcTextSize(heading).x;
 
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - w) * 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Text, COLOR_CYAN);
        ImGui::TextUnformatted(heading);
        ImGui::PopStyleColor();
    }
    ImGui::SetWindowFontScale(1.0f);
 
    ImGui::Spacing();
 
    // Body
    char body[256];
    std::snprintf(
        body, sizeof(body),
        "%s will show %s once it is linked to its HIDS monitoring module.",
        title, willShow
    );
 
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - wrapWidth) * 0.5f);
    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + wrapWidth);
    ImGui::PushStyleColor(ImGuiCol_Text, COLOR_MUTED);
    ImGui::TextWrapped("%s", body);
    ImGui::PopStyleColor();
    ImGui::PopTextWrapPos();
 
    ImGui::EndChild();
}
 
// ------------------------------------------------------------
// MAIN DASHBOARD CONTENT
// ------------------------------------------------------------
 
static void DrawMainContent()
{
    RefreshFromBackend();
 
    DrawTopBar();
    ImGui::Separator();
    ImGui::Spacing();
 
    switch (selectedPage)
    {
    case 0:
        DrawMetrics();
        ImGui::Spacing();
        DrawAlerts(true);
        ImGui::Spacing();
        DrawProcesses();
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
        DrawPlaceholderPage("Logs & Reports",
            "saved event logs and exportable reports");
        break;
 
    case 6:
        DrawSystemAnalysis();
        break;
 
    case 7:
        DrawPlaceholderPage("Settings",
            "monitoring rules, paths and notification options");
        break;
    }
}
 
// ------------------------------------------------------------
// MAIN FUNCTION
// ------------------------------------------------------------
 
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
 
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
 
    ImGuiIO& io = ImGui::GetIO();
 
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
 
    // Don't write imgui.ini next to the executable
    io.IniFilename = nullptr;
 
    LoadFonts(io);
    SetupStyle();
 
    // Seed sparkline history so the cards aren't empty on first frame
    for (int i = 0; i < HISTORY_SIZE; ++i)
    {
        cpuHistory[i] = cpuNow;
        ramHistory[i] = ramNow;
    }
 
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
 
    // Start the monitors (login, process, file) on background threads
    backend.start();
 
    // Main application loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
 
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
 
        // Full-window root
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
 
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
 
        DrawSidebar();
        ImGui::SameLine();
 
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
 
        ImGui::BeginChild(
            "MainContent",
            ImVec2(0.0f, 0.0f),
            false
        );
 
        DrawMainContent();
 
        ImGui::EndChild();
        ImGui::PopStyleColor();
 
        ImGui::End();
 
        ImGui::Render();
 
        int displayWidth = 0;
        int displayHeight = 0;
 
        glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
        glViewport(0, 0, displayWidth, displayHeight);
 
        glClearColor(
            COLOR_BACKGROUND.x,
            COLOR_BACKGROUND.y,
            COLOR_BACKGROUND.z,
            COLOR_BACKGROUND.w
        );
 
        glClear(GL_COLOR_BUFFER_BIT);
 
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
 
        glfwSwapBuffers(window);
    }
 
    // Clean shutdown (monitors first, so no thread outlives the UI)
    backend.shutdown();
 
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
 
    glfwDestroyWindow(window);
    glfwTerminate();
 
    return 0;
}