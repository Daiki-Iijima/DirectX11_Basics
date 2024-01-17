#pragma once

#include <DirectXMath.h>

using namespace DirectX;
using Microsoft::WRL::ComPtr;

struct Vertex {
    XMFLOAT3 position;
    XMFLOAT3 normal;
};

class Model
{
public:
    Model();

    //  インデックスバッファの数
    int IndiceCount;

    //  === CPUメモリ ===
    //  VertexBufferの生成ができたら解放してもいい気がする
    std::vector<Vertex> vertices;
    std::vector<unsigned short> indices;

    //  === GPUメモリ ===
    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;

    //  Bufferの生成
    HRESULT CreateBuffers(ID3D11Device& device);

private:
    //  Bufferの生成
    HRESULT CreateVertexBuffer(ID3D11Device& device);
    HRESULT CreateIndexBuffer(ID3D11Device& device);
};