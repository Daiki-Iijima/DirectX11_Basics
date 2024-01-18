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
    //  �R���X�g���N�^
    Transform();
    Transform(XMVECTOR position, XMVECTOR degressRotation, XMVECTOR scale);

    //  �ʒu
    void SetPosition(XMVECTOR position);
    void SetPosition(float x, float y, float z);
    //  �p�x
    void SetDegressRotation(float x, float y, float z);
    void SetRadianRotation(float x, float y, float z);

    //  Getter
    XMMATRIX GetWorldRotationMatrix();
    //  �ʒu
    XMVECTOR GetPosition() const;
    XMMATRIX GetWorldMatrix();
    //  �p�x
    XMVECTOR GetDegressRotation() const;
    XMVECTOR GetRadianRotation() const;
    //  ���x�N�g��
    XMVECTOR GetForward();
    XMVECTOR GetRight();
    XMVECTOR GetUp();

    //  �e�L�X�g�o��
    wstring GetInfoToWString(int precision) const;

private:
    //  �ʒu
    XMVECTOR m_position;

    //  �p�x
    XMVECTOR m_degressRotation;   //  �x��
    XMVECTOR m_radianRotation;    //  �ʓx

    //  �X�P�[��    
    XMVECTOR m_scale;
};

