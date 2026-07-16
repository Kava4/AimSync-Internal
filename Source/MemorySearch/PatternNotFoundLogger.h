#pragma once

#include <MemorySearch/BytePattern.h>

#include <Platform/Macros/IsPlatform.h>

#if IS_WIN64()
#include <Windows.h>
#endif

struct PatternNotFoundLogger {
    static void onPatternNotFound(BytePattern pattern) noexcept
    {
        char message[512]{};
        char* writePosition = message;
        const char* const messageEnd = message + sizeof(message) - 1;

        for (const char* prefix = "[AimSync] pattern not found: "; *prefix && writePosition < messageEnd; ++prefix)
            *writePosition++ = *prefix;

        bool printedFirst = false;
        const auto wildcardChar{pattern.getWildcardChar()};
        for (const auto byte : pattern.raw()) {
            if (writePosition >= messageEnd)
                break;
            if (printedFirst)
                *writePosition++ = ' ';
            if (writePosition >= messageEnd)
                break;

            if (byte != wildcardChar) {
                static constexpr char hexDigits[] = "0123456789ABCDEF";
                if (writePosition + 2 > messageEnd)
                    break;
                *writePosition++ = hexDigits[(static_cast<unsigned char>(byte) >> 4) & 0xF];
                *writePosition++ = hexDigits[static_cast<unsigned char>(byte) & 0xF];
            } else {
                *writePosition++ = static_cast<char>(byte);
            }
            printedFirst = true;
        }
        *writePosition = '\0';

#if IS_WIN64()
        OutputDebugStringA(message);
        OutputDebugStringA("\n");
#endif
    }
};
