#pragma once

#include <cstdint>

#include <Config/RangeConstrainedVariableParams.h>

namespace inventory_changer_params
{

struct SkinPreset {
    const char* name;
    std::uint16_t paintKit;
};

struct KnifePreset {
    const char* name;
    std::uint16_t defIndex;
};

inline constexpr SkinPreset kAkSkins[] = {
    {"Off", 0},
    {"Redline", 282},
    {"Vulcan", 302},
    {"Aquamarine Revenge", 474},
    {"Fuel Injector", 524},
    {"Bloodsport", 639},
    {"Neon Revolution", 600},
    {"The Empress", 675},
    {"Legion of Anubis", 1004},
    {"Nightwish", 1141},
};

inline constexpr SkinPreset kM4Skins[] = {
    {"Off", 0},
    {"Hyper Beast", 430},
    {"Mecha Industries", 587},
    {"Player Two", 946},
    {"Printstream", 984},
    {"Nightmare", 1001},
};

inline constexpr SkinPreset kAwpSkins[] = {
    {"Off", 0},
    {"Asiimov", 279},
    {"Hyper Beast", 475},
    {"Containment Breach", 708},
    {"Neo-Noir", 803},
    {"Chrome Cannon", 1144},
    {"Dragon Lore", 344},
};

inline constexpr SkinPreset kDeagleSkins[] = {
    {"Off", 0},
    {"Blaze", 37},
    {"Conspiracy", 351},
    {"Kumicho Dragon", 527},
    {"Printstream", 1058},
    {"Ocean Drive", 1003},
};

inline constexpr SkinPreset kKnifeSkins[] = {
    {"Off", 0},
    {"Doppler", 418},
    {"Marble Fade", 413},
    {"Tiger Tooth", 409},
    {"Fade", 38},
    {"Slaughter", 59},
    {"Crimson Web", 12},
};

inline constexpr KnifePreset kKnives[] = {
    {"Karambit", 507},
    {"M9 Bayonet", 508},
    {"Bayonet", 500},
    {"Butterfly", 515},
    {"Talon", 523},
    {"Skeleton", 525},
};

constexpr auto kAkSkinIndex = RangeConstrainedVariableParams<std::uint8_t>{.min = 0, .max = static_cast<std::uint8_t>(sizeof(kAkSkins) / sizeof(kAkSkins[0]) - 1), .def = 1};
constexpr auto kM4SkinIndex = RangeConstrainedVariableParams<std::uint8_t>{.min = 0, .max = static_cast<std::uint8_t>(sizeof(kM4Skins) / sizeof(kM4Skins[0]) - 1), .def = 1};
constexpr auto kAwpSkinIndex = RangeConstrainedVariableParams<std::uint8_t>{.min = 0, .max = static_cast<std::uint8_t>(sizeof(kAwpSkins) / sizeof(kAwpSkins[0]) - 1), .def = 1};
constexpr auto kDeagleSkinIndex = RangeConstrainedVariableParams<std::uint8_t>{.min = 0, .max = static_cast<std::uint8_t>(sizeof(kDeagleSkins) / sizeof(kDeagleSkins[0]) - 1), .def = 0};
constexpr auto kKnifeSkinIndex = RangeConstrainedVariableParams<std::uint8_t>{.min = 0, .max = static_cast<std::uint8_t>(sizeof(kKnifeSkins) / sizeof(kKnifeSkins[0]) - 1), .def = 0};
constexpr auto kKnifeTypeIndex = RangeConstrainedVariableParams<std::uint8_t>{.min = 0, .max = static_cast<std::uint8_t>(sizeof(kKnives) / sizeof(kKnives[0]) - 1), .def = 0};
constexpr auto kWear = RangeConstrainedVariableParams<std::uint16_t>{.min = 1, .max = 1000, .def = 1};
constexpr auto kSeed = RangeConstrainedVariableParams<std::uint16_t>{.min = 0, .max = 1000, .def = 1};

[[nodiscard]] constexpr std::uint16_t paintKitAt(const SkinPreset* presets, std::uint8_t index, std::uint8_t maxIndex) noexcept
{
    return index <= maxIndex ? presets[index].paintKit : 0;
}

}
