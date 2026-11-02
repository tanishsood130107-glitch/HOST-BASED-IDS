#pragma once

namespace HIDS {

enum class Theme {
    MidnightGreen = 0,
    WindowsLight  = 1,
    CyberBlue     = 2
};

void ApplyTheme(Theme theme);
Theme GetTheme();
void SetTheme(Theme theme);

} // namespace HIDS
