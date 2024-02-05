//
// Game.cpp
//
#pragma once

#include "Common/pch.h"
#include "Game.h"
#include <codecvt>
#include <locale>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include "LightUIDebugView.h"
#include "TankModel.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Camera* camera;

ID3D11VertexShader* verteShader = nullptr;
ID3D11PixelShader* pixelShader = nullptr;
ID3D11InputLayout* inputLayout = nullptr;
ID3D11Buffer* vsConstantBuffer = nullptr;
ID3D11Buffer* psConstantBuffer = nullptr;
ID3D11SamplerState* samplerState = nullptr;

ComPtr<IDWriteTextFormat> textFormat;
ComPtr<ID2D1Factory> d2dFactory;

//  �`�悷��e�L�X�g
std::wstring cameraInfoStr;

ModelManager* modelManager;

PsConstantBuffer psBufferData;

IComponentUIDebugView* cameraTransformView;
IComponentUIDebugView* lightTransformView;

TankModel* tankModel;

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

    //  �J�����̏�����
    camera = new Camera(XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));

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

    cameraTransformView = new TransformUIDebugView(camera->GetTransform());
    lightTransformView = new LightUIDebugView(psBufferData);

    psBufferData.LightDirection = DirectX::XMFLOAT3(.0f, -30.0f, 10.0f); // ��: X������
    psBufferData.LightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);     // ���F
    psBufferData.LightIntensity = 1.0f;                                 // ���x

    //  Direct2D�̏�����
    //  DirectWrite��Direct2D�̃t�@�N�g�����쐬
    Microsoft::WRL::ComPtr<IDWriteFactory> writeFactory;
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &writeFactory);

    HRESULT hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        __uuidof(ID2D1Factory),
        nullptr,  // �I�v�V�����ł���D2D1_FACTORY_OPTIONS�\���̂ւ̃|�C���^�inull�̏ꍇ�̓f�t�H���g�I�v�V�����j
        reinterpret_cast<void**>(d2dFactory.GetAddressOf())
    );
    if (FAILED(hr)) {
        // �G���[����
    }

    // �e�L�X�g�t�H�[�}�b�g���쐬
    hr = writeFactory->CreateTextFormat(
        L"Segoe UI",                // �t�H���g�t�@�~���[��
        nullptr,                    // �t�H���g�R���N�V�����inullptr = �V�X�e���t�H���g�R���N�V�����j
        DWRITE_FONT_WEIGHT_NORMAL,  // �t�H���g�̑���
        DWRITE_FONT_STYLE_NORMAL,   // �t�H���g�X�^�C��
        DWRITE_FONT_STRETCH_NORMAL, // �t�H���g�̐L�k
        20.0f,                      // �t�H���g�T�C�Y
        L"en-us",                   // ���P�[��
        &textFormat);

    if (FAILED(hr)) {
        // �G���[����
    }
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}


void UpdateCameraTransform(float moveSpeed,float rotateSpeed, float elapsedTime,XMMATRIX& view) {
    //  ���ʕ������v�Z���Đ��K��
    XMVECTOR forward = camera->GetTransform().GetForward();
    XMVECTOR right = camera->GetTransform().GetRight();

    //  ���݂̃J�����̈ʒu���擾
    XMVECTOR cameraPos = camera->GetTransform().GetPosition();
    XMVECTOR cameraRotation = camera->GetTransform().GetDegreesRotation();

    if (GetAsyncKeyState('W') & 0x8000) {
        cameraPos += forward * elapsedTime * moveSpeed;
    }
    if (GetAsyncKeyState('S') & 0x8000) {
        cameraPos -= forward * elapsedTime * moveSpeed;
    }
    if (GetAsyncKeyState('D') & 0x8000) {
        cameraPos -= right * elapsedTime * moveSpeed;
    }
    if (GetAsyncKeyState('A') & 0x8000) {
        cameraPos += right * elapsedTime * moveSpeed;
    }
    if (GetAsyncKeyState('Q') & 0x8000) {
        cameraPos -= XMVector3Cross(forward,right) * elapsedTime * moveSpeed;
    }
    if (GetAsyncKeyState('E') & 0x8000) {
        cameraPos += XMVector3Cross(forward,right) * elapsedTime * moveSpeed;
    }

    camera->GetTransform().SetPosition(cameraPos);

    //  ����������ύX���鏈����ǉ�
    if (GetAsyncKeyState(VK_UP) & 0x8000) {
        cameraRotation += XMVectorSet(-elapsedTime * rotateSpeed, 0.0f, 0.0f, 0.0f);
    }
    if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
        cameraRotation += XMVectorSet(elapsedTime * rotateSpeed, 0.0f, 0.0f, 0.0f);
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        cameraRotation += XMVectorSet(0.0f,elapsedTime * rotateSpeed,  0.0f, 0.0f);
    }
    if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
        cameraRotation += XMVectorSet(0.0f,-elapsedTime * rotateSpeed,  0.0f, 0.0f);
    }
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        cameraRotation += XMVectorSet(0.0f, 0.0f, elapsedTime * rotateSpeed, 0.0f);
    }
    if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
        cameraRotation += XMVectorSet(0.0f, 0.0f, -elapsedTime * rotateSpeed, 0.0f);
    }

    camera->GetTransform().SetDegreesRotation(XMVectorGetX(cameraRotation), XMVectorGetY(cameraRotation), XMVectorGetZ(cameraRotation));

    camera->GetViewMatrix(view);
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    //UpdateCameraTransform(5.0f, 90.0f, elapsedTime, m_view);

    //  �����J�����̍X�V
    camera->GetViewMatrix(m_view);

    //  �J�����̍X�V�������W���e�L�X�g�Ƃ��Ď擾
    cameraInfoStr = camera->GetTransform().GetInfoToWString(3);

    //  ���f���̍X�V
    modelManager->UpdateAll();

    //  TankModel�̍X�V
    tankModel->Update(elapsedTime);

    elapsedTime;
}

#pragma endregion

// wchar_t�������char������ɕϊ�
std::string wcharToChar(const wchar_t* wcharString)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wcharString);
}

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

    // Start the Dear ImGui frame
    ImVec2 imvec2 = ImVec2((float)m_deviceResources->GetOutputSize().right, (float)m_deviceResources->GetOutputSize().bottom);
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Properties");
    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
        cameraTransformView->ComponentUIRender();
    }
    if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
        lightTransformView->ComponentUIRender();
    }
    modelManager->DrawUIAll();
    ImGui::End();

    // Rendering
    ImGui::Render();
    ImDrawData* pDrawData = ImGui::GetDrawData();
    ImGui_ImplDX11_RenderDrawData(pDrawData);

    // TODO: Add your rendering code here.
    context->VSSetShader(verteShader,nullptr,0);
    context->VSSetConstantBuffers(0, 1, &vsConstantBuffer); //  ���_�V�F�[�_�[�p�̒萔�o�b�t�@��ݒ肷��
    context->PSSetShader(pixelShader,nullptr,0);
    context->PSSetConstantBuffers(1, 1, &psConstantBuffer); //  �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@��ݒ肷��
    context->IASetInputLayout(inputLayout);

    //  �e�N�X�`���T���v���[��ݒ肷��
    context->PSSetSamplers(0, 1, &samplerState);


    //  ���C�g�̐ݒ�
    context->UpdateSubresource(psConstantBuffer, 0, nullptr, &psBufferData, 0, 0);

    //  �R���X�^���g�o�b�t�@��ݒ肷��
    //  GPU�p�̍s��ɕϊ����AXMMATRIX��XMFLOAT4X4�ɕϊ�����
    //  ��ɃJ��������ݒ�
    VsConstantBuffer cb;
    XMStoreFloat4x4(&cb.view, XMMatrixTranspose(m_view));
    XMStoreFloat4x4(&cb.projection, XMMatrixTranspose(m_projection));

    //  ���_�V�F�[�_�[�p�̒萔�o�b�t�@�̓��f���̍��W�������Ă���̂ŁA�`�悷�郂�f�����ƂɍX�V����K�v������
    modelManager->DrawAll(*context, cb, *vsConstantBuffer);

    m_deviceResources->PIXEndEvent();

    //// DeviceResources �� SwapChain �̃o�b�N�o�b�t�@���擾
    //Microsoft::WRL::ComPtr<IDXGISurface> backBuffer;
    //HRESULT hr = m_deviceResources->GetSwapChain()->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

    //// Direct2D �����_�[�^�[�Q�b�g�v���p�e�B��ݒ�
    //D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
    //    D2D1_RENDER_TARGET_TYPE_DEFAULT,
    //    D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
    //    0,
    //    0);

    //// Direct2D �����_�[�^�[�Q�b�g���쐬
    //Microsoft::WRL::ComPtr<ID2D1RenderTarget> d2dRenderTarget;
    //hr = d2dFactory->CreateDxgiSurfaceRenderTarget(backBuffer.Get(), &props, &d2dRenderTarget);

    ////  �e�L�X�g�p�̃u���V���쐬
    //ComPtr<ID2D1SolidColorBrush> textColorBrush;
    //D2D1_COLOR_F textColor = D2D1::ColorF(D2D1::ColorF::Black,1.f);
    //d2dRenderTarget->CreateSolidColorBrush(textColor, &textColorBrush);

    ////  CameraPosition�p�̔w�i�F���쐬
    //ComPtr<ID2D1SolidColorBrush> positionBackColorBrush;
    //D2D1_COLOR_F positionBackColor = D2D1::ColorF(D2D1::ColorF::Gray,0.8f);
    //d2dRenderTarget->CreateSolidColorBrush(positionBackColor, &positionBackColorBrush);

    //// �`��J�n
    //d2dRenderTarget->BeginDraw();

    ////  �\���p�̈���`
    //D2D_RECT_F positionInfoRect = D2D1::RectF(20, 20, 330, 100);

    ////  �J�����ʒu����`��
    ////  �w�i��`��
    //d2dRenderTarget->FillRectangle(positionInfoRect, positionBackColorBrush.Get());
    ////  �e�L�X�g��`��
    //d2dRenderTarget->DrawText(
    //    cameraInfoStr.c_str(),         // �`�悷��e�L�X�g
    //    static_cast<UINT32>(cameraInfoStr.length()), // �e�L�X�g�̒���
    //    textFormat.Get(),           // �e�L�X�g�t�H�[�}�b�g
    //    positionInfoRect,               // �`�悷��̈�
    //    textColorBrush.Get());            // �u���V

    //// �`��I��
    //hr = d2dRenderTarget->EndDraw();

    //if (FAILED(hr)) {
    //    // �G���[����
    //    return;
    //}


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
    width = 1920;
    height = 1080;
}
#pragma endregion

/// <summary>
/// ���_�V�F�[�_�𐶐�
/// </summary>
ComPtr<ID3DBlob> CreateVertexShader(ID3D11Device* device, ID3D11VertexShader** createdShader) {

    //  ���_�V�F�[�_�[��ǂݍ��݃R���p�C������
    ComPtr<ID3DBlob> compiledVS;
    D3DCompileFromFile(L"Shader/VertexShader.hlsl",nullptr,nullptr,"main","vs_5_0",0,0,&compiledVS,nullptr);

    //  ���_�V�F�[�_�[�𐶐�����
    device->CreateVertexShader(compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), nullptr, createdShader);

    return compiledVS;
}

/// <summary>
/// �s�N�Z���V�F�[�_�𐶐�����
/// </summary>
ComPtr<ID3DBlob> CreatePixelShader(ID3D11Device* device, ID3D11PixelShader** createdShader) {
    //  �s�N�Z���V�F�[�_�[��ǂݍ��݃R���p�C������
    ComPtr<ID3DBlob> compiledPS;
    D3DCompileFromFile(L"Shader/PixelShader.hlsl", nullptr, nullptr, "main", "ps_5_0", 0, 0, &compiledPS, nullptr);

    //  �s�N�Z���V�F�[�_�[�𐶐�����
    device->CreatePixelShader(compiledPS->GetBufferPointer(), compiledPS->GetBufferSize(), nullptr, &pixelShader);

    return compiledPS;
}

/// <summary>
/// ���_�C���v�b�g���C�A�E�g�𐶐�����
/// </summary>
void CreateInputLayout(ID3D11Device* device, ID3DBlob* compiledVS, ID3D11InputLayout** createdLayout) {
    //  ���_�C���v�b�g���C�A�E�g�𐶐�
    std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
        { "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
        { "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
        { "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0}
    };

    //  ���_�C���v�b�g���C�A�E�g�𐶐�����
    //  �V�F�[�_�ƃ��C�A�E�g�̌^�̌݊�����S�ۂ���K�v������
    device->CreateInputLayout(&layout[0], layout.size(), compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), &inputLayout);
}

/// <summary>
/// ���_�萔�o�b�t�@�𐶐�����
/// </summary>
void CreateVSConstantBuffer(ID3D11Device* device, ID3D11Buffer** createdBuffer) {
    //  �萔�o�b�t�@�𐶐�����
    D3D11_BUFFER_DESC constantBufferDesc = {
        sizeof(VsConstantBuffer),
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_CONSTANT_BUFFER,
        0,0,0
    };

    HRESULT result = device->CreateBuffer(&constantBufferDesc, nullptr, createdBuffer);
}

/// <summary>
/// �s�N�Z���萔�o�b�t�@�𐶐�����
/// </summary>
void CreatePSConstantBuffer(ID3D11Device* device, ID3D11Buffer** createdBuffer) {
    //  �萔�o�b�t�@�𐶐�����
    D3D11_BUFFER_DESC constantBufferDesc = {
        sizeof(PsConstantBuffer),
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

    modelManager = new ModelManager(*device, *m_deviceResources->GetD3DDeviceContext());
    vector<Model*>* models = modelManager->CreateModelFromObj("Models/TankO.obj");
    tankModel = new TankModel(models, camera);
    modelManager->CreateModelFromObj("Models/Map.obj");

    //  ���_�V�F�[�_�[�𐶐�����
    ComPtr<ID3DBlob> compiledVS = CreateVertexShader(device,&verteShader);

    //  �s�N�Z���V�F�[�_�[�𐶐�����
    ComPtr<ID3DBlob> compiledPS = CreatePixelShader(device, &pixelShader);

    //  ���_�C���v�b�g���C�A�E�g�𐶐�����
    CreateInputLayout(device, compiledVS.Get(), &inputLayout);

    //  ���_�萔�o�b�t�@�𐶐�����
    CreateVSConstantBuffer(m_deviceResources->GetD3DDevice(), &vsConstantBuffer);
    //  �s�N�Z���萔�o�b�t�@�𐶐�����
    CreatePSConstantBuffer(m_deviceResources->GetD3DDevice(), &psConstantBuffer);

    //  �e�N�X�`���T���v���[�̐���
    samplerState = nullptr;
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT hr = device->CreateSamplerState(&sampDesc, &samplerState);
    if (FAILED(hr)) {
        // �G���[����
    }

    device;
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
    //  ��ʃT�C�Y���擾
    auto size = m_deviceResources->GetOutputSize();

    //  �J�����̐ݒ�
    XMVECTOR eyePosition = XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f); //  ���_�͌��_����Z��������
    XMVECTOR forcusPoint = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);  //  �����_�͌��_
    XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);  //  �������Y��

    //  �r���[�s��𐶐�
    m_view = XMMatrixLookAtLH(eyePosition,forcusPoint,upDirection);

    //  �v���W�F�N�V�����s��𐶐�
    float fovAngleY =  XMConvertToRadians( 45.0f );           // ���������̎���p�i���W�A���P�ʁj
    float aspectRatio = static_cast<float>(size.right) / static_cast<float>(size.bottom);             // �A�X�y�N�g��
    float nearZ = 0.01f;                                      // �߃N���b�v��
    float farZ = 100.0f;                                      // ���N���b�v��

    m_projection = DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);

    //  ImGui�̏�����
    if (ImGui::GetCurrentContext() == nullptr) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        bool result = ImGui_ImplWin32_Init(m_deviceResources->GetWindow());
        result = ImGui_ImplDX11_Init(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());

        ImGui::StyleColorsDark();   //  �_�[�N�e�[�}���g�p
    }
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
    verteShader->Release();
    pixelShader->Release();
    inputLayout->Release();
    vsConstantBuffer->Release();
    psConstantBuffer->Release();
    samplerState->Release();

    //  ImGui�̉��
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
