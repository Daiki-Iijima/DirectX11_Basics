#pragma once

#include <vector>
#include <functional>
#include "IComponent.h"
#include "ModelManager.h"

using namespace std;

//  �z�Q�Ƒ΍�
class Model;

class BaseHitDetection : public IComponent
{
public:
    BaseHitDetection(std::shared_ptr<Model> model, ModelManager* modelManager) : m_model(model), m_modelManager(modelManager) {
    }

    void Update() override {
        HitCheck(m_modelManager->GetAllModels());
    }

    void Render() override {

    }

    //  �����蔻��
    virtual void HitCheck(vector<std::shared_ptr<Model>> allModels) = 0;

    //  Getter
    std::shared_ptr<Model> GetModel() {
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

    //  ���̓����蔻��������Ă��郂�f��
    std::shared_ptr<Model> m_model;

    //  �R�[���o�b�N
    //  �����������ɌĂ΂��
    function<void(BaseHitDetection*)> OnHitStart;
    function<void(BaseHitDetection*)> OnHitStay;
    function<void(BaseHitDetection*)> OnHitExit;
};

