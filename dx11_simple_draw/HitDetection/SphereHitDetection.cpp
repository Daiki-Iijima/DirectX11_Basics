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

    //  ���S���W���璸�_�Ԃł̋��������ׂČv�Z
    //  ��ԉ����������𔼌a�Ƃ��ĕԂ�
    for (const auto& vertex : vertices) {
        XMVECTOR vertexPos = XMVectorSet(vertex.position.x, vertex.position.y, vertex.position.z, 0);
        float distanceSq = XMVectorGetX(XMVector3LengthSq(XMVectorSubtract(vertexPos, center)));
        if (distanceSq > maxDistanceSq) {
            maxDistanceSq = distanceSq;
        }
    }
    return sqrt(maxDistanceSq);
}


//  �����蔻��
void SphereHitDetection::HitCheck(vector<Model*> allModels) {

    std::vector<BaseHitDetection*> hitDetections = std::vector<BaseHitDetection*>();

    //  �����蔻��R���|�[�l���g�������Ă���I�u�W�F�N�g�𒊏o
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

    //  �����̃��f���̌��_���W���擾
    BoundingSphere myBoundingSphere = GetBoundingSphere();

    //  ���̃t���[���̏����œ��������I�u�W�F�N�g���X�g
    vector<BaseHitDetection*> thisFrameHitObjects = vector<BaseHitDetection*>();

    //  �����̃��f���̒��S���W�Ɣ��a���g���ċ����쐬
    for (BaseHitDetection* targetHitDetection : hitDetections) {

        //  �����蔻��̎�ނ��Ⴄ�ꍇ�͏������X�L�b�v
        SphereHitDetection* targetSphereHitDetection = dynamic_cast<SphereHitDetection*>(targetHitDetection);
        if (targetSphereHitDetection == nullptr) {
            OutputDebugStringW(L"�q�b�g���菈�����ʂ̂��̂ł�\n");
            continue;
        }

        //  �������g�Ƃ͓����蔻����s��Ȃ�
        if (m_model == targetSphereHitDetection->GetModel()) {
            continue;
        }

        //  ����̃��f�����擾
        BoundingSphere targetBoundingSphere = targetSphereHitDetection->GetBoundingSphere();

        //  �����̃��f���̒��S���W�Ƒ���̃��f���̒��S���W�̋��������߂�
        //  center�����ł̓��[�J���ȍ��W�ɂȂ��Ă��܂��̂ŁA���f���̌��݂̃��[���h���W�𑫂��K�v������
        XMVECTOR diff = XMVectorSubtract(
            myBoundingSphere.center + m_model->GetTransform().GetPosition(),
            targetBoundingSphere.center + targetSphereHitDetection->GetModel()->GetTransform().GetPosition()
        );

        //  X�ɋ����������Ă���
        float distance = XMVectorGetX(XMVector3Length(diff));

        //  �����̃��f���̔��a�Ƒ���̃��f���̔��a�𑫂�
        float detectionRadius = myBoundingSphere.radius + targetBoundingSphere.radius;

        //  ���������a��菬�������ǂ����𔻒肷��
        if (distance < detectionRadius) {
            //  �����蔻��̔���
            //  �����I�u�W�F�N�g���X�g�ɂȂ��ꍇ
            if (find(m_hitObjects.begin(), m_hitObjects.end(), targetHitDetection) == m_hitObjects.end()) {
                m_hitObjects.push_back(targetHitDetection);
                if (OnHitStart) {
                    OnHitStart(targetHitDetection);
                }
                thisFrameHitObjects.push_back(targetHitDetection);
            }
            //  �����I�u�W�F�N�g���X�g�ɂ���ꍇ
            else {
                if (OnHitStay) {
                    OnHitStay(targetHitDetection);
                }
                thisFrameHitObjects.push_back(targetHitDetection);
            }
        }
    }

    //  ���̃t���[���œ������Ă��Ȃ��I�u�W�F�N�g�͓����蔻����I������
    for (BaseHitDetection* hitDetection : m_hitObjects) {
        if (find(thisFrameHitObjects.begin(), thisFrameHitObjects.end(), hitDetection) == thisFrameHitObjects.end()) {
            //  �����蔻��̏I��
            if (OnHitExit) {
                OnHitExit(hitDetection);
                m_hitObjects.erase(remove(m_hitObjects.begin(), m_hitObjects.end(), hitDetection));
            }
        }
    }
}

