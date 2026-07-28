#pragma once

#include <d3d11.h>

struct AimSyncMenuTextures {
    ID3D11ShaderResourceView* logo{};
    ID3D11ShaderResourceView* aim{};
    ID3D11ShaderResourceView* aimPressed{};
    ID3D11ShaderResourceView* visual{};
    ID3D11ShaderResourceView* visualPressed{};
    ID3D11ShaderResourceView* misc{};
    ID3D11ShaderResourceView* miscPressed{};
    ID3D11ShaderResourceView* config{};
    ID3D11ShaderResourceView* configPressed{};
    int logoW{};
    int logoH{};
    int buttonW{};
    int buttonH{};
    bool loaded{false};
};

[[nodiscard]] bool aimSyncMenuLoadTextures(ID3D11Device* device, AimSyncMenuTextures& out) noexcept;
void aimSyncMenuReleaseTextures(AimSyncMenuTextures& textures) noexcept;
