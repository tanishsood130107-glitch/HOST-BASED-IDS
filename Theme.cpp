#include "Theme.h"
#include "imgui.h"

namespace HIDS {
namespace {
Theme currentTheme = Theme::MidnightGreen;

void SetCommonStyle()
{
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowPadding = ImVec2(18, 16);
    s.FramePadding = ImVec2(12, 9);
    s.ItemSpacing = ImVec2(10, 10);
    s.ItemInnerSpacing = ImVec2(8, 6);
    s.WindowRounding = 10.0f;
    s.ChildRounding = 9.0f;
    s.FrameRounding = 6.0f;
    s.PopupRounding = 7.0f;
    s.ScrollbarRounding = 8.0f;
    s.GrabRounding = 6.0f;
    s.WindowBorderSize = 0.0f;
    s.ChildBorderSize = 1.0f;
    s.FrameBorderSize = 0.0f;
    s.ScrollbarSize = 13.0f;
    s.GrabMinSize = 10.0f;
}

void ApplyMidnightGreen()
{
    ImGui::StyleColorsDark();
    SetCommonStyle();
    ImVec4* c = ImGui::GetStyle().Colors;

    c[ImGuiCol_WindowBg] = ImVec4(0.055f, 0.075f, 0.095f, 1);
    c[ImGuiCol_ChildBg] = ImVec4(0.085f, 0.110f, 0.135f, 1);
    c[ImGuiCol_PopupBg] = ImVec4(0.090f, 0.120f, 0.145f, 0.98f);
    c[ImGuiCol_Text] = ImVec4(0.91f, 0.94f, 0.96f, 1);
    c[ImGuiCol_TextDisabled] = ImVec4(0.48f, 0.55f, 0.60f, 1);
    c[ImGuiCol_Border] = ImVec4(0.17f, 0.22f, 0.26f, 1);
    c[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.16f, 0.19f, 1);
    c[ImGuiCol_FrameBgHovered] = ImVec4(0.15f, 0.22f, 0.23f, 1);
    c[ImGuiCol_FrameBgActive] = ImVec4(0.18f, 0.27f, 0.26f, 1);
    c[ImGuiCol_Button] = ImVec4(0.08f, 0.48f, 0.29f, 1);
    c[ImGuiCol_ButtonHovered] = ImVec4(0.10f, 0.60f, 0.36f, 1);
    c[ImGuiCol_ButtonActive] = ImVec4(0.05f, 0.36f, 0.22f, 1);
    c[ImGuiCol_Header] = ImVec4(0.10f, 0.34f, 0.24f, 1);
    c[ImGuiCol_HeaderHovered] = ImVec4(0.12f, 0.45f, 0.30f, 1);
    c[ImGuiCol_HeaderActive] = ImVec4(0.09f, 0.52f, 0.32f, 1);
    c[ImGuiCol_CheckMark] = ImVec4(0.18f, 0.82f, 0.48f, 1);
    c[ImGuiCol_SliderGrab] = ImVec4(0.12f, 0.72f, 0.42f, 1);
    c[ImGuiCol_SliderGrabActive] = ImVec4(0.20f, 0.90f, 0.54f, 1);
    c[ImGuiCol_Separator] = ImVec4(0.18f, 0.23f, 0.27f, 1);
    c[ImGuiCol_ResizeGrip] = ImVec4(0.10f, 0.55f, 0.34f, 0.50f);
    c[ImGuiCol_ResizeGripHovered] = ImVec4(0.12f, 0.72f, 0.42f, 0.75f);
    c[ImGuiCol_ResizeGripActive] = ImVec4(0.18f, 0.82f, 0.48f, 1);
}

void ApplyWindowsLight()
{
    ImGui::StyleColorsLight();
    SetCommonStyle();
    ImVec4* c = ImGui::GetStyle().Colors;

    c[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.95f, 0.96f, 1);
    c[ImGuiCol_ChildBg] = ImVec4(1, 1, 1, 1);
    c[ImGuiCol_PopupBg] = ImVec4(1, 1, 1, 0.98f);
    c[ImGuiCol_Text] = ImVec4(0.12f, 0.15f, 0.18f, 1);
    c[ImGuiCol_TextDisabled] = ImVec4(0.48f, 0.52f, 0.55f, 1);
    c[ImGuiCol_Border] = ImVec4(0.84f, 0.87f, 0.89f, 1);
    c[ImGuiCol_FrameBg] = ImVec4(0.90f, 0.92f, 0.93f, 1);
    c[ImGuiCol_FrameBgHovered] = ImVec4(0.85f, 0.92f, 0.87f, 1);
    c[ImGuiCol_FrameBgActive] = ImVec4(0.80f, 0.90f, 0.83f, 1);
    c[ImGuiCol_Button] = ImVec4(0.08f, 0.45f, 0.26f, 1);
    c[ImGuiCol_ButtonHovered] = ImVec4(0.10f, 0.55f, 0.32f, 1);
    c[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.36f, 0.21f, 1);
    c[ImGuiCol_Header] = ImVec4(0.83f, 0.92f, 0.86f, 1);
    c[ImGuiCol_HeaderHovered] = ImVec4(0.76f, 0.89f, 0.81f, 1);
    c[ImGuiCol_HeaderActive] = ImVec4(0.69f, 0.85f, 0.75f, 1);
    c[ImGuiCol_CheckMark] = ImVec4(0.08f, 0.52f, 0.29f, 1);
    c[ImGuiCol_SliderGrab] = ImVec4(0.08f, 0.52f, 0.29f, 1);
    c[ImGuiCol_SliderGrabActive] = ImVec4(0.05f, 0.40f, 0.22f, 1);
    c[ImGuiCol_Separator] = ImVec4(0.83f, 0.86f, 0.88f, 1);
}

void ApplyCyberBlue()
{
    ImGui::StyleColorsDark();
    SetCommonStyle();
    ImVec4* c = ImGui::GetStyle().Colors;

    c[ImGuiCol_WindowBg] = ImVec4(0.055f, 0.060f, 0.085f, 1);
    c[ImGuiCol_ChildBg] = ImVec4(0.085f, 0.095f, 0.135f, 1);
    c[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.09f, 0.13f, 0.98f);
    c[ImGuiCol_Text] = ImVec4(0.91f, 0.94f, 1, 1);
    c[ImGuiCol_TextDisabled] = ImVec4(0.49f, 0.54f, 0.64f, 1);
    c[ImGuiCol_Border] = ImVec4(0.20f, 0.24f, 0.34f, 1);
    c[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.14f, 0.20f, 1);
    c[ImGuiCol_FrameBgHovered] = ImVec4(0.14f, 0.20f, 0.29f, 1);
    c[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.23f, 0.34f, 1);
    c[ImGuiCol_Button] = ImVec4(0.10f, 0.38f, 0.67f, 1);
    c[ImGuiCol_ButtonHovered] = ImVec4(0.13f, 0.49f, 0.82f, 1);
    c[ImGuiCol_ButtonActive] = ImVec4(0.07f, 0.30f, 0.55f, 1);
    c[ImGuiCol_Header] = ImVec4(0.12f, 0.25f, 0.40f, 1);
    c[ImGuiCol_HeaderHovered] = ImVec4(0.15f, 0.34f, 0.52f, 1);
    c[ImGuiCol_HeaderActive] = ImVec4(0.10f, 0.42f, 0.67f, 1);
    c[ImGuiCol_CheckMark] = ImVec4(0.22f, 0.73f, 1, 1);
    c[ImGuiCol_SliderGrab] = ImVec4(0.17f, 0.63f, 0.96f, 1);
    c[ImGuiCol_SliderGrabActive] = ImVec4(0.35f, 0.80f, 1, 1);
    c[ImGuiCol_Separator] = ImVec4(0.19f, 0.24f, 0.34f, 1);
}
} // namespace

void ApplyTheme(Theme theme)
{
    currentTheme = theme;
    switch (theme) {
    case Theme::MidnightGreen: ApplyMidnightGreen(); break;
    case Theme::WindowsLight:  ApplyWindowsLight(); break;
    case Theme::CyberBlue:     ApplyCyberBlue(); break;
    default:
        currentTheme = Theme::MidnightGreen;
        ApplyMidnightGreen();
        break;
    }
}

Theme GetTheme() { return currentTheme; }
void SetTheme(Theme theme) { ApplyTheme(theme); }