#pragma once

#include <cstdint>

namespace cs2
{

struct CEconItem {
    void* vtable{};
    char pad0[0x8]{};
    std::uint64_t m_ulID{};
    std::uint64_t m_ulOriginalID{};
    void* m_pCustomDataOptimizedObject{};
    std::uint32_t m_unAccountID{};
    std::uint32_t m_unInventory{};
    std::uint16_t m_unDefIndex{};
    std::uint16_t m_unOrigin : 5;
    std::uint16_t m_nQuality : 4;
    std::uint16_t m_unLevel : 2;
    std::uint16_t m_nRarity : 4;
    std::uint16_t m_dirtybitInUse : 1;
    std::int16_t m_iItemSet{};
    int m_bSOUpdateFrame{};
    std::uint8_t m_unFlags{};
};

}
