#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
public:
    Camera(XMVECTOR position, XMVECTOR degressRotation);

    //  Setter
    //  位置
    void SetPosition(XMVECTOR position);
    void SetPosition(float x, float y, float z);
    //  角度
    void SetDegressRotation(float x, float y, float z);
    void SetRadianRotation(float x, float y, float z);

    //  ビュー行列
    void GetViewMatrix(XMMATRIX& viewMatrix);

    //  Getter
    //  位置
    XMVECTOR GetPosition() const;
    //  角度
    XMVECTOR GetDegressRotation() const;
    XMVECTOR GetRadianRotation() const;
    //  基底ベクトル
    XMVECTOR GetForward();
    XMVECTOR GetRight();
    XMVECTOR GetUp();

private:
    XMVECTOR m_position;       //  位置

    //  角度
    XMVECTOR m_degressRotation;   //  度数
    XMVECTOR m_radianRotation;    //  弧度
};

