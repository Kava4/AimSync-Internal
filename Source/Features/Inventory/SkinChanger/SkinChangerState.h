#pragma once

#include <cstdint>

struct SkinChangerState {
    std::uint8_t dirtyCounter{0};
    bool forceFullUpdate{true};
};
