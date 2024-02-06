#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <functional>

using namespace DirectX;


class ImGuiD3Utils
{
public:
    using DragFloatCallback = std::function<void(float[3])>;

    static void Float3Render(XMVECTOR initialValue, const char* header, DragFloatCallback callback, bool uniform = false, bool* isUniform = nullptr) {
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


};
