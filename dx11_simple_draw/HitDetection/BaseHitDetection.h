#pragma once

#include <vector>
#include <functional>
#include "IComponent.h"
#include "ModelManager.h"

using namespace std;

//  循環参照対策
class Model;

class BaseHitDetection : public IComponent
{
public:
    BaseHitDetection(Model* model,ModelManager* modelManager);

    void Update() override {
        HitCheck(m_modelManager->GetAllModels());
    }

    void Render() override {

    }

    //  当たり判定
    virtual void HitCheck(vector<Model*>& allModels) = 0;

    //  Getter
    Model* GetModel() {
        return m_model;
    }

    //  Setter
    void SetOnHitStart(const function<void(BaseHitDetection*)>& onHitStart) {
        OnHitStart = onHitStart;
    }
    void SetOnHitStay(const function<void(BaseHitDetection*)>& onHitStay) {
        OnHitStay = onHitStay;
    }
    void SetOnHitExit(const function<void(BaseHitDetection*)>& onHitExit) {
        OnHitExit = onHitExit;
    }

protected:
    ModelManager* m_modelManager;

    //  この当たり判定を持っているモデル
    Model* m_model;

    //  コールバック
    //  当たった時に呼ばれる
    function<void(BaseHitDetection*)> OnHitStart;
    function<void(BaseHitDetection*)> OnHitStay;
    function<void(BaseHitDetection*)> OnHitExit;
};

