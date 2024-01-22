#pragma once

#include <vector>
#include <functional>

using namespace std;

//  �z�Q�Ƒ΍�
class Model;

class BaseHitDetection
{
public:
    BaseHitDetection(Model* model);

    //  �����蔻��
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
    //  ���̓����蔻��������Ă��郂�f��
    Model* m_model;

    //  �R�[���o�b�N
    //  �����������ɌĂ΂��
    function<void(BaseHitDetection*)> OnHitStart;
    function<void(BaseHitDetection*)> OnHitStay;
    function<void(BaseHitDetection*)> OnHitExit;
};

