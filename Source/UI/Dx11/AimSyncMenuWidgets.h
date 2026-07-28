#pragma once

#include <Utils/ManuallyDestructible.h>
#include <GlobalContext/GlobalContext.h>
#include <HookContext/HookContext.h>
#include <HookContext/HookContextMacros.h>
#include <ThirdParty/ImGui/imgui.h>
#include <ThirdParty/ImGui/imgui_internal.h>

#include "AimSyncMenuStyle.h"

namespace aimsync_menu_widgets
{

inline void labelText(const char* label) noexcept
{
    // Text*() does not hide "##id" — strip like Button/Checkbox do.
    const char* textEnd = ImGui::FindRenderedTextEnd(label);
    ImGui::TextUnformatted(label, textEnd);
}

inline void alignRight(float contentWidth) noexcept
{
    const float columnWidth = ImGui::GetColumnWidth() - ImGui::GetStyle().ItemSpacing.x;
    ImGui::SetCursorPosX(ImGui::GetColumnOffset() + columnWidth - contentWidth);
}

inline void gradientText(const char* text) noexcept
{
    const ImVec2 cursor = ImGui::GetCursorScreenPos();
    constexpr float width = 180.0f;
    const float height = ImGui::GetTextLineHeight() + 4.0f;

    ImVec4 color = ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
    const ImU32 opaque = ImGui::ColorConvertFloat4ToU32(color);
    color.w = 0.0f;
    const ImU32 transparent = ImGui::ColorConvertFloat4ToU32(color);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilledMultiColor(
        ImVec2{cursor.x - 12.0f, cursor.y},
        ImVec2{cursor.x - 12.0f + width, cursor.y + height},
        opaque, transparent, transparent, opaque);

    labelText(text);
    ImGui::Dummy(ImVec2{0.0f, 2.0f});
}

inline void switchButton(const char* id, bool* value) noexcept
{
    const ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const float height = ImGui::GetFrameHeight() * 1.5f;
    const float width = height;
    const float radius = height / 4.0f - 1.0f;

    ImGui::InvisibleButton(id, ImVec2{width, height - 10.0f});
    if (ImGui::IsItemClicked())
        *value = !*value;

    const float t = *value ? 1.0f : 0.0f;
    const ImU32 knobColor = ImGui::GetColorU32(*value ? ImGuiCol_CheckMark : ImGuiCol_ButtonHovered);

    drawList->AddRectFilled(
        ImVec2{p.x, p.y + height * 0.30f},
        ImVec2{p.x + width, p.y + height * 0.70f},
        ImGui::GetColorU32(ImGuiCol_Border),
        height);
    drawList->AddCircleFilled(
        ImVec2{p.x + radius + t * (width - radius * 2.0f), p.y + radius + 9.5f},
        radius,
        knobColor,
        24);
}

template <typename ConfigVariable>
void putSwitch(HookContext<GlobalContext>& hookContext, const char* label) noexcept
{
    ImGui::PushID(label);
    const float cursorX = ImGui::GetCursorPosX();
    const float cursorY = ImGui::GetCursorPosY();
    ImGui::SetCursorPosX(cursorX + 10.0f);
    labelText(label);
    ImGui::SameLine();
    ImGui::SetCursorPosY(cursorY - 2.0f);

    bool value = static_cast<bool>(GET_CONFIG_VAR(ConfigVariable));
    const float switchWidth = ImGui::GetFrameHeight() * 1.7f;
    // Columns can report tiny widths — avoid negative cursor math crashes.
    if (ImGui::GetColumnWidth() > switchWidth + 20.0f)
        alignRight(switchWidth);
    switchButton("##sw", &value);
    if (value != static_cast<bool>(GET_CONFIG_VAR(ConfigVariable)))
        hookContext.config().template setVariable<ConfigVariable>(value);

    ImGui::PopID();
}

template <typename ConfigVariable>
void putSliderU8(HookContext<GlobalContext>& hookContext, const char* label) noexcept
{
    using ValueType = typename ConfigVariable::ValueType;
    using Underlying = typename ValueType::ValueType;

    ImGui::PushID(label);
    const float cursorX = ImGui::GetCursorPosX();
    const float sliderWidth = ImGui::GetColumnWidth() - ImGui::GetStyle().ItemSpacing.x - 25.0f;

    int value = static_cast<int>(static_cast<Underlying>(GET_CONFIG_VAR(ConfigVariable)));
    ImGui::SetCursorPosX(cursorX + 10.0f);
    labelText(label);
    ImGui::SameLine();
    ImGui::Text("%d", value);

    ImGui::SetCursorPosX(cursorX + 10.0f);
    ImGui::SetNextItemWidth(sliderWidth);
    if (ImGui::SliderInt("##sl", &value, static_cast<int>(ValueType::kMin), static_cast<int>(ValueType::kMax), ""))
        hookContext.config().template setVariable<ConfigVariable>(ValueType{static_cast<Underlying>(value)});

    ImGui::PopID();
}

inline void putButton(const char* label, float width, float height) noexcept
{
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
    ImGui::Button(label, ImVec2{width, height});
}

}
