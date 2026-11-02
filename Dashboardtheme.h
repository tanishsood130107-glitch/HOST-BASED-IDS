// #pragma once
// // ============================================================
// // DashboardTheme.h
// // Themes, sidebar icons, status pills, the Settings page and
// // saved UI preferences (hids_ui.cfg) for the HIDS dashboard.
// //
// // Include it right after the imgui / GLFW includes in
// // HIDSDashboard.cpp. It REPLACES the old COLOR_* constants,
// // WithAlpha() and SetupStyle() in that file (delete those).
// // The COLOR_* names are kept, so the rest of your code works
// // unchanged - they are simply variables now, set by the theme.
// // ============================================================
 
// #include "imgui.h"
 
// #include <algorithm>
// #include <cmath>
// #include <cstdio>
// #include <cstring>
// #include <vector>
 
// // IM_PI lives in imgui_internal.h in some ImGui versions
// #ifndef IM_PI
// #define IM_PI 3.14159265358979323846f
// #endif
 
// // ------------------------------------------------------------
// // Colours. Status colours are fixed so severity always reads
// // the same; the rest change with the theme.
// // ------------------------------------------------------------
 
// static ImVec4 COLOR_BACKGROUND;
// static ImVec4 COLOR_SIDEBAR;
// static ImVec4 COLOR_PANEL;
// static ImVec4 COLOR_BORDER;
// static ImVec4 COLOR_BLUE;      // primary accent
// static ImVec4 COLOR_CYAN;      // secondary accent
// static ImVec4 COLOR_GREEN;     // "good" / active
// static ImVec4 COLOR_TEXT;
// static ImVec4 COLOR_MUTED;
// static ImVec4 COLOR_FRAME;     // inputs, inactive buttons, bar tracks
 
// static const ImVec4 COLOR_RED    = ImVec4(1.0f, 0.25f, 0.30f, 1.0f);
// static const ImVec4 COLOR_ORANGE = ImVec4(1.0f, 0.50f, 0.15f, 1.0f);
// static const ImVec4 COLOR_YELLOW = ImVec4(1.0f, 0.78f, 0.15f, 1.0f);
 
// static ImVec4 WithAlpha(const ImVec4& c, float a)
// {
//     return ImVec4(c.x, c.y, c.z, a);
// }
 
// static ImVec4 MixColor(const ImVec4& a, const ImVec4& b, float t)
// {
//     return ImVec4(a.x + (b.x - a.x) * t,
//                   a.y + (b.y - a.y) * t,
//                   a.z + (b.z - a.z) * t,
//                   1.0f);
// }
 
// // ------------------------------------------------------------
// // Theme list
// // ------------------------------------------------------------
 
// struct UiTheme
// {
//     const char* name;
//     const char* tagline;
//     ImVec4 bg, sidebar, panel, border;
//     ImVec4 primary, secondary, good;
//     ImVec4 text, muted;
// };
 
// static const UiTheme THEMES[] =
// {
//     {   // 0 - the original look
//         "Midnight Blue", "Default, blue accents",
//         ImVec4(0.025f, 0.045f, 0.065f, 1.0f), ImVec4(0.020f, 0.036f, 0.054f, 1.0f),
//         ImVec4(0.035f, 0.065f, 0.090f, 1.0f), ImVec4(0.09f, 0.20f, 0.28f, 1.0f),
//         ImVec4(0.10f, 0.55f, 0.95f, 1.0f), ImVec4(0.0f, 0.75f, 1.0f, 1.0f),
//         ImVec4(0.0f, 0.85f, 0.55f, 1.0f),
//         ImVec4(0.88f, 0.93f, 0.97f, 1.0f), ImVec4(0.50f, 0.59f, 0.66f, 1.0f)
//     },
//     {   // 1
//         "Emerald Terminal", "Green accents",
//         ImVec4(0.020f, 0.050f, 0.040f, 1.0f), ImVec4(0.015f, 0.040f, 0.032f, 1.0f),
//         ImVec4(0.030f, 0.075f, 0.060f, 1.0f), ImVec4(0.08f, 0.25f, 0.18f, 1.0f),
//         ImVec4(0.10f, 0.78f, 0.45f, 1.0f), ImVec4(0.30f, 0.95f, 0.65f, 1.0f),
//         ImVec4(0.20f, 0.90f, 0.50f, 1.0f),
//         ImVec4(0.86f, 0.96f, 0.92f, 1.0f), ImVec4(0.50f, 0.65f, 0.58f, 1.0f)
//     },
//     {   // 2
//         "Ocean Teal", "Blue-green accents",
//         ImVec4(0.020f, 0.050f, 0.070f, 1.0f), ImVec4(0.015f, 0.040f, 0.055f, 1.0f),
//         ImVec4(0.030f, 0.080f, 0.100f, 1.0f), ImVec4(0.08f, 0.26f, 0.30f, 1.0f),
//         ImVec4(0.00f, 0.70f, 0.80f, 1.0f), ImVec4(0.35f, 0.90f, 0.95f, 1.0f),
//         ImVec4(0.20f, 0.90f, 0.60f, 1.0f),
//         ImVec4(0.88f, 0.95f, 0.97f, 1.0f), ImVec4(0.50f, 0.62f, 0.66f, 1.0f)
//     },
//     {   // 3
//         "Graphite", "Neutral grey, blue accents",
//         ImVec4(0.065f, 0.070f, 0.080f, 1.0f), ImVec4(0.050f, 0.055f, 0.065f, 1.0f),
//         ImVec4(0.095f, 0.100f, 0.115f, 1.0f), ImVec4(0.20f, 0.22f, 0.26f, 1.0f),
//         ImVec4(0.30f, 0.60f, 1.00f, 1.0f), ImVec4(0.45f, 0.80f, 1.00f, 1.0f),
//         ImVec4(0.25f, 0.85f, 0.55f, 1.0f),
//         ImVec4(0.90f, 0.92f, 0.95f, 1.0f), ImVec4(0.58f, 0.62f, 0.68f, 1.0f)
//     }
// };
 
// static const int THEME_COUNT = (int)IM_ARRAYSIZE(THEMES);
 
// // ------------------------------------------------------------
// // UI preferences (saved to hids_ui.cfg next to the .exe)
// // ------------------------------------------------------------
 
// static int   themeIndex    = 0;
// static int   pendingTheme  = -1;     // applied at the start of the next frame
// static bool  demoMode      = false;  // sample data, clearly labelled
// static float uiScale       = 1.0f;
// static float baseFontScale = 1.0f;
 
// static const char* UI_CONFIG_FILE = "hids_ui.cfg";
 
// static void SaveUiConfig()
// {
//     FILE* f = std::fopen(UI_CONFIG_FILE, "w");
//     if (f == nullptr)
//         return;
 
//     std::fprintf(f, "theme=%d\ndemo=%d\nscale=%.2f\n",
//                  themeIndex, demoMode ? 1 : 0, uiScale);
//     std::fclose(f);
// }
 
// static void LoadUiConfig()
// {
//     FILE* f = std::fopen(UI_CONFIG_FILE, "r");
//     if (f == nullptr)
//         return;
 
//     char line[128];
 
//     while (std::fgets(line, sizeof(line), f))
//     {
//         int   i = 0;
//         float v = 0.0f;
 
//         if (std::sscanf(line, "theme=%d", &i) == 1)      themeIndex = i;
//         else if (std::sscanf(line, "demo=%d", &i) == 1)  demoMode   = (i != 0);
//         else if (std::sscanf(line, "scale=%f", &v) == 1) uiScale    = v;
//     }
 
//     std::fclose(f);
 
//     if (themeIndex < 0 || themeIndex >= THEME_COUNT) themeIndex = 0;
//     if (uiScale < 0.85f) uiScale = 0.85f;
//     if (uiScale > 1.40f) uiScale = 1.40f;
// }
 
// // Call once after LoadFonts(); remembers the font's own scale.
// static void InitUiScale(ImGuiIO& io)
// {
//     baseFontScale = io.FontGlobalScale;
//     io.FontGlobalScale = baseFontScale * uiScale;
// }
 
// // ------------------------------------------------------------
// // Style - built from the active theme (replaces SetupStyle)
// // ------------------------------------------------------------
 
// static void SetupStyle()
// {
//     ImGui::StyleColorsDark();
 
//     ImGuiStyle& style = ImGui::GetStyle();
 
//     style.WindowPadding    = ImVec2(20.0f, 18.0f);
//     style.FramePadding     = ImVec2(12.0f, 8.0f);
//     style.ItemSpacing      = ImVec2(12.0f, 11.0f);
//     style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);
//     style.CellPadding      = ImVec2(12.0f, 9.0f);
//     style.ScrollbarSize    = 10.0f;
 
//     style.WindowRounding    = 0.0f;
//     style.ChildRounding     = 14.0f;
//     style.FrameRounding     = 8.0f;
//     style.PopupRounding     = 10.0f;
//     style.ScrollbarRounding = 10.0f;
//     style.GrabRounding      = 8.0f;
//     style.TabRounding       = 8.0f;
 
//     style.WindowBorderSize = 0.0f;
//     style.ChildBorderSize  = 1.0f;
//     style.FrameBorderSize  = 0.0f;
 
//     ImVec4* c = style.Colors;
 
//     c[ImGuiCol_WindowBg] = COLOR_BACKGROUND;
//     c[ImGuiCol_ChildBg]  = WithAlpha(COLOR_PANEL, 0.94f);   // lets the backdrop glow through
//     c[ImGuiCol_PopupBg]  = MixColor(COLOR_PANEL, COLOR_BACKGROUND, 0.2f);
//     c[ImGuiCol_Border]   = WithAlpha(COLOR_BORDER, 0.70f);
 
//     c[ImGuiCol_Text]         = COLOR_TEXT;
//     c[ImGuiCol_TextDisabled] = COLOR_MUTED;
 
//     c[ImGuiCol_FrameBg]        = COLOR_FRAME;
//     c[ImGuiCol_FrameBgHovered] = MixColor(COLOR_FRAME, COLOR_BLUE, 0.22f);
//     c[ImGuiCol_FrameBgActive]  = MixColor(COLOR_FRAME, COLOR_BLUE, 0.34f);
 
//     c[ImGuiCol_Button]        = MixColor(COLOR_PANEL, COLOR_BLUE, 0.42f);
//     c[ImGuiCol_ButtonHovered] = MixColor(COLOR_PANEL, COLOR_BLUE, 0.65f);
//     c[ImGuiCol_ButtonActive]  = COLOR_BLUE;
 
//     c[ImGuiCol_Header]        = WithAlpha(COLOR_BLUE, 0.18f);
//     c[ImGuiCol_HeaderHovered] = WithAlpha(COLOR_BLUE, 0.28f);
//     c[ImGuiCol_HeaderActive]  = WithAlpha(COLOR_BLUE, 0.40f);
 
//     c[ImGuiCol_TableHeaderBg]     = MixColor(COLOR_PANEL, COLOR_BORDER, 0.35f);
//     c[ImGuiCol_TableBorderStrong] = COLOR_BORDER;
//     c[ImGuiCol_TableBorderLight]  = WithAlpha(COLOR_BORDER, 0.55f);
//     c[ImGuiCol_TableRowBg]        = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
//     c[ImGuiCol_TableRowBgAlt]     = ImVec4(1.0f, 1.0f, 1.0f, 0.025f);
 
//     c[ImGuiCol_Separator]       = COLOR_BORDER;
//     c[ImGuiCol_ScrollbarBg]     = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
//     c[ImGuiCol_ScrollbarGrab]   = MixColor(COLOR_PANEL, COLOR_BLUE, 0.35f);
//     c[ImGuiCol_ScrollbarGrabHovered] = MixColor(COLOR_PANEL, COLOR_BLUE, 0.55f);
 
//     c[ImGuiCol_CheckMark]        = COLOR_GREEN;
//     c[ImGuiCol_SliderGrab]       = COLOR_BLUE;
//     c[ImGuiCol_SliderGrabActive] = COLOR_CYAN;
 
//     c[ImGuiCol_PlotLines]     = COLOR_CYAN;
//     c[ImGuiCol_PlotHistogram] = COLOR_CYAN;
// }
 
// static void ApplyTheme(int index)
// {
//     if (index < 0 || index >= THEME_COUNT)
//         index = 0;
 
//     const UiTheme& t = THEMES[index];
 
//     themeIndex = index;
 
//     COLOR_BACKGROUND = t.bg;
//     COLOR_SIDEBAR    = t.sidebar;
//     COLOR_PANEL      = t.panel;
//     COLOR_BORDER     = t.border;
//     COLOR_BLUE       = t.primary;
//     COLOR_CYAN       = t.secondary;
//     COLOR_GREEN      = t.good;
//     COLOR_TEXT       = t.text;
//     COLOR_MUTED      = t.muted;
//     COLOR_FRAME      = MixColor(t.panel, t.primary, 0.14f);
 
//     SetupStyle();
// }
 
// // Call at the top of every frame, BEFORE ImGui::NewFrame().
// // (Changing colours in the middle of a frame would corrupt the
// // style-colour stack, so clicks only queue the change.)
// static void ApplyPendingTheme()
// {
//     if (pendingTheme >= 0)
//     {
//         ApplyTheme(pendingTheme);
//         pendingTheme = -1;
//     }
// }
 
// // ------------------------------------------------------------
// // Pill label ("LIVE", "DEMO DATA", ...)
// // ------------------------------------------------------------
 
// static void DrawPill(const char* text, const ImVec4& color)
// {
//     const ImVec2 ts  = ImGui::CalcTextSize(text);
//     const ImVec2 pad(10.0f, 3.0f);
//     const ImVec2 pos = ImGui::GetCursorScreenPos();
//     const ImVec2 size(ts.x + pad.x * 2.0f, ts.y + pad.y * 2.0f);
 
//     ImDrawList* d = ImGui::GetWindowDrawList();
 
//     d->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y),
//         ImGui::GetColorU32(WithAlpha(color, 0.16f)), size.y * 0.5f);
//     d->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y),
//         ImGui::GetColorU32(WithAlpha(color, 0.65f)), size.y * 0.5f);
//     d->AddText(ImVec2(pos.x + pad.x, pos.y + pad.y),
//         ImGui::GetColorU32(color), text);
 
//     ImGui::Dummy(size);
// }
 
// // ------------------------------------------------------------
// // CHARTS
// // Custom-drawn replacements for ImGui::PlotLines / PlotHistogram:
// // gradient area fill, grid with axis labels, pulsing "now" dot,
// // hover crosshair + tooltip. Values come from a ring buffer
// // (oldest = values[offset]), one sample per second.
// // ------------------------------------------------------------
 
// struct ChartRect
// {
//     ImVec2 mn, mx;          // full widget rectangle
//     float  x0, x1, y0, y1;  // plotting area (y1 is the baseline)
// };
 
// static ChartRect BeginChartArea(const char* id, ImVec2 size, bool detailed)
// {
//     if (size.x <= 0.0f)
//         size.x = ImGui::GetContentRegionAvail().x;
 
//     ImGui::InvisibleButton(id, size);
 
//     ChartRect r;
//     r.mn = ImGui::GetItemRectMin();
//     r.mx = ImGui::GetItemRectMax();
 
//     const float padL = detailed ? 42.0f : 2.0f;
//     const float padR = detailed ? 12.0f : 2.0f;
//     const float padT = detailed ? 10.0f : 4.0f;
//     const float padB = detailed ? 26.0f : 4.0f;
 
//     r.x0 = r.mn.x + padL;
//     r.x1 = r.mx.x - padR;
//     r.y0 = r.mn.y + padT;
//     r.y1 = r.mx.y - padB;
 
//     return r;
// }
 
// static void DrawChartGrid(const ChartRect& r, float yMax, int count, const char* unit)
// {
//     ImDrawList* d = ImGui::GetWindowDrawList();
 
//     d->AddRectFilled(r.mn, r.mx,
//         ImGui::GetColorU32(WithAlpha(COLOR_FRAME, 0.55f)), 10.0f);
 
//     for (int k = 0; k <= 4; ++k)
//     {
//         const float y = r.y1 - (r.y1 - r.y0) * (float)k / 4.0f;
 
//         d->AddLine(ImVec2(r.x0, y), ImVec2(r.x1, y),
//             ImGui::GetColorU32(WithAlpha(COLOR_TEXT, k == 0 ? 0.14f : 0.06f)));
 
//         if (k % 2 == 0)
//         {
//             char label[24];
//             std::snprintf(label, sizeof(label), "%.0f%s", yMax * (float)k / 4.0f, unit);
 
//             d->AddText(
//                 ImVec2(r.mn.x + 6.0f, y - ImGui::GetTextLineHeight() * 0.5f),
//                 ImGui::GetColorU32(COLOR_MUTED), label);
//         }
//     }
 
//     char ago[24];
//     std::snprintf(ago, sizeof(ago), "%d s ago", count - 1);
 
//     d->AddText(ImVec2(r.x0, r.y1 + 5.0f),
//         ImGui::GetColorU32(COLOR_MUTED), ago);
 
//     const float nowW = ImGui::CalcTextSize("now").x;
 
//     d->AddText(ImVec2(r.x1 - nowW, r.y1 + 5.0f),
//         ImGui::GetColorU32(COLOR_MUTED), "now");
// }
 
// // Smooth gradient-filled line chart.
// // detailed = false gives a bare sparkline (no grid, labels or tooltip).
// static void DrawAreaChart(const char* id, const float* values, int count,
//                           int offset, float yMax, ImVec2 size,
//                           const ImVec4& color, const char* unit = "%",
//                           bool detailed = true)
// {
//     const ChartRect r = BeginChartArea(id, size, detailed);
 
//     if (count < 2 || yMax <= 0.0f)
//         return;
 
//     const bool hovered = detailed && ImGui::IsItemHovered();
//     ImDrawList* d = ImGui::GetWindowDrawList();
 
//     if (detailed)
//         DrawChartGrid(r, yMax, count, unit);
 
//     // Unwrap the ring buffer, then smooth lightly so the line looks fluid
//     std::vector<float> raw(count), smooth(count);
 
//     for (int i = 0; i < count; ++i)
//         raw[i] = values[(offset + i) % count];
 
//     for (int i = 0; i < count; ++i)
//     {
//         const float a = raw[i > 0 ? i - 1 : 0];
//         const float c = raw[i < count - 1 ? i + 1 : count - 1];
 
//         smooth[i] = (a + 2.0f * raw[i] + c) * 0.25f;
//     }
 
//     std::vector<ImVec2> pts(count);
 
//     for (int i = 0; i < count; ++i)
//     {
//         float t = smooth[i] / yMax;
//         t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
 
//         pts[i] = ImVec2(
//             r.x0 + (r.x1 - r.x0) * (float)i / (float)(count - 1),
//             r.y1 - (r.y1 - r.y0) * t);
//     }
 
//     // Gradient fill: strong at the line, fading to nothing at the baseline
//     {
//         const ImVec2 uv  = ImGui::GetFontTexUvWhitePixel();
//         const ImU32  top = ImGui::GetColorU32(WithAlpha(color, 0.38f));
//         const ImU32  bot = ImGui::GetColorU32(WithAlpha(color, 0.0f));
 
//         d->PrimReserve((count - 1) * 6, (count - 1) * 4);
 
//         for (int i = 0; i < count - 1; ++i)
//         {
//             const ImDrawIdx base = (ImDrawIdx)d->_VtxCurrentIdx;
 
//             d->PrimWriteVtx(pts[i],                         uv, top);
//             d->PrimWriteVtx(pts[i + 1],                     uv, top);
//             d->PrimWriteVtx(ImVec2(pts[i + 1].x, r.y1),     uv, bot);
//             d->PrimWriteVtx(ImVec2(pts[i].x,     r.y1),     uv, bot);
 
//             d->PrimWriteIdx(base);     d->PrimWriteIdx(base + 1); d->PrimWriteIdx(base + 2);
//             d->PrimWriteIdx(base);     d->PrimWriteIdx(base + 2); d->PrimWriteIdx(base + 3);
//         }
//     }
 
//     // Line on top
//     d->AddPolyline(pts.data(), count, ImGui::GetColorU32(color), 0, 2.2f);
 
//     // Pulsing dot on the latest sample
//     {
//         const ImVec2 last = pts[count - 1];
//         const float wave = 0.5f + 0.5f * std::sin((float)ImGui::GetTime() * 3.0f);
 
//         d->AddCircleFilled(last, detailed ? 9.0f + wave * 3.0f : 6.0f,
//             ImGui::GetColorU32(WithAlpha(color, 0.12f + 0.10f * (1.0f - wave))));
//         d->AddCircleFilled(last, detailed ? 4.0f : 3.0f, ImGui::GetColorU32(color));
//     }
 
//     // Hover: crosshair, marker and tooltip
//     if (hovered)
//     {
//         const float mx = ImGui::GetIO().MousePos.x;
 
//         int idx = (int)std::lround((mx - r.x0) / (r.x1 - r.x0) * (float)(count - 1));
//         idx = idx < 0 ? 0 : (idx > count - 1 ? count - 1 : idx);
 
//         const ImVec2 p = pts[idx];
 
//         d->AddLine(ImVec2(p.x, r.y0), ImVec2(p.x, r.y1),
//             ImGui::GetColorU32(WithAlpha(COLOR_TEXT, 0.25f)));
//         d->AddCircleFilled(p, 5.0f, ImGui::GetColorU32(color));
//         d->AddCircle(p, 5.0f, IM_COL32(255, 255, 255, 220), 0, 1.5f);
 
//         ImGui::SetTooltip("%.1f%s\n%d s ago", raw[idx], unit, count - 1 - idx);
//     }
 
//     // Summary line under detailed charts
//     if (detailed)
//     {
//         float sum = 0.0f, peak = 0.0f;
 
//         for (float v : raw)
//         {
//             sum += v;
//             peak = std::max(peak, v);
//         }
 
//         ImGui::TextDisabled("avg %.0f%s    peak %.0f%s    now %.0f%s",
//             sum / (float)count, unit, peak, unit, raw[count - 1], unit);
//     }
// }
 
// // Rounded bar chart; bar colour rises yellow -> orange -> red with the value.
// static void DrawBarChart(const char* id, const float* values, int count,
//                          int offset, float yMax, ImVec2 size,
//                          const char* unit = "")
// {
//     const ChartRect r = BeginChartArea(id, size, true);
 
//     if (count < 1 || yMax <= 0.0f)
//         return;
 
//     const bool hovered = ImGui::IsItemHovered();
//     ImDrawList* d = ImGui::GetWindowDrawList();
 
//     DrawChartGrid(r, yMax, count, unit);
 
//     const float slot = (r.x1 - r.x0) / (float)count;
//     const float barW = std::max(1.5f, slot * 0.62f);
 
//     int hoverIdx = -1;
 
//     if (hovered)
//     {
//         hoverIdx = (int)((ImGui::GetIO().MousePos.x - r.x0) / slot);
//         hoverIdx = hoverIdx < 0 ? 0 : (hoverIdx > count - 1 ? count - 1 : hoverIdx);
 
//         d->AddRectFilled(
//             ImVec2(r.x0 + slot * hoverIdx, r.y0),
//             ImVec2(r.x0 + slot * (hoverIdx + 1), r.y1),
//             ImGui::GetColorU32(WithAlpha(COLOR_TEXT, 0.06f)), 3.0f);
//     }
 
//     for (int i = 0; i < count; ++i)
//     {
//         const float v = values[(offset + i) % count];
//         const float x = r.x0 + slot * (float)i + (slot - barW) * 0.5f;
 
//         if (v <= 0.0f)
//         {
//             d->AddRectFilled(ImVec2(x, r.y1 - 2.0f), ImVec2(x + barW, r.y1),
//                 ImGui::GetColorU32(WithAlpha(COLOR_MUTED, 0.30f)), 1.0f);
//             continue;
//         }
 
//         float t = v / yMax;
//         t = t > 1.0f ? 1.0f : t;
 
//         const float h = std::max(4.0f, (r.y1 - r.y0) * t);
 
//         const ImVec4 c = t < 0.34f ? COLOR_YELLOW
//                        : t < 0.67f ? COLOR_ORANGE : COLOR_RED;
 
//         d->AddRectFilled(ImVec2(x, r.y1 - h), ImVec2(x + barW, r.y1),
//             ImGui::GetColorU32(c), 3.0f, ImDrawFlags_RoundCornersTop);
//     }
 
//     if (hoverIdx >= 0)
//     {
//         const float v = values[(offset + hoverIdx) % count];
 
//         ImGui::SetTooltip("Threat score %.0f\n%d s ago", v, count - 1 - hoverIdx);
//     }
// }
 
// // Soft diagonal colour wash behind everything. Call once per frame,
// // after NewFrame() and before the root window is drawn.
// static void DrawBackdrop()
// {
//     const ImVec2 size = ImGui::GetIO().DisplaySize;
 
//     ImGui::GetBackgroundDrawList()->AddRectFilledMultiColor(
//         ImVec2(0.0f, 0.0f), size,
//         ImGui::GetColorU32(MixColor(COLOR_BACKGROUND, COLOR_BLUE, 0.16f)),
//         ImGui::GetColorU32(MixColor(COLOR_BACKGROUND, COLOR_CYAN, 0.07f)),
//         ImGui::GetColorU32(COLOR_BACKGROUND),
//         ImGui::GetColorU32(MixColor(COLOR_BACKGROUND, COLOR_BLUE, 0.04f)));
// }
 
// // Faint radial glow (stacked translucent circles)
// static void DrawGlow(ImDrawList* d, ImVec2 center, float radius, const ImVec4& color)
// {
//     for (int i = 0; i < 5; ++i)
//         d->AddCircleFilled(center, radius * (1.0f - i * 0.18f),
//             ImGui::GetColorU32(WithAlpha(color, 0.035f)), 36);
// }
 
// // Ring gauge, fraction 0..1, starts at 12 o'clock
// static void DrawGauge(ImDrawList* d, ImVec2 center, float radius,
//                       float thickness, float fraction, const ImVec4& color)
// {
//     if (fraction < 0.0f) fraction = 0.0f;
//     if (fraction > 1.0f) fraction = 1.0f;
 
//     d->AddCircle(center, radius,
//         ImGui::GetColorU32(WithAlpha(color, 0.18f)), 48, thickness);
 
//     if (fraction > 0.005f)
//     {
//         const float a0 = -IM_PI * 0.5f;
 
//         d->PathClear();
//         d->PathArcTo(center, radius, a0, a0 + fraction * IM_PI * 2.0f, 48);
//         d->PathStroke(ImGui::GetColorU32(color), 0, thickness);
//     }
// }
 
// // Shows whether the numbers on screen are real or sample data.
// static void DataSourceBadge(bool monitoringOn)
// {
//     if (demoMode)
//         DrawPill("DEMO DATA ON", COLOR_YELLOW);
//     else if (monitoringOn)
//         DrawPill("LIVE MONITORING", COLOR_GREEN);
//     else
//         DrawPill("MONITORING STOPPED", COLOR_RED);
// }
 
// // ------------------------------------------------------------
// // Sidebar icons - drawn with vector shapes, so no icon font is
// // needed. c = centre, r = half size (about 8 px).
// // ------------------------------------------------------------
 
// static void DrawPageIcon(ImDrawList* d, ImVec2 c, float r, int page, ImU32 col)
// {
//     const float t = 1.6f;
 
//     switch (page)
//     {
//     case 0: // Dashboard - four tiles
//     {
//         const float g = 1.5f;
 
//         for (int i = 0; i < 4; ++i)
//         {
//             const float x0 = c.x + ((i % 2) ? g : -r);
//             const float y0 = c.y + ((i / 2) ? g : -r);
 
//             d->AddRectFilled(ImVec2(x0, y0),
//                 ImVec2(x0 + (r - g), y0 + (r - g)), col, 2.0f);
//         }
//         break;
//     }
//     case 1: // Processes - list
//         for (int i = -1; i <= 1; ++i)
//         {
//             const float y = c.y + i * r * 0.7f;
 
//             d->AddCircleFilled(ImVec2(c.x - r * 0.8f, y), 1.6f, col);
//             d->AddLine(ImVec2(c.x - r * 0.35f, y), ImVec2(c.x + r, y), col, t);
//         }
//         break;
 
//     case 2: // Login - person
//     {
//         d->AddCircle(ImVec2(c.x, c.y - r * 0.40f), r * 0.38f, col, 16, t);
 
//         d->PathClear();
//         d->PathArcTo(ImVec2(c.x, c.y + r * 1.0f), r * 0.85f,
//                      IM_PI, IM_PI * 2.0f, 16);
//         d->PathStroke(col, 0, t);
//         break;
//     }
//     case 3: // File monitoring - folder
//         d->AddRectFilled(ImVec2(c.x - r, c.y - r * 0.9f),
//                          ImVec2(c.x - r * 0.1f, c.y - r * 0.5f), col, 1.5f);
//         d->AddRect(ImVec2(c.x - r, c.y - r * 0.6f),
//                    ImVec2(c.x + r, c.y + r * 0.8f), col, 2.0f, 0, t);
//         break;
 
//     case 4: // Alerts - warning triangle
//         d->AddTriangle(ImVec2(c.x, c.y - r),
//                        ImVec2(c.x - r, c.y + r * 0.8f),
//                        ImVec2(c.x + r, c.y + r * 0.8f), col, t);
//         d->AddLine(ImVec2(c.x, c.y - r * 0.3f),
//                    ImVec2(c.x, c.y + r * 0.2f), col, t);
//         d->AddCircleFilled(ImVec2(c.x, c.y + r * 0.52f), 1.4f, col);
//         break;
 
//     case 5: // Logs & reports - document
//         d->AddRect(ImVec2(c.x - r * 0.75f, c.y - r),
//                    ImVec2(c.x + r * 0.75f, c.y + r), col, 2.0f, 0, t);
 
//         for (int i = 0; i < 3; ++i)
//         {
//             const float y = c.y - r * 0.4f + i * r * 0.5f;
//             d->AddLine(ImVec2(c.x - r * 0.4f, y),
//                        ImVec2(c.x + r * 0.4f, y), col, 1.2f);
//         }
//         break;
 
//     case 6: // System analysis - line chart
//     {
//         const ImVec2 pts[4] = {
//             ImVec2(c.x - r,        c.y + r * 0.6f),
//             ImVec2(c.x - r * 0.3f, c.y - r * 0.1f),
//             ImVec2(c.x + r * 0.2f, c.y + r * 0.3f),
//             ImVec2(c.x + r,        c.y - r * 0.7f)
//         };
 
//         d->AddPolyline(pts, 4, col, 0, t);
//         break;
//     }
//     case 7: // File scanner - magnifier
//         d->AddCircle(ImVec2(c.x - r * 0.15f, c.y - r * 0.15f),
//                      r * 0.65f, col, 20, t);
//         d->AddLine(ImVec2(c.x + r * 0.35f, c.y + r * 0.35f),
//                    ImVec2(c.x + r, c.y + r), col, t + 0.5f);
//         break;
 
//     case 8: // Settings - gear
//         d->AddCircle(c, r * 0.62f, col, 20, t);
 
//         for (int i = 0; i < 8; ++i)
//         {
//             const float a = i * IM_PI * 0.25f;
//             const float ca = std::cos(a);
//             const float sa = std::sin(a);
 
//             d->AddLine(ImVec2(c.x + ca * r * 0.8f, c.y + sa * r * 0.8f),
//                        ImVec2(c.x + ca * r,        c.y + sa * r), col, t + 0.6f);
//         }
//         break;
//     }
// }
 
// // ------------------------------------------------------------
// // SETTINGS PAGE
// // ------------------------------------------------------------
 
// static void ThemeHeading(const char* title, const char* hint = nullptr)
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
 
// static void DrawSettingsPage(bool monitoringOn)
// {
//     ImGui::BeginChild("SettingsPanel", ImVec2(0.0f, 0.0f), true);
 
//     // ---------------- Appearance ----------------
//     ThemeHeading("Appearance", "click a theme to apply it instantly");
 
//     const float gap   = ImGui::GetStyle().ItemSpacing.x;
//     const float cardW = (ImGui::GetContentRegionAvail().x - gap * (THEME_COUNT - 1))
//                         / (float)THEME_COUNT;
//     const float cardH = 132.0f;
 
//     for (int i = 0; i < THEME_COUNT; ++i)
//     {
//         if (i > 0)
//             ImGui::SameLine();
 
//         ImGui::PushID(i);
 
//         const ImVec2 p = ImGui::GetCursorScreenPos();
 
//         const bool clicked = ImGui::InvisibleButton("##theme", ImVec2(cardW, cardH));
//         const bool hovered = ImGui::IsItemHovered();
 
//         if (hovered)
//             ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
 
//         if (clicked)
//         {
//             themeIndex   = i;       // selection border updates at once
//             pendingTheme = i;       // colours change next frame
//             SaveUiConfig();
//         }
 
//         const UiTheme& t = THEMES[i];
//         const bool selected = (themeIndex == i);
//         ImDrawList* d = ImGui::GetWindowDrawList();
 
//         auto U = [](const ImVec4& c) { return ImGui::GetColorU32(c); };
//         auto P = [&](float fx, float y) { return ImVec2(p.x + cardW * fx, p.y + y); };
 
//         // Mini preview of the real layout
//         d->AddRectFilled(p, ImVec2(p.x + cardW, p.y + cardH), U(t.bg), 10.0f);
//         d->AddRectFilled(p, P(0.20f, cardH), U(t.sidebar), 10.0f,
//                          ImDrawFlags_RoundCornersLeft);
 
//         for (int n = 0; n < 4; ++n)     // sidebar items
//             d->AddRectFilled(P(0.04f, 14.0f + n * 12.0f),
//                              P(n == 0 ? 0.17f : 0.14f, 20.0f + n * 12.0f),
//                              U(n == 0 ? t.secondary : WithAlpha(t.muted, 0.5f)), 3.0f);
 
//         d->AddRectFilled(P(0.25f, 12.0f), P(0.60f, 54.0f), U(t.panel), 6.0f);
//         d->AddRectFilled(P(0.25f, 12.0f), P(0.26f, 54.0f), U(t.primary), 2.0f);
//         d->AddRectFilled(P(0.64f, 12.0f), P(0.95f, 54.0f), U(t.panel), 6.0f);
//         d->AddRectFilled(P(0.64f, 12.0f), P(0.65f, 54.0f), U(t.good), 2.0f);
 
//         const ImVec2 line[5] = {
//             P(0.29f, 46.0f), P(0.36f, 36.0f), P(0.43f, 42.0f),
//             P(0.50f, 28.0f), P(0.57f, 33.0f)
//         };
//         d->AddPolyline(line, 5, U(t.secondary), 0, 1.6f);
 
//         d->AddCircleFilled(P(0.27f, 70.0f), 5.0f, U(t.primary));
//         d->AddCircleFilled(ImVec2(P(0.27f, 70.0f).x + 16.0f, p.y + 70.0f), 5.0f, U(t.secondary));
//         d->AddCircleFilled(ImVec2(P(0.27f, 70.0f).x + 32.0f, p.y + 70.0f), 5.0f, U(t.good));
 
//         d->AddText(P(0.25f, 86.0f), U(t.text), t.name);
//         d->AddText(P(0.25f, 86.0f + ImGui::GetTextLineHeight() + 2.0f),
//                    U(t.muted), t.tagline);
 
//         // Border, thicker + accent colour when selected
//         d->AddRect(p, ImVec2(p.x + cardW, p.y + cardH),
//             U(selected ? t.primary
//                        : (hovered ? WithAlpha(t.primary, 0.60f) : t.border)),
//             10.0f, 0, selected ? 2.5f : 1.0f);
 
//         if (selected)
//         {
//             const ImVec2 ctr(p.x + cardW - 16.0f, p.y + 16.0f);
//             d->AddCircleFilled(ctr, 9.0f, U(t.primary));
 
//             const ImVec2 tick[3] = {
//                 ImVec2(ctr.x - 4.0f, ctr.y),
//                 ImVec2(ctr.x - 1.0f, ctr.y + 3.5f),
//                 ImVec2(ctr.x + 4.5f, ctr.y - 3.5f)
//             };
//             d->AddPolyline(tick, 3, IM_COL32(255, 255, 255, 255), 0, 2.0f);
//         }
 
//         ImGui::PopID();
//     }
 
//     ImGui::Spacing();
//     ImGui::Separator();
//     ImGui::Spacing();
 
//     // ---------------- Interface size ----------------
//     ThemeHeading("Interface");
 
//     ImGui::TextUnformatted("Interface size");
//     ImGui::SetNextItemWidth(260.0f);
 
//     if (ImGui::SliderFloat("##uiscale", &uiScale, 0.85f, 1.40f, "%.2fx"))
//         ImGui::GetIO().FontGlobalScale = baseFontScale * uiScale;
 
//     if (ImGui::IsItemDeactivatedAfterEdit())
//         SaveUiConfig();
 
//     ImGui::Spacing();
//     ImGui::Separator();
//     ImGui::Spacing();
 
//     // ---------------- Data source ----------------
//     ThemeHeading("Data source", "real monitoring vs sample data");
 
//     ImGui::Text("Real monitoring:");
//     ImGui::SameLine();
//     ImGui::TextColored(monitoringOn ? COLOR_GREEN : COLOR_RED,
//         monitoringOn ? "running" : "stopped");
 
//     if (ImGui::Checkbox("Show demo data", &demoMode))
//         SaveUiConfig();
 
//     ImGui::SameLine();
//     ImGui::TextDisabled("adds a few sample alerts, processes and events");
 
//     if (demoMode)
//     {
//         ImGui::TextColored(COLOR_YELLOW,
//             "Demo rows are tagged [DEMO], use source \"DEMO\" and can never be "
//             "ended or exported as real findings.");
//     }
//     else
//     {
//         ImGui::TextDisabled(
//             "Everything shown comes from the HIDS monitors on this machine.");
//     }
 
//     ImGui::Spacing();
//     ImGui::Separator();
//     ImGui::Spacing();
 
//     if (ImGui::Button("Reset appearance to defaults"))
//     {
//         themeIndex   = 0;
//         pendingTheme = 0;
//         uiScale      = 1.0f;
//         demoMode     = false;
//         ImGui::GetIO().FontGlobalScale = baseFontScale;
//         SaveUiConfig();
//     }
 
//     ImGui::EndChild();
// }


#pragma once

// ============================================================
// DashboardTheme.h
// Themes, sidebar icons, status pills, the Settings page and
// saved UI preferences (hids_ui.cfg) for the HIDS dashboard.
// ============================================================

#include "imgui.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <vector>

#ifndef IM_PI
#define IM_PI 3.14159265358979323846f
#endif

// ------------------------------------------------------------
// Colours
// ------------------------------------------------------------

static ImVec4 HIDS_COLOR_BACKGROUND;
static ImVec4 COLOR_SIDEBAR;
static ImVec4 COLOR_PANEL;
static ImVec4 COLOR_BORDER;
static ImVec4 COLOR_BLUE;
static ImVec4 COLOR_CYAN;
static ImVec4 COLOR_GREEN;
static ImVec4 COLOR_TEXT;
static ImVec4 COLOR_MUTED;
static ImVec4 COLOR_FRAME;

static const ImVec4 COLOR_RED =
    ImVec4(1.0f, 0.25f, 0.30f, 1.0f);

static const ImVec4 COLOR_ORANGE =
    ImVec4(1.0f, 0.50f, 0.15f, 1.0f);

static const ImVec4 COLOR_YELLOW =
    ImVec4(1.0f, 0.78f, 0.15f, 1.0f);

static ImVec4 WithAlpha(const ImVec4& c, float a)
{
    return ImVec4(c.x, c.y, c.z, a);
}

static ImVec4 MixColor(
    const ImVec4& a,
    const ImVec4& b,
    float t)
{
    return ImVec4(
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t,
        1.0f
    );
}

// ------------------------------------------------------------
// Theme list
// ------------------------------------------------------------

struct UiTheme
{
    const char* name;
    const char* tagline;

    ImVec4 bg, sidebar, panel, border;
    ImVec4 primary, secondary, good;
    ImVec4 text, muted;
};

static const UiTheme THEMES[] =
{
    {
        "Midnight Blue",
        "Default, blue accents",

        ImVec4(0.025f, 0.045f, 0.065f, 1.0f),
        ImVec4(0.020f, 0.036f, 0.054f, 1.0f),
        ImVec4(0.035f, 0.065f, 0.090f, 1.0f),
        ImVec4(0.09f, 0.20f, 0.28f, 1.0f),

        ImVec4(0.10f, 0.55f, 0.95f, 1.0f),
        ImVec4(0.0f, 0.75f, 1.0f, 1.0f),
        ImVec4(0.0f, 0.85f, 0.55f, 1.0f),

        ImVec4(0.88f, 0.93f, 0.97f, 1.0f),
        ImVec4(0.50f, 0.59f, 0.66f, 1.0f)
    },

    {
        "Emerald Terminal",
        "Green accents",

        ImVec4(0.020f, 0.050f, 0.040f, 1.0f),
        ImVec4(0.015f, 0.040f, 0.032f, 1.0f),
        ImVec4(0.030f, 0.075f, 0.060f, 1.0f),
        ImVec4(0.08f, 0.25f, 0.18f, 1.0f),

        ImVec4(0.10f, 0.78f, 0.45f, 1.0f),
        ImVec4(0.30f, 0.95f, 0.65f, 1.0f),
        ImVec4(0.20f, 0.90f, 0.50f, 1.0f),

        ImVec4(0.86f, 0.96f, 0.92f, 1.0f),
        ImVec4(0.50f, 0.65f, 0.58f, 1.0f)
    },

    {
        "Ocean Teal",
        "Blue-green accents",

        ImVec4(0.020f, 0.050f, 0.070f, 1.0f),
        ImVec4(0.015f, 0.040f, 0.055f, 1.0f),
        ImVec4(0.030f, 0.080f, 0.100f, 1.0f),
        ImVec4(0.08f, 0.26f, 0.30f, 1.0f),

        ImVec4(0.00f, 0.70f, 0.80f, 1.0f),
        ImVec4(0.35f, 0.90f, 0.95f, 1.0f),
        ImVec4(0.20f, 0.90f, 0.60f, 1.0f),

        ImVec4(0.88f, 0.95f, 0.97f, 1.0f),
        ImVec4(0.50f, 0.62f, 0.66f, 1.0f)
    },

    {
        "Graphite",
        "Neutral grey, blue accents",

        ImVec4(0.065f, 0.070f, 0.080f, 1.0f),
        ImVec4(0.050f, 0.055f, 0.065f, 1.0f),
        ImVec4(0.095f, 0.100f, 0.115f, 1.0f),
        ImVec4(0.20f, 0.22f, 0.26f, 1.0f),

        ImVec4(0.30f, 0.60f, 1.00f, 1.0f),
        ImVec4(0.45f, 0.80f, 1.00f, 1.0f),
        ImVec4(0.25f, 0.85f, 0.55f, 1.0f),

        ImVec4(0.90f, 0.92f, 0.95f, 1.0f),
        ImVec4(0.58f, 0.62f, 0.68f, 1.0f)
    }
};

static const int THEME_COUNT =
    (int)IM_ARRAYSIZE(THEMES);

// ------------------------------------------------------------
// UI preferences
// ------------------------------------------------------------

static int themeIndex = 0;
static int pendingTheme = -1;
static bool demoMode = false;
static float uiScale = 1.0f;
static float baseFontScale = 1.0f;

static const char* UI_CONFIG_FILE = "hids_ui.cfg";

static void SaveUiConfig()
{
    FILE* f = std::fopen(UI_CONFIG_FILE, "w");

    if (f == nullptr)
        return;

    std::fprintf(
        f,
        "theme=%d\ndemo=%d\nscale=%.2f\n",
        themeIndex,
        demoMode ? 1 : 0,
        uiScale
    );

    std::fclose(f);
}

static void LoadUiConfig()
{
    FILE* f = std::fopen(UI_CONFIG_FILE, "r");

    if (f == nullptr)
        return;

    char line[128];

    while (std::fgets(line, sizeof(line), f))
    {
        int i = 0;
        float v = 0.0f;

        if (std::sscanf(line, "theme=%d", &i) == 1)
            themeIndex = i;

        else if (std::sscanf(line, "demo=%d", &i) == 1)
            demoMode = (i != 0);

        else if (std::sscanf(line, "scale=%f", &v) == 1)
            uiScale = v;
    }

    std::fclose(f);

    if (themeIndex < 0 || themeIndex >= THEME_COUNT)
        themeIndex = 0;

    if (uiScale < 0.85f)
        uiScale = 0.85f;

    if (uiScale > 1.40f)
        uiScale = 1.40f;
}

static void InitUiScale(ImGuiIO& io)
{
    baseFontScale = io.FontGlobalScale;
    io.FontGlobalScale = baseFontScale * uiScale;
}

// ------------------------------------------------------------
// Style
// ------------------------------------------------------------

static void SetupStyle()
{
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowPadding = ImVec2(20.0f, 18.0f);
    style.FramePadding = ImVec2(12.0f, 8.0f);
    style.ItemSpacing = ImVec2(12.0f, 11.0f);
    style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);
    style.CellPadding = ImVec2(12.0f, 9.0f);
    style.ScrollbarSize = 10.0f;

    style.WindowRounding = 0.0f;
    style.ChildRounding = 14.0f;
    style.FrameRounding = 8.0f;
    style.PopupRounding = 10.0f;
    style.ScrollbarRounding = 10.0f;
    style.GrabRounding = 8.0f;
    style.TabRounding = 8.0f;

    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;

    ImVec4* c = style.Colors;

    c[ImGuiCol_WindowBg] = HIDS_COLOR_BACKGROUND;
    c[ImGuiCol_ChildBg] = WithAlpha(COLOR_PANEL, 0.94f);
    c[ImGuiCol_PopupBg] =
        MixColor(COLOR_PANEL, HIDS_COLOR_BACKGROUND, 0.2f);

    c[ImGuiCol_Border] =
        WithAlpha(COLOR_BORDER, 0.70f);

    c[ImGuiCol_Text] = COLOR_TEXT;
    c[ImGuiCol_TextDisabled] = COLOR_MUTED;

    c[ImGuiCol_FrameBg] = COLOR_FRAME;
    c[ImGuiCol_FrameBgHovered] =
        MixColor(COLOR_FRAME, COLOR_BLUE, 0.22f);

    c[ImGuiCol_FrameBgActive] =
        MixColor(COLOR_FRAME, COLOR_BLUE, 0.34f);

    c[ImGuiCol_Button] =
        MixColor(COLOR_PANEL, COLOR_BLUE, 0.42f);

    c[ImGuiCol_ButtonHovered] =
        MixColor(COLOR_PANEL, COLOR_BLUE, 0.65f);

    c[ImGuiCol_ButtonActive] = COLOR_BLUE;

    c[ImGuiCol_Header] =
        WithAlpha(COLOR_BLUE, 0.18f);

    c[ImGuiCol_HeaderHovered] =
        WithAlpha(COLOR_BLUE, 0.28f);

    c[ImGuiCol_HeaderActive] =
        WithAlpha(COLOR_BLUE, 0.40f);

    c[ImGuiCol_TableHeaderBg] =
        MixColor(COLOR_PANEL, COLOR_BORDER, 0.35f);

    c[ImGuiCol_TableBorderStrong] = COLOR_BORDER;

    c[ImGuiCol_TableBorderLight] =
        WithAlpha(COLOR_BORDER, 0.55f);

    c[ImGuiCol_TableRowBg] =
        ImVec4(0, 0, 0, 0);

    c[ImGuiCol_TableRowBgAlt] =
        ImVec4(1, 1, 1, 0.025f);

    c[ImGuiCol_Separator] = COLOR_BORDER;

    c[ImGuiCol_ScrollbarBg] =
        ImVec4(0, 0, 0, 0);

    c[ImGuiCol_ScrollbarGrab] =
        MixColor(COLOR_PANEL, COLOR_BLUE, 0.35f);

    c[ImGuiCol_ScrollbarGrabHovered] =
        MixColor(COLOR_PANEL, COLOR_BLUE, 0.55f);

    c[ImGuiCol_CheckMark] = COLOR_GREEN;
    c[ImGuiCol_SliderGrab] = COLOR_BLUE;
    c[ImGuiCol_SliderGrabActive] = COLOR_CYAN;

    c[ImGuiCol_PlotLines] = COLOR_CYAN;
    c[ImGuiCol_PlotHistogram] = COLOR_CYAN;
}

// ------------------------------------------------------------
// Apply theme
// ------------------------------------------------------------

static void ApplyTheme(int index)
{
    if (index < 0 || index >= THEME_COUNT)
        index = 0;

    const UiTheme& t = THEMES[index];

    themeIndex = index;

    HIDS_COLOR_BACKGROUND = t.bg;
    COLOR_SIDEBAR = t.sidebar;
    COLOR_PANEL = t.panel;
    COLOR_BORDER = t.border;
    COLOR_BLUE = t.primary;
    COLOR_CYAN = t.secondary;
    COLOR_GREEN = t.good;
    COLOR_TEXT = t.text;
    COLOR_MUTED = t.muted;

    COLOR_FRAME =
        MixColor(t.panel, t.primary, 0.14f);

    SetupStyle();
}

static void ApplyPendingTheme()
{
    if (pendingTheme >= 0)
    {
        ApplyTheme(pendingTheme);
        pendingTheme = -1;
    }
}

// ------------------------------------------------------------
// Pill label
// ------------------------------------------------------------

static void DrawPill(
    const char* text,
    const ImVec4& color)
{
    const ImVec2 ts = ImGui::CalcTextSize(text);
    const ImVec2 pad(10.0f, 3.0f);

    const ImVec2 pos = ImGui::GetCursorScreenPos();

    const ImVec2 size(
        ts.x + pad.x * 2.0f,
        ts.y + pad.y * 2.0f
    );

    ImDrawList* d = ImGui::GetWindowDrawList();

    d->AddRectFilled(
        pos,
        ImVec2(pos.x + size.x, pos.y + size.y),
        ImGui::GetColorU32(WithAlpha(color, 0.16f)),
        size.y * 0.5f
    );

    d->AddRect(
        pos,
        ImVec2(pos.x + size.x, pos.y + size.y),
        ImGui::GetColorU32(WithAlpha(color, 0.65f)),
        size.y * 0.5f
    );

    d->AddText(
        ImVec2(pos.x + pad.x, pos.y + pad.y),
        ImGui::GetColorU32(color),
        text
    );

    ImGui::Dummy(size);
}

// ------------------------------------------------------------
// Charts
// ------------------------------------------------------------

struct ChartRect
{
    ImVec2 mn, mx;
    float x0, x1, y0, y1;
};

static ChartRect BeginChartArea(
    const char* id,
    ImVec2 size,
    bool detailed)
{
    if (size.x <= 0.0f)
        size.x = ImGui::GetContentRegionAvail().x;

    ImGui::InvisibleButton(id, size);

    ChartRect r;

    r.mn = ImGui::GetItemRectMin();
    r.mx = ImGui::GetItemRectMax();

    const float padL = detailed ? 42.0f : 2.0f;
    const float padR = detailed ? 12.0f : 2.0f;
    const float padT = detailed ? 10.0f : 4.0f;
    const float padB = detailed ? 26.0f : 4.0f;

    r.x0 = r.mn.x + padL;
    r.x1 = r.mx.x - padR;
    r.y0 = r.mn.y + padT;
    r.y1 = r.mx.y - padB;

    return r;
}

static void DrawChartGrid(
    const ChartRect& r,
    float yMax,
    int count,
    const char* unit)
{
    ImDrawList* d = ImGui::GetWindowDrawList();

    d->AddRectFilled(
        r.mn,
        r.mx,
        ImGui::GetColorU32(WithAlpha(COLOR_FRAME, 0.55f)),
        10.0f
    );

    for (int k = 0; k <= 4; ++k)
    {
        const float y =
            r.y1 - (r.y1 - r.y0) * (float)k / 4.0f;

        d->AddLine(
            ImVec2(r.x0, y),
            ImVec2(r.x1, y),
            ImGui::GetColorU32(
                WithAlpha(COLOR_TEXT, k == 0 ? 0.14f : 0.06f)
            )
        );

        if (k % 2 == 0)
        {
            char label[24];

            std::snprintf(
                label,
                sizeof(label),
                "%.0f%s",
                yMax * (float)k / 4.0f,
                unit
            );

            d->AddText(
                ImVec2(
                    r.mn.x + 6.0f,
                    y - ImGui::GetTextLineHeight() * 0.5f
                ),
                ImGui::GetColorU32(COLOR_MUTED),
                label
            );
        }
    }

    char ago[24];

    std::snprintf(
        ago,
        sizeof(ago),
        "%d s ago",
        count - 1
    );

    d->AddText(
        ImVec2(r.x0, r.y1 + 5.0f),
        ImGui::GetColorU32(COLOR_MUTED),
        ago
    );

    const float nowW =
        ImGui::CalcTextSize("now").x;

    d->AddText(
        ImVec2(r.x1 - nowW, r.y1 + 5.0f),
        ImGui::GetColorU32(COLOR_MUTED),
        "now"
    );
}

static void DrawAreaChart(
    const char* id,
    const float* values,
    int count,
    int offset,
    float yMax,
    ImVec2 size,
    const ImVec4& color,
    const char* unit = "%",
    bool detailed = true)
{
    const ChartRect r =
        BeginChartArea(id, size, detailed);

    if (count < 2 || yMax <= 0.0f)
        return;

    const bool hovered =
        detailed && ImGui::IsItemHovered();

    ImDrawList* d = ImGui::GetWindowDrawList();

    if (detailed)
        DrawChartGrid(r, yMax, count, unit);

    std::vector<float> raw(count);
    std::vector<float> smooth(count);

    for (int i = 0; i < count; ++i)
        raw[i] = values[(offset + i) % count];

    for (int i = 0; i < count; ++i)
    {
        const float a = raw[i > 0 ? i - 1 : 0];

        const float c =
            raw[i < count - 1 ? i + 1 : count - 1];

        smooth[i] =
            (a + 2.0f * raw[i] + c) * 0.25f;
    }

    std::vector<ImVec2> pts(count);

    for (int i = 0; i < count; ++i)
    {
        float t = smooth[i] / yMax;

        t = t < 0.0f ? 0.0f :
            (t > 1.0f ? 1.0f : t);

        pts[i] = ImVec2(
            r.x0 + (r.x1 - r.x0) *
            (float)i / (float)(count - 1),

            r.y1 - (r.y1 - r.y0) * t
        );
    }

    // Gradient fill
    {
        const ImVec2 uv =
            ImGui::GetFontTexUvWhitePixel();

        const ImU32 top =
            ImGui::GetColorU32(WithAlpha(color, 0.38f));

        const ImU32 bot =
            ImGui::GetColorU32(WithAlpha(color, 0.0f));

        d->PrimReserve(
            (count - 1) * 6,
            (count - 1) * 4
        );

        for (int i = 0; i < count - 1; ++i)
        {
            const ImDrawIdx base =
                (ImDrawIdx)d->_VtxCurrentIdx;

            d->PrimWriteVtx(pts[i], uv, top);

            d->PrimWriteVtx(
                pts[i + 1],
                uv,
                top
            );

            d->PrimWriteVtx(
                ImVec2(pts[i + 1].x, r.y1),
                uv,
                bot
            );

            d->PrimWriteVtx(
                ImVec2(pts[i].x, r.y1),
                uv,
                bot
            );

            d->PrimWriteIdx(base);
            d->PrimWriteIdx(base + 1);
            d->PrimWriteIdx(base + 2);

            d->PrimWriteIdx(base);
            d->PrimWriteIdx(base + 2);
            d->PrimWriteIdx(base + 3);
        }
    }

    d->AddPolyline(
        pts.data(),
        count,
        ImGui::GetColorU32(color),
        0,
        2.2f
    );

    // Latest sample pulse
    {
        const ImVec2 last = pts[count - 1];

        const float wave =
            0.5f + 0.5f *
            std::sin((float)ImGui::GetTime() * 3.0f);

        d->AddCircleFilled(
            last,
            detailed ? 9.0f + wave * 3.0f : 6.0f,
            ImGui::GetColorU32(
                WithAlpha(color, 0.12f + 0.10f * (1.0f - wave))
            )
        );

        d->AddCircleFilled(
            last,
            detailed ? 4.0f : 3.0f,
            ImGui::GetColorU32(color)
        );
    }

    // Hover tooltip
    if (hovered)
    {
        const float mx = ImGui::GetIO().MousePos.x;

        int idx = (int)std::lround(
            (mx - r.x0) / (r.x1 - r.x0) *
            (float)(count - 1)
        );

        idx = idx < 0 ? 0 :
            (idx > count - 1 ? count - 1 : idx);

        const ImVec2 p = pts[idx];

        d->AddLine(
            ImVec2(p.x, r.y0),
            ImVec2(p.x, r.y1),
            ImGui::GetColorU32(
                WithAlpha(COLOR_TEXT, 0.25f)
            )
        );

        d->AddCircleFilled(
            p,
            5.0f,
            ImGui::GetColorU32(color)
        );

        d->AddCircle(
            p,
            5.0f,
            IM_COL32(255, 255, 255, 220),
            0,
            1.5f
        );

        ImGui::SetTooltip(
            "%.1f%s\n%d s ago",
            raw[idx],
            unit,
            count - 1 - idx
        );
    }

    if (detailed)
    {
        float sum = 0.0f;
        float peak = 0.0f;

        for (float v : raw)
        {
            sum += v;
            peak = std::max(peak, v);
        }

        ImGui::TextDisabled(
            "avg %.0f%s    peak %.0f%s    now %.0f%s",
            sum / (float)count,
            unit,
            peak,
            unit,
            raw[count - 1],
            unit
        );
    }
}

// ------------------------------------------------------------
// Bar chart
// ------------------------------------------------------------

static void DrawBarChart(
    const char* id,
    const float* values,
    int count,
    int offset,
    float yMax,
    ImVec2 size,
    const char* unit = "")
{
    const ChartRect r =
        BeginChartArea(id, size, true);

    if (count < 1 || yMax <= 0.0f)
        return;

    const bool hovered = ImGui::IsItemHovered();

    ImDrawList* d = ImGui::GetWindowDrawList();

    DrawChartGrid(r, yMax, count, unit);

    const float slot =
        (r.x1 - r.x0) / (float)count;

    const float barW =
        std::max(1.5f, slot * 0.62f);

    int hoverIdx = -1;

    if (hovered)
    {
        hoverIdx =
            (int)((ImGui::GetIO().MousePos.x - r.x0) / slot);

        hoverIdx = hoverIdx < 0 ? 0 :
            (hoverIdx > count - 1 ? count - 1 : hoverIdx);

        d->AddRectFilled(
            ImVec2(r.x0 + slot * hoverIdx, r.y0),
            ImVec2(r.x0 + slot * (hoverIdx + 1), r.y1),
            ImGui::GetColorU32(
                WithAlpha(COLOR_TEXT, 0.06f)
            ),
            3.0f
        );
    }

    for (int i = 0; i < count; ++i)
    {
        const float v =
            values[(offset + i) % count];

        const float x =
            r.x0 + slot * (float)i +
            (slot - barW) * 0.5f;

        if (v <= 0.0f)
        {
            d->AddRectFilled(
                ImVec2(x, r.y1 - 2.0f),
                ImVec2(x + barW, r.y1),
                ImGui::GetColorU32(
                    WithAlpha(COLOR_MUTED, 0.30f)
                ),
                1.0f
            );

            continue;
        }

        float t = v / yMax;

        t = t > 1.0f ? 1.0f : t;

        const float h =
            std::max(4.0f, (r.y1 - r.y0) * t);

        const ImVec4 c =
            t < 0.34f ? COLOR_YELLOW :
            t < 0.67f ? COLOR_ORANGE :
            COLOR_RED;

        d->AddRectFilled(
            ImVec2(x, r.y1 - h),
            ImVec2(x + barW, r.y1),
            ImGui::GetColorU32(c),
            3.0f,
            ImDrawFlags_RoundCornersTop
        );
    }

    if (hoverIdx >= 0)
    {
        const float v =
            values[(offset + hoverIdx) % count];

        ImGui::SetTooltip(
            "Threat score %.0f\n%d s ago",
            v,
            count - 1 - hoverIdx
        );
    }
}

// ------------------------------------------------------------
// Backdrop
// ------------------------------------------------------------

static void DrawBackdrop()
{
    const ImVec2 size =
        ImGui::GetIO().DisplaySize;

    ImGui::GetBackgroundDrawList()->AddRectFilledMultiColor(
        ImVec2(0.0f, 0.0f),
        size,

        ImGui::GetColorU32(
            MixColor(HIDS_COLOR_BACKGROUND, COLOR_BLUE, 0.16f)
        ),

        ImGui::GetColorU32(
            MixColor(HIDS_COLOR_BACKGROUND, COLOR_CYAN, 0.07f)
        ),

        ImGui::GetColorU32(HIDS_COLOR_BACKGROUND),

        ImGui::GetColorU32(
            MixColor(HIDS_COLOR_BACKGROUND, COLOR_BLUE, 0.04f)
        )
    );
}

static void DrawGlow(
    ImDrawList* d,
    ImVec2 center,
    float radius,
    const ImVec4& color)
{
    for (int i = 0; i < 5; ++i)
    {
        d->AddCircleFilled(
            center,
            radius * (1.0f - i * 0.18f),
            ImGui::GetColorU32(
                WithAlpha(color, 0.035f)
            ),
            36
        );
    }
}

// ------------------------------------------------------------
// Ring gauge
// ------------------------------------------------------------

static void DrawGauge(
    ImDrawList* d,
    ImVec2 center,
    float radius,
    float thickness,
    float fraction,
    const ImVec4& color)
{
    if (fraction < 0.0f)
        fraction = 0.0f;

    if (fraction > 1.0f)
        fraction = 1.0f;

    d->AddCircle(
        center,
        radius,
        ImGui::GetColorU32(
            WithAlpha(color, 0.18f)
        ),
        48,
        thickness
    );

    if (fraction > 0.005f)
    {
        const float a0 = -IM_PI * 0.5f;

        d->PathClear();

        d->PathArcTo(
            center,
            radius,
            a0,
            a0 + fraction * IM_PI * 2.0f,
            48
        );

        d->PathStroke(
            ImGui::GetColorU32(color),
            0,
            thickness
        );
    }
}

// ------------------------------------------------------------
// Data source badge
// ------------------------------------------------------------

static void DataSourceBadge(bool monitoringOn)
{
    if (demoMode)
        DrawPill("DEMO DATA ON", COLOR_YELLOW);

    else if (monitoringOn)
        DrawPill("LIVE MONITORING", COLOR_GREEN);

    else
        DrawPill("MONITORING STOPPED", COLOR_RED);
}

// ------------------------------------------------------------
// Sidebar icons
// ------------------------------------------------------------

static void DrawPageIcon(
    ImDrawList* d,
    ImVec2 c,
    float r,
    int page,
    ImU32 col)
{
    const float t = 1.6f;

    switch (page)
    {
    case 0:
    {
        const float g = 1.5f;

        for (int i = 0; i < 4; ++i)
        {
            const float x0 =
                c.x + ((i % 2) ? g : -r);

            const float y0 =
                c.y + ((i / 2) ? g : -r);

            d->AddRectFilled(
                ImVec2(x0, y0),
                ImVec2(x0 + (r - g), y0 + (r - g)),
                col,
                2.0f
            );
        }

        break;
    }

    case 1:
        for (int i = -1; i <= 1; ++i)
        {
            const float y = c.y + i * r * 0.7f;

            d->AddCircleFilled(
                ImVec2(c.x - r * 0.8f, y),
                1.6f,
                col
            );

            d->AddLine(
                ImVec2(c.x - r * 0.35f, y),
                ImVec2(c.x + r, y),
                col,
                t
            );
        }
        break;

    case 2:
        d->AddCircle(
            ImVec2(c.x, c.y - r * 0.40f),
            r * 0.38f,
            col,
            16,
            t
        );

        d->PathClear();

        d->PathArcTo(
            ImVec2(c.x, c.y + r * 1.0f),
            r * 0.85f,
            IM_PI,
            IM_PI * 2.0f,
            16
        );

        d->PathStroke(col, 0, t);
        break;

    case 3:
        d->AddRectFilled(
            ImVec2(c.x - r, c.y - r * 0.9f),
            ImVec2(c.x - r * 0.1f, c.y - r * 0.5f),
            col,
            1.5f
        );

        d->AddRect(
            ImVec2(c.x - r, c.y - r * 0.6f),
            ImVec2(c.x + r, c.y + r * 0.8f),
            col,
            2.0f,
            0,
            t
        );
        break;

    case 4:
        d->AddTriangle(
            ImVec2(c.x, c.y - r),
            ImVec2(c.x - r, c.y + r * 0.8f),
            ImVec2(c.x + r, c.y + r * 0.8f),
            col,
            t
        );

        d->AddLine(
            ImVec2(c.x, c.y - r * 0.3f),
            ImVec2(c.x, c.y + r * 0.2f),
            col,
            t
        );

        d->AddCircleFilled(
            ImVec2(c.x, c.y + r * 0.52f),
            1.4f,
            col
        );
        break;

    case 5:
        d->AddRect(
            ImVec2(c.x - r * 0.75f, c.y - r),
            ImVec2(c.x + r * 0.75f, c.y + r),
            col,
            2.0f,
            0,
            t
        );

        for (int i = 0; i < 3; ++i)
        {
            const float y =
                c.y - r * 0.4f + i * r * 0.5f;

            d->AddLine(
                ImVec2(c.x - r * 0.4f, y),
                ImVec2(c.x + r * 0.4f, y),
                col,
                1.2f
            );
        }
        break;

    case 6:
    {
        const ImVec2 pts[4] =
        {
            ImVec2(c.x - r, c.y + r * 0.6f),
            ImVec2(c.x - r * 0.3f, c.y - r * 0.1f),
            ImVec2(c.x + r * 0.2f, c.y + r * 0.3f),
            ImVec2(c.x + r, c.y - r * 0.7f)
        };

        d->AddPolyline(pts, 4, col, 0, t);
        break;
    }

    case 7:
        d->AddCircle(
            ImVec2(c.x - r * 0.15f, c.y - r * 0.15f),
            r * 0.65f,
            col,
            20,
            t
        );

        d->AddLine(
            ImVec2(c.x + r * 0.35f, c.y + r * 0.35f),
            ImVec2(c.x + r, c.y + r),
            col,
            t + 0.5f
        );
        break;

    case 8:
        d->AddCircle(c, r * 0.62f, col, 20, t);

        for (int i = 0; i < 8; ++i)
        {
            const float a = i * IM_PI * 0.25f;

            const float ca = std::cos(a);
            const float sa = std::sin(a);

            d->AddLine(
                ImVec2(
                    c.x + ca * r * 0.8f,
                    c.y + sa * r * 0.8f
                ),

                ImVec2(
                    c.x + ca * r,
                    c.y + sa * r
                ),

                col,
                t + 0.6f
            );
        }
        break;
    }
}

// ------------------------------------------------------------
// Settings page helpers
// ------------------------------------------------------------

static void ThemeHeading(
    const char* title,
    const char* hint = nullptr)
{
    {
        const ImVec2 p =
            ImGui::GetCursorScreenPos();

        const float h =
            ImGui::GetTextLineHeight() * 1.1f;

        ImGui::GetWindowDrawList()->AddRectFilled(
            ImVec2(p.x, p.y + 1.0f),
            ImVec2(p.x + 4.0f, p.y + h),
            ImGui::GetColorU32(COLOR_CYAN),
            2.0f
        );

        ImGui::SetCursorPosX(
            ImGui::GetCursorPosX() + 12.0f
        );
    }

    ImGui::PushStyleColor(
        ImGuiCol_Text,
        COLOR_CYAN
    );

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

// ------------------------------------------------------------
// Settings page
// ------------------------------------------------------------

static void DrawSettingsPage(bool monitoringOn)
{
    ImGui::BeginChild(
        "SettingsPanel",
        ImVec2(0.0f, 0.0f),
        true
    );

    ThemeHeading(
        "Appearance",
        "click a theme to apply it instantly"
    );

    const float gap =
        ImGui::GetStyle().ItemSpacing.x;

    const float cardW =
        (ImGui::GetContentRegionAvail().x -
         gap * (THEME_COUNT - 1)) /
        (float)THEME_COUNT;

    const float cardH = 132.0f;

    for (int i = 0; i < THEME_COUNT; ++i)
    {
        if (i > 0)
            ImGui::SameLine();

        ImGui::PushID(i);

        const ImVec2 p =
            ImGui::GetCursorScreenPos();

        const bool clicked =
            ImGui::InvisibleButton(
                "##theme",
                ImVec2(cardW, cardH)
            );

        const bool hovered =
            ImGui::IsItemHovered();

        if (hovered)
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        if (clicked)
        {
            themeIndex = i;
            pendingTheme = i;
            SaveUiConfig();
        }

        const UiTheme& t = THEMES[i];

        const bool selected =
            (themeIndex == i);

        ImDrawList* d =
            ImGui::GetWindowDrawList();

        auto U = [](const ImVec4& c)
        {
            return ImGui::GetColorU32(c);
        };

        auto P = [&](float fx, float y)
        {
            return ImVec2(
                p.x + cardW * fx,
                p.y + y
            );
        };

        d->AddRectFilled(
            p,
            ImVec2(p.x + cardW, p.y + cardH),
            U(t.bg),
            10.0f
        );

        d->AddRectFilled(
            p,
            P(0.20f, cardH),
            U(t.sidebar),
            10.0f,
            ImDrawFlags_RoundCornersLeft
        );

        for (int n = 0; n < 4; ++n)
        {
            d->AddRectFilled(
                P(0.04f, 14.0f + n * 12.0f),
                P(
                    n == 0 ? 0.17f : 0.14f,
                    20.0f + n * 12.0f
                ),
                U(
                    n == 0
                        ? t.secondary
                        : WithAlpha(t.muted, 0.5f)
                ),
                3.0f
            );
        }

        d->AddRectFilled(
            P(0.25f, 12.0f),
            P(0.60f, 54.0f),
            U(t.panel),
            6.0f
        );

        d->AddRectFilled(
            P(0.25f, 12.0f),
            P(0.26f, 54.0f),
            U(t.primary),
            2.0f
        );

        d->AddRectFilled(
            P(0.64f, 12.0f),
            P(0.95f, 54.0f),
            U(t.panel),
            6.0f
        );

        d->AddRectFilled(
            P(0.64f, 12.0f),
            P(0.65f, 54.0f),
            U(t.good),
            2.0f
        );

        const ImVec2 line[5] =
        {
            P(0.29f, 46.0f),
            P(0.36f, 36.0f),
            P(0.43f, 42.0f),
            P(0.50f, 28.0f),
            P(0.57f, 33.0f)
        };

        d->AddPolyline(
            line,
            5,
            U(t.secondary),
            0,
            1.6f
        );

        d->AddCircleFilled(
            P(0.27f, 70.0f),
            5.0f,
            U(t.primary)
        );

        d->AddCircleFilled(
            ImVec2(
                P(0.27f, 70.0f).x + 16.0f,
                p.y + 70.0f
            ),
            5.0f,
            U(t.secondary)
        );

        d->AddCircleFilled(
            ImVec2(
                P(0.27f, 70.0f).x + 32.0f,
                p.y + 70.0f
            ),
            5.0f,
            U(t.good)
        );

        d->AddText(
            P(0.25f, 86.0f),
            U(t.text),
            t.name
        );

        d->AddText(
            P(
                0.25f,
                86.0f + ImGui::GetTextLineHeight() + 2.0f
            ),
            U(t.muted),
            t.tagline
        );

        d->AddRect(
            p,
            ImVec2(p.x + cardW, p.y + cardH),

            U(
                selected
                    ? t.primary
                    : (
                        hovered
                            ? WithAlpha(t.primary, 0.60f)
                            : t.border
                    )
            ),

            10.0f,
            0,
            selected ? 2.5f : 1.0f
        );

        if (selected)
        {
            const ImVec2 ctr(
                p.x + cardW - 16.0f,
                p.y + 16.0f
            );

            d->AddCircleFilled(
                ctr,
                9.0f,
                U(t.primary)
            );

            const ImVec2 tick[3] =
            {
                ImVec2(ctr.x - 4.0f, ctr.y),
                ImVec2(ctr.x - 1.0f, ctr.y + 3.5f),
                ImVec2(ctr.x + 4.5f, ctr.y - 3.5f)
            };

            d->AddPolyline(
                tick,
                3,
                IM_COL32(255, 255, 255, 255),
                0,
                2.0f
            );
        }

        ImGui::PopID();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Interface size
    ThemeHeading("Interface");

    ImGui::TextUnformatted("Interface size");

    ImGui::SetNextItemWidth(260.0f);

    if (ImGui::SliderFloat(
        "##uiscale",
        &uiScale,
        0.85f,
        1.40f,
        "%.2fx"
    ))
    {
        ImGui::GetIO().FontGlobalScale =
            baseFontScale * uiScale;
    }

    if (ImGui::IsItemDeactivatedAfterEdit())
        SaveUiConfig();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Data source
    ThemeHeading(
        "Data source",
        "real monitoring vs sample data"
    );

    ImGui::Text("Real monitoring:");
    ImGui::SameLine();

    ImGui::TextColored(
        monitoringOn ? COLOR_GREEN : COLOR_RED,
        monitoringOn ? "running" : "stopped"
    );

    if (ImGui::Checkbox(
        "Show demo data",
        &demoMode
    ))
    {
        SaveUiConfig();
    }

    ImGui::SameLine();

    ImGui::TextDisabled(
        "adds a few sample alerts, processes and events"
    );

    if (demoMode)
    {
        ImGui::TextColored(
            COLOR_YELLOW,
            "Demo rows are tagged [DEMO], use source \"DEMO\" and can never be "
            "ended or exported as real findings."
        );
    }
    else
    {
        ImGui::TextDisabled(
            "Everything shown comes from the HIDS monitors on this machine."
        );
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button(
        "Reset appearance to defaults"
    ))
    {
        themeIndex = 0;
        pendingTheme = 0;
        uiScale = 1.0f;
        demoMode = false;

        ImGui::GetIO().FontGlobalScale =
            baseFontScale;

        SaveUiConfig();
    }

    ImGui::EndChild();
}