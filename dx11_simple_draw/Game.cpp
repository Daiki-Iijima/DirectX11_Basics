//
// Game.cpp
//

#include "Common/pch.h"
#include "Game.h"

#pragma comment(lib,"d3dcompiler.lib")
#include <d3dcompiler.h>
#include <iostream>
#include <Camera.h>

#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")
#include<shtypes.h>
#include<wingdi.h>
#include<d2d1.h>
#include<dwrite.h>
#include "Model.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

struct ConstantBuffer {
    XMFLOAT4X4 world;
    XMFLOAT4X4 view;
    XMFLOAT4X4 projection;
};

Model* model;
//  ���[���h���W�s��
DirectX::XMMATRIX modelWorldMatrix;

Model* model1;
//  ���[���h���W�s��
DirectX::XMMATRIX model1WorldMatrix;

Camera* camera;

ID3D11VertexShader* verteShader = nullptr;
ID3D11PixelShader* pixelShader = nullptr;
ID3D11InputLayout* inputLayout = nullptr;

ID3D11Buffer* constantBuffer = nullptr;

ComPtr<IDWriteTextFormat> textFormat;
ComPtr<ID2D1Factory> d2dFactory;

//  �`�悷��e�L�X�g
std::wstring cameraInfoStr;

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

    //  ���[���h���W��ݒ肷��
    modelWorldMatrix = XMMatrixTranslation(0.f, -1.5f, 5.f);
    model1WorldMatrix = XMMatrixTranslation(2.f, 0.f, 5.f);
    //  �J�����̏�����
    camera = new Camera(XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));

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
    XMVECTOR cameraRotation = camera->GetTransform().GetDegressRotation();

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

    camera->GetTransform().SetDegressRotation(XMVectorGetX(cameraRotation), XMVectorGetY(cameraRotation), XMVectorGetZ(cameraRotation));

    camera->GetViewMatrix(view);
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    UpdateCameraTransform(5.0f, 90.0f, elapsedTime, m_view);

    //  �J�����̍X�V�������W���e�L�X�g�Ƃ��Ď擾
    cameraInfoStr = camera->GetTransform().GetInfoToWString(3);

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


    //  �R���X�^���g�o�b�t�@��ݒ肷��
    ConstantBuffer cb;
    //  GPU�p�̍s��ɕϊ����AXMMATRIX��XMFLOAT4X4�ɕϊ�����
    XMStoreFloat4x4(&cb.world, XMMatrixTranspose(modelWorldMatrix));
    XMStoreFloat4x4(&cb.view, XMMatrixTranspose(m_view));
    XMStoreFloat4x4(&cb.projection, XMMatrixTranspose(m_projection));
    context->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);

    //  �萔�o�b�t�@��ݒ肷��
    context->VSSetConstantBuffers(0,1,&constantBuffer);

    //  �Ƃ肠�����󂯎��
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    
    context->IASetVertexBuffers(0,1, model->vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(model->indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->DrawIndexed(model->IndiceCount, 0, 0);

    //  �R���X�^���g�o�b�t�@��ݒ肷��
    //  GPU�p�̍s��ɕϊ����AXMMATRIX��XMFLOAT4X4�ɕϊ�����
    XMStoreFloat4x4(&cb.world, XMMatrixTranspose(model1WorldMatrix));
    XMStoreFloat4x4(&cb.view, XMMatrixTranspose(m_view));
    XMStoreFloat4x4(&cb.projection, XMMatrixTranspose(m_projection));
    context->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);

    //  �萔�o�b�t�@��ݒ肷��
    context->VSSetConstantBuffers(0,1,&constantBuffer);
    
    context->IASetVertexBuffers(0,1, model1->vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(model1->indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->DrawIndexed(model1->IndiceCount, 0, 0);
    
    context;

    m_deviceResources->PIXEndEvent();


    // DeviceResources �� SwapChain �̃o�b�N�o�b�t�@���擾
    Microsoft::WRL::ComPtr<IDXGISurface> backBuffer;
    HRESULT hr = m_deviceResources->GetSwapChain()->GetBuffer(0, IID_PPV_ARGS(&backBuffer));


    // Direct2D �����_�[�^�[�Q�b�g�v���p�e�B��ݒ�
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        0,
        0);

    // Direct2D �����_�[�^�[�Q�b�g���쐬
    Microsoft::WRL::ComPtr<ID2D1RenderTarget> d2dRenderTarget;
    hr = d2dFactory->CreateDxgiSurfaceRenderTarget(backBuffer.Get(), &props, &d2dRenderTarget);

    //  �e�L�X�g�p�̃u���V���쐬
    ComPtr<ID2D1SolidColorBrush> textColorBrush;
    D2D1_COLOR_F textColor = D2D1::ColorF(D2D1::ColorF::Black,1.f);
    d2dRenderTarget->CreateSolidColorBrush(textColor, &textColorBrush);

    //  CameraPosition�p�̔w�i�F���쐬
    ComPtr<ID2D1SolidColorBrush> positionBackColorBrush;
    D2D1_COLOR_F positionBackColor = D2D1::ColorF(D2D1::ColorF::Gray,0.8f);
    d2dRenderTarget->CreateSolidColorBrush(positionBackColor, &positionBackColorBrush);

    // �`��J�n
    d2dRenderTarget->BeginDraw();

    //  �\���p�̈���`
    D2D_RECT_F positionInfoRect = D2D1::RectF(20, 20, 330, 100);

    //  �J�����ʒu����`��
    //  �w�i��`��
    d2dRenderTarget->FillRectangle(positionInfoRect, positionBackColorBrush.Get());
    //  �e�L�X�g��`��
    d2dRenderTarget->DrawText(
        cameraInfoStr.c_str(),         // �`�悷��e�L�X�g
        static_cast<UINT32>(cameraInfoStr.length()), // �e�L�X�g�̒���
        textFormat.Get(),           // �e�L�X�g�t�H�[�}�b�g
        positionInfoRect,               // �`�悷��̈�
        textColorBrush.Get());            // �u���V

    // �`��I��
    hr = d2dRenderTarget->EndDraw();

    if (FAILED(hr)) {
        // �G���[����
        return;
    }

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
        { "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}
    };

    //  ���_�C���v�b�g���C�A�E�g�𐶐�����
    //  �V�F�[�_�ƃ��C�A�E�g�̌^�̌݊�����S�ۂ���K�v������
    device->CreateInputLayout(&layout[0], layout.size(), compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), &inputLayout);
}

/// <summary>
/// �萔�o�b�t�@�𐶐�����
/// </summary>
void CreateConstantBuffer(ID3D11Device* device, ID3D11Buffer** createdBuffer) {
    //  �萔�o�b�t�@�𐶐�����
    D3D11_BUFFER_DESC constantBufferDesc = {
        sizeof(ConstantBuffer),
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_CONSTANT_BUFFER,
        0,0,0
    };

    HRESULT result = device->CreateBuffer(&constantBufferDesc, nullptr, createdBuffer);
}

void LoadModel(Model& distModel,string modelPath) {
    //  ���f���̓ǂݍ���
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        modelPath,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_GenNormals  // �@�����̌v�Z��ǉ�
    );

    if (!scene) {
        // �G���[�n���h�����O
        std::string errorStr = "ERROR: " + std::string(importer.GetErrorString()) + "\n";
        std::wstring errorWStr = std::wstring(errorStr.begin(), errorStr.end());
        OutputDebugString(errorWStr.c_str());
    }

    for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
        aiMesh* mesh = scene->mMeshes[m];

        // ���_���̒��o
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            aiVector3D pos = mesh->mVertices[i];
            aiVector3D normal = mesh->mNormals[i];
            vertex.position = DirectX::XMFLOAT3(pos.x, pos.y, pos.z);
            vertex.normal = DirectX::XMFLOAT3(normal.x, normal.y, normal.z);
            distModel.vertices.push_back(vertex);
        }

        // �C���f�b�N�X���̒��o
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                distModel.indices.push_back(static_cast<unsigned short>(face.mIndices[j]));
            }
        }
    }
}

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    model = new Model();
    model1 = new Model();

    //  ���f���̓ǂݍ���
    LoadModel(*model,"Models/teapot.obj");
    LoadModel(*model1,"Models/skull.obj");
    //  ���f���̃o�b�t�@�𐶐�
    model->CreateBuffers(*device);
    model1->CreateBuffers(*device);

    //  ���_�V�F�[�_�[�𐶐�����
    ComPtr<ID3DBlob> compiledVS = CreateVertexShader(device,&verteShader);

    //  �s�N�Z���V�F�[�_�[�𐶐�����
    ComPtr<ID3DBlob> compiledPS = CreatePixelShader(device, &pixelShader);

    //  ���_�C���v�b�g���C�A�E�g�𐶐�����
    CreateInputLayout(device, compiledVS.Get(), &inputLayout);

    //  �萔�o�b�t�@�𐶐�����
    CreateConstantBuffer(m_deviceResources->GetD3DDevice(), &constantBuffer);

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
    float aspectRatio = size.right / size.bottom;             // �A�X�y�N�g��
    float nearZ = 0.01f;                                      // �߃N���b�v��
    float farZ = 100.0f;                                      // ���N���b�v��

    m_projection = DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
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
