#include "bx/editor/views/scene_view.hpp"

#include "bx/editor/core/selection.hpp"
#include "bx/editor/gizmos/transform_gizmo.hpp"

#include <bx/engine/core/input.hpp>
#include <bx/engine/core/ecs.hpp>
#include <bx/engine/core/time.hpp>
#include <bx/engine/core/file.hpp>
#include <bx/engine/core/data.hpp>
#include <bx/engine/core/inspector.hpp>
#include <bx/engine/modules/window.hpp>
#include <bx/engine/modules/graphics.hpp>
#include <bx/engine/modules/physics.hpp>

#ifdef BX_GRAPHICS_OPENGL_BACKEND
#include <bx/engine/modules/graphics/backend/graphics_opengl.hpp>
#endif

#include <bx/framework/components/transform.hpp>
#include <bx/framework/components/camera.hpp>
#include <bx/framework/components/mesh_filter.hpp>
#include <bx/framework/components/mesh_renderer.hpp>
#include <bx/framework/components/animator.hpp>
#include <bx/framework/components/collider.hpp>
#include <bx/framework/components/rigidbody.hpp>
#include <bx/framework/components/character_controller.hpp>
#include <bx/framework/systems/renderer.hpp>
#include <bx/framework/systems/dynamics.hpp>
#include <bx/framework/systems/acoustics.hpp>
#include <bx/framework/gameobject.hpp>

#include <cstring>
#include <fstream>
#include <sstream>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>
#include <IconsFontAwesome5.h>

#define SCENE_PAYLOAD_HEADER "SCEN"

static Vec2 s_mousePos;
static Transform g_sceneTrx;
static Camera g_sceneCam;
static Vec3 g_eulerAngles;

static ImVec2 g_sceneSize;

static bool g_physicsDebugDraw = false;

static GraphicsHandle g_vertShader = INVALID_GRAPHICS_HANDLE;
static GraphicsHandle g_pixelShader = INVALID_GRAPHICS_HANDLE;
static GraphicsHandle g_pipeline = INVALID_GRAPHICS_HANDLE;

static GraphicsHandle g_constantBuffer = INVALID_GRAPHICS_HANDLE;
static GraphicsHandle g_modelBuffer = INVALID_GRAPHICS_HANDLE;

static GraphicsHandle g_resources = INVALID_GRAPHICS_HANDLE;
static GraphicsHandle g_renderTarget = INVALID_GRAPHICS_HANDLE;
static GraphicsHandle g_renderTargetIDs = INVALID_GRAPHICS_HANDLE;
static GraphicsHandle g_depthStencil = INVALID_GRAPHICS_HANDLE;

struct SceneConstantData
{
    Mat4 viewProjMtx = Mat4::Identity();
};

struct SceneModelData
{
    Mat4 worldMeshMtx = Mat4::Identity();
    EntityId entityId = INVALID_ENTITY_ID;
};

struct SceneDrawCommandData
{
    SceneModelData model;
    
    GraphicsHandle vbuffers = INVALID_GRAPHICS_HANDLE;
    GraphicsHandle ibuffer = INVALID_GRAPHICS_HANDLE;
    u32 numIndices = 0;
};

static List<SceneDrawCommandData> g_drawCmds;

void SceneView::Initialize()
{
    f32 cpx = Data::GetFloat("Scene View Cam Pos X", 0, DataTarget::EDITOR);
    f32 cpy = Data::GetFloat("Scene View Cam Pos Y", 0, DataTarget::EDITOR);
    f32 cpz = Data::GetFloat("Scene View Cam Pos Z", 0, DataTarget::EDITOR);

    g_eulerAngles.x = Data::GetFloat("Scene View Cam Rot X", 0, DataTarget::EDITOR);
    g_eulerAngles.y = Data::GetFloat("Scene View Cam Rot Y", 0, DataTarget::EDITOR);
    g_eulerAngles.z = Data::GetFloat("Scene View Cam Rot Z", 0, DataTarget::EDITOR);

    g_sceneTrx.SetPosition(Vec3(cpx, cpy, cpz));
    g_sceneTrx.SetRotation(Quat::Euler(g_eulerAngles.x, g_eulerAngles.y, g_eulerAngles.z));

    // Create shaders
    {
        static const char* vsrc =
            "layout (location = 0) in vec3 Position;\n"
            "layout (std140) uniform ConstantBuffer\n"
            "{\n"
            "   mat4 ViewProjMtx;\n"
            "};\n"
            "layout (std140) uniform ModelBuffer\n"
            "{\n"
            "   mat4 WorldMeshMtx;\n"
            "   uvec2 EntityID; \n"
            "};\n"
            "flat out uvec2 Frag_EntityID;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = ViewProjMtx * WorldMeshMtx * vec4(Position, 1.0);\n"
            "   Frag_EntityID = EntityID;\n"
            "};";

        static const char* psrc =
            "layout (location = 0) out uvec2 Out_Color;\n"
            "flat in uvec2 Frag_EntityID;\n"
            "void main()\n"
            "{\n"
            "   Out_Color = Frag_EntityID;\n"
            "};";

        ShaderInfo info;

        info.shaderType = ShaderType::VERTEX;
        info.source = vsrc;
        g_vertShader = Graphics::CreateShader(info);

        info.shaderType = ShaderType::PIXEL;
        info.source = psrc;
        g_pixelShader = Graphics::CreateShader(info);
    }

    // Create pipeline
    {
        PipelineInfo info;

        info.numRenderTargets = 1;
        info.renderTargetFormats[0] = Graphics::GetColorBufferFormat();
        info.depthStencilFormat = Graphics::GetDepthBufferFormat();

        info.topology = PipelineTopology::TRIANGLES;
        info.faceCull = PipelineFaceCull::CCW;
        info.depthEnable = true;

        LayoutElement layoutElems[] =
        {
            LayoutElement { 0, 0, 3, GraphicsValueType::FLOAT32, false }
        };

        info.layoutElements = layoutElems;
        info.numElements = 1;

        info.vertShader = g_vertShader;
        info.pixelShader = g_pixelShader;

        g_pipeline = Graphics::CreatePipeline(info);
    }

    // Create buffers
    {
        BufferInfo info;

        info.type = BufferType::UNIFORM_BUFFER;
        info.usage = BufferUsage::DYNAMIC;
        info.access = BufferAccess::WRITE;
        g_constantBuffer = Graphics::CreateBuffer(info);

        info.type = BufferType::UNIFORM_BUFFER;
        info.usage = BufferUsage::DYNAMIC;
        info.access = BufferAccess::WRITE;
        g_modelBuffer = Graphics::CreateBuffer(info);
    }

    // Create resources
    {
        ResourceBindingElement resourceElems[] =
        {
            ResourceBindingElement { ShaderType::VERTEX, "ConstantBuffer", 1, ResourceBindingType::UNIFORM_BUFFER, ResourceBindingAccess::STATIC },
            ResourceBindingElement { ShaderType::VERTEX, "ModelBuffer", 1, ResourceBindingType::UNIFORM_BUFFER, ResourceBindingAccess::STATIC }
        };

        ResourceBindingInfo resourceBindingInfo;
        resourceBindingInfo.resources = resourceElems;
        resourceBindingInfo.numResources = 2;

        g_resources = Graphics::CreateResourceBinding(resourceBindingInfo);
        Graphics::BindResource(g_resources, "ConstantBuffer", g_constantBuffer);
        Graphics::BindResource(g_resources, "ModelBuffer", g_modelBuffer);
    }
}

void SceneView::Shutdown()
{
    const auto& cp = g_sceneTrx.GetPosition();

    Data::SetFloat("Scene View Cam Pos X", cp.x, DataTarget::EDITOR);
    Data::SetFloat("Scene View Cam Pos Y", cp.y, DataTarget::EDITOR);
    Data::SetFloat("Scene View Cam Pos Z", cp.z, DataTarget::EDITOR);

    Data::SetFloat("Scene View Cam Rot X", g_eulerAngles.x, DataTarget::EDITOR);
    Data::SetFloat("Scene View Cam Rot Y", g_eulerAngles.y, DataTarget::EDITOR);
    Data::SetFloat("Scene View Cam Rot Z", g_eulerAngles.z, DataTarget::EDITOR);
}

static void Update(const ImVec2& size)
{
    Vec2 mousePos = Vec2(Input::GetMouseX(), Input::GetMouseY());
    f32 dx = s_mousePos.x - mousePos.x;
    f32 dy = s_mousePos.y - mousePos.y;
    s_mousePos = mousePos;

    if (Input::GetMouseButton(MouseButton::MOUSE_BUTTON_RIGHT))
    {
        f32 s = Data::GetFloat("Scene View Cam Look Speed", 0.5f, DataTarget::EDITOR);
        g_eulerAngles += Vec3(-dy * s, dx * s, 0);
        g_eulerAngles.x = Math::Clamp(g_eulerAngles.x, -89.0f, 89.0f);
        g_sceneTrx.SetRotation(Quat::Euler(g_eulerAngles.x, g_eulerAngles.y, g_eulerAngles.z));

        Vec3 move = Vec3(Input::GetAxis(GamepadAxis::STICK_LEFT_X), 0, -Input::GetAxis(GamepadAxis::STICK_LEFT_Y));
        if (Input::GetKey(Key::W))
            move.z = 1.0f;
        if (Input::GetKey(Key::S))
            move.z = -1.0f;
        if (Input::GetKey(Key::Q))
            move.y = -1.0f;
        if (Input::GetKey(Key::E))
            move.y = 1.0f;
        if (Input::GetKey(Key::D))
            move.x = -1.0f;
        if (Input::GetKey(Key::A))
            move.x = 1.0f;

        f32 lsq = move.SqrMagnitude();
        if (lsq > 1)
            Vec3::Normalize(move);

        f32 speed = Data::GetFloat("Scene View Cam Move Speed", 50.f, DataTarget::EDITOR);
        f32 dz = Data::GetFloat("Scene View Cam Input Dead Zone", 0.1f, DataTarget::EDITOR);
        speed = lsq > dz ? speed : 0;
        move = move * speed * Time::GetDeltaTime();

        move = g_sceneTrx.GetRotation() * move;
        g_sceneTrx.SetPosition(g_sceneTrx.GetPosition() + move);
    }

    g_sceneTrx.Update();
    g_sceneCam.SetView(Mat4::LookAt(g_sceneTrx.GetPosition(), g_sceneTrx.GetPosition() + (g_sceneTrx.GetRotation() * Vec3::Forward()), Vec3::Up()));
}

static void Render(const ImVec2& size)
{
    if (g_sceneSize.x != size.x || g_sceneSize.y != size.y)
    {
        g_sceneSize = size;
        g_sceneSize.x = Math::Max(1.f, g_sceneSize.x);
        g_sceneSize.y = Math::Max(1.f, g_sceneSize.y);

        g_sceneCam.SetFov(60);
        g_sceneCam.SetAspect(g_sceneSize.x / g_sceneSize.y);
        g_sceneCam.SetZNear(0.1f);
        g_sceneCam.SetZFar(1000.0f);

        // Check if there are old render targets
        if (g_renderTargetIDs != INVALID_GRAPHICS_HANDLE)
            Graphics::DestroyTexture(g_renderTargetIDs);

        if (g_renderTarget != INVALID_GRAPHICS_HANDLE)
            Graphics::DestroyTexture(g_renderTarget);

        if (g_depthStencil != INVALID_GRAPHICS_HANDLE)
            Graphics::DestroyTexture(g_depthStencil);

        // Create render targets
        {
            TextureInfo info;
            info.width = (u32)g_sceneSize.x;
            info.height = (u32)g_sceneSize.y;

            info.format = TextureFormat::RG32_UINT;
            info.flags = TextureFlags::SHADER_RESOURCE | TextureFlags::RENDER_TARGET;
            g_renderTargetIDs = Graphics::CreateTexture(info);

            info.format = TextureFormat::RGBA8_UNORM;
            info.flags = TextureFlags::SHADER_RESOURCE | TextureFlags::RENDER_TARGET;
            g_renderTarget = Graphics::CreateTexture(info);

            info.format = TextureFormat::D24_UNORM_S8_UINT;
            info.flags = TextureFlags::DEPTH_STENCIL;
            g_depthStencil = Graphics::CreateTexture(info);
        }
    }

    g_sceneCam.Update();

    EntityManager::ForEach<Transform>(
        [&](Entity entity, Transform& trx)
        {
            trx.Update();
        });

    EntityManager::ForEach<Transform, Collider>(
        [&](Entity entity, Transform& trx, Collider& coll)
        {
            bool hasRigidBody = entity.HasComponent<RigidBody>();
            coll.Build(!hasRigidBody, trx.GetMatrix());

            if (hasRigidBody)
            {
                auto& rb = entity.GetComponent<RigidBody>();
                if (rb.GetCollider() != coll.GetCollider())
                {
                    rb.SetCollider(coll.GetCollider());
                }

                rb.Build(trx.GetMatrix());
                Physics::SetRigidBodyMatrix(rb.GetRigidBody(), trx.GetMatrix());
            }

            Physics::SetColliderMatrix(coll.GetCollider(), trx.GetMatrix());
        });

    EntityManager::ForEach<Transform, CharacterController>(
        [&](Entity entity, Transform& trx, CharacterController& cc)
        {
            cc.Build(trx.GetMatrix());
            Physics::SetCharacterControllerMatrix(cc.GetCharacterController(), trx.GetMatrix());
        });

    auto& renderer = SystemManager::GetSystem<Renderer>();
    renderer.UpdateAnimators();
    renderer.UpdateCameras();
    renderer.UpdateLights();
    renderer.CollectDrawCommands();

    g_drawCmds.clear();

    EntityManager::ForEach<Transform, MeshFilter>(
        [&](Entity entity, const Transform& trx, const MeshFilter& mf)
        {
            for (const auto& mesh : mf.GetMeshes())
            {
                if (!mesh) continue;
                const auto& meshData = mesh.GetData();

                SceneDrawCommandData cmd;
                cmd.model.worldMeshMtx = trx.GetMatrix() * meshData.GetMatrix();
                cmd.model.entityId = entity.GetId();
                cmd.vbuffers = meshData.GetVertexBuffers();
                cmd.ibuffer = meshData.GetIndexBuffer();
                cmd.numIndices = static_cast<u32>(meshData.GetTriangles().size());

                g_drawCmds.emplace_back(cmd);
            }
        });

    const f32 viewport[] = { 0.0f, 0.0f, g_sceneSize.x, g_sceneSize.y };
    Graphics::SetViewport(viewport);

    // Render to the normal color render target
    Graphics::SetRenderTarget(g_renderTarget, g_depthStencil);

    const f32 clearColor[] = { 0, 0, 0, 1 };
    Graphics::ClearRenderTarget(g_renderTarget, clearColor);
    Graphics::ClearDepthStencil(g_depthStencil, GraphicsClearFlags::DEPTH, 1.0f, 0);

    renderer.BindConstants(g_sceneCam.GetView(), g_sceneCam.GetProjection(), g_sceneCam.GetViewProjection());
    renderer.DrawCommands();

    Physics::DebugDraw();

    Graphics::UpdateDebugLines();
    Graphics::DrawDebugLines(g_sceneCam.GetViewProjection());

    // Render to the ID render target
    Graphics::SetRenderTarget(g_renderTargetIDs, g_depthStencil);

    Graphics::SetPipeline(g_pipeline);
    Graphics::ClearRenderTarget(g_renderTargetIDs, clearColor);
    Graphics::ClearDepthStencil(g_depthStencil, GraphicsClearFlags::DEPTH, 1.0f, 0);

    BufferData bufferData;
    SceneConstantData constants;
    constants.viewProjMtx = g_sceneCam.GetViewProjection();
    bufferData.dataSize = sizeof(SceneConstantData);
    bufferData.pData = &constants;
    Graphics::UpdateBuffer(g_constantBuffer, bufferData);

    for (auto& cmd : g_drawCmds)
    {
        bufferData.dataSize = sizeof(SceneModelData);
        bufferData.pData = &cmd.model;
        Graphics::UpdateBuffer(g_modelBuffer, bufferData);
        
        const u64 offset = 0;
        renderer.DrawCommand(g_pipeline, 1, &g_resources, 1, &cmd.vbuffers, &offset, cmd.ibuffer, cmd.numIndices);
    }
}

static void CopySceneClipboard()
{
    auto selected = Selection::GetSelected();
    if (!selected.Is<Entity>())
        return;

    EntityId id = selected.As<Entity>()->GetId();

    static char buff[21];
    std::sprintf(buff, "%s%016llx", SCENE_PAYLOAD_HEADER, id);

    ImGui::SetClipboardText(buff);
}

static void PasteSceneClipboard()
{
    const char* clipboard = ImGui::GetClipboardText();
    if (clipboard == nullptr || strlen(clipboard) < 20)
        return;

    static char buff[21];
    std::strncpy(buff, clipboard, 20);
    buff[20] = '\0';

    static char header[5];
    std::strncpy(header, buff, 4);
    header[4] = '\0';
    if (std::strcmp(header, SCENE_PAYLOAD_HEADER) != 0)
        return;

    try
    {
        EntityId id = INVALID_ENTITY_ID;
        id = std::stoull(std::string(buff + 4, 16), nullptr, 16);
        if (id != INVALID_ENTITY_ID)
        {
            const auto& copy = GameObject::Duplicate(GameObject::Find(Scene::GetCurrent(), id));
            Selection::SetSelected(Object<Entity>(copy.GetEntity()));
        }
    }
    catch (...) {}
}

static void LoadGameObject(const char* path)
{
    if (path == nullptr)
        return;

    auto& obj = GameObject::Load(Scene::GetCurrent(), path);
    if (obj.GetEntity().HasComponent<Transform>())
    {
        auto& trx = obj.GetEntity().GetComponent<Transform>();
        trx.SetPosition(g_sceneTrx.GetPosition() + g_sceneTrx.GetRotation() * Vec3(0, 0, 5));
    }
}

void SceneView::Present(bool& show)
{
    ObjectRef selected = Selection::GetSelected();
    EntityId deletedId = INVALID_ENTITY_ID;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin(ICON_FA_LIST"  Scene", &show, ImGuiWindowFlags_NoDecoration);
    ImGui::PopStyleVar(1);

    if (ImGui::BeginDragDropTargetCustom(ImGui::GetCurrentWindow()->ContentRegionRect, ImGui::GetCurrentWindow()->ID))
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("gameobject"))
        {
            auto path = (const char*)payload->Data;
            LoadGameObject(path);
        }

        ImGui::EndDragDropTarget();
    }

    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 cursorPos = ImGui::GetCursorPos();
    ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();
    ImVec2 gizmoPos = ImVec2(windowPos.x + cursorPos.x, windowPos.y + cursorPos.y);

    Render(contentRegionAvail);
    ImGui::Image((void*)(intptr_t)GraphicsOpenGL::GetTextureHandle(g_renderTarget), contentRegionAvail, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::SetCursorScreenPos(gizmoPos);

    const String& scene = Data::GetString("Current Scene", "", DataTarget::EDITOR);
    ImGui::Text("Current Scene: %s", scene.c_str());

    ImGui::Text("Physics Debug Draw: ");
    ImGui::SameLine();
    if (ImGui::Checkbox("##PhysicsDebugDraw", &g_physicsDebugDraw))
        Physics::SetDebugDraw(g_physicsDebugDraw);
    
    bool sceneActive = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows) || ImGui::IsWindowHovered();
    if (sceneActive)
    {
        Update(contentRegionAvail);
    }

    // Use transform gizmo if selected
    bool usingGizmo = false;
    if (selected.Is<Entity>())
    {
        Entity entity = *selected.As<Entity>();
        if (entity.IsValid())
        {
            auto& trx = entity.GetComponent<Transform>();

            f32 rect[] = { gizmoPos.x, gizmoPos.y, contentRegionAvail.x, contentRegionAvail.y };
            usingGizmo = TransformGizmo::Edit(rect, g_sceneCam, trx, true, sceneActive && !ImGui::IsMouseDown(ImGuiMouseButton_Right));
        }
    }

    if (!usingGizmo && ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered()) // Left mouse button
    {
        ImVec2 mousePos = ImGui::GetMousePos();
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 relMousePos = ImVec2(mousePos.x - windowPos.x, windowPos.y + windowSize.y - mousePos.y);
        
        u64 pixelData = 0;
        Graphics::ReadPixels((u32)relMousePos.x, (u32)relMousePos.y, 1, 1, &pixelData, g_renderTargetIDs);
        if (pixelData != 0)
        {
            Selection::SetSelected(Object<Entity>(pixelData));
        }
        else
        {
            Selection::ClearSelection();
        }
    }

    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
    {
        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_C))
        {
            CopySceneClipboard();
        }

        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_V))
        {
            PasteSceneClipboard();
        }

        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_S))
        {
            const String& scene = Data::GetString("Current Scene", "", DataTarget::EDITOR);
            Scene::Save(scene);
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Delete))
        {
            if (Selection::GetSelected().Is<Entity>())
            {
                deletedId = Selection::GetSelected().As<Entity>()->GetId();
            }
        }
    }
    
    ImGui::End();

	ImGui::Begin("Hierarchy", &show, ImGuiWindowFlags_NoCollapse);
    ImGui::BeginChild("panel", ImVec2(0, 0), true);

    if (ImGui::BeginDragDropTargetCustom(ImGui::GetCurrentWindow()->ContentRegionRect, ImGui::GetCurrentWindow()->ID))
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("gameobject"))
        {
            auto path = (const char*)payload->Data;
            LoadGameObject(path);
        }

        ImGui::EndDragDropTarget();
    }

    if (ImGui::IsItemClicked())
    {
        Selection::ClearSelection();
    }

    for (const auto gameObj : Scene::GetCurrent().GetGameObjects())
    {
        bool isParent = false; // TODO
        bool isSelected = selected.Is<Entity>() ? gameObj->GetEntity() == *selected.As<Entity>() : false;

        ImGuiTreeNodeFlags flags =
            ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth
            | (!isParent ? ImGuiTreeNodeFlags_Leaf : 0)
            | (isSelected ? ImGuiTreeNodeFlags_Selected : 0);

        EntityId entityId = gameObj->GetEntity().GetId();
        ImGui::PushID((int)entityId);
        if (ImGui::TreeNodeEx(gameObj->GetName().c_str(), flags))
        {
            if (ImGui::IsItemClicked())
            {
                Selection::SetSelected(Object<Entity>(gameObj->GetEntity()));
            }

            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Duplicate"))
                {
                    GameObject::Duplicate(*gameObj);
                }

                if (ImGui::MenuItem("Delete"))
                {
                    deletedId = gameObj->GetEntity().GetId();
                }

                ImGui::EndPopup();
            }

            ImGui::TreePop();
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
        {
            ImGui::SetDragDropPayload("gameobject", &entityId, sizeof(EntityId), ImGuiCond_Once);
            ImGui::Text(gameObj->GetName().c_str());
            ImGui::EndDragDropSource();
        }

        ImGui::PopID();
    }

    ImGui::EndChild();
	ImGui::End();

    if (deletedId != INVALID_ENTITY_ID)
    {
        if (selected.Is<Entity>() && selected.As<Entity>()->GetId() == deletedId)
        {
            Selection::ClearSelection();
        }

        Entity(deletedId).Destroy();
    }
}