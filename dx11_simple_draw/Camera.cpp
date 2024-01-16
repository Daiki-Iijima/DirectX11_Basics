#include "Common/pch.h"
#include "Camera.h"

//  �R���X�g���N�^
Camera::Camera(XMVECTOR position, XMVECTOR degressRotation) {
    m_transform = Transform(position, degressRotation, XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
}

Transform& Camera::GetTransform() {
    return m_transform;
}

//  �r���[�s��
void Camera::GetViewMatrix(XMMATRIX& viewMatrix) {
    XMMATRIX rotationMatrix = m_transform.GetWorldRotationMatrix();

    // �J�����̕������v�Z
    XMVECTOR cameraDirection = XMVector3TransformCoord(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationMatrix);

    XMVECTOR position = m_transform.GetPosition();
    XMVECTOR targetPosition = position + cameraDirection;

    // ������x�N�g�����v�Z�i���[�����l���j
    XMMATRIX rollMatrix = XMMatrixRotationZ(XMVectorGetZ(m_transform.GetRadianRotation()));
    XMVECTOR upDirection = XMVector3TransformCoord(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rollMatrix);

    viewMatrix = XMMatrixLookAtLH(position, targetPosition, upDirection);
}
