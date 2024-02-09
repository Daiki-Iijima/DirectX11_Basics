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
        auto hitDetectionModel = m_pModelManager->CreateModelFromObj("Models/Sphere/Sphere.obj");
    }

private:
    BaseHitDetection* m_pHitDetection;
    ModelManager* m_pModelManager;
};
