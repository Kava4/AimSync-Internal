#pragma once

// AimSync DX11 overlay menu shell — layout helpers bound to AimSync config variables.

#include <Utils/ManuallyDestructible.h>
#include <GlobalContext/GlobalContext.h>
#include <HookContext/HookContext.h>
#include <HookContext/HookContextMacros.h>

#include <Features/Combat/Aimbot/AimbotConfigVariables.h>
#include <Features/Combat/NoRecoil/NoRecoilConfigVariables.h>
#include <Features/Combat/NoSpread/NoSpreadConfigVariables.h>
#include <Features/Combat/Triggerbot/TriggerbotConfigVariables.h>
#include <Features/Combat/SniperRifles/NoScopeInaccuracyVis/NoScopeInaccuracyVisConfigVariables.h>
#include <Features/Hud/BombPlantAlert/BombPlantAlertConfigVariables.h>
#include <Features/Hud/BombTimer/BombTimerConfigVariables.h>
#include <Features/Hud/DefusingAlert/DefusingAlertConfigVariables.h>
#include <Features/Hud/KillfeedPreserver/KillfeedPreserverConfigVariables.h>
#include <Features/Hud/PostRoundTimer/PostRoundTimerConfigVariables.h>
#include <Features/Misc/AutoStrafe/AutoStrafeConfigVariables.h>
#include <Features/Misc/BunnyHop/BunnyHopConfigVariables.h>
#include <Features/Misc/ThirdPerson/ThirdPersonConfigVariables.h>
#include <Features/Sound/SoundVisualizationConfigVariables.h>
#include <Features/Visuals/GrenadePrediction/GrenadePredictionConfigVariables.h>
#include <Features/Visuals/ModelGlow/ModelGlowConfigVariables.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowConfigVariables.h>
#include <Features/Visuals/PlayerEsp/PlayerEspConfigVariables.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorldConfigVariables.h>
#include <Features/Visuals/ViewmodelMod/ViewmodelModConfigVariables.h>

#include <ThirdParty/ImGui/imgui.h>
#include <ThirdParty/ImGui/imgui_internal.h>

#include <cstring>

// Forward decl — avoid circular include with Dx11Menu.h
void dx11MenuRequestUnload() noexcept;

namespace aimsync_gui
{

inline constexpr ImVec2 kMainWinSize{600.0f, 500.0f};
inline constexpr ImVec2 kLogoPos{0.0f, 0.0f};
inline constexpr ImVec2 kButton1Pos{0.0f, 59.0f};
inline constexpr ImVec2 kButton2Pos{0.0f, 118.0f};
inline constexpr ImVec2 kButton3Pos{0.0f, 177.0f};
inline constexpr ImVec2 kButton4Pos{0.0f, 236.0f};
inline constexpr ImVec2 kChildPos{60.0f, 0.0f};
inline constexpr ImVec2 kChildSize{540.0f, 500.0f};
inline constexpr ImVec2 kSideBtnSize{60.0f, 59.0f};

inline int g_menuPage = 0;
inline bool g_btn1 = true;
inline bool g_btn2 = false;
inline bool g_btn3 = false;
inline bool g_btn4 = false;

inline void applyStyle() noexcept
{
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = 0.0f;
    s.ChildRounding = 0.0f;
    s.FrameRounding = 0.0f;
    s.GrabRounding = 0.0f;
    s.WindowBorderSize = 1.0f;
    s.ChildBorderSize = 0.0f;
    s.FrameBorderSize = 0.0f;
    s.WindowPadding = ImVec2{0.0f, 0.0f};
    s.ItemSpacing = ImVec2{8.0f, 6.0f};
    s.ScrollbarSize = 10.0f;
    s.AntiAliasedLines = true;
    s.AntiAliasedFill = true;

    ImVec4* c = s.Colors;
    c[ImGuiCol_WindowBg] = ImVec4{0.06f, 0.06f, 0.07f, 0.96f};
    c[ImGuiCol_ChildBg] = ImVec4{0.07f, 0.07f, 0.08f, 1.0f};
    c[ImGuiCol_Border] = ImVec4{0.20f, 0.22f, 0.25f, 1.0f};
    c[ImGuiCol_FrameBg] = ImVec4{0.12f, 0.13f, 0.15f, 1.0f};
    c[ImGuiCol_FrameBgHovered] = ImVec4{0.16f, 0.18f, 0.20f, 1.0f};
    c[ImGuiCol_FrameBgActive] = ImVec4{0.18f, 0.22f, 0.26f, 1.0f};
    c[ImGuiCol_Button] = ImVec4{0.12f, 0.13f, 0.15f, 1.0f};
    c[ImGuiCol_ButtonHovered] = ImVec4{0.23f, 0.28f, 0.34f, 1.0f};
    c[ImGuiCol_ButtonActive] = ImVec4{0.28f, 0.42f, 0.55f, 1.0f};
    c[ImGuiCol_CheckMark] = ImVec4{0.35f, 0.72f, 0.92f, 1.0f};
    c[ImGuiCol_SliderGrab] = ImVec4{0.35f, 0.72f, 0.92f, 1.0f};
    c[ImGuiCol_SliderGrabActive] = ImVec4{0.45f, 0.82f, 1.0f, 1.0f};
    c[ImGuiCol_Text] = ImVec4{0.90f, 0.91f, 0.93f, 1.0f};
    c[ImGuiCol_TextDisabled] = ImVec4{0.55f, 0.57f, 0.60f, 1.0f};
    c[ImGuiCol_Separator] = ImVec4{0.20f, 0.22f, 0.25f, 1.0f};
    c[ImGuiCol_Header] = ImVec4{0.14f, 0.16f, 0.18f, 1.0f};
    c[ImGuiCol_HeaderHovered] = ImVec4{0.20f, 0.32f, 0.42f, 1.0f};
    c[ImGuiCol_HeaderActive] = ImVec4{0.25f, 0.40f, 0.52f, 1.0f};
}

inline void switchButton(const char* strId, bool* v) noexcept
{
    const ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const float height = ImGui::GetFrameHeight() * 1.5f;
    const float width = height;
    const float radius = height / 4.0f - 1.0f;

    ImGui::InvisibleButton(strId, ImVec2{width, height - 10.0f});
    if (ImGui::IsItemClicked())
        *v = !*v;

    float t = *v ? 1.0f : 0.0f;
    ImGuiContext& g = *GImGui;
    constexpr float animSpeed = 0.08f;
    if (g.LastActiveId == g.CurrentWindow->GetID(strId)) {
        const float anim = ImSaturate(g.LastActiveIdTimer / animSpeed);
        t = *v ? anim : (1.0f - anim);
    }

    const ImU32 color = ImGui::GetColorU32(ImLerp(
        ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered),
        ImGui::GetStyleColorVec4(ImGuiCol_CheckMark),
        t));

    drawList->AddRectFilled(
        ImVec2{p.x, p.y + height * 0.30f},
        ImVec2{p.x + width, p.y + height * 0.70f},
        ImGui::GetColorU32(ImGuiCol_Border),
        height);
    drawList->AddCircleFilled(
        ImVec2{p.x + radius + t * (width - radius * 2.0f), p.y + radius + 9.5f},
        radius,
        color,
        360);
}

inline void gradientText(const char* text) noexcept
{
    const ImVec2 textSize = ImGui::CalcTextSize(text);
    constexpr float width = 180.0f;
    const float height = textSize.y + 2.0f;
    const ImVec2 cursor = ImGui::GetCursorScreenPos();

    ImVec4 color = ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
    const ImU32 opaque = ImGui::ColorConvertFloat4ToU32(color);
    color.w = 0.0f;
    const ImU32 transparent = ImGui::ColorConvertFloat4ToU32(color);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const ImVec2 p1{cursor.x - 15.0f, cursor.y + 3.5f};
    const ImVec2 p2{p1.x + width, p1.y + height};
    drawList->AddRectFilledMultiColor(p1, p2, opaque, transparent, transparent, opaque);

    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_ChildBg));
    ImGui::SetCursorScreenPos(ImVec2{cursor.x, cursor.y + 3.5f});
    ImGui::TextUnformatted(text);
    ImGui::PopStyleColor();
    ImGui::Dummy(ImVec2{0.0f, 4.0f});
}

inline void alignRight(float contentWidth) noexcept
{
    const float columnWidth = ImGui::GetColumnWidth() - ImGui::GetStyle().ItemSpacing.x;
    ImGui::SetCursorPosX(ImGui::GetColumnOffset() + columnWidth - contentWidth);
}

inline void putSwitch(const char* label, float cursorX, float contentWidth, bool* v) noexcept
{
    ImGui::PushID(label);
    const float x = ImGui::GetCursorPosX();
    const float y = ImGui::GetCursorPosY();
    ImGui::SetCursorPosX(x + cursorX);
    ImGui::TextDisabled("%s", label);
    ImGui::SameLine();
    ImGui::SetCursorPosY(y - 2.0f);
    alignRight(contentWidth);
    switchButton(label, v);
    ImGui::PopID();
}

template <typename ConfigVariable>
void putConfigSwitch(HookContext<GlobalContext>& hookContext, const char* label, float cursorX = 10.0f) noexcept
{
    bool value = static_cast<bool>(GET_CONFIG_VAR(ConfigVariable));
    putSwitch(label, cursorX, ImGui::GetFrameHeight() * 1.7f, &value);
    if (value != static_cast<bool>(GET_CONFIG_VAR(ConfigVariable)))
        hookContext.config().template setVariable<ConfigVariable>(value);
}

template <typename ConfigVariable>
void putConfigSliderU8(HookContext<GlobalContext>& hookContext, const char* label, float cursorX = 10.0f) noexcept
{
    using ValueType = typename ConfigVariable::ValueType;
    using Underlying = typename ValueType::ValueType;

    ImGui::PushID(label);
    const float x = ImGui::GetCursorPosX();
    const float sliderWidth = ImGui::GetColumnWidth() - ImGui::GetStyle().ItemSpacing.x - cursorX - 15.0f;
    int value = static_cast<int>(static_cast<Underlying>(GET_CONFIG_VAR(ConfigVariable)));

    ImGui::SetCursorPosX(x + cursorX);
    ImGui::TextDisabled("%s", label);
    ImGui::SameLine();
    ImGui::TextDisabled("%d", value);
    ImGui::SetCursorPosX(x + cursorX);
    ImGui::SetNextItemWidth(sliderWidth > 40.0f ? sliderWidth : 40.0f);
    if (ImGui::SliderInt("##sl", &value, static_cast<int>(ValueType::kMin), static_cast<int>(ValueType::kMax), ""))
        hookContext.config().template setVariable<ConfigVariable>(ValueType{static_cast<Underlying>(value)});
    ImGui::PopID();
}

inline void drawSideButton(const ImVec2& pos, const char* label, bool pressed, int page) noexcept
{
    ImGui::SetCursorPos(pos);
    if (pressed)
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_CheckMark));
    if (ImGui::Button(label, kSideBtnSize)) {
        g_menuPage = page;
        g_btn1 = page == 0;
        g_btn2 = page == 1;
        g_btn3 = page == 2;
        g_btn4 = page == 3;
    }
    if (pressed)
        ImGui::PopStyleColor();

    const ImVec2 win = ImGui::GetWindowPos();
    ImGui::GetWindowDrawList()->AddRect(
        ImVec2{pos.x + win.x, pos.y + win.y},
        ImVec2{pos.x + kSideBtnSize.x + win.x, pos.y + kSideBtnSize.y + win.y},
        ImGui::GetColorU32(ImGuiCol_Border),
        0.0f,
        0,
        1.0f);
}

inline void drawEspPreview(HookContext<GlobalContext>& hookContext) noexcept
{
    (void)hookContext;
    const ImVec2 origin = ImGui::GetCursorScreenPos();
    const ImVec2 size{220.0f, 260.0f};
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(origin, ImVec2{origin.x + size.x, origin.y + size.y}, IM_COL32(18, 20, 26, 255));
    dl->AddRect(origin, ImVec2{origin.x + size.x, origin.y + size.y}, ImGui::GetColorU32(ImGuiCol_Border));

    const ImVec2 boxMin{origin.x + 70.0f, origin.y + 40.0f};
    const ImVec2 boxMax{origin.x + 150.0f, origin.y + 200.0f};
    const bool boxOn = GET_CONFIG_VAR(player_esp_vars::Enabled) && GET_CONFIG_VAR(player_esp_vars::ShowBox);
    const bool skelOn = GET_CONFIG_VAR(player_esp_vars::Enabled) && GET_CONFIG_VAR(player_esp_vars::ShowSkeleton);
    const bool hpOn = GET_CONFIG_VAR(player_esp_vars::Enabled) && GET_CONFIG_VAR(player_esp_vars::ShowHealthBar);

    if (boxOn)
        dl->AddRect(boxMin, boxMax, IM_COL32(90, 140, 255, 220), 0.0f, 0, 1.5f);

    const ImU32 skel = skelOn ? IM_COL32(230, 230, 235, 255) : IM_COL32(100, 100, 110, 120);
    dl->AddCircle(ImVec2{(boxMin.x + boxMax.x) * 0.5f, boxMin.y + 18.0f}, 8.0f, skel, 0, 1.5f);
    dl->AddLine(ImVec2{(boxMin.x + boxMax.x) * 0.5f, boxMin.y + 28.0f}, ImVec2{(boxMin.x + boxMax.x) * 0.5f, boxMin.y + 90.0f}, skel, 2.0f);
    dl->AddLine(ImVec2{(boxMin.x + boxMax.x) * 0.5f, boxMin.y + 45.0f}, ImVec2{boxMin.x + 20.0f, boxMin.y + 70.0f}, skel, 2.0f);
    dl->AddLine(ImVec2{(boxMin.x + boxMax.x) * 0.5f, boxMin.y + 45.0f}, ImVec2{boxMax.x - 20.0f, boxMin.y + 70.0f}, skel, 2.0f);
    dl->AddLine(ImVec2{(boxMin.x + boxMax.x) * 0.5f, boxMin.y + 90.0f}, ImVec2{boxMin.x + 22.0f, boxMin.y + 140.0f}, skel, 2.0f);
    dl->AddLine(ImVec2{(boxMin.x + boxMax.x) * 0.5f, boxMin.y + 90.0f}, ImVec2{boxMax.x - 22.0f, boxMin.y + 140.0f}, skel, 2.0f);

    if (hpOn)
        dl->AddRectFilled(ImVec2{boxMin.x - 8.0f, boxMin.y}, ImVec2{boxMin.x - 3.0f, boxMax.y}, IM_COL32(70, 200, 90, 255));

    ImGui::Dummy(size);
}

inline void drawAimPage(HookContext<GlobalContext>& hookContext) noexcept
{
    ImGui::Columns(2, nullptr, false);
    ImGui::SetCursorPos(ImVec2{15.0f, 24.0f});
    gradientText("Aimbot");

    putConfigSwitch<aimbot_vars::Enabled>(hookContext, "Enable");
    if (GET_CONFIG_VAR(aimbot_vars::Enabled)) {
        putConfigSwitch<aimbot_vars::RequireHoldKey>(hookContext, "Hold LMB");
        putConfigSwitch<aimbot_vars::DrawFov>(hookContext, "Draw FOV");
        putConfigSwitch<aimbot_vars::VisibilityCheck>(hookContext, "Visible Check");
        putConfigSwitch<aimbot_vars::OnlyWhenScoped>(hookContext, "Scope Only");
        putConfigSwitch<aimbot_vars::DisableWhenFlashed>(hookContext, "Ignore Flash");
        putConfigSwitch<aimbot_vars::TeamCheck>(hookContext, "Team Check");
        putConfigSwitch<aimbot_vars::RecoilControl>(hookContext, "Recoil Control");
        putConfigSliderU8<aimbot_vars::Fov>(hookContext, "FOV");
        putConfigSliderU8<aimbot_vars::Smoothness>(hookContext, "Smooth");
        putConfigSliderU8<aimbot_vars::Hitbox>(hookContext, "Hitbox");
        ImGui::SetCursorPosX(25.0f);
        ImGui::TextDisabled("0 Head  1 Neck  2 Chest  3 Pelvis  4 Nearest");
    }

    ImGui::NextColumn();
    ImGui::SetCursorPosY(24.0f);
    gradientText("RCS");
    putConfigSwitch<no_recoil_vars::Enabled>(hookContext, "No Recoil", 5.0f);
    if (GET_CONFIG_VAR(no_recoil_vars::Enabled))
        putConfigSliderU8<no_recoil_vars::Strength>(hookContext, "Strength", 5.0f);
    putConfigSwitch<no_spread_vars::Enabled>(hookContext, "No Spread", 5.0f);
    putConfigSwitch<no_scope_inaccuracy_vis_vars::Enabled>(hookContext, "No-Scope Vis", 5.0f);

    ImGui::NewLine();
    gradientText("Triggerbot");
    putConfigSwitch<triggerbot_vars::Enabled>(hookContext, "Enable", 5.0f);
    if (GET_CONFIG_VAR(triggerbot_vars::Enabled)) {
        putConfigSwitch<triggerbot_vars::TeamCheck>(hookContext, "Team Check", 5.0f);
        putConfigSwitch<triggerbot_vars::OnlyWhenScoped>(hookContext, "Scope Only", 5.0f);
        putConfigSwitch<triggerbot_vars::DisableWhenFlashed>(hookContext, "Ignore Flash", 5.0f);
        putConfigSliderU8<triggerbot_vars::DelayMs>(hookContext, "Delay ms", 5.0f);
    }
    ImGui::Columns(1);
}

inline void drawVisualPage(HookContext<GlobalContext>& hookContext) noexcept
{
    ImGui::Columns(2, nullptr, false);
    ImGui::SetCursorPos(ImVec2{15.0f, 24.0f});
    gradientText("ESP");

    putConfigSwitch<player_esp_vars::Enabled>(hookContext, "Enable");
    if (GET_CONFIG_VAR(player_esp_vars::Enabled)) {
        putConfigSwitch<player_esp_vars::ShowBox>(hookContext, "Box");
        if (GET_CONFIG_VAR(player_esp_vars::ShowBox))
            putConfigSliderU8<player_esp_vars::BoxType>(hookContext, "Box Type");
        putConfigSwitch<player_esp_vars::ShowSkeleton>(hookContext, "Skeleton");
        putConfigSwitch<player_esp_vars::ShowHeadDot>(hookContext, "Head Box");
        putConfigSwitch<player_esp_vars::ShowHealthBar>(hookContext, "Health Bar");
        putConfigSwitch<player_esp_vars::OnlyEnemies>(hookContext, "Only Enemies");
        putConfigSwitch<player_esp_vars::VisibilityCheck>(hookContext, "Visible Check");
    }

    ImGui::NewLine();
    gradientText("Glow");
    putConfigSwitch<model_glow_vars::Enabled>(hookContext, "Model Glow");
    putConfigSwitch<model_glow_vars::GlowPlayers>(hookContext, "Players");
    putConfigSwitch<model_glow_vars::GlowOnlyEnemies>(hookContext, "Only Enemies");
    putConfigSwitch<model_glow_vars::GlowWeapons>(hookContext, "Weapons");
    putConfigSwitch<outline_glow_vars::Enabled>(hookContext, "Outline Glow");
    putConfigSwitch<outline_glow_vars::GlowPlayers>(hookContext, "Outline Players");

    ImGui::NewLine();
    gradientText("Player Info");
    putConfigSwitch<player_info_vars::Enabled>(hookContext, "Enable Info");
    putConfigSwitch<player_info_vars::PlayerNameEnabled>(hookContext, "Name");
    putConfigSwitch<player_info_vars::PlayerHealthEnabled>(hookContext, "Health");
    putConfigSwitch<player_info_vars::ActiveWeaponIconEnabled>(hookContext, "Weapon");
    putConfigSwitch<grenade_prediction_vars::Enabled>(hookContext, "Grenade Pred");
    putConfigSwitch<viewmodel_mod_vars::Enabled>(hookContext, "Viewmodel Mod");

    ImGui::NextColumn();
    ImGui::SetCursorPosY(24.0f);
    gradientText("ESP Preview");
    drawEspPreview(hookContext);
    ImGui::Columns(1);
}

inline void drawMiscPage(HookContext<GlobalContext>& hookContext) noexcept
{
    ImGui::Columns(2, nullptr, false);
    ImGui::SetCursorPos(ImVec2{15.0f, 24.0f});
    gradientText("Misc");

    putConfigSwitch<BombTimerEnabled>(hookContext, "Bomb Timer");
    putConfigSwitch<DefusingAlertEnabled>(hookContext, "Defusing Alert");
    putConfigSwitch<KillfeedPreserverEnabled>(hookContext, "Killfeed Preserver");
    putConfigSwitch<PostRoundTimerEnabled>(hookContext, "Post-Round Timer");
    putConfigSwitch<BombPlantAlertEnabled>(hookContext, "Bomb Plant Alert");
    putConfigSwitch<bunnyhop_vars::Enabled>(hookContext, "Bunny Hop");
    putConfigSwitch<autostrafe_vars::Enabled>(hookContext, "Auto Strafe");
    putConfigSwitch<thirdperson_vars::Enabled>(hookContext, "Third Person");
    if (GET_CONFIG_VAR(thirdperson_vars::Enabled))
        putConfigSliderU8<thirdperson_vars::Distance>(hookContext, "Distance");
    putConfigSwitch<no_scope_inaccuracy_vis_vars::Enabled>(hookContext, "Sniper Crosshair");

    ImGui::NextColumn();
    ImGui::SetCursorPosY(24.0f);
    gradientText("Global Settings");
    ImGui::SetCursorPosX(15.0f);
    ImGui::TextDisabled("Menu Key");
    ImGui::SameLine();
    alignRight(140.0f);
    ImGui::Button("INSERT / PAUSE", ImVec2{140.0f, 25.0f});

    ImGui::NewLine();
    gradientText("Sound ESP");
    putConfigSwitch<FootstepSoundVisualizationEnabled>(hookContext, "Footsteps", 5.0f);
    putConfigSwitch<BombPlantSoundVisualizationEnabled>(hookContext, "Bomb Plant", 5.0f);
    putConfigSwitch<BombBeepSoundVisualizationEnabled>(hookContext, "Bomb Beep", 5.0f);
    putConfigSwitch<BombDefuseSoundVisualizationEnabled>(hookContext, "Bomb Defuse", 5.0f);
    putConfigSwitch<WeaponScopeSoundVisualizationEnabled>(hookContext, "Scope", 5.0f);
    putConfigSwitch<WeaponReloadSoundVisualizationEnabled>(hookContext, "Reload", 5.0f);

    ImGui::NewLine();
    ImGui::SetCursorPosX(15.0f);
    if (ImGui::Button("Unhook", ImVec2{125.0f, 25.0f}))
        dx11MenuRequestUnload();

    ImGui::Columns(1);
}

inline void drawConfigPage(HookContext<GlobalContext>& hookContext) noexcept
{
    static char newConfigName[48]{};
    static int selectedIndex = 0;
    static bool listNeedsRefresh = true;

    auto config = hookContext.config();

    if (listNeedsRefresh) {
        config.refreshConfigList();
        listNeedsRefresh = false;
        selectedIndex = 0;
        for (std::size_t i = 0; i < config.configCount(); ++i) {
            if (std::strcmp(config.configNameAt(i), config.currentConfigName()) == 0) {
                selectedIndex = static_cast<int>(i);
                break;
            }
        }
    }

    ImGui::SetCursorPos(ImVec2{15.0f, 24.0f});
    gradientText("Config Manager");

    ImGui::SetCursorPosX(25.0f);
    ImGui::Text("Active: %s", config.currentConfigName());
    ImGui::SetCursorPosX(25.0f);
    ImGui::TextDisabled("%s", config.statusMessage());
    ImGui::Spacing();

    ImGui::SetCursorPosX(25.0f);
    ImGui::BeginChild("ConfigList", ImVec2{320.0f, 210.0f}, true);
    {
        const int count = static_cast<int>(config.configCount());
        for (int i = 0; i < count; ++i) {
            const bool isActive = std::strcmp(config.configNameAt(static_cast<std::size_t>(i)), config.currentConfigName()) == 0;
            if (ImGui::Selectable(config.configNameAt(static_cast<std::size_t>(i)), selectedIndex == i))
                selectedIndex = i;
            if (isActive) {
                ImGui::SameLine();
                ImGui::TextDisabled("(active)");
            }
        }
        if (count == 0)
            ImGui::TextDisabled("No configs yet");
    }
    ImGui::EndChild();

    ImGui::Spacing();
    ImGui::SetCursorPosX(25.0f);
    if (ImGui::Button("Refresh", ImVec2{90.0f, 26.0f}))
        listNeedsRefresh = true;

    ImGui::SameLine();
    if (ImGui::Button("Load", ImVec2{90.0f, 26.0f})) {
        if (selectedIndex >= 0 && static_cast<std::size_t>(selectedIndex) < config.configCount())
            config.loadConfigByName(config.configNameAt(static_cast<std::size_t>(selectedIndex)));
    }

    ImGui::SameLine();
    if (ImGui::Button("Save", ImVec2{90.0f, 26.0f}))
        config.saveActiveConfig();

    ImGui::SetCursorPosX(25.0f);
    if (ImGui::Button("Delete", ImVec2{90.0f, 26.0f})) {
        if (selectedIndex >= 0 && static_cast<std::size_t>(selectedIndex) < config.configCount()) {
            config.deleteConfigByName(config.configNameAt(static_cast<std::size_t>(selectedIndex)));
            listNeedsRefresh = true;
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Defaults", ImVec2{90.0f, 26.0f}))
        config.restoreDefaults();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::SetCursorPosX(25.0f);
    ImGui::TextUnformatted("New config");
    ImGui::SetCursorPosX(25.0f);
    ImGui::SetNextItemWidth(220.0f);
    ImGui::InputTextWithHint("##newcfg", "name (e.g. rage)", newConfigName, sizeof(newConfigName));
    ImGui::SameLine();
    if (ImGui::Button("Create", ImVec2{90.0f, 0.0f})) {
        if (config.createAndSaveConfig(newConfigName)) {
            newConfigName[0] = '\0';
            listNeedsRefresh = true;
        }
    }

    ImGui::Spacing();
    ImGui::SetCursorPosX(25.0f);
    ImGui::TextWrapped("Configs live in %%appdata%%\\AimSyncCS2\\configs\\. Autosave still updates the active file.");
}

inline void draw(HookContext<GlobalContext>& hookContext) noexcept
{
    ImGui::SetNextWindowPos(
        ImVec2{(ImGui::GetIO().DisplaySize.x - kMainWinSize.x) * 0.5f,
               (ImGui::GetIO().DisplaySize.y - kMainWinSize.y) * 0.5f},
        ImGuiCond_Once);
    ImGui::SetNextWindowSize(kMainWinSize);

    constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar;

    if (!ImGui::Begin("AimSync", nullptr, flags)) {
        ImGui::End();
        return;
    }

    // Sidebar navigation (text buttons; icon textures optional later)
    ImGui::SetCursorPos(ImVec2{8.0f, 18.0f});
    ImGui::TextUnformatted("AS");
    drawSideButton(kButton1Pos, "Aim", g_btn1, 0);
    drawSideButton(kButton2Pos, "Vis", g_btn2, 1);
    drawSideButton(kButton3Pos, "Misc", g_btn3, 2);
    drawSideButton(kButton4Pos, "Cfg", g_btn4, 3);

    ImGui::SetCursorPos(kChildPos);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{8.0f, 8.0f});
    ImGui::BeginChild("Page", kChildSize, false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
    {
        ImGui::Text("   AimSync");
        ImGui::Separator();

        switch (g_menuPage) {
        case 0: drawAimPage(hookContext); break;
        case 1: drawVisualPage(hookContext); break;
        case 2: drawMiscPage(hookContext); break;
        default: drawConfigPage(hookContext); break;
        }
        ImGui::NewLine();
    }
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::End();
}

}
