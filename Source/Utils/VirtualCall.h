#pragma once

#include <cstddef>

namespace Utils
{

template <typename Ret, typename T, typename... Args>
[[nodiscard]] Ret callVirtual(T* thisptr, std::size_t index, Args... args) noexcept
{
    using Fn = Ret(*)(T*, Args...);
    return (*reinterpret_cast<Fn**>(thisptr))[index](thisptr, args...);
}

}
