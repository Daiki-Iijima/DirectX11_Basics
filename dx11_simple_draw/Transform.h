#pragma once
#include <DirectXMath.h>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace DirectX;

class Transform
{
public :
    //  コンストラクタ
    Transform();
    Transform(XMVECTOR position, XMVECTOR degressRotation, XMVECTOR scale);

    //  位置
    void SetPosition(XMVECTOR position);
    void SetPosition(float x, float y, float z);
    //  角度
    void SetDegressRotation(float x, float y, float z);
    void SetRadianRotation(float x, float y, float z);

    //  Getter
    XMMATRIX GetWorldRotationMatrix();
    //  位置
    XMVECTOR GetPosition() const;
    XMMATRIX GetWorldMatrix();
    //  角度
    XMVECTOR GetDegressRotation() const;
    XMVECTOR GetRadianRotation() const;
    //  基底ベクトル
    XMVECTOR GetForward();
    XMVECTOR GetRight();
    XMVECTOR GetUp();

    //  テキスト出力
    wstring GetInfoToWString(int precision) const;

private:
    //  位置
    XMVECTOR m_position;

    //  角度
    XMVECTOR m_degressRotation;   //  度数
    XMVECTOR m_radianRotation;    //  弧度

    //  スケール    
    XMVECTOR m_scale;
};

