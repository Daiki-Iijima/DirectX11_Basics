#pragma once
#include "Transform.h"
#include "IComponentUIDebugView.h"
#include "ImGuiD3Utils.h"

class TransformUIDebugView : public IComponentUIDebugView {
public:
    TransformUIDebugView(Transform& transform) :m_transform(transform), m_isScaleUniform(false) {}

    void ComponentUIRender() override {
        // ポインタの番地を基にユニークなIDを生成
        uintptr_t ptrId = reinterpret_cast<uintptr_t>(&m_transform);
        std::string treeNodeLabel = "Transform##" + std::to_string(ptrId);

        if (ImGui::TreeNode(treeNodeLabel.c_str())) {
            ImGuiD3Utils::Float3Render(m_transform.GetPosition(), "Position", [this](float position[3]) {
                this->m_transform.SetPosition(position[0], position[1], position[2]);
                });
            ImGuiD3Utils::Float3Render(m_transform.GetDegreesRotation(), "Rotation", [this](float rotation[3]) {
                this->m_transform.SetDegreesRotation(rotation[0], rotation[1], rotation[2]);
                });
            ImGuiD3Utils::Float3Render(m_transform.GetScale(), "Scale", [this](float scale[3]) {
                this->m_transform.SetScale(scale[0], scale[1], scale[2]);
                }, true, &m_isScaleUniform);
            ImGui::TreePop();
        }
    }

    int GetRenderPriority() override {
        return 0;
    }

private:
    Transform& m_transform;
    bool m_isScaleUniform;
};

