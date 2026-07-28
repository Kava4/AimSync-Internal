#pragma once

#include <Features/Combat/SniperRifles/NoScopeInaccuracyVis/NoScopeInaccuracyVisConfigVariables.h>
#include <Features/Combat/Aimbot/AimbotConfigVariables.h>
#include <Features/Combat/NoRecoil/NoRecoilConfigVariables.h>
#include <Features/Combat/NoSpread/NoSpreadConfigVariables.h>
#include <Features/Combat/Triggerbot/TriggerbotConfigVariables.h>
#include <Features/Inventory/InventoryChanger/InventoryChangerConfigVariables.h>
#include <GameClient/Panorama/PanoramaDropDown.h>
#include <GameClient/Panorama/TextEntry.h>
#include <EntryPoints/GuiEntryPoints.h>
#include <HookContext/HookContextMacros.h>
#include <Platform/Macros/FunctionAttributes.h>
#include <Utils/StringBuilder.h>
#include "IndexDropdownSelectionChangeHandler.h"
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
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, aimbot_vars::OnlyWhenScoped>>(guiPanel, "aimbot_scoped_only");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, aimbot_vars::DisableWhenFlashed>>(guiPanel, "aimbot_no_flash");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, no_recoil_vars::Enabled>>(guiPanel, "no_recoil_enable");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, no_spread_vars::Enabled>>(guiPanel, "no_spread_enable");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, triggerbot_vars::Enabled>>(guiPanel, "triggerbot_enable");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, triggerbot_vars::TeamCheck>>(guiPanel, "triggerbot_team_check");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, triggerbot_vars::OnlyWhenScoped>>(guiPanel, "triggerbot_scoped_only");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, triggerbot_vars::DisableWhenFlashed>>(guiPanel, "triggerbot_no_flash");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, inventory_changer_vars::Enabled>>(guiPanel, "inventory_enable");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, inventory_changer_vars::StatTrak>>(guiPanel, "inventory_stattrak");
        initDropDown<IndexDropdownSelectionChangeHandler<HookContext, inventory_changer_vars::AkSkin>>(guiPanel, "inventory_ak_skin");
        initDropDown<IndexDropdownSelectionChangeHandler<HookContext, inventory_changer_vars::M4Skin>>(guiPanel, "inventory_m4_skin");
        initDropDown<IndexDropdownSelectionChangeHandler<HookContext, inventory_changer_vars::AwpSkin>>(guiPanel, "inventory_awp_skin");
        initDropDown<IndexDropdownSelectionChangeHandler<HookContext, inventory_changer_vars::DeagleSkin>>(guiPanel, "inventory_deagle_skin");
        initDropDown<IndexDropdownSelectionChangeHandler<HookContext, inventory_changer_vars::KnifeType>>(guiPanel, "inventory_knife_type");
        initDropDown<IndexDropdownSelectionChangeHandler<HookContext, inventory_changer_vars::KnifeSkin>>(guiPanel, "inventory_knife_skin");
    }

    void updateFromConfig(auto&& mainMenu) const noexcept
    {
        setDropDownSelectedIndex(mainMenu, "no_scope_inacc_vis", !GET_CONFIG_VAR(no_scope_inaccuracy_vis_vars::Enabled));
        setDropDownSelectedIndex(mainMenu, "aimbot_enable", !GET_CONFIG_VAR(aimbot_vars::Enabled));
        setDropDownSelectedIndex(mainMenu, "aimbot_team_check", !GET_CONFIG_VAR(aimbot_vars::TeamCheck));
        setDropDownSelectedIndex(mainMenu, "aimbot_vis_check", !GET_CONFIG_VAR(aimbot_vars::VisibilityCheck));
        setDropDownSelectedIndex(mainMenu, "aimbot_rcs", !GET_CONFIG_VAR(aimbot_vars::RecoilControl));
        setDropDownSelectedIndex(mainMenu, "aimbot_scoped_only", !GET_CONFIG_VAR(aimbot_vars::OnlyWhenScoped));
        setDropDownSelectedIndex(mainMenu, "aimbot_no_flash", !GET_CONFIG_VAR(aimbot_vars::DisableWhenFlashed));
        setDropDownSelectedIndex(mainMenu, "no_recoil_enable", !GET_CONFIG_VAR(no_recoil_vars::Enabled));
        setDropDownSelectedIndex(mainMenu, "no_spread_enable", !GET_CONFIG_VAR(no_spread_vars::Enabled));
        setDropDownSelectedIndex(mainMenu, "triggerbot_enable", !GET_CONFIG_VAR(triggerbot_vars::Enabled));
        setDropDownSelectedIndex(mainMenu, "triggerbot_team_check", !GET_CONFIG_VAR(triggerbot_vars::TeamCheck));
        setDropDownSelectedIndex(mainMenu, "triggerbot_scoped_only", !GET_CONFIG_VAR(triggerbot_vars::OnlyWhenScoped));
        setDropDownSelectedIndex(mainMenu, "triggerbot_no_flash", !GET_CONFIG_VAR(triggerbot_vars::DisableWhenFlashed));
        setDropDownSelectedIndex(mainMenu, "inventory_enable", !GET_CONFIG_VAR(inventory_changer_vars::Enabled));
        setDropDownSelectedIndex(mainMenu, "inventory_stattrak", !GET_CONFIG_VAR(inventory_changer_vars::StatTrak));
        setDropDownSelectedIndex(mainMenu, "inventory_ak_skin", static_cast<int>(GET_CONFIG_VAR(inventory_changer_vars::AkSkin)));
        setDropDownSelectedIndex(mainMenu, "inventory_m4_skin", static_cast<int>(GET_CONFIG_VAR(inventory_changer_vars::M4Skin)));
        setDropDownSelectedIndex(mainMenu, "inventory_awp_skin", static_cast<int>(GET_CONFIG_VAR(inventory_changer_vars::AwpSkin)));
        setDropDownSelectedIndex(mainMenu, "inventory_deagle_skin", static_cast<int>(GET_CONFIG_VAR(inventory_changer_vars::DeagleSkin)));
        setDropDownSelectedIndex(mainMenu, "inventory_knife_type", static_cast<int>(GET_CONFIG_VAR(inventory_changer_vars::KnifeType)));
        setDropDownSelectedIndex(mainMenu, "inventory_knife_skin", static_cast<int>(GET_CONFIG_VAR(inventory_changer_vars::KnifeSkin)));
        updateSlider<aimbot_vars::Fov>(mainMenu, "aimbot_fov");
        updateSlider<aimbot_vars::Smoothness>(mainMenu, "aimbot_smooth");
        updateSlider<no_recoil_vars::Strength>(mainMenu, "no_recoil_strength");
        updateSlider<triggerbot_vars::DelayMs>(mainMenu, "triggerbot_delay");
        updateUint16TextEntry<inventory_changer_vars::Wear>(mainMenu, "skin_wear");
        updateUint16TextEntry<inventory_changer_vars::Seed>(mainMenu, "skin_seed");
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

    template <typename ConfigVariable>
    void updateUint16TextEntry(auto&& mainMenu, const char* textEntryId) const noexcept
    {
        auto&& textEntry = mainMenu.findChildInLayoutFile(textEntryId).clientPanel().template as<TextEntry>();
        textEntry.setText(StringBuilderStorage<100>{}.builder().put(static_cast<std::uint16_t>(GET_CONFIG_VAR(ConfigVariable))).cstring());
    }

    HookContext& hookContext;
};
