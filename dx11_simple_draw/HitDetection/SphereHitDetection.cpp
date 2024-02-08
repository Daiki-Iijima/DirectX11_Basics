#include "Common/pch.h"
#include "BaseHitDetection.h"
#include "HitDetection/SphereHitDetection.h"
#include "Model.h"

SphereHitDetection::SphereHitDetection(Model* model, ModelManager* modelManager) :BaseHitDetection(model, modelManager), m_radius(1.f) {
    m_hitObjects = vector<BaseHitDetection*>();
}


BoundingSphere SphereHitDetection::GetBoundingSphere() {
    BoundingSphere boundingSphere;

    boundingSphere.center = m_model->GetMesh().GetCenter();
    boundingSphere.radius = CalulateBoundingSphereRadius(boundingSphere.center, m_model->GetMesh().GetVertices());

    return boundingSphere;
}

float SphereHitDetection::CalulateBoundingSphereRadius(const XMVECTOR& center, const std::vector<Vertex>& vertices) {

    float maxDistanceSq = 0.0f;

    //  中心座標から頂点間での距離をすべて計算
    //  一番遠いい距離を半径として返す
    for (const auto& vertex : vertices) {
        XMVECTOR vertexPos = XMVectorSet(vertex.position.x, vertex.position.y, vertex.position.z, 0);
        float distanceSq = XMVectorGetX(XMVector3LengthSq(XMVectorSubtract(vertexPos, center)));
        if (distanceSq > maxDistanceSq) {
            maxDistanceSq = distanceSq;
        }
    }
    return sqrt(maxDistanceSq);
}


//  当たり判定
void SphereHitDetection::HitCheck(vector<Model*> allModels) {

    std::vector<BaseHitDetection*> hitDetections = std::vector<BaseHitDetection*>();

    //  当たり判定コンポーネントを持っているオブジェクトを抽出
    for (Model* model : allModels) {
        auto components = model->GetComponents();
        for (auto& component : components) {
            BaseHitDetection* hitDetection = dynamic_cast<BaseHitDetection*>(component.get());
            if (hitDetection != nullptr) {
                hitDetections.push_back(hitDetection);
            }
        }
    }

    if (hitDetections.size() == 0) {
        return;
    }

    //  自分のモデルの原点座標を取得
    BoundingSphere myBoundingSphere = GetBoundingSphere();

    //  このフレームの処理で当たったオブジェクトリスト
    vector<BaseHitDetection*> thisFrameHitObjects = vector<BaseHitDetection*>();

    //  自分のモデルの中心座標と半径を使って球を作成
    for (BaseHitDetection* targetHitDetection : hitDetections) {

        //  当たり判定の種類が違う場合は処理をスキップ
        SphereHitDetection* targetSphereHitDetection = dynamic_cast<SphereHitDetection*>(targetHitDetection);
        if (targetSphereHitDetection == nullptr) {
            OutputDebugStringW(L"ヒット判定処理が別のものです\n");
            continue;
        }

        //  自分自身とは当たり判定を行わない
        if (m_model == targetSphereHitDetection->GetModel()) {
            continue;
        }

        //  相手のモデルを取得
        BoundingSphere targetBoundingSphere = targetSphereHitDetection->GetBoundingSphere();

        //  自分のモデルの中心座標と相手のモデルの中心座標の距離を求める
        //  centerだけではローカルな座標になってしまうので、モデルの現在のワールド座標を足す必要がある
        XMVECTOR diff = XMVectorSubtract(
            myBoundingSphere.center + m_model->GetTransform().GetPosition(),
            targetBoundingSphere.center + targetSphereHitDetection->GetModel()->GetTransform().GetPosition()
        );

        //  Xに距離が入っている
        float distance = XMVectorGetX(XMVector3Length(diff));

        //  自分のモデルの半径と相手のモデルの半径を足す
        float detectionRadius = myBoundingSphere.radius + targetBoundingSphere.radius;

        //  距離が半径より小さいかどうかを判定する
        if (distance < detectionRadius) {
            //  当たり判定の発火
            //  既存オブジェクトリストにない場合
            if (find(m_hitObjects.begin(), m_hitObjects.end(), targetHitDetection) == m_hitObjects.end()) {
                m_hitObjects.push_back(targetHitDetection);
                if (OnHitStart) {
                    OnHitStart(targetHitDetection);
                }
                thisFrameHitObjects.push_back(targetHitDetection);
            }
            //  既存オブジェクトリストにある場合
            else {
                if (OnHitStay) {
                    OnHitStay(targetHitDetection);
                }
                thisFrameHitObjects.push_back(targetHitDetection);
            }
        }
    }

    //  このフレームで当たっていないオブジェクトは当たり判定を終了する
    for (BaseHitDetection* hitDetection : m_hitObjects) {
        if (find(thisFrameHitObjects.begin(), thisFrameHitObjects.end(), hitDetection) == thisFrameHitObjects.end()) {
            //  当たり判定の終了
            if (OnHitExit) {
                OnHitExit(hitDetection);
                m_hitObjects.erase(remove(m_hitObjects.begin(), m_hitObjects.end(), hitDetection));
            }
        }
    }
}

