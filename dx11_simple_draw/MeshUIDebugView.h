#pragma once

#include "IUIDebugComponent.h"
#include <imgui.h>

class MeshUIDebugView : public IUIDebugComponent {
public:

    MeshUIDebugView(std::shared_ptr<Mesh>& mesh) :m_mesh(mesh) {}

    void ComponentUIRender() override {
        uintptr_t ptrId = reinterpret_cast<uintptr_t>(&m_mesh);
        std::string treeNodeLabel = "Mesh##" + std::to_string(ptrId);
        if (ImGui::TreeNode(treeNodeLabel.c_str())) {
            std::string uniform_id = std::string("MeshRenderEnabled") + std::to_string(ptrId);
            std::string meshEnableLabel = "Enable##" + uniform_id;
            bool renderEnabled = m_mesh->GetRenderEnabled();
            if (ImGui::Checkbox(meshEnableLabel.c_str(), &renderEnabled)) {
                m_mesh->SetRenderEnabled(renderEnabled);
            }
            ImGui::TreePop();
        }
    }

    int GetRenderPriority() {
        return 1;
    }

private:
    std::shared_ptr<Mesh>& m_mesh;
};
