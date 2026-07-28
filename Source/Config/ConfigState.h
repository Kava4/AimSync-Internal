#pragma once

#include "ConfigFileOperation.h"
#include <cstddef>
#include <MemoryAllocation/UniquePtr.h>
#include <Platform/PlatformPath.h>

#include "ConfigVariables.h"

struct ConfigState {
    static constexpr std::size_t kMaxConfigs{32};
    static constexpr std::size_t kMaxConfigNameLength{64};
    static constexpr std::size_t kStatusMessageLength{96};

    bool autoSaveScheduled{false};
    bool loadScheduled{false};
    ConfigFileOperation currentFileOperation{ConfigFileOperation::None};
    char8_t* fileOperationBuffer{};
    std::size_t bufferUsedBytes{};
    UniquePtr<platform::PathCharType[]> pathToConfigDirectory{};
    UniquePtr<platform::PathCharType[]> pathToConfigFile{};
    UniquePtr<platform::PathCharType[]> pathToConfigTempFile{};
    ConfigVariables configVariables{};

    char currentConfigName[kMaxConfigNameLength]{"default.cfg"};
    char configNames[kMaxConfigs][kMaxConfigNameLength]{};
    std::size_t configCount{0};
    char statusMessage[kStatusMessageLength]{};
};
