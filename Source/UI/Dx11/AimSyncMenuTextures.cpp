#include "AimSyncMenuTextures.h"
#include "AimSyncMenuImages.h"

#include <cstddef>

#define STB_IMAGE_IMPLEMENTATION
#include <ThirdParty/stb_image.h>

namespace
{

bool loadSrv(ID3D11Device* device, const unsigned char* bytes, std::size_t size, ID3D11ShaderResourceView** outSrv, int* outW, int* outH) noexcept
{
    if (!device || !bytes || !outSrv)
        return false;

    int width = 0;
    int height = 0;
    unsigned char* pixels = stbi_load_from_memory(bytes, static_cast<int>(size), &width, &height, nullptr, 4);
    if (!pixels)
        return false;

    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = static_cast<UINT>(width);
    desc.Height = static_cast<UINT>(height);
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA sub{};
    sub.pSysMem = pixels;
    sub.SysMemPitch = static_cast<UINT>(width * 4);

    ID3D11Texture2D* texture = nullptr;
    if (FAILED(device->CreateTexture2D(&desc, &sub, &texture)) || !texture) {
        stbi_image_free(pixels);
        return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    const HRESULT hr = device->CreateShaderResourceView(texture, &srvDesc, outSrv);
    texture->Release();
    stbi_image_free(pixels);

    if (FAILED(hr) || !*outSrv)
        return false;

    if (outW)
        *outW = width;
    if (outH)
        *outH = height;
    return true;
}

void releaseSrv(ID3D11ShaderResourceView*& srv) noexcept
{
    if (srv) {
        srv->Release();
        srv = nullptr;
    }
}

}

bool aimSyncMenuLoadTextures(ID3D11Device* device, AimSyncMenuTextures& out) noexcept
{
    if (out.loaded || !device)
        return out.loaded;

    using namespace aimsync_menu_images;

    // Best-effort: missing icons fall back to text buttons in the sidebar.
    (void)loadSrv(device, Logo, LogoSize, &out.logo, &out.logoW, &out.logoH);
    (void)loadSrv(device, Aim, AimSize, &out.aim, &out.buttonW, &out.buttonH);
    (void)loadSrv(device, AimPressed, AimPressedSize, &out.aimPressed, nullptr, nullptr);
    (void)loadSrv(device, Visual, VisualSize, &out.visual, nullptr, nullptr);
    (void)loadSrv(device, VisualPressed, VisualPressedSize, &out.visualPressed, nullptr, nullptr);
    (void)loadSrv(device, Misc, MiscSize, &out.misc, nullptr, nullptr);
    (void)loadSrv(device, MiscPressed, MiscPressedSize, &out.miscPressed, nullptr, nullptr);
    (void)loadSrv(device, Config, ConfigSize, &out.config, nullptr, nullptr);
    (void)loadSrv(device, ConfigPressed, ConfigPressedSize, &out.configPressed, nullptr, nullptr);

    out.loaded = true;
    return true;
}

void aimSyncMenuReleaseTextures(AimSyncMenuTextures& textures) noexcept
{
    releaseSrv(textures.logo);
    releaseSrv(textures.aim);
    releaseSrv(textures.aimPressed);
    releaseSrv(textures.visual);
    releaseSrv(textures.visualPressed);
    releaseSrv(textures.misc);
    releaseSrv(textures.miscPressed);
    releaseSrv(textures.config);
    releaseSrv(textures.configPressed);
    textures = {};
}
