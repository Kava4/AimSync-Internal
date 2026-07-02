#pragma once

#include <Features/Combat/SniperRifles/NoScopeInaccuracyVis/NoScopeInaccuracyVisConfigVariables.h>
#include <Features/Combat/Aimbot/AimbotConfigVariables.h>
#include <Features/Combat/Triggerbot/TriggerbotConfigVariables.h>
#include <GameClient/Panorama/PanoramaDropDown.h>
#include <EntryPoints/GuiEntryPoints.h>
#include <HookContext/HookContextMacros.h>
#include <Platform/Macros/FunctionAttributes.h>
#include "OnOffDropdownSelectionChangeHandler.h"
#include "Tabs/VisualsTab/IntSlider.h"

template <typename HookContext>
class CombatTab {
public:
    explicit CombatTab(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void init(auto&& guiPanel) const
    {
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, no_scope_inaccuracy_vis_vars::Enabled>>(guiPanel, "no_scope_inacc_vis");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, aimbot_vars::Enabled>>(guiPanel, "aimbot_enable");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, aimbot_vars::TeamCheck>>(guiPanel, "aimbot_team_check");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, aimbot_vars::VisibilityCheck>>(guiPanel, "aimbot_vis_check");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, aimbot_vars::RecoilControl>>(guiPanel, "aimbot_rcs");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, triggerbot_vars::Enabled>>(guiPanel, "triggerbot_enable");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, triggerbot_vars::TeamCheck>>(guiPanel, "triggerbot_team_check");
    }

    void updateFromConfig(auto&& mainMenu) const noexcept
    {
        setDropDownSelectedIndex(mainMenu, "no_scope_inacc_vis", !GET_CONFIG_VAR(no_scope_inaccuracy_vis_vars::Enabled));
        setDropDownSelectedIndex(mainMenu, "aimbot_enable", !GET_CONFIG_VAR(aimbot_vars::Enabled));
        setDropDownSelectedIndex(mainMenu, "aimbot_team_check", !GET_CONFIG_VAR(aimbot_vars::TeamCheck));
        setDropDownSelectedIndex(mainMenu, "aimbot_vis_check", !GET_CONFIG_VAR(aimbot_vars::VisibilityCheck));
        setDropDownSelectedIndex(mainMenu, "aimbot_rcs", !GET_CONFIG_VAR(aimbot_vars::RecoilControl));
        setDropDownSelectedIndex(mainMenu, "triggerbot_enable", !GET_CONFIG_VAR(triggerbot_vars::Enabled));
        setDropDownSelectedIndex(mainMenu, "triggerbot_team_check", !GET_CONFIG_VAR(triggerbot_vars::TeamCheck));
        updateSlider<aimbot_vars::Fov>(mainMenu, "aimbot_fov");
        updateSlider<aimbot_vars::Smoothness>(mainMenu, "aimbot_smooth");
        updateSlider<triggerbot_vars::DelayMs>(mainMenu, "triggerbot_delay");
    }

private:
    template <typename Handler>
    void initDropDown(auto&& guiPanel, const char* panelId) const
    {
        auto&& dropDown = guiPanel.findChildInLayoutFile(panelId).clientPanel().template as<PanoramaDropDown>();
        dropDown.registerSelectionChangedHandler(&GuiEntryPoints<HookContext>::template dropDownSelectionChanged<Handler>);
    }

    [[NOINLINE]] void setDropDownSelectedIndex(auto&& mainMenu, const char* dropDownId, int selectedIndex) const noexcept
    {
        mainMenu.findChildInLayoutFile(dropDownId).clientPanel().template as<PanoramaDropDown>().setSelectedIndex(selectedIndex);
    }

    template <typename ConfigVariable>
    void updateSlider(auto&& mainMenu, const char* sliderId) const noexcept
    {
        auto&& slider = hookContext.template make<IntSlider>(mainMenu.findChildInLayoutFile(sliderId));
        const auto value = static_cast<std::uint8_t>(GET_CONFIG_VAR(ConfigVariable));
        slider.updateSlider(value);
        slider.updateTextEntry(value);
    }

    HookContext& hookContext;
};
