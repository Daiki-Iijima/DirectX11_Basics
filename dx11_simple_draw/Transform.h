#pragma once
#include <DirectXMath.h>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace DirectX;

class Transform
{
public:
    //  コンストラクタ
    Transform();
    Transform(XMVECTOR position, XMVECTOR degreesRotation, XMVECTOR scale);

    //  位置
    void SetPosition(XMVECTOR position);
    void SetPosition(float x, float y, float z);
    //  角度
    void SetDegreesRotation(float x, float y, float z);
    void SetRadianRotation(float x, float y, float z);
    //  スケール
    void SetScale(float x, float y, float z);

    //  Getter
    XMMATRIX GetWorldRotationMatrix();
    //  位置
    XMVECTOR GetPosition() const;
    XMMATRIX GetWorldMatrix();
    //  角度
    XMVECTOR GetDegreesRotation() const;
    XMVECTOR GetRadianRotation() const;
    //  スケール
    XMVECTOR GetScale() const;

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
    XMVECTOR m_degreesRotation;   //  度数
    XMVECTOR m_radianRotation;    //  弧度

    //  スケール    
    XMVECTOR m_scale;
};

