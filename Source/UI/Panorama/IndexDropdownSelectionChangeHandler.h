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
        using Value = typename ConfigVariable::ValueType;
        SET_CONFIG_VAR(ConfigVariable, Value{static_cast<typename Value::ValueType>(selectedIndex)});
    }

private:
    HookContext& hookContext;
};
