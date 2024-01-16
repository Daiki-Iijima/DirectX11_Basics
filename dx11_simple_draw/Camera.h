#pragma once
#include <DirectXMath.h>
#include "Transform.h"

using namespace DirectX;

class Camera
{
public:
    Camera(XMVECTOR position, XMVECTOR degressRotation);

    //  �r���[�s��
    void GetViewMatrix(XMMATRIX& viewMatrix);

    Transform& GetTransform();

private:

    Transform m_transform;
};

