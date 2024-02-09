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

    //  当たり判定
    void HitCheck(vector<std::shared_ptr<Model>> allModels) override;

    BoundingSphere GetBoundingSphere();

    //  Getter
    float GetRadius() const {
        return m_radius;
    }

private:
    //  BoundingSphereの半径の計算
    float CalulateBoundingSphereRadius(const XMVECTOR& center, std::shared_ptr<std::vector<Vertex>>& vertices);

    //  半径
    float m_radius;

    //  当たっているオブジェクトリスト
    vector<BaseHitDetection*> m_hitObjects;
};

