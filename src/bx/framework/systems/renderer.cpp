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

                GraphicsPipelineCreateInfo createInfo{};
                createInfo.name = Optional<String>::Some("Shader Pipeline");
                createInfo.vertexShader = shader.GetVertexShader();
                createInfo.fragmentShader = shader.GetFragmentShader();
                createInfo.vertexBuffers = { vertexBufferLayout };
                createInfo.colorTarget = Optional<ColorTargetState>::Some(colorTargetState);
                createInfo.cullMode = Optional<Face>::Some(Face::BACK);
                createInfo.layout = pipelineLayoutDescriptor;
                createInfo.depthFormat = Optional<TextureFormat>::Some(depthFormat);
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

    
}