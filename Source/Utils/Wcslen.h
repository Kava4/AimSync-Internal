#pragma once

#include <cstddef>

namespace utils
{

[[nodiscard]] inline std::size_t wcslen(const wchar_t* str) noexcept
{
    std::size_t length = 0;
    while (*str) {
        ++length;
        ++str;
    }
    return length;
}

}
