#include "bx/framework/systems/renderer.hpp"

#include "bx/framework/components/transform.hpp"
#include "bx/framework/components/camera.hpp"
#include "bx/framework/components/mesh_filter.hpp"
#include "bx/framework/components/mesh_renderer.hpp"
#include "bx/framework/components/animator.hpp"
#include "bx/framework/components/light.hpp"

#include <bx/engine/core/file.hpp>
#include <bx/engine/core/data.hpp>
#include <bx/engine/core/profiler.hpp>
#include <bx/engine/core/resource.hpp>
#include <bx/engine/containers/tree.hpp>
#include <bx/engine/modules/graphics.hpp>
#include <bx/engine/modules/window.hpp>

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

struct State : NoCopy
{
    HashMap<UUID, HGraphicsPipeline> shaderPipelines;

    HTexture colorTarget = HTexture::null;
    HTexture depthTarget = HTexture::null;
};
static std::unique_ptr<State> s;

struct ModelData
{
    Mat4 worldMtx = Mat4::Identity();
    Mat4 meshMtx = Mat4::Identity();
    Vec4i lightIndices = Vec4i(-1, -1, -1, -1);
};

void BuildShaderPipelines()
{
    VertexBufferLayout vertexBufferLayout{};
    vertexBufferLayout.stride = sizeof(Mesh::Vertex);
    vertexBufferLayout.attributes = {
        VertexAttribute(VertexFormat::FLOAT_32X3, offsetof(Mesh::Vertex, position), 32 * 3 / 4),
        VertexAttribute(VertexFormat::FLOAT_32X4, offsetof(Mesh::Vertex, color), 32 * 4 / 4),
        VertexAttribute(VertexFormat::FLOAT_32X3, offsetof(Mesh::Vertex, normal), 32 * 3 / 4),
        VertexAttribute(VertexFormat::FLOAT_32X3, offsetof(Mesh::Vertex, tangent), 32 * 3 / 4),
        VertexAttribute(VertexFormat::FLOAT_32X2, offsetof(Mesh::Vertex, uv), 32 * 2 / 4),
        VertexAttribute(VertexFormat::SINT_32X4,  offsetof(Mesh::Vertex, bones), 32 * 3 / 4),
        VertexAttribute(VertexFormat::FLOAT_32X4, offsetof(Mesh::Vertex, weights), 32 * 3 / 4)
    };

    PipelineLayoutDescriptor pipelineLayoutDescriptor{};
    pipelineLayoutDescriptor.bindGroupLayouts = {
        BindGroupLayoutEntry(0, ShaderStageFlags::VERTEX, BindingTypeDescriptor::UniformBuffer()),
        BindGroupLayoutEntry(1, ShaderStageFlags::VERTEX, BindingTypeDescriptor::UniformBuffer()),
        BindGroupLayoutEntry(2, ShaderStageFlags::VERTEX, BindingTypeDescriptor::UniformBuffer()),
        BindGroupLayoutEntry(3, ShaderStageFlags::FRAGMENT, BindingTypeDescriptor::Sampler()),
        BindGroupLayoutEntry(4, ShaderStageFlags::FRAGMENT, BindingTypeDescriptor::UniformBuffer()),
    };

    ColorTargetState colorTargetState{};
    colorTargetState.format = Graphics::GetTextureCreateInfo(s->colorTarget).format;

    TextureFormat depthFormat = Graphics::GetTextureCreateInfo(s->depthTarget).format;

    EntityManager::ForEach<Transform, MeshFilter, MeshRenderer>(
        [&](Entity entity, const Transform& trx, const MeshFilter& mf, const MeshRenderer& mr)
        {
            if (mr.GetMaterialCount() == 0)
                return;

            for (const auto& material : mr.GetMaterials())
            {
                if (!material)
                    continue;

                const Material& materialData = material.GetData();
                const Resource<Shader>& shaderResource = materialData.GetShader();
                const Shader& shader = shaderResource.GetData();

                if (s->shaderPipelines.find(shaderResource.GetUUID()) == s->shaderPipelines.end())
                {
                    GraphicsPipelineCreateInfo createInfo{};
                    createInfo.name = Optional<String>::Some("Shader Pipeline");
                    createInfo.vertexShader = shader.GetVertexShader();
                    createInfo.fragmentShader = shader.GetFragmentShader();
                    createInfo.vertexBuffers = { vertexBufferLayout };
                    createInfo.colorTarget = Optional<ColorTargetState>::Some(colorTargetState);
                    createInfo.cullMode = Optional<Face>::Some(Face::BACK);
                    createInfo.layout = pipelineLayoutDescriptor;
                    createInfo.depthFormat = Optional<TextureFormat>::Some(depthFormat);

                    HGraphicsPipeline graphicsPipeline = Graphics::CreateGraphicsPipeline(createInfo);
                    s->shaderPipelines.insert(std::make_pair(shaderResource.GetUUID(), graphicsPipeline));
                }
            }
        });
}

void Renderer::Initialize()
{
    s = std::make_unique<State>();
}

void Renderer::Shutdown()
{
    s.reset();
}

void Renderer::Update()
{
    if (Window::WasResized())
    {
        i32 w, h;
        Window::GetSize(&w, &h);

        TextureCreateInfo colorTargetCreateInfo{};
        colorTargetCreateInfo.name = Optional<String>::Some("Color Target");
        colorTargetCreateInfo.size = Extend3D(w, h, 1);
        colorTargetCreateInfo.format = TextureFormat::RGBA8_UNORM_SRGB;
        colorTargetCreateInfo.usageFlags = TextureUsageFlags::RENDER_ATTACHMENT;
        s->colorTarget = Graphics::CreateTexture(colorTargetCreateInfo);

        TextureCreateInfo depthTargetCreateInfo{};
        depthTargetCreateInfo.name = Optional<String>::Some("Depth Target");
        depthTargetCreateInfo.size = Extend3D(w, h, 1);
        depthTargetCreateInfo.format = TextureFormat::DEPTH24_PLUS_STENCIL8;
        depthTargetCreateInfo.usageFlags = TextureUsageFlags::RENDER_ATTACHMENT;
        s->depthTarget = Graphics::CreateTexture(depthTargetCreateInfo);
    }
}

void Renderer::Render()
{
    Graphics::UpdateDebugLines();

    HTextureView colorTargetView = Graphics::CreateTextureView(s->colorTarget);
    HTextureView depthTargetView = Graphics::CreateTextureView(s->depthTarget);

    RenderPassDescriptor renderPassDescriptor{};
    renderPassDescriptor.name = Optional<String>::Some("Draw Meshes");
    renderPassDescriptor.colorAttachments = { RenderPassColorAttachment(colorTargetView) };
    renderPassDescriptor.depthStencilAttachment = Optional<RenderPassDepthStencilAttachment>::Some(depthTargetView);

    HRenderPass renderPass = Graphics::BeginRenderPass(renderPassDescriptor);
    {
        EntityManager::ForEach<Transform, MeshFilter, MeshRenderer>(
            [&](Entity entity, const Transform& trx, const MeshFilter& mf, const MeshRenderer& mr)
            {
                if (mr.GetMaterialCount() == 0)
                    return;

                for (const auto& material : mr.GetMaterials())
                {
                    if (!material)
                        continue;

                    const auto& materialData = material.GetData();

                    /*GraphicsHandle resources = materialData.GetResources();
                    for (const auto& entry : materialData.GetTextures())
                    {
                        if (!entry.second)
                            Graphics::BindResource(resources, entry.first.c_str(), INVALID_GRAPHICS_HANDLE);
                        else
                            Graphics::BindResource(resources, entry.first.c_str(), entry.second->GetTexture());
                    }*/
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
                    const auto& shader = materialData.GetShader();

                    auto graphicsPipeline = s->shaderPipelines.find(shader.GetUUID())->second;

                    Graphics::SetGraphicsPipeline(renderPass, graphicsPipeline);
                    Graphics::SetVertexBuffer(renderPass, 0, BufferSlice(meshData.GetVertexBuffer()));
                    Graphics::SetIndexBuffer(renderPass, BufferSlice(meshData.GetIndexBuffer()), IndexFormat::UINT32);
                    // TODO: bind group
                    Graphics::DrawIndexed(renderPass, meshData.GetIndices().size());

                    /*DrawCommandData cmd;
                    cmd.model.meshMtx = meshData.GetMatrix();
                    cmd.model.worldMtx = trx.GetMatrix();
                    cmd.model.lightIndices = GetLightIndices(trx.GetPosition());

                    cmd.vbuffers = meshData.GetVertexBuffers();
                    cmd.ibuffer = meshData.GetIndexBuffer();
                    cmd.numIndices = static_cast<u32>(meshData.GetTriangles().size());
                    cmd.pipeline = materialData.GetPipeline();
                    cmd.matResources = materialData.GetResources();
                    cmd.animResources = animResources;

                    m_impl->drawCmds.emplace_back(cmd);*/
                }
            });
    }
    Graphics::EndRenderPass(renderPass);
}