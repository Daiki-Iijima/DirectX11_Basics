#pragma once

//  ImGui���g�p���ăQ�[����ŃR���|�[�l���g�̃v���p�e�B��\���A�ҏW�ł���悤�ɂ��邽�߂̃N���X
//  �������z�N���X(�C���^�[�t�F�C�X)�Ƃ��Ď�������

class IComponentUIDebugView {
public:
    virtual void ComponentUIRender() = 0;
    virtual int GetRenderPriority() = 0;
};
