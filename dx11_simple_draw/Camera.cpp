#include "Common/pch.h"
#include "Camera.h"

//  コンストラクタ
Camera::Camera(XMVECTOR position, XMVECTOR degressRotation) {
    m_transform = Transform(position, degressRotation, XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
}

Transform& Camera::GetTransform() {
    return m_transform;
}

//  ビュー行列
void Camera::GetViewMatrix(XMMATRIX& viewMatrix) {
    XMMATRIX rotationMatrix = m_transform.GetWorldRotationMatrix();

    // カメラの方向を計算
    XMVECTOR cameraDirection = XMVector3TransformCoord(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationMatrix);

    XMVECTOR position = m_transform.GetPosition();
    XMVECTOR targetPosition = position + cameraDirection;

    // 上方向ベクトルを計算（ロールを考慮）
    XMMATRIX rollMatrix = XMMatrixRotationZ(XMVectorGetZ(m_transform.GetRadianRotation()));
    XMVECTOR upDirection = XMVector3TransformCoord(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rollMatrix);

    viewMatrix = XMMatrixLookAtLH(position, targetPosition, upDirection);
}
