//
// Game.cpp
//

#include "Common/pch.h"
#include "Game.h"

#pragma comment(lib,"d3dcompiler.lib")
#include <d3dcompiler.h>
#include <iostream>

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

struct Vertex {
    XMFLOAT3 position;
    XMFLOAT3 normal;
};

struct ConstantBuffer {
    XMFLOAT4X4 world;
    XMFLOAT4X4 view;
    XMFLOAT4X4 projection;
};

struct Model {
    std::vector<Vertex> vertices;
    std::vector<unsigned short> indices;
};

Model CreateRegularPolygon(int n) {
    Model model;
    if (n < 3) {
        // ƒGƒ‰[ƒ`ƒFƒbƒNF3–¢–‚Ì’¸“_”‚Í³‘½ŠpŒ`‚ğŒ`¬‚Å‚«‚Ü‚¹‚ñ
        return model;
    }

    float radius = 1.f;

    for (int i = 0; i < n; i++) {
        float angle = XM_2PI * i / n;
        float x = radius * cosf(angle);
        float y = radius * sinf(angle);
        model.vertices.push_back({ XMFLOAT3(x, y, 0.0f) });
    }

    //  Œv‰ñ‚è‚Å’¸“_‚ğŒ‹‚Ô
    for (int i = 1; i < n - 1; i++) {
        model.indices.push_back(0);
        model.indices.push_back(i + 1);
        model.indices.push_back(i);
    }

    return model;
}

Model model;

ID3D11Buffer* vertexBuffer = nullptr;
ID3D11Buffer* indexBuffer = nullptr;

ID3D11VertexShader* verteShader = nullptr;
ID3D11PixelShader* pixelShader = nullptr;
ID3D11InputLayout* inputLayout = nullptr;

ID3D11Buffer* constantBuffer = nullptr;

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    // TODO: Provide parameters for swapchain format, depth/stencil format, and backbuffer count.
    //   Add DX::DeviceResources::c_AllowTearing to opt-in to variable rate displays.
    //   Add DX::DeviceResources::c_EnableHDR for HDR10 display.
    m_deviceResources->RegisterDeviceNotify(this);

}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

    //  ƒ[ƒ‹ƒhÀ•W‚ğİ’è‚·‚é
    m_world = XMMatrixTranslation(0.0f, -1.5f, 5.0f);
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
struct Vertex {
    XMFLOAT3 position;
};
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

float z = -5.0f;
float x = 0.f;

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.
    if (GetAsyncKeyState('W') & 0x8000) {
        z += 1.f * elapsedTime;
    }
    if (GetAsyncKeyState('S') & 0x8000) {
        z -= 1.f * elapsedTime;
    }
    if (GetAsyncKeyState('D') & 0x8000) {
        x += 1.f * elapsedTime;
    }
    if (GetAsyncKeyState('A') & 0x8000) {
        x -= 1.f * elapsedTime;
    }

    //  ï¿½Jï¿½ï¿½ï¿½ï¿½ï¿½Ìİ’ï¿½
    XMVECTOR eyePosition = XMVectorSet(x, 0.0f, z, 0.0f); //  ï¿½ï¿½ï¿½_ï¿½ÍŒï¿½ï¿½_ï¿½ï¿½ï¿½ï¿½Zï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
    XMVECTOR forcusPoint = XMVectorSet(x, 0.0f, z + 5.f, 0.0f);  //  ï¿½ï¿½ï¿½ï¿½ï¿½_ï¿½Íï¿½Éï¿½ï¿½ï¿½+5
    XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);  //  ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Yï¿½ï¿½

    //  ï¿½rï¿½ï¿½ï¿½[ï¿½sï¿½ï¿½ğ¶ï¿½
    m_view = XMMatrixLookAtLH(eyePosition, forcusPoint, upDirection);

    elapsedTime;
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Add your rendering code here.
    context->VSSetShader(verteShader,nullptr,0);
    context->PSSetShader(pixelShader,nullptr,0);
    context->IASetInputLayout(inputLayout);

    //  ƒRƒ“ƒXƒ^ƒ“ƒgƒoƒbƒtƒ@‚ğİ’è‚·‚é
    ConstantBuffer cb;
    //  GPU—p‚Ìs—ñ‚É•ÏŠ·‚µ‚Â‚ÂAXMMATRIX‚ğXMFLOAT4X4‚É•ÏŠ·‚·‚é
    XMStoreFloat4x4(&cb.world, XMMatrixTranspose(m_world));
    XMStoreFloat4x4(&cb.view, XMMatrixTranspose(m_view));
    XMStoreFloat4x4(&cb.projection, XMMatrixTranspose(m_projection));
    context->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0,1,&vertexBuffer,&stride,&offset);
    context->IASetIndexBuffer(indexBuffer,DXGI_FORMAT_R16_UINT,0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //  ’è”ƒoƒbƒtƒ@‚ğİ’è‚·‚é
    context->VSSetConstantBuffers(0,1,&constantBuffer);

    context->DrawIndexed(model.indices.size(), 0, 0);
    
    context;

    m_deviceResources->PIXEndEvent();

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto const viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    auto const r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange()
{
    m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 800;
    height = 600;
}
#pragma endregion

/// <summary>
/// ’¸“_î•ñ‚ğì¬‚·‚é
/// </summary>
void CreateVertexBuffer(ID3D11Device* device,Model& model,ID3D11Buffer** createdBuffer){

    D3D11_BUFFER_DESC vertexBufferDesc = {
        static_cast<UINT>(model.vertices.size() * sizeof(Vertex)),
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_VERTEX_BUFFER,
        0, 0, 0
    };

    D3D11_SUBRESOURCE_DATA vertexSubresourceData = {
        model.vertices.data(), // C³: ÀÛ‚Ìƒf[ƒ^‚Ö‚Ìƒ|ƒCƒ“ƒ^
        0, 0
    };

    //   ’¸“_ƒoƒbƒtƒ@‚ğ¶¬‚·‚é
    device->CreateBuffer(&vertexBufferDesc,&vertexSubresourceData,createdBuffer);
}

void CreateIndexBuffer(ID3D11Device* device,Model& model,ID3D11Buffer** createdBuffer) {

    D3D11_BUFFER_DESC indexBufferDesc = {
        static_cast<UINT>(model.indices.size() * sizeof(unsigned short)),
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_INDEX_BUFFER,
        0,0,0
    };

    D3D11_SUBRESOURCE_DATA indexSubresourceData = {
        model.indices.data(),
        0,0
    };

    device->CreateBuffer(&indexBufferDesc, &indexSubresourceData, createdBuffer);
}

/// <summary>
/// ’¸“_ƒVƒF[ƒ_‚ğ¶¬
/// </summary>
ComPtr<ID3DBlob> CreateVertexShader(ID3D11Device* device, ID3D11VertexShader** createdShader) {

    //  ’¸“_ƒVƒF[ƒ_[‚ğ“Ç‚İ‚İƒRƒ“ƒpƒCƒ‹‚·‚é
    ComPtr<ID3DBlob> compiledVS;
    D3DCompileFromFile(L"Shader/VertexShader.hlsl",nullptr,nullptr,"main","vs_5_0",0,0,&compiledVS,nullptr);

    //  ’¸“_ƒVƒF[ƒ_[‚ğ¶¬‚·‚é
    device->CreateVertexShader(compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), nullptr, createdShader);

    return compiledVS;
}

/// <summary>
/// ƒsƒNƒZƒ‹ƒVƒF[ƒ_‚ğ¶¬‚·‚é
/// </summary>
ComPtr<ID3DBlob> CreatePixelShader(ID3D11Device* device, ID3D11PixelShader** createdShader) {
    //  ƒsƒNƒZƒ‹ƒVƒF[ƒ_[‚ğ“Ç‚İ‚İƒRƒ“ƒpƒCƒ‹‚·‚é
    ComPtr<ID3DBlob> compiledPS;
    D3DCompileFromFile(L"Shader/PixelShader.hlsl", nullptr, nullptr, "main", "ps_5_0", 0, 0, &compiledPS, nullptr);

    //  ƒsƒNƒZƒ‹ƒVƒF[ƒ_[‚ğ¶¬‚·‚é
    device->CreatePixelShader(compiledPS->GetBufferPointer(), compiledPS->GetBufferSize(), nullptr, &pixelShader);

    return compiledPS;
}

/// <summary>
/// ’¸“_ƒCƒ“ƒvƒbƒgƒŒƒCƒAƒEƒg‚ğ¶¬‚·‚é
/// </summary>
void CreateInputLayout(ID3D11Device* device, ID3DBlob* compiledVS, ID3D11InputLayout** createdLayout) {
    //  ’¸“_ƒCƒ“ƒvƒbƒgƒŒƒCƒAƒEƒg‚ğ¶¬
    std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
        { "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
        { "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}
    };

    //  ’¸“_ƒCƒ“ƒvƒbƒgƒŒƒCƒAƒEƒg‚ğ¶¬‚·‚é
    //  ƒVƒF[ƒ_‚ÆƒŒƒCƒAƒEƒg‚ÌŒ^‚ÌŒİŠ·«‚ğ’S•Û‚·‚é•K—v‚ª‚ ‚é
    device->CreateInputLayout(&layout[0], layout.size(), compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), &inputLayout);
}

/// <summary>
/// ’è”ƒoƒbƒtƒ@‚ğ¶¬‚·‚é
/// </summary>
void CreateConstantBuffer(ID3D11Device* device, ID3D11Buffer** createdBuffer) {
    //  ’è”ƒoƒbƒtƒ@‚ğ¶¬‚·‚é
    D3D11_BUFFER_DESC constantBufferDesc = {
        sizeof(ConstantBuffer),
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_CONSTANT_BUFFER,
        0,0,0
    };

    HRESULT result = device->CreateBuffer(&constantBufferDesc, nullptr, createdBuffer);
}

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    //  ƒ‚ƒfƒ‹‚Ìî•ñ‚ğ¶¬
    //model = CreateRegularPolygon(5);
    //  ƒ‚ƒfƒ‹‚Ì“Ç‚İ‚İ
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        "Models/teapot.obj",
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_GenNormals  // –@üî•ñ‚ÌŒvZ‚ğ’Ç‰Á
    );

    if (!scene) {
        // ƒGƒ‰[ƒnƒ“ƒhƒŠƒ“ƒO
        std::string errorStr = "ERROR: " + std::string(importer.GetErrorString()) + "\n";
        std::wstring errorWStr = std::wstring(errorStr.begin(), errorStr.end());
        OutputDebugString(errorWStr.c_str());
    }

    for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
        aiMesh* mesh = scene->mMeshes[m];

        // ’¸“_î•ñ‚Ì’Šo
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            aiVector3D pos = mesh->mVertices[i];
            aiVector3D normal = mesh->mNormals[i];
            vertex.position = DirectX::XMFLOAT3(pos.x, pos.y, pos.z);
            vertex.normal = DirectX::XMFLOAT3(normal.x, normal.y, normal.z);
            model.vertices.push_back(vertex);
        }

        // ƒCƒ“ƒfƒbƒNƒXî•ñ‚Ì’Šo
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                model.indices.push_back(static_cast<unsigned short>(face.mIndices[j]));
            }
        }
    }

    //  ’¸“_î•ñ‚ğì¬‚µ‚ÄAGPU‚É“]‘—‚·‚é
    CreateVertexBuffer(device, model, &vertexBuffer);

    //  ƒCƒ“ƒfƒbƒNƒXî•ñ‚ğì¬‚µ‚ÄAGPU‚É“]‘—‚·‚é
    CreateIndexBuffer(device, model, &indexBuffer);

    //  ’¸“_ƒVƒF[ƒ_[‚ğ¶¬‚·‚é
    ComPtr<ID3DBlob> compiledVS = CreateVertexShader(device,&verteShader);

    //  ƒsƒNƒZƒ‹ƒVƒF[ƒ_[‚ğ¶¬‚·‚é
    ComPtr<ID3DBlob> compiledPS = CreatePixelShader(device, &pixelShader);

    //  ’¸“_ƒCƒ“ƒvƒbƒgƒŒƒCƒAƒEƒg‚ğ¶¬‚·‚é
    CreateInputLayout(device, compiledVS.Get(), &inputLayout);

    //  ’è”ƒoƒbƒtƒ@‚ğ¶¬‚·‚é
    CreateConstantBuffer(m_deviceResources->GetD3DDevice(), &constantBuffer);

    device;
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
    //  ‰æ–ÊƒTƒCƒY‚ğæ“¾
    auto size = m_deviceResources->GetOutputSize();

    //  ƒJƒƒ‰‚Ìİ’è
    XMVECTOR eyePosition = XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f); //  ‹“_‚ÍŒ´“_‚©‚çZ²•‰•ûŒü
    XMVECTOR forcusPoint = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);  //  ’‹“_‚ÍŒ´“_
    XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);  //  ã•ûŒü‚ÍY²

    //  ƒrƒ…[s—ñ‚ğ¶¬
    m_view = XMMatrixLookAtLH(eyePosition,forcusPoint,upDirection);

    //  ƒvƒƒWƒFƒNƒVƒ‡ƒ“s—ñ‚ğ¶¬
    float fovAngleY =  XMConvertToRadians( 45.0f );           // ‚’¼•ûŒü‚Ì‹–ìŠpiƒ‰ƒWƒAƒ“’PˆÊj
    float aspectRatio = size.right / size.bottom;             // ƒAƒXƒyƒNƒg”ä
    float nearZ = 0.01f;                                      // ‹ßƒNƒŠƒbƒv–Ê
    float farZ = 100.0f;                                      // ‰“ƒNƒŠƒbƒv–Ê

    m_projection = DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
    vertexBuffer->Release();
    indexBuffer->Release();
    verteShader->Release();
    pixelShader->Release();
    inputLayout->Release();
    constantBuffer->Release();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
