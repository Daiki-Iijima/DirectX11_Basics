#pragma once

#include <vector>
#include <functional>

using namespace std;

//  循環参照対策
class Model;

class BaseHitDetection
{
public:
    BaseHitDetection(Model* model);

    //  当たり判定
    virtual void HitCheck(vector<BaseHitDetection*> targetHitDetections) = 0;

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
    //  この当たり判定を持っているモデル
    Model* m_model;

    //  コールバック
    //  当たった時に呼ばれる
    function<void(BaseHitDetection*)> OnHitStart;
    function<void(BaseHitDetection*)> OnHitStay;
    function<void(BaseHitDetection*)> OnHitExit;
};

