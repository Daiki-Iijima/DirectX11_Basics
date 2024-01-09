#include "Common/pch.h"
#include "Camera.h"

//  �R���X�g���N�^
Camera::Camera(XMVECTOR position, XMVECTOR degressRotation) {
    m_position = position;
    m_degressRotation = degressRotation;
    m_radianRotation = XMVectorSet(XMConvertToRadians(XMVectorGetX(degressRotation)), XMConvertToRadians(XMVectorGetY(degressRotation)), XMConvertToRadians(XMVectorGetZ(degressRotation)), 0.0f);
}

//  Setter
//  �ʒu
void Camera::SetPosition(float x, float y, float z) {
    m_position = XMVectorSet(x, y, z, 0.0f);
}
void Camera::SetPosition(XMVECTOR position) {
    m_position = position;
}

void Camera::SetDegressRotation(float x, float y, float z) {
    m_degressRotation = XMVectorSet(x, y, z, 0.0f);
    m_radianRotation = XMVectorSet(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z), 0.0f);
}

void Camera::SetRadianRotation(float x, float y, float z) {
    m_radianRotation = XMVectorSet(x, y, z, 0.0f);
    m_degressRotation = XMVectorSet(XMConvertToDegrees(x), XMConvertToDegrees(y), XMConvertToDegrees(z), 0.0f);
}

//  Getter
//  �ʒu
XMVECTOR Camera::GetPosition() const {
    return m_position;
}
//  �p�x
XMVECTOR Camera::GetDegressRotation() const {
    return m_degressRotation;
}
XMVECTOR Camera::GetRadianRotation() const {
    return  m_radianRotation;
}

//  �r���[�s��
void Camera::GetViewMatrix(XMMATRIX& viewMatrix) {
    //  �p�x��������x�N�g�����Z�o
    XMVECTOR cameraDirection = XMVectorSet(
        cosf(XMVectorGetX(m_radianRotation)) * sinf(XMVectorGetY(m_radianRotation)), // X����
        sinf(XMVectorGetX(m_radianRotation)),                                        // Y���� (�s�b�`)
        cosf(XMVectorGetX(m_radianRotation)) * cosf(XMVectorGetY(m_radianRotation)), // Z����
        0.0f);

    XMVECTOR targetPosition = m_position + cameraDirection;

    XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    viewMatrix = XMMatrixLookAtLH(m_position, targetPosition, upDirection);
}

//  ���x�N�g��
XMVECTOR Camera::GetForward() {

    //  �p�x��������x�N�g�����Z�o
    XMVECTOR cameraDirection = XMVectorSet(
        cosf(XMVectorGetX(m_radianRotation)) * sinf(XMVectorGetY(m_radianRotation)), // X����
        sinf(XMVectorGetX(m_radianRotation)),                                        // Y���� (�s�b�`)
        cosf(XMVectorGetX(m_radianRotation)) * cosf(XMVectorGetY(m_radianRotation)), // Z����
        0.0f);

    XMVECTOR targetPosition = m_position + cameraDirection;

    XMVECTOR forward = targetPosition - m_position;

    XMVector3Normalize(forward);

    return forward;
}

XMVECTOR Camera::GetRight() {
    XMVECTOR right = XMVector3Cross(GetForward(), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
    return right;
}

XMVECTOR Camera::GetUp() {
    XMVECTOR up = XMVector3Cross(GetRight(), GetForward());
    return up;
}
