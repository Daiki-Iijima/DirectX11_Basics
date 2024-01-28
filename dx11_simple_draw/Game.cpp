//
// Game.cpp
//

#include "Common/pch.h"
#include "Game.h"
#include <codecvt>
#include <locale>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include "TransformDebugView.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Camera* camera;

ID3D11VertexShader* verteShader = nullptr;
ID3D11PixelShader* pixelShader = nullptr;
ID3D11InputLayout* inputLayout = nullptr;
ID3D11Buffer* constantBuffer = nullptr;
ID3D11SamplerState* samplerState = nullptr;

ComPtr<IDWriteTextFormat> textFormat;
ComPtr<ID2D1Factory> d2dFactory;

//  描画するテキスト
std::wstring cameraInfoStr;

ModelManager* modelManager;

Model* cube1;
TransformDebugView* cameraTransformView;

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

    //  カメラの初期化
    camera = new Camera(XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
    cameraTransformView = new TransformDebugView(camera->GetTransform());

    //  Direct2Dの初期化
    //  DirectWriteとDirect2Dのファクトリを作成
    Microsoft::WRL::ComPtr<IDWriteFactory> writeFactory;
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &writeFactory);

    HRESULT hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        __uuidof(ID2D1Factory),
        nullptr,  // オプションであるD2D1_FACTORY_OPTIONS構造体へのポインタ（nullの場合はデフォルトオプション）
        reinterpret_cast<void**>(d2dFactory.GetAddressOf())
    );
    if (FAILED(hr)) {
        // エラー処理
    }

    // テキストフォーマットを作成
    hr = writeFactory->CreateTextFormat(
        L"Segoe UI",                // フォントファミリー名
        nullptr,                    // フォントコレクション（nullptr = システムフォントコレクション）
        DWRITE_FONT_WEIGHT_NORMAL,  // フォントの太さ
        DWRITE_FONT_STYLE_NORMAL,   // フォントスタイル
        DWRITE_FONT_STRETCH_NORMAL, // フォントの伸縮
        20.0f,                      // フォントサイズ
        L"en-us",                   // ロケール
        &textFormat);

    if (FAILED(hr)) {
        // エラー処理
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
    //  正面方向を計算して正規化
    XMVECTOR forward = camera->GetTransform().GetForward();
    XMVECTOR right = camera->GetTransform().GetRight();

    //  現在のカメラの位置を取得
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

    //  視線方向を変更する処理を追加
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

    UpdateCameraTransform(5.0f, 90.0f, elapsedTime, m_view);

    //  カメラの更新した座標をテキストとして取得
    cameraInfoStr = camera->GetTransform().GetInfoToWString(3);

    //  Cubeの移動
    //XMVECTOR cube1Pos = cube1->GetTransform().GetPosition();
    //float x = XMVectorGetX(cube1Pos) - 1.8f * elapsedTime;
    //cube1->GetTransform().SetPosition(XMVectorSet(x, 0.0f, 0.0f, 0.0f));

    //if (XMVectorGetX(cube1->GetTransform().GetPosition()) < -10.0f) {
    //    cube1->GetTransform().SetPosition(XMVectorSet(4, 0.0f, 0.0f, 0.0f));
    //}

    //  モデルの更新
    modelManager->UpdateAll();

    elapsedTime;
}

#pragma endregion

// wchar_t文字列をchar文字列に変換
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

    // TODO: Add your rendering code here.
    context->VSSetShader(verteShader,nullptr,0);
    context->PSSetShader(pixelShader,nullptr,0);
    context->IASetInputLayout(inputLayout);
    //  定数バッファを設定する
    context->VSSetConstantBuffers(0,1,&constantBuffer);

    //  テクスチャサンプラーを設定する
    context->PSSetSamplers(0, 1, &samplerState);

    //  コンスタントバッファを設定する
    //  GPU用の行列に変換しつつ、XMMATRIXをXMFLOAT4X4に変換する
    //  先にカメラ情報を設定
    ConstantBuffer cb;
    XMStoreFloat4x4(&cb.view, XMMatrixTranspose(m_view));
    XMStoreFloat4x4(&cb.projection, XMMatrixTranspose(m_projection));

    modelManager->DrawAll(*context, cb, *constantBuffer);

    m_deviceResources->PIXEndEvent();

    //// DeviceResources で SwapChain のバックバッファを取得
    //Microsoft::WRL::ComPtr<IDXGISurface> backBuffer;
    //HRESULT hr = m_deviceResources->GetSwapChain()->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

    //// Direct2D レンダーターゲットプロパティを設定
    //D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
    //    D2D1_RENDER_TARGET_TYPE_DEFAULT,
    //    D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
    //    0,
    //    0);

    //// Direct2D レンダーターゲットを作成
    //Microsoft::WRL::ComPtr<ID2D1RenderTarget> d2dRenderTarget;
    //hr = d2dFactory->CreateDxgiSurfaceRenderTarget(backBuffer.Get(), &props, &d2dRenderTarget);

    ////  テキスト用のブラシを作成
    //ComPtr<ID2D1SolidColorBrush> textColorBrush;
    //D2D1_COLOR_F textColor = D2D1::ColorF(D2D1::ColorF::Black,1.f);
    //d2dRenderTarget->CreateSolidColorBrush(textColor, &textColorBrush);

    ////  CameraPosition用の背景色を作成
    //ComPtr<ID2D1SolidColorBrush> positionBackColorBrush;
    //D2D1_COLOR_F positionBackColor = D2D1::ColorF(D2D1::ColorF::Gray,0.8f);
    //d2dRenderTarget->CreateSolidColorBrush(positionBackColor, &positionBackColorBrush);

    //// 描画開始
    //d2dRenderTarget->BeginDraw();

    ////  表示用領域を定義
    //D2D_RECT_F positionInfoRect = D2D1::RectF(20, 20, 330, 100);

    ////  カメラ位置情報を描画
    ////  背景を描画
    //d2dRenderTarget->FillRectangle(positionInfoRect, positionBackColorBrush.Get());
    ////  テキストを描画
    //d2dRenderTarget->DrawText(
    //    cameraInfoStr.c_str(),         // 描画するテキスト
    //    static_cast<UINT32>(cameraInfoStr.length()), // テキストの長さ
    //    textFormat.Get(),           // テキストフォーマット
    //    positionInfoRect,               // 描画する領域
    //    textColorBrush.Get());            // ブラシ

    //// 描画終了
    //hr = d2dRenderTarget->EndDraw();

    //if (FAILED(hr)) {
    //    // エラー処理
    //    return;
    //}

    // Start the Dear ImGui frame
    ImVec2 imvec2 = ImVec2((float)m_deviceResources->GetOutputSize().right, (float)m_deviceResources->GetOutputSize().bottom);
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Properties");
    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
        cameraTransformView->RenderComponent();
    }
    modelManager->DrawUIAll();
    ImGui::End();

    // Rendering
    ImGui::Render();
    ImDrawData* pDrawData = ImGui::GetDrawData();
    ImGui_ImplDX11_RenderDrawData(pDrawData);

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
/// 頂点シェーダを生成
/// </summary>
ComPtr<ID3DBlob> CreateVertexShader(ID3D11Device* device, ID3D11VertexShader** createdShader) {

    //  頂点シェーダーを読み込みコンパイルする
    ComPtr<ID3DBlob> compiledVS;
    D3DCompileFromFile(L"Shader/VertexShader.hlsl",nullptr,nullptr,"main","vs_5_0",0,0,&compiledVS,nullptr);

    //  頂点シェーダーを生成する
    device->CreateVertexShader(compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), nullptr, createdShader);

    return compiledVS;
}

/// <summary>
/// ピクセルシェーダを生成する
/// </summary>
ComPtr<ID3DBlob> CreatePixelShader(ID3D11Device* device, ID3D11PixelShader** createdShader) {
    //  ピクセルシェーダーを読み込みコンパイルする
    ComPtr<ID3DBlob> compiledPS;
    D3DCompileFromFile(L"Shader/PixelShader.hlsl", nullptr, nullptr, "main", "ps_5_0", 0, 0, &compiledPS, nullptr);

    //  ピクセルシェーダーを生成する
    device->CreatePixelShader(compiledPS->GetBufferPointer(), compiledPS->GetBufferSize(), nullptr, &pixelShader);

    return compiledPS;
}

/// <summary>
/// 頂点インプットレイアウトを生成する
/// </summary>
void CreateInputLayout(ID3D11Device* device, ID3DBlob* compiledVS, ID3D11InputLayout** createdLayout) {
    //  頂点インプットレイアウトを生成
    std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
        { "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
        { "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
        { "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0}
    };

    //  頂点インプットレイアウトを生成する
    //  シェーダとレイアウトの型の互換性を担保する必要がある
    device->CreateInputLayout(&layout[0], layout.size(), compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), &inputLayout);
}

/// <summary>
/// 定数バッファを生成する
/// </summary>
void CreateConstantBuffer(ID3D11Device* device, ID3D11Buffer** createdBuffer) {
    //  定数バッファを生成する
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

    modelManager = new ModelManager(*device, *m_deviceResources->GetD3DDeviceContext());
    modelManager->AddModel("Models/TankO.obj");

    //  頂点シェーダーを生成する
    ComPtr<ID3DBlob> compiledVS = CreateVertexShader(device,&verteShader);

    //  ピクセルシェーダーを生成する
    ComPtr<ID3DBlob> compiledPS = CreatePixelShader(device, &pixelShader);

    //  頂点インプットレイアウトを生成する
    CreateInputLayout(device, compiledVS.Get(), &inputLayout);

    //  定数バッファを生成する
    CreateConstantBuffer(m_deviceResources->GetD3DDevice(), &constantBuffer);

    //  テクスチャサンプラーの生成
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
        // エラー処理
    }

    device;
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
    //  画面サイズを取得
    auto size = m_deviceResources->GetOutputSize();

    //  カメラの設定
    XMVECTOR eyePosition = XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f); //  視点は原点からZ軸負方向
    XMVECTOR forcusPoint = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);  //  注視点は原点
    XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);  //  上方向はY軸

    //  ビュー行列を生成
    m_view = XMMatrixLookAtLH(eyePosition,forcusPoint,upDirection);

    //  プロジェクション行列を生成
    float fovAngleY =  XMConvertToRadians( 45.0f );           // 垂直方向の視野角（ラジアン単位）
    float aspectRatio = static_cast<float>(size.right) / static_cast<float>(size.bottom);             // アスペクト比
    float nearZ = 0.01f;                                      // 近クリップ面
    float farZ = 100.0f;                                      // 遠クリップ面

    m_projection = DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);

    //  ImGuiの初期化
    if (ImGui::GetCurrentContext() == nullptr) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        bool result = ImGui_ImplWin32_Init(m_deviceResources->GetWindow());
        result = ImGui_ImplDX11_Init(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());

        ImGui::StyleColorsDark();   //  ダークテーマを使用
    }
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
    verteShader->Release();
    pixelShader->Release();
    inputLayout->Release();
    constantBuffer->Release();

    //  ImGuiの解放
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
