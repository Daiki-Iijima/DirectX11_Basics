#pragma once
#include "Transform.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <functional>
#include "IComponentUIDebugView.h"

class TransformUIDebugView : public IComponentUIDebugView {
public:
    TransformUIDebugView(Transform& transform) :m_transform(transform), m_isScaleUniform(false) {}

    using DragFloatCallback = std::function<void(float[3])>;

    void Float3Render(XMVECTOR initialValue, const char* header, DragFloatCallback callback, bool uniform = false, bool* isUniform = nullptr) {
        float values[3] = { XMVectorGetX(initialValue), XMVectorGetY(initialValue), XMVectorGetZ(initialValue) };
        float width = 80.0f; // 入力フィールドの幅を設定
        float delta = 0.1f;  //  ドラッグ時の変化量を設定

        // Position入力フィールドの描画
        ImGui::AlignTextToFramePadding();
        ImGui::Text(header);
        if (uniform) {
            ImGui::SameLine(0.0f, 20.0f);
            std::string uniform_id = std::string("##Uniform") + header;
            ImGui::Checkbox(uniform_id.c_str(), isUniform);
            ImGui::SameLine(0.f, 12.5f);
        }
        else {
            ImGui::SameLine(0.0f, 30.0f);
        }

        ImGui::AlignTextToFramePadding();
        ImGui::Text("X");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(width);
        std::string x_id = std::string("##X") + header;
        if (ImGui::DragFloat(x_id.c_str(), &values[0], delta)) {
            if (uniform && isUniform) {
                values[1] = values[0];
                values[2] = values[0];
                callback(values);
            }
            else {
                callback(values);
            }
        }
        ImGui::SameLine();

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Y");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(width);
        std::string y_id = std::string("##Y") + header;
        if (ImGui::DragFloat(y_id.c_str(), &values[1], delta)) {
            if (uniform && isUniform) {
                values[1] = values[0];
                values[2] = values[0];
                callback(values);
            }
            else {
                callback(values);
            }
        }
        ImGui::SameLine();

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Z");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(width);
        std::string z_id = std::string("##Z") + header;
        if (ImGui::DragFloat(z_id.c_str(), &values[2], delta)) {
            if (uniform && isUniform) {
                values[1] = values[0];
                values[2] = values[0];
                callback(values);
            }
            else {
                callback(values);
            }
        }
    }


    void ComponentUIRender() override {
        // ポインタの番地を基にユニークなIDを生成
        uintptr_t ptrId = reinterpret_cast<uintptr_t>(&m_transform);
        std::string treeNodeLabel = "Transform##" + std::to_string(ptrId);

        if (ImGui::TreeNode(treeNodeLabel.c_str())) {
            Float3Render(m_transform.GetPosition(), "Position", [this](float position[3]) {
                this->m_transform.SetPosition(position[0], position[1], position[2]);
                });
            Float3Render(m_transform.GetDegreesRotation(), "Rotation", [this](float rotation[3]) {
                this->m_transform.SetDegreesRotation(rotation[0], rotation[1], rotation[2]);
                });
            Float3Render(m_transform.GetScale(), "Scale", [this](float scale[3]) {
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

