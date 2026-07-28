#pragma once

#include <optional>

#include <CS2/Classes/Vector.h>
#include <ThirdParty/ImGui/imgui.h>

#include "WorldToClipSpaceConverter.h"

template <typename HookContext>
struct WorldToScreen {
    explicit WorldToScreen(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] std::optional<ImVec2> toScreen(const cs2::Vector& worldPosition) const noexcept
    {
        const auto clip = hookContext.template make<WorldToClipSpaceConverter>().toClipSpace(worldPosition);
        if (!clip.onScreen())
            return std::nullopt;

        const auto inverseW = 1.0f / clip.w;
        const float ndcX = clip.x * inverseW;
        const float ndcY = clip.y * inverseW;
        const ImVec2 display = ImGui::GetIO().DisplaySize;
        return ImVec2{
            (ndcX + 1.0f) * 0.5f * display.x,
            (1.0f - ndcY) * 0.5f * display.y
        };
    }

private:
    HookContext& hookContext;
};
