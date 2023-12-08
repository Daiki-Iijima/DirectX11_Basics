//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "vector"

#pragma comment(lib,"d3dcompiler.lib")
#include <d3dcompiler.h>

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

struct Vertex {
    XMFLOAT3 position;
};


struct Model {
    std::vector<Vertex> vertices;
    std::vector<unsigned short> indices;
};

Model CreateTriangle() {
    Model model;
    model.vertices = {
        { XMFLOAT3(0.0f, 0.5f, 0.0f) },   //  ���_ : 0
        { XMFLOAT3(0.5f, -0.5f, 0.0f) },  //  ���_ : 1
        { XMFLOAT3(-0.5f, -0.5f, 0.0f) }, //  ���_ : 2
    };
    model.indices = {
        0,1,2,
    };
    return model;
}

Model model;

ID3D11Buffer* vertexBuffer = nullptr;
ID3D11Buffer* indexBuffer = nullptr;

ID3D11VertexShader* verteShader = nullptr;
ID3D11PixelShader* pixelShader = nullptr;
ID3D11InputLayout* inputLayout = nullptr;

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

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.
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

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0,1,&vertexBuffer,&stride,&offset);
    context->IASetIndexBuffer(indexBuffer,DXGI_FORMAT_R16_UINT,0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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
/// ���_�����쐬����
/// </summary>
void CreateVertexBuffer(ID3D11Device* device,Model& model,ID3D11Buffer** createdBuffer){

    D3D11_BUFFER_DESC vertexBufferDesc = {
        static_cast<UINT>(model.vertices.size() * sizeof(Vertex)),
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_VERTEX_BUFFER,
        0, 0, 0
    };

    D3D11_SUBRESOURCE_DATA vertexSubresourceData = {
        model.vertices.data(), // �C��: ���ۂ̃f�[�^�ւ̃|�C���^
        0, 0
    };

    //   ���_�o�b�t�@�𐶐�����
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

ComPtr<ID3DBlob> CreateVertexShader(ID3D11Device* device, ID3D11VertexShader** createdShader) {

    //  ���_�V�F�[�_�[��ǂݍ��݃R���p�C������
    ComPtr<ID3DBlob> compiledVS;
    D3DCompileFromFile(L"VertexShader.hlsl",nullptr,nullptr,"main","vs_5_0",0,0,&compiledVS,nullptr);

    //  ���_�V�F�[�_�[�𐶐�����
    device->CreateVertexShader(compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), nullptr, createdShader);

    return compiledVS;
}

ComPtr<ID3DBlob> CreatePixelShader(ID3D11Device* device, ID3D11PixelShader** createdShader) {
    //  �s�N�Z���V�F�[�_�[��ǂݍ��݃R���p�C������
    ComPtr<ID3DBlob> compiledPS;
    D3DCompileFromFile(L"PixelShader.hlsl", nullptr, nullptr, "main", "ps_5_0", 0, 0, &compiledPS, nullptr);

    //  �s�N�Z���V�F�[�_�[�𐶐�����
    device->CreatePixelShader(compiledPS->GetBufferPointer(), compiledPS->GetBufferSize(), nullptr, &pixelShader);

    return compiledPS;
}

void CreateInputLayout(ID3D11Device* device, ID3DBlob* compiledVS, ID3D11InputLayout** createdLayout) {
    //  ���_�C���v�b�g���C�A�E�g�𐶐�
    std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
        { "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 }
    };

    //  ���_�C���v�b�g���C�A�E�g�𐶐�����
    device->CreateInputLayout(&layout[0], layout.size(), compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), &inputLayout);
}

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    //  ���f���̏��𐶐�
    model = CreateTriangle();

    //  ���_�����쐬���āAGPU�ɓ]������
    CreateVertexBuffer(device, model, &vertexBuffer);

    //  �C���f�b�N�X�����쐬���āAGPU�ɓ]������
    CreateIndexBuffer(device, model, &indexBuffer);

    //  ���_�V�F�[�_�[�𐶐�����
    ComPtr<ID3DBlob> compiledVS = CreateVertexShader(device,&verteShader);
    //  ���_�C���v�b�g���C�A�E�g�𐶐�����
    CreateInputLayout(device, compiledVS.Get(), &inputLayout);

    //  �s�N�Z���V�F�[�_�[�𐶐�����
    ComPtr<ID3DBlob> compiledPS = CreatePixelShader(device, &pixelShader);


    device;
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
