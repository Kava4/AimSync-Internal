#pragma once

#include <Features/Combat/SniperRifles/NoScopeInaccuracyVis/NoScopeInaccuracyVisConfigVariables.h>
#include <Features/Combat/Aimbot/AimbotConfigVariables.h>
#include <Features/Combat/Triggerbot/TriggerbotConfigVariables.h>
#include <Features/Inventory/SkinChanger/SkinChangerConfigVariables.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorld.h>
#include <GameClient/Panorama/Slider.h>
#include <GameClient/Panorama/TextEntry.h>
#include <HookContext/HookContextMacros.h>
#include <Platform/Macros/FunctionAttributes.h>
#include <Utils/StringBuilder.h>
#include <Utils/StringParser.h>
#include "Tabs/VisualsTab/HueSlider.h"
#include "Tabs/VisualsTab/IntSlider.h"

template <typename HookContext>
struct SetCommandHandler {
    SetCommandHandler(StringParser& parser, HookContext& hookContext) noexcept
        : parser{parser}
        , hookContext{hookContext}
    {
    }

    void operator()() noexcept
    {
        if (const auto section = parser.getLine('/'); section == "combat") {
            handleCombatSection();
        } else if (section == "hud") {
            handleHudSection();
        } else if (section == "visuals") {
            handleVisualsSection();
        } else if (section == "sound") {
            handleSoundSection();
        }
    }

private:
    void handleCombatSection() noexcept
    {
        if (const auto feature = parser.getLine('/'); feature == "aimbot_fov") {
            handleIntSlider<aimbot_vars::Fov>("aimbot_fov");
        } else if (feature == "aimbot_fov_text") {
            handleIntSliderTextEntry<aimbot_vars::Fov>("aimbot_fov");
        } else if (feature == "aimbot_smooth") {
            handleIntSlider<aimbot_vars::Smoothness>("aimbot_smooth");
        } else if (feature == "aimbot_smooth_text") {
            handleIntSliderTextEntry<aimbot_vars::Smoothness>("aimbot_smooth");
        } else if (feature == "triggerbot_delay") {
            handleIntSlider<triggerbot_vars::DelayMs>("triggerbot_delay");
        } else if (feature == "triggerbot_delay_text") {
            handleIntSliderTextEntry<triggerbot_vars::DelayMs>("triggerbot_delay");
        } else if (feature == "skin_paint_ak47_text") {
            handleUint16TextEntry<skin_changer_vars::PaintKitAK47>("skin_paint_ak47");
        } else if (feature == "skin_paint_m4a1s_text") {
            handleUint16TextEntry<skin_changer_vars::PaintKitM4A1S>("skin_paint_m4a1s");
        } else if (feature == "skin_paint_awp_text") {
            handleUint16TextEntry<skin_changer_vars::PaintKitAWP>("skin_paint_awp");
        } else if (feature == "skin_paint_deagle_text") {
            handleUint16TextEntry<skin_changer_vars::PaintKitDeagle>("skin_paint_deagle");
        } else if (feature == "skin_paint_knife_text") {
            handleUint16TextEntry<skin_changer_vars::PaintKitKnife>("skin_paint_knife");
        } else if (feature == "skin_wear_text") {
            handleUint16TextEntry<skin_changer_vars::Wear>("skin_wear");
        }
    }

    void handleHudSection() const noexcept
    {
    }

    void handleSoundSection() const noexcept
    {
    }

    void handleVisualsSection() const noexcept
    {
        if (const auto feature = parser.getLine('/'); feature == "viewmodel_fov") {
            handleIntSlider<viewmodel_mod_vars::Fov>("viewmodel_fov");
        } else if (feature == "viewmodel_fov_text") {
            handleIntSliderTextEntry<viewmodel_mod_vars::Fov>("viewmodel_fov");
        }
    }

    template <typename ConfigVariable>
    void handleUint16TextEntry(const char* textEntryId) noexcept
    {
        const auto current = static_cast<std::uint16_t>(GET_CONFIG_VAR(ConfigVariable));
        std::uint16_t value{};
        if (!parser.parseInt(value) || value < ConfigVariable::ValueType::kMin || value > ConfigVariable::ValueType::kMax) {
            updateTextEntry(textEntryId, current);
            return;
        }

        if (value == current)
            return;

        hookContext.config().template setVariable<ConfigVariable>(typename ConfigVariable::ValueType{value});
        hookContext.featuresStates().skinChangerState.forceFullUpdate = true;
        updateTextEntry(textEntryId, value);
    }

    template <typename ConfigVariable>
    void handleIntSlider(const char* sliderId) const noexcept
    {
        const auto newVariableValue = handleIntSlider(sliderId, ConfigVariable::ValueType::kMin, ConfigVariable::ValueType::kMax, GET_CONFIG_VAR(ConfigVariable));
        hookContext.config().template setVariable<ConfigVariable>(typename ConfigVariable::ValueType{newVariableValue});
    }

    [[nodiscard]] std::uint8_t handleIntSlider(const char* sliderId, std::uint8_t min, std::uint8_t max, std::uint8_t current) const noexcept
    {
        std::uint8_t value{};
        if (!parser.parseInt(value) || value == current || value < min || value > max)
            return current;

        auto&& hueSlider = getIntSlider(sliderId);
        hueSlider.updateTextEntry(value);
        return value;
    }

    template <typename ConfigVariable>
    void handleIntSliderTextEntry(const char* sliderId) const noexcept
    {
        const auto newVariableValue = handleIntSliderTextEntry(sliderId, ConfigVariable::ValueType::kMin, ConfigVariable::ValueType::kMax, GET_CONFIG_VAR(ConfigVariable));
        hookContext.config().template setVariable<ConfigVariable>(typename ConfigVariable::ValueType{newVariableValue});
    }

    [[nodiscard]] std::uint8_t handleIntSliderTextEntry(const char* sliderId, std::uint8_t min, std::uint8_t max, std::uint8_t current) const noexcept
    {
        auto&& slider = getIntSlider(sliderId);
        std::uint8_t value{};
        if (!parser.parseInt(value) || value < min || value > max) {
            slider.updateTextEntry(current);
            return current;
        }

        if (value == current)
            return current;

        slider.updateSlider(value);
        return value;
    }

    void updateTextEntry(const char* textEntryId, std::uint16_t value) const noexcept
    {
        const auto mainMenuPointer = hookContext.patternSearchResults().template get<MainMenuPanelPointer>();
        auto&& mainMenu = hookContext.template make<ClientPanel>(mainMenuPointer ? *mainMenuPointer : nullptr).uiPanel();
        auto&& textEntry = mainMenu.findChildInLayoutFile(textEntryId).clientPanel().template as<TextEntry>();
        textEntry.setText(StringBuilderStorage<100>{}.builder().put(value).cstring());
    }

    [[nodiscard]] decltype(auto) getIntSlider(const char* sliderId) const noexcept
    {
        const auto mainMenuPointer = hookContext.patternSearchResults().template get<MainMenuPanelPointer>();
        auto&& mainMenu = hookContext.template make<ClientPanel>(mainMenuPointer ? *mainMenuPointer : nullptr).uiPanel();
        return hookContext.template make<IntSlider>(mainMenu.findChildInLayoutFile(sliderId));
    }

    StringParser& parser;
    HookContext& hookContext;
};
