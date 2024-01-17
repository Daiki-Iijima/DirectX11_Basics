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

    //  �C���f�b�N�X�o�b�t�@�̐�
    int IndiceCount;

    //  === CPU������ ===
    //  VertexBuffer�̐������ł����������Ă������C������
    std::vector<Vertex> vertices;
    std::vector<unsigned short> indices;

    //  === GPU������ ===
    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;

    //  Buffer�̐���
    HRESULT CreateBuffers(ID3D11Device& device);

private:
    //  Buffer�̐���
    HRESULT CreateVertexBuffer(ID3D11Device& device);
    HRESULT CreateIndexBuffer(ID3D11Device& device);
};