#pragma once

#include <array>
#include <cstddef>
#include <optional>
#include <span>
#include <string_view>

#include <MemorySearch/BytePattern.h>
#include <MemorySearch/HybridPatternFinder.h>
#include <MemorySearch/PatternStringWildcard.h>
#include <Platform/DynamicLibrary.h>
#include <Platform/Macros/IsPlatform.h>
#include <Utils/HexChars.h>

#if IS_WIN64()
#include <Windows.h>
#endif

namespace runtime_pattern_search
{

[[nodiscard]] inline std::size_t patternStringLength(const char* patternString) noexcept
{
    std::size_t length = 0;
    while (patternString[length] != '\0')
        ++length;
    return length;
}

[[nodiscard]] inline std::optional<std::pair<std::span<const char>, std::size_t>> parseHexPattern(const char* patternString, std::array<char, 512>& storage) noexcept
{
    if (!patternString)
        return std::nullopt;

    std::size_t writePosition = 0;
    bool spaceExpected = false;

    for (std::size_t readPosition = 0; patternString[readPosition] != '\0'; ) {
        const char c = patternString[readPosition];

        if (spaceExpected) {
            if (c != ' ')
                return std::nullopt;
            ++readPosition;
            spaceExpected = false;
            continue;
        }

        if (c == kPatternStringWildcard) {
            if (writePosition >= storage.size())
                return std::nullopt;
            storage[writePosition++] = kPatternStringWildcard;
            ++readPosition;
            spaceExpected = true;
            continue;
        }

        if (readPosition + 1 >= patternStringLength(patternString))
            return std::nullopt;

        const auto converted = hexCharsToByte({patternString[readPosition], patternString[readPosition + 1]});
        if (!converted.has_value())
            return std::nullopt;

        if (writePosition >= storage.size())
            return std::nullopt;

        storage[writePosition++] = *converted;
        readPosition += 2;
        spaceExpected = true;
    }

    if (writePosition == 0)
        return std::nullopt;

    return std::pair{std::span<const char>{storage.data(), writePosition}, writePosition};
}

[[nodiscard]] inline void* findInSpan(std::span<const std::byte> bytes, const char* patternString) noexcept
{
    if (bytes.empty())
        return nullptr;

    std::array<char, 512> patternStorage{};
    const auto parsed = parseHexPattern(patternString, patternStorage);
    if (!parsed.has_value())
        return nullptr;

    const BytePattern pattern{std::string_view{parsed->first.data(), parsed->first.size()}, kPatternStringWildcard};
    HybridPatternFinder finder{bytes, pattern};
    if (const auto* found = finder.findNextOccurrence())
        return const_cast<std::byte*>(found);

    return nullptr;
}

[[nodiscard]] inline std::span<const std::byte> moduleImageSpan(HMODULE handle) noexcept
{
#if IS_WIN64()
    if (!handle)
        return {};

    const auto* const base = reinterpret_cast<const std::byte*>(handle);
    const auto* const dosHeader = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
        return {};

    const auto* const ntHeaders = reinterpret_cast<const IMAGE_NT_HEADERS*>(base + dosHeader->e_lfanew);
    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)
        return {};

    return {base, ntHeaders->OptionalHeader.SizeOfImage};
#else
    (void)handle;
    return {};
#endif
}

[[nodiscard]] inline void* findInModuleImage(const DynamicLibrary& library, const char* patternString) noexcept
{
    if (!library)
        return nullptr;

    if (void* const found = findInSpan(library.getCodeSection().raw(), patternString))
        return found;

    return findInSpan(moduleImageSpan(library.getHandle()), patternString);
}

[[nodiscard]] inline void* findInModule(const DynamicLibrary& library, const char* patternString) noexcept
{
    return findInModuleImage(library, patternString);
}

}
