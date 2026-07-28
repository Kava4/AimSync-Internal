#pragma once

#include <ThirdParty/ImGui/imgui.h>

namespace aimsync_menu_style
{

inline constexpr ImVec2 kMainWinSize{640.0f, 520.0f};
inline constexpr ImVec2 kLogoPos{0.0f, 0.0f};
inline constexpr ImVec2 kButton1Pos{0.0f, 64.0f};
inline constexpr ImVec2 kButton2Pos{0.0f, 128.0f};
inline constexpr ImVec2 kButton3Pos{0.0f, 192.0f};
inline constexpr ImVec2 kButton4Pos{0.0f, 256.0f};
inline constexpr ImVec2 kChildPos{68.0f, 0.0f};
inline constexpr ImVec2 kChildSize{572.0f, 520.0f};

inline void apply() noexcept
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 6.0f;
    style.ChildRounding = 4.0f;
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.WindowBorderSize = 1.0f;
    style.ChildBorderSize = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.ItemSpacing = ImVec2(8.0f, 6.0f);
    style.WindowPadding = ImVec2(0.0f, 0.0f);
    style.ScrollbarSize = 10.0f;
    style.AntiAliasedLines = true;
    style.AntiAliasedFill = true;
    style.AntiAliasedLinesUseTex = true;

    ImVec4* c = style.Colors;
    c[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.07f, 0.09f, 0.96f);
    c[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.08f, 0.10f, 1.0f);
    c[ImGuiCol_Border] = ImVec4(0.18f, 0.20f, 0.24f, 1.0f);
    c[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.13f, 0.16f, 1.0f);
    c[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.18f, 0.22f, 1.0f);
    c[ImGuiCol_FrameBgActive] = ImVec4(0.18f, 0.22f, 0.28f, 1.0f);
    c[ImGuiCol_Button] = ImVec4(0.14f, 0.16f, 0.20f, 1.0f);
    c[ImGuiCol_ButtonHovered] = ImVec4(0.22f, 0.40f, 0.55f, 1.0f);
    c[ImGuiCol_ButtonActive] = ImVec4(0.28f, 0.55f, 0.75f, 1.0f);
    c[ImGuiCol_Header] = ImVec4(0.14f, 0.16f, 0.20f, 1.0f);
    c[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.35f, 0.48f, 1.0f);
    c[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.45f, 0.62f, 1.0f);
    c[ImGuiCol_CheckMark] = ImVec4(0.35f, 0.75f, 0.95f, 1.0f);
    c[ImGuiCol_SliderGrab] = ImVec4(0.35f, 0.72f, 0.92f, 1.0f);
    c[ImGuiCol_SliderGrabActive] = ImVec4(0.45f, 0.82f, 1.0f, 1.0f);
    c[ImGuiCol_Text] = ImVec4(0.92f, 0.93f, 0.95f, 1.0f);
    c[ImGuiCol_TextDisabled] = ImVec4(0.55f, 0.58f, 0.64f, 1.0f);
    c[ImGuiCol_Separator] = ImVec4(0.20f, 0.22f, 0.26f, 1.0f);
    c[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.06f, 0.07f, 1.0f);
    c[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.28f, 0.32f, 1.0f);
}

}
