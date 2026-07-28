#pragma once

#include <HookContext/HookContextMacros.h>

template <typename HookContext, typename ConfigVariable>
struct IndexDropdownSelectionChangeHandler {
    explicit IndexDropdownSelectionChangeHandler(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void onSelectionChanged(int selectedIndex)
    {
        if (selectedIndex < ConfigVariable::ValueType::kMin || selectedIndex > ConfigVariable::ValueType::kMax)
            return;
        SET_CONFIG_VAR(ConfigVariable, static_cast<typename ConfigVariable::ValueType>(selectedIndex));
    }

private:
    HookContext& hookContext;
};
