#include "bx/engine/modules/graphics/toolkit/id_pass.hpp"

#include "bx/engine/containers/lazy_init.hpp"

const char* ID_PASS_SHADER_SRC = R""""(

#ifdef VERTEX

layout (location = 0) in vec3 Position;

flat out uvec2 Frag_EntityID;

layout (binding = 0, std140) uniform ConstantBuffer
{
    mat4 ViewProjMtx;
};

layout (binding = 1, std140) uniform ModelBuffer
{
    mat4 WorldMeshMtx;
    uvec2 EntityID;
};

#endif // VERTEX

#ifdef FRAGMENT

flat in uvec2 Frag_EntityID;

layout (location = 0) out uvec2 Out_Color;

void main()
{
    Out_Color = Frag_EntityID;
}

#endif // FRAGMENT

)"""";


struct IdPipelineArgs
{
    TextureHandle colorTarget;
    TextureHandle depthTarget;

    b8 operator==(const IdPipelineArgs& other) const { return colorTarget == other.colorTarget && depthTarget == other.depthTarget; }
};

template <>
struct std::hash<IdPipelineArgs>
{
    std::size_t operator()(const IdPipelineArgs& h) const
    {
        // TODO: fix
        return std::hash<TextureHandle>()(h.colorTarget) & std::hash<TextureHandle>()(h.depthTarget);
    }
};

struct IdPipeline : public LazyInitMap<IdPipeline, GraphicsPipelineHandle, IdPipelineArgs>
{
    IdPipeline(const IdPipelineArgs& args)
    {
        ShaderCreateInfo vertexCreateInfo{};
        vertexCreateInfo.name = Optional<String>::Some("Id Vertex Shader");
        vertexCreateInfo.shaderType = ShaderType::VERTEX;
        vertexCreateInfo.src = ID_PASS_SHADER_SRC;
        ShaderHandle vertexShader = Graphics::CreateShader(vertexCreateInfo);

        ShaderCreateInfo fragmentCreateInfo{};
        fragmentCreateInfo.name = Optional<String>::Some("Id Fragment Shader");
        fragmentCreateInfo.shaderType = ShaderType::FRAGMENT;
        fragmentCreateInfo.src = ID_PASS_SHADER_SRC;
        ShaderHandle fragmentShader = Graphics::CreateShader(fragmentCreateInfo);

        PipelineLayoutDescriptor pipelineLayoutDescriptor{};
        pipelineLayoutDescriptor.bindGroupLayouts = {
            BindGroupLayoutDescriptor(0, {
                BindGroupLayoutEntry(0, ShaderStageFlags::VERTEX, BindingTypeDescriptor::UniformBuffer()),
                BindGroupLayoutEntry(1, ShaderStageFlags::VERTEX, BindingTypeDescriptor::UniformBuffer())
            })
        };

        VertexBufferLayout vertexBufferLayout{};
        vertexBufferLayout.stride = 1;//TODO!!
        vertexBufferLayout.attributes = {
            VertexAttribute(VertexFormat::FLOAT_32X3, 0, 0)
        };

        ColorTargetState colorTargetState{};
        colorTargetState.format = Graphics::GetTextureCreateInfo(args.colorTarget).format;

        TextureFormat depthFormat = Graphics::GetTextureCreateInfo(args.depthTarget).format;

        GraphicsPipelineCreateInfo pipelineCreateInfo{};
        pipelineCreateInfo.name = Optional<String>::Some("Present Pipeline");
        pipelineCreateInfo.layout = pipelineLayoutDescriptor;
        pipelineCreateInfo.vertexShader = vertexShader;
        pipelineCreateInfo.fragmentShader = fragmentShader;
        pipelineCreateInfo.vertexBuffers = { vertexBufferLayout };
        pipelineCreateInfo.cullMode = Optional<Face>::None();
        pipelineCreateInfo.colorTarget = Optional<ColorTargetState>::Some(colorTargetState);
        pipelineCreateInfo.depthFormat = Optional<TextureFormat>::Some(depthFormat);
        data = Graphics::CreateGraphicsPipeline(pipelineCreateInfo);

        Graphics::DestroyShader(vertexShader);
        Graphics::DestroyShader(fragmentShader);
    }
};

HashMap<IdPipelineArgs, std::unique_ptr<IdPipeline>> LazyInitMap<IdPipeline, GraphicsPipelineHandle, IdPipelineArgs>::cache = {};

IdPass::IdPass(TextureHandle colorTarget, TextureHandle depthTarget)
    : colorTarget(colorTarget), depthTarget(depthTarget)
{
    const TextureCreateInfo& textureCreateInfo = Graphics::GetTextureCreateInfo(colorTarget);
    width = textureCreateInfo.size.width;
    height = textureCreateInfo.size.height;

    colorTargetView = Graphics::CreateTextureView(colorTarget);
    depthTargetView = Graphics::CreateTextureView(depthTarget);

    // TODO: const buffer
    // TODO: model buffer with max limit

    /*BindGroupCreateInfo createInfo{};
    createInfo.name = Optional<String>::Some("Id BindGroup");
    createInfo.layout = Graphics::GetBindGroupLayout(IdPipeline::Get(), 0);
    createInfo.entries = {
        BindGroupEntry(0, BindingResource::TextureView(textureView)),
    };
    bindGroup = Graphics::CreateBindGroup(createInfo);*/
}

IdPass::~IdPass()
{
    //Graphics::DestroyBindGroup(bindGroup);
    Graphics::DestroyTextureView(colorTargetView);
    Graphics::DestroyTextureView(depthTargetView);
}

void IdPass::Dispatch()
{
    RenderPassDescriptor renderPassDescriptor{};
    renderPassDescriptor.name = Optional<String>::Some("Id");
    renderPassDescriptor.colorAttachments = { RenderPassColorAttachment(Graphics::GetSwapchainColorTargetView()) };

    RenderPassHandle renderPass = Graphics::BeginRenderPass(renderPassDescriptor);
    {
        // TODO: loop over ecs, prepare and upload mesh ids and matrices, then draw

        Graphics::SetGraphicsPipeline(IdPipeline::Get({ colorTarget, depthTarget }));
        Graphics::SetBindGroup(0, bindGroup);
        Graphics::Draw(3);
    }
    Graphics::EndRenderPass(renderPass);
}

void IdPass::ClearPipelineCache()
{
    IdPipeline::Clear();
}