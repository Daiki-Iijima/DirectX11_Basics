#pragma once
#include <stdint.h>
#include "Game.h"
#include "IUIDebugComponent.h"
#include "ImGuiD3Utils.h"

class LightUIDebugView : public IUIDebugComponent
{
public:
    LightUIDebugView(PsConstantBuffer& psConstantBuffer) : m_psConstantBuffer(psConstantBuffer){}

    void ComponentUIRender() override {
        // ポインタの番地を基にユニークなIDを生成
        uintptr_t ptrId = reinterpret_cast<uintptr_t>(&m_psConstantBuffer);
        std::string treeNodeLabel = "Setting##" + std::to_string(ptrId);

        if (ImGui::TreeNode(treeNodeLabel.c_str())) {
            ImGuiD3Utils::Float3Render(DirectX::XMLoadFloat3(&m_psConstantBuffer.LightDirection), "Position", [this](float direction[3]) {
                this->m_psConstantBuffer.LightDirection = DirectX::XMFLOAT3(direction);
                });
            ImGuiD3Utils::Float3Render(DirectX::XMLoadFloat3(&m_psConstantBuffer.LightColor), "Color", [this](float color[3]) {
                this->m_psConstantBuffer.LightColor = DirectX::XMFLOAT3(color);
                });
            ImGui::DragFloat("Ambient", &m_psConstantBuffer.LightIntensity, 0.01f, 0.0f, 5.0f);

            ImGui::TreePop();
        }
    }

    int GetRenderPriority() override {
        return 2;
    }
private:
    PsConstantBuffer& m_psConstantBuffer;
};
