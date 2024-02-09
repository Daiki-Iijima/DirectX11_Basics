#pragma once

#include<DirectXMath.h>
#include "HitDetection/BaseHitDetection.h"
#include "Model.h"

using namespace DirectX;

struct BoundingSphere {
    XMVECTOR center;
    float radius;
};

class SphereHitDetection : public BaseHitDetection
{
public:
    SphereHitDetection(std::shared_ptr<Model> model,ModelManager* modelManager);

    //  �����蔻��
    void HitCheck(vector<std::shared_ptr<Model>> allModels) override;

    BoundingSphere GetBoundingSphere();

    //  Getter
    float GetRadius() const {
        return m_radius;
    }

private:
    //  BoundingSphere�̔��a�̌v�Z
    float CalulateBoundingSphereRadius(const XMVECTOR& center, std::shared_ptr<std::vector<Vertex>>& vertices);

    //  ���a
    float m_radius;

    //  �������Ă���I�u�W�F�N�g���X�g
    vector<BaseHitDetection*> m_hitObjects;
};

