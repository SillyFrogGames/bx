#include "Framework/Systems/Renderer.hpp"

#include "Framework/Components/Transform.hpp"
#include "Framework/Components/Camera.hpp"
#include "Framework/Components/MeshFilter.hpp"
#include "Framework/Components/MeshRenderer.hpp"
#include "Framework/Components/Animator.hpp"
#include "Framework/Components/Light.hpp"

#include <Engine/Core/File.hpp>
#include <Engine/Core/Data.hpp>
#include <Engine/Core/Profiler.hpp>
#include <Engine/Core/Resource.hpp>
#include <Engine/Containers/Tree.hpp>
#include <Engine/Modules/Graphics.hpp>
#include <Engine/Modules/Window.hpp>

struct ViewData
{
    Mat4 viewMtx = Mat4::Identity();
    Mat4 projMtx = Mat4::Identity();
    Mat4 viewProjMtx = Mat4::Identity();
};

struct ConstantData
{
    ViewData view;
};

struct LightData
{
    u32 type = 0;
    f32 intensity = 1.0f;

    i32 shadowMapIndex = -1;
    u32 cascadeCount = 0;
    
    Vec3 position = Vec3(0, 0, 0);
    f32 constant = 1.0f;

    Vec3 direction = Vec3(0, 0, 0);
    f32 linear_cutoff = 0.1f;

    Vec3 color = Vec3(1, 1, 1);
    f32 quadratic_outerCutoff = 0.01f;
};

struct ModelData
{
    Mat4 worldMtx = Mat4::Identity();
    Mat4 meshMtx = Mat4::Identity();
    Vec4i lightIndices = Vec4i(-1, -1, -1, -1);
};

struct DrawCommandData
{
    ModelData model;

    GraphicsHandle vbuffers = INVALID_GRAPHICS_HANDLE;
    GraphicsHandle ibuffer = INVALID_GRAPHICS_HANDLE;
    u32 numIndices = 0;

    GraphicsHandle pipeline = INVALID_GRAPHICS_HANDLE;

    GraphicsHandle matResources = INVALID_GRAPHICS_HANDLE;
    GraphicsHandle animResources = INVALID_GRAPHICS_HANDLE;
};

class Renderer::Impl
{
public:
    GraphicsHandle pipelineOverride = INVALID_GRAPHICS_HANDLE;

    GraphicsHandle constantBuffer = INVALID_GRAPHICS_HANDLE;
    GraphicsHandle modelBuffer = INVALID_GRAPHICS_HANDLE;
    GraphicsHandle lightBuffer = INVALID_GRAPHICS_HANDLE;

    GraphicsHandle resources = INVALID_GRAPHICS_HANDLE;

    List<ViewData> views;
    List<LightData> lights;
    List<DrawCommandData> drawCmds;
};

void Renderer::Initialize()
{
    m_impl = new Renderer::Impl();

    BufferInfo info;

    info.type = BufferType::UNIFORM_BUFFER;
    info.usage = BufferUsage::DYNAMIC;
    info.access = BufferAccess::WRITE;
    m_impl->constantBuffer = Graphics::CreateBuffer(info);
    
    info.type = BufferType::UNIFORM_BUFFER;
    info.usage = BufferUsage::DYNAMIC;
    info.access = BufferAccess::WRITE;
    m_impl->modelBuffer = Graphics::CreateBuffer(info);

    info.type = BufferType::UNIFORM_BUFFER;
    info.usage = BufferUsage::DYNAMIC;
    info.access = BufferAccess::WRITE;
    info.strideBytes = sizeof(LightData);
    m_impl->lightBuffer = Graphics::CreateBuffer(info);

    ResourceBindingElement resourceElems[] =
    {
        ResourceBindingElement { ShaderType::VERTEX, "ConstantBuffer", 1, ResourceBindingType::UNIFORM_BUFFER, ResourceBindingAccess::STATIC },
        ResourceBindingElement { ShaderType::VERTEX, "ModelBuffer", 1, ResourceBindingType::UNIFORM_BUFFER, ResourceBindingAccess::STATIC },
        ResourceBindingElement { ShaderType::PIXEL, "LightBuffer", 1, ResourceBindingType::UNIFORM_BUFFER, ResourceBindingAccess::STATIC }
    };

    ResourceBindingInfo resourceBindingInfo;
    resourceBindingInfo.resources = resourceElems;
    resourceBindingInfo.numResources = 3;

    m_impl->resources = Graphics::CreateResourceBinding(resourceBindingInfo);

    Graphics::BindResource(m_impl->resources, "ConstantBuffer", m_impl->constantBuffer);
    Graphics::BindResource(m_impl->resources, "ModelBuffer", m_impl->modelBuffer);
    Graphics::BindResource(m_impl->resources, "LightBuffer", m_impl->lightBuffer);
}

void Renderer::Shutdown()
{
    Graphics::DestroyBuffer(m_impl->constantBuffer);
    Graphics::DestroyBuffer(m_impl->modelBuffer);
    Graphics::DestroyBuffer(m_impl->lightBuffer);

    Graphics::DestroyResourceBinding(m_impl->resources);

    delete m_impl;
    m_impl = nullptr;
}

void Renderer::SetPipelineOverride(const GraphicsHandle pipeline)
{
    m_impl->pipelineOverride = pipeline;
}

Vec4i Renderer::GetLightIndices(const Vec3& pos)
{
    Vec4i res = Vec4i(-1, -1, -1, -1);
    for (SizeType i = 0; i < m_impl->lights.size(); ++i)
    {
        if (i >= 4) break;
        res[i] = static_cast<i32>(i);
    }
    return res;
}

void Renderer::UpdateAnimators()
{
    EntityManager::ForEach<Animator>(
        [&](Entity entity, Animator& anim)
        {
            anim.Update();
        });
}

void Renderer::UpdateCameras()
{
    m_impl->views.clear();

    EntityManager::ForEach<Transform, Camera>(
        [&](Entity entity, const Transform& trx, Camera& cam)
        {
            i32 width, height;
            Window::GetSize(&width, &height);
            cam.SetAspect((f32)width / (height == 0 ? 1.0f : (f32)height));

            Vec3 fwd = trx.GetRotation() * Vec3::Forward();
            cam.SetView(Mat4::LookAt(trx.GetPosition(), trx.GetPosition() + fwd, Vec3(0, 1, 0)));
            cam.Update();

            ViewData view;
            view.viewMtx = cam.GetView();
            view.projMtx = cam.GetProjection();
            view.viewProjMtx = cam.GetViewProjection();
            m_impl->views.emplace_back(view);
        });
}

void Renderer::UpdateLights()
{
    m_impl->lights.clear();

    EntityManager::ForEach<Transform, Light>(
        [&](Entity entity, const Transform& trx, const Light& l)
        {
            LightData light;
            light.position = trx.GetPosition();
            light.intensity = l.GetIntensity();
            light.constant = l.GetConstant();
            light.linear_cutoff = l.GetLinear();
            light.quadratic_outerCutoff = l.GetQuadratic();
            light.color = l.GetColor();
            m_impl->lights.emplace_back(light);
        });

    BufferData bufferData;
    bufferData.dataSize = sizeof(LightData) * m_impl->lights.size();
    bufferData.pData = m_impl->lights.data();
    Graphics::UpdateBuffer(m_impl->lightBuffer, bufferData);
}

void Renderer::Update()
{
    UpdateAnimators();
    UpdateCameras();
    UpdateLights();
}

void Renderer::CollectDrawCommands()
{
    m_impl->drawCmds.clear();

    EntityManager::ForEach<Transform, MeshFilter, MeshRenderer>(
        [&](Entity entity, const Transform& trx, const MeshFilter& mf, const MeshRenderer& mr)
        {
            GraphicsHandle animResources = INVALID_GRAPHICS_HANDLE;
            if (entity.HasComponent<Animator>())
            {
                const auto& anim = entity.GetComponent<Animator>();
                animResources = anim.GetResources();
            }

            if (mr.GetMaterialCount() == 0)
                return;

            for (const auto& material : mr.GetMaterials())
            {
                if (!material)
                    continue;

                const auto& materialData = material.GetData();

                GraphicsHandle resources = materialData.GetResources();
                for (const auto& entry : materialData.GetTextures())
                {
                    if (!entry.second)
                        Graphics::BindResource(resources, entry.first.c_str(), INVALID_GRAPHICS_HANDLE);
                    else
                        Graphics::BindResource(resources, entry.first.c_str(), entry.second->GetTexture());
                }
            }

            SizeType index = 0;
            for (const auto& mesh : mf.GetMeshes())
            {
                const auto& material = mr.GetMaterial(index++);
                index %= mr.GetMaterialCount();

                if (!mesh || !material)
                    continue;

                const auto& meshData = mesh.GetData();
                const auto& materialData = material.GetData();

                DrawCommandData cmd;
                cmd.model.meshMtx = meshData.GetMatrix();
                cmd.model.worldMtx = trx.GetMatrix();
                cmd.model.lightIndices = GetLightIndices(trx.GetPosition());

                cmd.vbuffers = meshData.GetVertexBuffers();
                cmd.ibuffer = meshData.GetIndexBuffer();
                cmd.numIndices = static_cast<u32>(meshData.GetTriangles().size());
                cmd.pipeline = materialData.GetPipeline();
                cmd.matResources = materialData.GetResources();
                cmd.animResources = animResources;

                m_impl->drawCmds.emplace_back(cmd);
            }
        });
}

void Renderer::DrawCommand(const GraphicsHandle pipeline, u32 numResourceBindings, const GraphicsHandle* pResourcesBindings, u32 numBuffers, const GraphicsHandle* pBuffers, const u64* offset, const GraphicsHandle indexBuffer, u32 count)
{
    if (m_impl->pipelineOverride != INVALID_GRAPHICS_HANDLE)
        Graphics::SetPipeline(m_impl->pipelineOverride);
    else
        Graphics::SetPipeline(pipeline);
    
    for (u32 i = 0; i < numResourceBindings; ++i)
    {
        auto resourceBinding = pResourcesBindings[i];
        if (resourceBinding != INVALID_GRAPHICS_HANDLE)
            Graphics::CommitResources(pipeline, resourceBinding);
    }

    Graphics::SetVertexBuffers(0, 1, pBuffers, offset);
    Graphics::SetIndexBuffer(indexBuffer, 0);

    DrawIndexedAttribs attribs;
    attribs.indexType = GraphicsValueType::UINT32;
    attribs.numIndices = count;
    Graphics::DrawIndexed(attribs);
}

void Renderer::DrawCommands()
{
    for (const auto& cmd : m_impl->drawCmds)
    {
        BufferData bufferData;
        bufferData.dataSize = sizeof(ModelData);
        bufferData.pData = &cmd.model;
        Graphics::UpdateBuffer(m_impl->modelBuffer, bufferData);

        GraphicsHandle resourceBindings[]
        {
            m_impl->resources,
            cmd.matResources,
            cmd.animResources
        };

        const u64 offset = 0;
        GraphicsHandle pBuffs[] = { cmd.vbuffers };

        DrawCommand(cmd.pipeline, 3, resourceBindings, 1, pBuffs, &offset, cmd.ibuffer, cmd.numIndices);
    }
}

void Renderer::BindConstants(const Mat4& viewMtx, const Mat4& projMtx, const Mat4& viewProjMtx)
{
    ConstantData constants;
    constants.view.viewMtx = viewMtx;
    constants.view.projMtx = projMtx;
    constants.view.viewProjMtx = viewProjMtx;

    BufferData bufferData;
    bufferData.dataSize = sizeof(ConstantData);
    bufferData.pData = &constants;
    Graphics::UpdateBuffer(m_impl->constantBuffer, bufferData);
}

void Renderer::Render()
{
    CollectDrawCommands();
    Graphics::UpdateDebugLines();

    i32 w, h;
    Window::GetSize(&w, &h);
    for (const auto& view : m_impl->views)
    {
        GraphicsHandle renderTarget = Graphics::GetCurrentBackBufferRT();
        GraphicsHandle depthStencil = Graphics::GetDepthBuffer();
        Graphics::SetRenderTarget(renderTarget, depthStencil);

        const f32 viewport[] = { 0.0f, 0.0f, (f32)w, (f32)h };
        Graphics::SetViewport(viewport);

        const f32 clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
        Graphics::ClearRenderTarget(renderTarget, clearColor);
        Graphics::ClearDepthStencil(depthStencil, GraphicsClearFlags::DEPTH, 1.0f, 0);
        
        BindConstants(view.viewMtx, view.projMtx, view.viewProjMtx);
        DrawCommands();

        Graphics::DrawDebugLines(view.viewProjMtx);
    }
}