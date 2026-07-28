#pragma once

#include <cassert>
#include <cctype>
#include <cstring>
#include <BuildConfig.h>
#include <MemoryAllocation/UniquePtr.h>
#include <Platform/PlatformPath.h>

#include "ConfigFileOperation.h"
#include "ConfigFromString.h"
#include "ConfigSchema.h"
#include "ConfigStringConversionState.h"
#include "ConfigToString.h"
#include "ConfigVariableChangeHandler.h"

#if IS_WIN64()
#include <Platform/Windows/FileSystem/WindowsFileSystem.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <Utils/Wcslen.h>
#elif IS_LINUX()
#include <Platform/Linux/LinuxPlatformApi.h>
#endif

template <typename HookContext>
class Config {
public:
    explicit Config(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void init() noexcept
    {
        buildConfigDirectoryPath();
        setCurrentConfigName("default.cfg");
        buildConfigFilePath(WIN64_LINUX(L"default.cfg", "default.cfg"));
        buildConfigTempFilePath();
        static constinit char8_t fileOperationBuffer[build::kConfigFileBufferSize];
        state().fileOperationBuffer = fileOperationBuffer;
        refreshConfigList();
        setStatus("Ready");
    }

    template <typename ConfigVariable>
    [[nodiscard]] auto getVariable() noexcept
    {
        return state().configVariables.template getVariableValue<ConfigVariable>();
    }

    template <typename ConfigVariable>
    bool setVariable(ConfigVariable::ValueType newValue) noexcept
    {
        if (changeVariableValue<ConfigVariable>(newValue)) {
            scheduleAutoSave();
            return true;
        }
        return false;
    }

    template <typename ConfigVariable>
    void setVariableWithoutAutoSave(ConfigVariable::ValueType newValue) noexcept
    {
        changeVariableValue<ConfigVariable>(newValue);
    }

    void restoreDefaults() noexcept
    {
        ConfigVariableTypes::forEach([this] <typename ConfigVariable> (std::type_identity<ConfigVariable>) {
            this->setVariableWithoutAutoSave<ConfigVariable>(ConfigVariable::kDefaultValue);
        });
        scheduleAutoSave();
        hookContext.gui().updateFromConfig();
        setStatus("Defaults restored");
    }

    void scheduleLoad() noexcept
    {
        state().loadScheduled = true;
    }

    void scheduleSave() noexcept
    {
        scheduleAutoSave();
    }

    [[nodiscard]] const char* currentConfigName() noexcept
    {
        return state().currentConfigName;
    }

    [[nodiscard]] std::size_t configCount() noexcept
    {
        return state().configCount;
    }

    [[nodiscard]] const char* configNameAt(std::size_t index) noexcept
    {
        if (index >= state().configCount)
            return "";
        return state().configNames[index];
    }

    [[nodiscard]] const char* statusMessage() noexcept
    {
        return state().statusMessage;
    }

    void refreshConfigList() noexcept
    {
        state().configCount = 0;
#if IS_WIN64()
        if (!state().pathToConfigDirectory)
            return;

        WindowsFileSystem::forEachCfgFile(state().pathToConfigDirectory.get(), [this](const wchar_t* fileName) noexcept {
            if (state().configCount >= ConfigState::kMaxConfigs)
                return;

            char utf8Name[ConfigState::kMaxConfigNameLength]{};
            if (!wideAsciiToUtf8(fileName, utf8Name, sizeof(utf8Name)))
                return;
            if (!isValidConfigFileName(utf8Name))
                return;

            copyCString(state().configNames[state().configCount], ConfigState::kMaxConfigNameLength, utf8Name);
            ++state().configCount;
        });
#endif
        ensureDefaultListed();
    }

    bool loadConfigByName(const char* utf8Name) noexcept
    {
        char normalized[ConfigState::kMaxConfigNameLength]{};
        if (!normalizeConfigName(utf8Name, normalized, sizeof(normalized))) {
            setStatus("Invalid config name");
            return false;
        }
        if (state().currentFileOperation != ConfigFileOperation::None) {
            setStatus("Busy — try again");
            return false;
        }

        wchar_t wideName[ConfigState::kMaxConfigNameLength]{};
        if (!utf8AsciiToWide(normalized, wideName, ConfigState::kMaxConfigNameLength)) {
            setStatus("Invalid config name");
            return false;
        }

        setCurrentConfigName(normalized);
        buildConfigFilePath(wideName);
        buildConfigTempFilePath();
        scheduleLoad();
        setStatus("Loading...");
        return true;
    }

    bool saveActiveConfig() noexcept
    {
        if (state().currentFileOperation != ConfigFileOperation::None) {
            setStatus("Busy — try again");
            return false;
        }
        scheduleAutoSave();
        setStatus("Saving...");
        return true;
    }

    bool createAndSaveConfig(const char* utf8Name) noexcept
    {
        char normalized[ConfigState::kMaxConfigNameLength]{};
        if (!normalizeConfigName(utf8Name, normalized, sizeof(normalized))) {
            setStatus("Use a-z, 0-9, _ or -");
            return false;
        }
        if (state().currentFileOperation != ConfigFileOperation::None) {
            setStatus("Busy — try again");
            return false;
        }

        wchar_t wideName[ConfigState::kMaxConfigNameLength]{};
        if (!utf8AsciiToWide(normalized, wideName, ConfigState::kMaxConfigNameLength)) {
            setStatus("Invalid config name");
            return false;
        }

        setCurrentConfigName(normalized);
        buildConfigFilePath(wideName);
        buildConfigTempFilePath();
        scheduleAutoSave();
        refreshConfigList();
        setStatus("Created / saving...");
        return true;
    }

    bool deleteConfigByName(const char* utf8Name) noexcept
    {
        char normalized[ConfigState::kMaxConfigNameLength]{};
        if (!normalizeConfigName(utf8Name, normalized, sizeof(normalized))) {
            setStatus("Invalid config name");
            return false;
        }
        if (std::strcmp(normalized, "default.cfg") == 0) {
            setStatus("Cannot delete default.cfg");
            return false;
        }
        if (state().currentFileOperation != ConfigFileOperation::None) {
            setStatus("Busy — try again");
            return false;
        }

#if IS_WIN64()
        if (!state().pathToConfigDirectory)
            return false;

        wchar_t wideName[ConfigState::kMaxConfigNameLength]{};
        if (!utf8AsciiToWide(normalized, wideName, ConfigState::kMaxConfigNameLength))
            return false;

        const std::basic_string_view directory{state().pathToConfigDirectory.get(), utils::wcslen(state().pathToConfigDirectory.get())};
        const auto pathLength = directory.length() + 1 + utils::wcslen(wideName) + 1;
        auto fullPath = mem::makeUniqueForOverwrite<wchar_t[]>(pathLength);
        if (!fullPath)
            return false;

        std::size_t writeIndex{0};
        std::ranges::copy(directory, fullPath.get() + writeIndex);
        writeIndex += directory.length();
        fullPath.get()[writeIndex++] = L'\\';
        const auto nameLen = utils::wcslen(wideName);
        std::ranges::copy(wideName, wideName + nameLen, fullPath.get() + writeIndex);
        writeIndex += nameLen;
        fullPath.get()[writeIndex] = L'\0';

        if (!WindowsFileSystem::deleteFile(fullPath.get())) {
            setStatus("Delete failed");
            return false;
        }

        const bool deletingActive = std::strcmp(state().currentConfigName, normalized) == 0;
        refreshConfigList();
        if (deletingActive)
            loadConfigByName("default.cfg");
        else
            setStatus("Deleted");
        return true;
#else
        (void)normalized;
        setStatus("Delete unsupported");
        return false;
#endif
    }

    void update()
    {
        switch (state().currentFileOperation) {
        case ConfigFileOperation::None:
            if (state().autoSaveScheduled) {
                state().currentFileOperation = ConfigFileOperation::Save;
                prepareSaveToFile();
                state().autoSaveScheduled = false;
                break;
            }
            if (state().loadScheduled) {
                state().currentFileOperation = ConfigFileOperation::Load;
                state().bufferUsedBytes = 0;
                state().loadScheduled = false;
                break;
            }
            break;
        case ConfigFileOperation::Load:
            finishLoadFromFile();
            break;
        default:
            break;
        }
    }

    void performFileOperation() noexcept
    {
        switch (state().currentFileOperation) {
        case ConfigFileOperation::Load:
            loadFromFile();
            break;
        case ConfigFileOperation::Save:
            saveToFile();
            break;
        default:
            break;
        }
    }

private:
    template <typename ConfigVariable>
    bool changeVariableValue(ConfigVariable::ValueType newValue) noexcept
    {
        if (getVariable<ConfigVariable>() == newValue)
            return false;

        ConfigVariableChangeHandler{hookContext}.template onConfigVariableValueChanged<ConfigVariable>(newValue);
        state().configVariables.template storeVariableValue<ConfigVariable>(newValue);
        return true;
    }

    void scheduleAutoSave() noexcept
    {
        state().autoSaveScheduled = true;
    }

    [[nodiscard]] auto& state()
    {
        return hookContext.configState();
    }

    static void copyCString(char* dest, std::size_t destSize, const char* src) noexcept
    {
        if (!dest || destSize == 0)
            return;
        if (!src) {
            dest[0] = '\0';
            return;
        }
        std::size_t i = 0;
        for (; src[i] != '\0' && i + 1 < destSize; ++i)
            dest[i] = src[i];
        dest[i] = '\0';
    }

    void setStatus(const char* message) noexcept
    {
        copyCString(state().statusMessage, ConfigState::kStatusMessageLength, message ? message : "");
    }

    void setCurrentConfigName(const char* name) noexcept
    {
        copyCString(state().currentConfigName, ConfigState::kMaxConfigNameLength, name ? name : "default.cfg");
    }

    void ensureDefaultListed() noexcept
    {
        for (std::size_t i = 0; i < state().configCount; ++i) {
            if (std::strcmp(state().configNames[i], "default.cfg") == 0)
                return;
        }
        if (state().configCount >= ConfigState::kMaxConfigs)
            return;
        copyCString(state().configNames[state().configCount], ConfigState::kMaxConfigNameLength, "default.cfg");
        ++state().configCount;
    }

    [[nodiscard]] static bool isValidConfigFileName(const char* name) noexcept
    {
        if (!name || !*name)
            return false;
        const auto len = std::strlen(name);
        if (len < 5 || len >= ConfigState::kMaxConfigNameLength)
            return false;
        if (std::strcmp(name + (len - 4), ".cfg") != 0)
            return false;
        for (std::size_t i = 0; i < len - 4; ++i) {
            const unsigned char c = static_cast<unsigned char>(name[i]);
            if (!(std::isalnum(c) || c == '_' || c == '-'))
                return false;
        }
        return true;
    }

    [[nodiscard]] static bool normalizeConfigName(const char* input, char* out, std::size_t outSize) noexcept
    {
        if (!input || !out || outSize < 6)
            return false;

        char temp[ConfigState::kMaxConfigNameLength]{};
        std::size_t write = 0;
        for (std::size_t i = 0; input[i] != '\0' && write + 1 < sizeof(temp); ++i) {
            const unsigned char c = static_cast<unsigned char>(input[i]);
            if (c == ' ')
                continue;
            if (!(std::isalnum(c) || c == '_' || c == '-' || c == '.'))
                return false;
            temp[write++] = static_cast<char>(c);
        }
        temp[write] = '\0';
        if (write == 0)
            return false;

        if (write < 4 || std::strcmp(temp + (write - 4), ".cfg") != 0) {
            if (write + 4 >= sizeof(temp))
                return false;
            temp[write++] = '.';
            temp[write++] = 'c';
            temp[write++] = 'f';
            temp[write++] = 'g';
            temp[write] = '\0';
        }

        if (!isValidConfigFileName(temp))
            return false;
        copyCString(out, outSize, temp);
        return true;
    }

    [[nodiscard]] static bool utf8AsciiToWide(const char* utf8, wchar_t* wide, std::size_t wideCount) noexcept
    {
        if (!utf8 || !wide || wideCount == 0)
            return false;
        std::size_t i = 0;
        for (; utf8[i] != '\0' && i + 1 < wideCount; ++i) {
            const unsigned char c = static_cast<unsigned char>(utf8[i]);
            if (c >= 128)
                return false;
            wide[i] = static_cast<wchar_t>(c);
        }
        wide[i] = L'\0';
        return i > 0;
    }

    [[nodiscard]] static bool wideAsciiToUtf8(const wchar_t* wide, char* utf8, std::size_t utf8Count) noexcept
    {
        if (!wide || !utf8 || utf8Count == 0)
            return false;
        std::size_t i = 0;
        for (; wide[i] != L'\0' && i + 1 < utf8Count; ++i) {
            if (wide[i] > 127)
                return false;
            utf8[i] = static_cast<char>(wide[i]);
        }
        utf8[i] = '\0';
        return i > 0;
    }

    void loadFromFile() noexcept
    {
        if (!state().pathToConfigFile)
            return;

#if IS_WIN64()
        const std::basic_string_view path{state().pathToConfigFile.get(), utils::wcslen(state().pathToConfigFile.get())};
        UNICODE_STRING pathStr{.Length = static_cast<USHORT>(path.length() * sizeof(wchar_t)), .MaximumLength = static_cast<USHORT>(path.length() * sizeof(wchar_t)), .Buffer = const_cast<wchar_t*>(path.data())};
        if (const auto handle = WindowsFileSystem::openFileForReading(pathStr); handle != INVALID_HANDLE_VALUE) {
            state().bufferUsedBytes = WindowsFileSystem::readFile(handle, 0, state().fileOperationBuffer, build::kConfigFileBufferSize);
            WindowsSyscalls::NtClose(handle);
        }
#elif IS_LINUX()
        if (const auto fd = LinuxPlatformApi::open(state().pathToConfigFile.get(), O_RDONLY); fd >= 0) {
            if (const auto read = LinuxPlatformApi::pread(fd, state().fileOperationBuffer, build::kConfigFileBufferSize, 0); read > 0)
                state().bufferUsedBytes = static_cast<std::size_t>(read);
            LinuxPlatformApi::close(fd);
        }
#endif
    }

    void finishLoadFromFile()
    {
        assert(state().currentFileOperation == ConfigFileOperation::Load);
        state().currentFileOperation = ConfigFileOperation::None;

        const auto readBytes = state().bufferUsedBytes;
        assert(readBytes < build::kConfigFileBufferSize && "Currently file must fit into a buffer");
        ConfigStringConversionState conversionState;
        std::size_t parsedBytes{0};
        do {
            assert(conversionState.offset <= readBytes);
            ConfigFromString configFromString{std::span{state().fileOperationBuffer + conversionState.offset, readBytes - conversionState.offset}, conversionState};
            parsedBytes = ConfigSchema{hookContext}.performConversion(configFromString);
        } while (parsedBytes != 0 && (conversionState.nestingLevel != 0 || conversionState.indexInNestingLevel[0] != 1));
        
        assert(readBytes == 0 || (conversionState.nestingLevel == 0 && conversionState.indexInNestingLevel[0] == 1));
        hookContext.gui().updateFromConfig();
        setStatus(readBytes == 0 ? "Loaded (empty/new)" : "Loaded");
    }

    void prepareSaveToFile()
    {
        ConfigStringConversionState conversionState;
        ConfigToString configToString{std::span{state().fileOperationBuffer, build::kConfigFileBufferSize}, conversionState};
        state().bufferUsedBytes = ConfigSchema{hookContext}.performConversion(configToString);
        assert(conversionState.nestingLevel == 0 && conversionState.indexInNestingLevel[0] == 1);
    }

    void saveToFile() noexcept
    {
        assert(state().currentFileOperation == ConfigFileOperation::Save);
        state().currentFileOperation = ConfigFileOperation::None;

        if (!hookContext.aimsyncDirectoryPath().get() || !state().pathToConfigDirectory || !state().pathToConfigFile || !state().pathToConfigTempFile)
            return;

        const auto numberOfBytesToWrite = state().bufferUsedBytes;
#if IS_WIN64()
        WindowsFileSystem::createDirectory(hookContext.aimsyncDirectoryPath().get());
        WindowsFileSystem::createDirectory(state().pathToConfigDirectory.get());

        if (const auto handle = WindowsFileSystem::createFileForOverwrite(state().pathToConfigTempFile.get()); handle != INVALID_HANDLE_VALUE) {
            if (WindowsFileSystem::writeFile(handle, 0, state().fileOperationBuffer, numberOfBytesToWrite) == numberOfBytesToWrite)
                WindowsFileSystem::renameFile(handle, state().pathToConfigFile.get());
            WindowsSyscalls::NtClose(handle);
            refreshConfigList();
            setStatus("Saved");
        } else {
            setStatus("Save failed");
        }
#elif IS_LINUX()
        mkdir(hookContext.aimsyncDirectoryPath().get(), 0777);
        mkdir(state().pathToConfigDirectory.get(), 0777);

        if (const auto fd = LinuxPlatformApi::open(state().pathToConfigTempFile.get(), O_CREAT | O_WRONLY, 0666); fd >= 0) {
            if (std::cmp_equal(LinuxPlatformApi::write(fd, state().fileOperationBuffer, numberOfBytesToWrite), numberOfBytesToWrite))
                rename(state().pathToConfigTempFile.get(), state().pathToConfigFile.get());
            LinuxPlatformApi::close(fd);
        }
#endif
    }

    void buildConfigDirectoryPath() noexcept
    {
        if (!hookContext.aimsyncDirectoryPath().get())
            return;

        const std::basic_string_view aimsyncDirectoryPath{hookContext.aimsyncDirectoryPath().get(), WIN64_LINUX(utils::wcslen, std::strlen)(hookContext.aimsyncDirectoryPath().get())};
        constexpr auto kPathSeparatorLength{1};
        constexpr auto kNullTerminatorLength{1};
        const auto length = aimsyncDirectoryPath.length() + kPathSeparatorLength + build::kConfigDirectoryName.length() + kNullTerminatorLength;
        state().pathToConfigDirectory = mem::makeUniqueForOverwrite<platform::PathCharType[]>(length);
        if (!state().pathToConfigDirectory)
            return;
        std::size_t writeIndex{0};
        std::ranges::copy(aimsyncDirectoryPath, state().pathToConfigDirectory.get() + writeIndex);
        writeIndex += aimsyncDirectoryPath.length();
        state().pathToConfigDirectory.get()[writeIndex++] = platform::kPathSeparator;
        std::ranges::copy(build::kConfigDirectoryName, state().pathToConfigDirectory.get() + writeIndex);
        writeIndex += build::kConfigDirectoryName.length();
        state().pathToConfigDirectory.get()[writeIndex++] = 0;
    }

    void buildConfigFilePath(std::basic_string_view<platform::PathCharType> configFileName) noexcept
    {
        if (!state().pathToConfigDirectory)
            return;

        const std::basic_string_view pathToConfigDirectory{state().pathToConfigDirectory.get(), WIN64_LINUX(utils::wcslen, std::strlen)(state().pathToConfigDirectory.get())};
        constexpr auto kPathSeparatorLength{1};
        constexpr auto kNullTerminatorLength{1};
        const auto length = pathToConfigDirectory.length() + kPathSeparatorLength + configFileName.length() + kNullTerminatorLength;
        state().pathToConfigFile = mem::makeUniqueForOverwrite<platform::PathCharType[]>(length);
        if (!state().pathToConfigFile)
            return;
        std::size_t writeIndex{0};
        std::ranges::copy(pathToConfigDirectory, state().pathToConfigFile.get() + writeIndex);
        writeIndex += pathToConfigDirectory.length();
        state().pathToConfigFile.get()[writeIndex++] = platform::kPathSeparator;
        std::ranges::copy(configFileName, state().pathToConfigFile.get() + writeIndex);
        writeIndex += configFileName.length();
        state().pathToConfigFile.get()[writeIndex++] = 0;
    }

    void buildConfigTempFilePath() noexcept
    {
        if (!state().pathToConfigFile)
            return;

        const std::basic_string_view pathToConfigFile{state().pathToConfigFile.get(), WIN64_LINUX(utils::wcslen, std::strlen)(state().pathToConfigFile.get())};
        const std::basic_string_view configTempFileSuffix{WIN64_LINUX(L".new", ".new")};
        constexpr auto kNullTerminatorLength{1};
        const auto length = pathToConfigFile.length() + configTempFileSuffix.length() + kNullTerminatorLength;
        state().pathToConfigTempFile = mem::makeUniqueForOverwrite<platform::PathCharType[]>(length);
        if (!state().pathToConfigTempFile)
            return;
        std::size_t writeIndex{0};
        std::ranges::copy(pathToConfigFile, state().pathToConfigTempFile.get() + writeIndex);
        writeIndex += pathToConfigFile.length();
        std::ranges::copy(configTempFileSuffix, state().pathToConfigTempFile.get() + writeIndex);
        writeIndex += configTempFileSuffix.length();
        state().pathToConfigTempFile.get()[writeIndex++] = 0;
    }

    HookContext& hookContext;
};
