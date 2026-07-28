#pragma once

#include <Features/Combat/SniperRifles/NoScopeInaccuracyVis/NoScopeInaccuracyVisConfigVariables.h>
#include <Features/Combat/Aimbot/AimbotConfigVariables.h>
#include <Features/Combat/NoRecoil/NoRecoilConfigVariables.h>
#include <Features/Combat/Triggerbot/TriggerbotConfigVariables.h>
#include <Features/Inventory/InventoryChanger/InventoryChangerConfigVariables.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorld.h>
#include <Features/Visuals/GrenadePrediction/GrenadePredictionConfigVariables.h>
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
        } else if (feature == "no_recoil_strength") {
            handleIntSlider<no_recoil_vars::Strength>("no_recoil_strength");
        } else if (feature == "no_recoil_strength_text") {
            handleIntSliderTextEntry<no_recoil_vars::Strength>("no_recoil_strength");
        } else if (feature == "triggerbot_delay") {
            handleIntSlider<triggerbot_vars::DelayMs>("triggerbot_delay");
        } else if (feature == "triggerbot_delay_text") {
            handleIntSliderTextEntry<triggerbot_vars::DelayMs>("triggerbot_delay");
        } else if (feature == "skin_wear_text") {
            handleUint16TextEntry<inventory_changer_vars::Wear>("skin_wear");
        } else if (feature == "skin_seed_text") {
            handleUint16TextEntry<inventory_changer_vars::Seed>("skin_seed");
        } else if (feature == "skin_apply") {
            hookContext.config().template setVariable<inventory_changer_vars::ApplyRequested>(true);
            hookContext.featuresStates().inventoryChangerState.seeded = false;
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
        } else if (feature == "grenade_prediction_bounce_friction") {
            handleIntSlider<grenade_prediction_vars::BounceFriction>("grenade_prediction_bounce_friction");
        } else if (feature == "grenade_prediction_bounce_friction_text") {
            handleIntSliderTextEntry<grenade_prediction_vars::BounceFriction>("grenade_prediction_bounce_friction");
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
