#pragma once

#include <DirectXMath.h>
#include "BaseHitDetection.h"
#include <d3d11.h>
#include <ModelManager.h>

using namespace DirectX;

class HitDetectionDebugView
{
public:
    HitDetectionDebugView(BaseHitDetection* baseHitDetection,ModelManager* modelManager) : m_pHitDetection(baseHitDetection),m_pModelManager(modelManager)
    {}

    void RenderDebugHitDetection() {
        Model* hitDetectionModel = m_pModelManager->AddModel("Models/Sphere/Sphere.obj", "DebugHitDetection");
    }

private:
    BaseHitDetection* m_pHitDetection;
    ModelManager* m_pModelManager;
};
