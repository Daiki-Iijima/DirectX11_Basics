#include "Common/pch.h"
#include "Transform.h"

Transform::Transform() {
    m_position = XMVECTOR();
    m_degreesRotation = XMVECTOR();
    m_radianRotation = XMVECTOR();
    m_scale = XMVectorSet(1.f, 1.f, 1.f, 1.f);
}

Transform::Transform(XMVECTOR position, XMVECTOR degressRotation, XMVECTOR scale) {
    m_position = position;
    m_degreesRotation = degressRotation;
    m_radianRotation = XMVectorSet(XMConvertToRadians(XMVectorGetX(degressRotation)), XMConvertToRadians(XMVectorGetY(degressRotation)), XMConvertToRadians(XMVectorGetZ(degressRotation)), 0.0f);
    m_scale = scale;
}

wstring Transform::GetInfoToWString(int precision) const {
    wstringstream ss;
    ss << fixed << setprecision(precision); //  小数点以下3桁で表示
    ss << L"Position: " << XMVectorGetX(m_position) << L"," << XMVectorGetY(m_position) << L"," << XMVectorGetZ(m_position) <<
          L"\nRotation: " << XMVectorGetX(m_degreesRotation) << L"," << XMVectorGetY(m_degreesRotation) << L"," << XMVectorGetZ(m_degreesRotation) << 
          L"\nScale: " << XMVectorGetX(m_scale) << L"," << XMVectorGetY(m_scale) << L"," << XMVectorGetZ(m_scale);
    return ss.str();
}

XMMATRIX Transform::GetWorldRotationMatrix() {
    // ピッチ、ヨー、ロールの回転行列を作成
    XMMATRIX pitchMatrix = XMMatrixRotationX(XMVectorGetX(m_radianRotation));
    XMMATRIX yawMatrix = XMMatrixRotationY(XMVectorGetY(m_radianRotation));
    XMMATRIX rollMatrix = XMMatrixRotationZ(XMVectorGetZ(m_radianRotation));

    // 回転行列を結合
    XMMATRIX rotationMatrix = rollMatrix * pitchMatrix * yawMatrix;

    return rotationMatrix;
}


XMMATRIX Transform::GetWorldMatrix() {
    XMMATRIX retMatrix = XMMatrixIdentity();

    //  回転行列を適応
    retMatrix *= GetWorldRotationMatrix();
    //  位置行列を適応
    retMatrix *= XMMatrixTranslation(XMVectorGetX(m_position), XMVectorGetY(m_position), XMVectorGetZ(m_position));
    //  スケール行列を適応
    retMatrix *= XMMatrixScaling(XMVectorGetX(m_scale), XMVectorGetY(m_scale), XMVectorGetZ(m_scale));

    return retMatrix;
}

//  Setter
//  位置
void Transform::SetPosition(float x, float y, float z) {
    m_position = XMVectorSet(x, y, z, 0.0f);
}
void Transform::SetPosition(XMVECTOR position) {
    m_position = position;
}

void Transform::SetDegreesRotation(float x, float y, float z) {
    m_degreesRotation = XMVectorSet(x, y, z, 0.0f);
    m_radianRotation = XMVectorSet(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z), 0.0f);
}

void Transform::SetRadianRotation(float x, float y, float z) {
    m_radianRotation = XMVectorSet(x, y, z, 0.0f);
    m_degreesRotation = XMVectorSet(XMConvertToDegrees(x), XMConvertToDegrees(y), XMConvertToDegrees(z), 0.0f);
}

void Transform::SetScale(float x, float y, float z) {
    m_scale = XMVectorSet(x, y, z, 0.0f);
}

//  Getter
//  位置
XMVECTOR Transform::GetPosition() const {
    return m_position;
}
//  角度
XMVECTOR Transform::GetDegreesRotation() const {
    return m_degreesRotation;
}
XMVECTOR Transform::GetRadianRotation() const {
    return  m_radianRotation;
}
//  スケール
XMVECTOR Transform::GetScale() const {
    return m_scale;
}

//  基底ベクトル
XMVECTOR Transform::GetForward() {
    //  回転行列を取得
    XMMATRIX rotationMatrix = GetWorldRotationMatrix();

    // カメラの方向を計算
    XMVECTOR forward = XMVector3TransformCoord(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationMatrix);

    //  正規化
    XMVector3Normalize(forward);

    return forward;
}

XMVECTOR Transform::GetRight() {
    XMVECTOR right = XMVector3Cross(GetForward(), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
    return right;
}

XMVECTOR Transform::GetUp() {
    XMVECTOR up = XMVector3Cross(GetRight(), GetForward());
    return up;
}
