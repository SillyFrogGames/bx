#include "Editor/Gizmos/TransformGizmo.hpp"

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <ImGuizmo.h>
#include <IconsFontAwesome5.h>

static ImGuizmo::OPERATION currentGizmoOperation = ImGuizmo::TRANSLATE;

bool TransformGizmo::Edit(f32 rect[4], const Camera& camera, Transform& trx, bool showControls, bool allowHotkeys)
{
    static ImGuizmo::MODE currentGizmoMode = ImGuizmo::LOCAL;
    static bool useSnap = false;
    static float unitSnap[3] = { 1.f, 1.f, 1.f };
    static float angleSnap = 10.0f;
    static float scaleSnap = 0.1f;
    static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
    static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
    static bool boundSizing = false;
    static bool boundSizingSnap = false;

    Mat4 matrix = trx.GetMatrix();

    if (showControls)
    {
        if (allowHotkeys)
        {
            if (ImGui::IsKeyPressed(ImGuiKey_W))
                currentGizmoOperation = ImGuizmo::TRANSLATE;
            if (ImGui::IsKeyPressed(ImGuiKey_E))
                currentGizmoOperation = ImGuizmo::ROTATE;
            if (ImGui::IsKeyPressed(ImGuiKey_R))
                currentGizmoOperation = ImGuizmo::SCALE;
        }

        ImGui::BeginChild("##GizmoControls", ImVec2(200, 200), true);

        if (ImGui::RadioButton("Translate", currentGizmoOperation == ImGuizmo::TRANSLATE))
            currentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", currentGizmoOperation == ImGuizmo::ROTATE))
            currentGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", currentGizmoOperation == ImGuizmo::SCALE))
            currentGizmoOperation = ImGuizmo::SCALE;

        if (ImGui::RadioButton("Universal", currentGizmoOperation == ImGuizmo::UNIVERSAL))
            currentGizmoOperation = ImGuizmo::UNIVERSAL;

        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(matrix.data, matrixTranslation, matrixRotation, matrixScale);
        ImGui::InputFloat3("Tr", matrixTranslation);
        ImGui::InputFloat3("Rt", matrixRotation);
        ImGui::InputFloat3("Sc", matrixScale);
        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix.data);

        if (currentGizmoOperation != ImGuizmo::SCALE)
        {
            if (ImGui::RadioButton("Local", currentGizmoMode == ImGuizmo::LOCAL))
                currentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", currentGizmoMode == ImGuizmo::WORLD))
                currentGizmoMode = ImGuizmo::WORLD;
        }

        //if (ImGui::IsKeyPressed(ImGuiKey_S))
        //    useSnap = !useSnap;

        ImGui::Checkbox("##UseSnap", &useSnap);
        ImGui::SameLine();

        switch (currentGizmoOperation)
        {
        case ImGuizmo::TRANSLATE:
            ImGui::InputFloat3("Unit Snap", &unitSnap[0]);
            break;
        case ImGuizmo::ROTATE:
            ImGui::InputFloat("Angle Snap", &angleSnap);
            break;
        case ImGuizmo::SCALE:
            ImGui::InputFloat("Scale Snap", &scaleSnap);
            break;
        }
        ImGui::Checkbox("Bound Sizing", &boundSizing);
        if (boundSizing)
        {
            ImGui::PushID(3);
            ImGui::Checkbox("##BoundSizing", &boundSizingSnap);
            ImGui::SameLine();
            ImGui::InputFloat3("Snap", boundsSnap);
            ImGui::PopID();
        }

        ImGui::EndChild();
    }

    ImGuizmo::SetOrthographic(false);

    ImGuizmo::BeginFrame();
    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(rect[0], rect[1], rect[2], rect[3]);

    float* snap = nullptr;
    switch (currentGizmoOperation)
    {
    case ImGuizmo::TRANSLATE:
        snap = &unitSnap[0];
        break;
    case ImGuizmo::ROTATE:
        snap = &angleSnap;
        break;
    case ImGuizmo::SCALE:
        snap = &scaleSnap;
        break;
    }

    if (ImGuizmo::Manipulate(camera.GetView().data, camera.GetProjection().data, currentGizmoOperation, currentGizmoMode, matrix.data, NULL, useSnap ? snap : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL))
    {
        trx.SetMatrix(matrix);
    }

    return ImGuizmo::IsUsing() || ImGuizmo::IsOver();
}