#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
public:
    Camera(XMVECTOR position, XMVECTOR degressRotation);

    //  Setter
    //  �ʒu
    void SetPosition(XMVECTOR position);
    void SetPosition(float x, float y, float z);
    //  �p�x
    void SetDegressRotation(float x, float y, float z);
    void SetRadianRotation(float x, float y, float z);

    //  �r���[�s��
    void GetViewMatrix(XMMATRIX& viewMatrix);

    //  Getter
    //  �ʒu
    XMVECTOR GetPosition() const;
    //  �p�x
    XMVECTOR GetDegressRotation() const;
    XMVECTOR GetRadianRotation() const;
    //  ���x�N�g��
    XMVECTOR GetForward();
    XMVECTOR GetRight();
    XMVECTOR GetUp();

private:
    XMVECTOR m_position;       //  �ʒu

    //  �p�x
    XMVECTOR m_degressRotation;   //  �x��
    XMVECTOR m_radianRotation;    //  �ʓx
};

