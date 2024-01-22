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
    SphereHitDetection(Model* model);

    //  当たり判定
    void HitCheck(vector<BaseHitDetection*> targetHitDetections) override;

    BoundingSphere GetBoundingSphere();

    //  Getter
    float GetRadius() {
        return m_radius;
    }

private:
    //  BoundingSphereの半径の計算
    float CalulateBoundingSphereRadius(const XMVECTOR& center, const std::vector<Vertex>& vertices);

    //  半径
    float m_radius;

    //  当たっているオブジェクトリスト
    vector<BaseHitDetection*> m_hitObjects;
};

