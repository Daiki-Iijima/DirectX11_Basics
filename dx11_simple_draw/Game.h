//
// Game.h
//

#pragma once

#include "Common/DeviceResources.h"
#include "Common/StepTimer.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#pragma comment(lib,"d3dcompiler.lib")
#include <d3dcompiler.h>
#include <iostream>

#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")
#include<shtypes.h>
#include<wingdi.h>
#include<d2d1.h>
#include<dwrite.h>
#include "Camera.h"
#include "ModelManager.h"
#include "Model.h"

//  ピクセルシェーダーの定数バッファ
struct PsConstantBuffer
{
    DirectX::XMFLOAT3 LightDirection;
    float padding1;                   // 4バイトのパディング
    DirectX::XMFLOAT3 LightColor;
    float LightIntensity;
};

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
    ~Game() = default;

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnDisplayChange();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const noexcept;

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

    //  ビュー行列
    DirectX::XMMATRIX                      m_view;
    //  プロジェクション行列
    DirectX::XMMATRIX                      m_projection;
};
