#include "bx/engine/modules/graphics/backend/graphics_opengl.hpp"

#include "bx/engine/modules/graphics/type_validation.hpp"

#include "bx/engine/core/file.hpp"
#include "bx/engine/core/profiler.hpp"
#include "bx/engine/core/memory.hpp"
#include "bx/engine/core/macros.hpp"
#include "bx/engine/containers/array.hpp"

#include "bx/engine/modules/window.hpp"
#include "bx/engine/modules/imgui.hpp"

#include "bx/engine/modules/graphics/backend/opengl/buffer.hpp"
#include "bx/engine/modules/graphics/backend/opengl/conversion.hpp"
#include "bx/engine/modules/graphics/backend/opengl/shader.hpp"
#include "bx/engine/modules/graphics/backend/opengl/validation.hpp"
using namespace Gl;

#ifdef BX_WINDOW_GLFW_BACKEND
#include "bx/engine/modules/window/backend/window_glfw.hpp"
#endif

struct State : NoCopy
{
    HandlePool<BufferApi> bufferHandlePool;
    HandlePool<TextureApi> textureHandlePool;
    HandlePool<TextureViewApi> textureViewHandlePool;
    HandlePool<SamplerApi> samplerHandlePool;
    HandlePool<ShaderApi> shaderHandlePool;
    HandlePool<GraphicsPipelineApi> graphicsPipelineHandlePool;
    HandlePool<ComputePipelineApi> computePipelineHandlePool;
    HandlePool<RenderPassApi> renderPassHandlePool;
    HandlePool<BindGroupApi> bindGroupHandlePool;
    HandlePool<BindGroupLayoutApi> bindGroupLayoutHandlePool;
    HandlePool<RenderPassApi> renderPassHandlePool;

    HashMap<TextureHandle, GLuint> textures;
    HashMap<TextureViewHandle, GLuint> textureViews;
    HashMap<BufferHandle, GLuint> buffers;
    HashMap<ShaderHandle, Shader> shaders;
    HashMap<GraphicsPipelineHandle, ShaderProgram> graphicsPipelines;
    HashMap<ComputePipelineHandle, ShaderProgram> computePipelines;
};
static std::unique_ptr<State> s;

b8 Graphics::Initialize()
{
    s_createInfoCache = std::make_unique<CreateInfoCache>();

    s = std::make_unique<State>();

    return true;
}

void Graphics::Reload()
{

}

void Graphics::Shutdown()
{
    s.reset();
}

void Graphics::NewFrame()
{

}

void Graphics::EndFrame()
{
    ImGuiImpl::EndFrame();
}

const BufferHandle& Graphics::EmptyBuffer()
{

}

const TextureHandle& Graphics::EmptyTexture()
{

}

TextureFormat Graphics::GetSwapchainFormat()
{
    // TODO: something like this in the WindowGLFW backend itself, can also be used on vulkan
    /*GLFWvidmode* vid_mode = glfwGetVideoMode(glfwGetWindowMonitor(win));
    vid_mode->redBits;
    vid_mode->greenBits;
    vid_mode->blueBits;*/
    // The resulting bits will then somehow need to be converted into a `TextureFormat`
    return TextureFormat::RGBA8_UNORM_SRGB;
}

TextureHandle Graphics::CreateTexture(const TextureCreateInfo& createInfo)
{
    CreateTexture(createInfo, nullptr);
}

TextureHandle Graphics::CreateTexture(const TextureCreateInfo& createInfo, const void* data)
{
    BX_ENSURE(ValidateTextureCreateInfo(createInfo));

    TextureHandle textureHandle = s->textureHandlePool.Create();
    s_createInfoCache->textureCreateInfos.insert(std::make_pair(textureHandle, createInfo));

    GLenum type = TextureDimensionToGl(createInfo.dimension, createInfo.size.depthOrArrayLayers);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(type, texture);

    if (type == GL_TEXTURE_1D)
    {
        // TODO
        BX_FAIL("TODO");
    }
    else if (type == GL_TEXTURE_2D || type == GL_TEXTURE_1D_ARRAY)
    {
        u32 height = (type == GL_TEXTURE_1D_ARRAY) ? createInfo.size.depthOrArrayLayers : createInfo.size.height;

        glTexImage2D(
            type,
            0,
            TextureFormatToGlInternalFormat(createInfo.format),
            createInfo.size.width,
            height,
            0,
            TextureFormatToGlFormat(createInfo.format),
            TextureFormatToGlType(createInfo.format),
            data
        );
    }
    else if (type == GL_TEXTURE_3D || type == GL_TEXTURE_2D_ARRAY)
    {
        glTexImage3D(
            type,
            0,
            TextureFormatToGlInternalFormat(createInfo.format),
            createInfo.size.width,
            createInfo.size.height,
            createInfo.size.depthOrArrayLayers,
            0,
            TextureFormatToGlFormat(createInfo.format),
            TextureFormatToGlType(createInfo.format),
            data
        );
    }

    s->textures.insert(std::make_pair(textureHandle, texture));

    return textureHandle;
}

void Graphics::DestroyTexture(TextureHandle& texture)
{
    BX_ENSURE(texture);

    auto& textureIter = s->textures.find(texture);
    BX_ENSURE(textureIter != s->textures.end());
    glDeleteTextures(1, &textureIter->second);

    s->textures.erase(texture);
    s_createInfoCache->textureCreateInfos.erase(texture);
    s->textureHandlePool.Destroy(texture);
}

TextureViewHandle Graphics::CreateTextureView(TextureHandle texture)
{
    BX_ENSURE(texture);

    TextureViewHandle textureViewHandle = s->textureViewHandlePool.Create();
    
    auto& textureIter = s->textures.find(texture);
    BX_ENSURE(textureIter != s->textures.end());
    GLuint texture = textureIter->second;

    s->textureViews.insert(std::make_pair(textureViewHandle, texture));

    return textureViewHandle;
}

void Graphics::DestroyTextureView(TextureViewHandle& textureView)
{
    BX_ENSURE(textureView);

    s->textureViews.erase(textureView);
    s->textureViewHandlePool.Destroy(textureView);
}

SamplerHandle Graphics::CreateSampler(const SamplerCreateInfo& create)
{

}

void Graphics::DestroySampler(SamplerHandle& sampler)
{

}

BufferHandle Graphics::CreateBuffer(const BufferCreateInfo& createInfo)
{
    CreateBuffer(createInfo, nullptr);
}

BufferHandle Graphics::CreateBuffer(const BufferCreateInfo& createInfo, const void* data)
{
    BX_ENSURE(ValidateBufferCreateInfo(createInfo));

    BufferHandle bufferHandle = s->bufferHandlePool.Create();
    s_createInfoCache->bufferCreateInfos.insert(std::make_pair(bufferHandle, createInfo));

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBufferData(buffer, createInfo.size, data, GL_DYNAMIC_DRAW);

    s->buffers.insert(std::make_pair(bufferHandle, buffer));

    return bufferHandle;
}

void Graphics::DestroyBuffer(BufferHandle& buffer)
{
    BX_ENSURE(buffer);

    auto& bufferIter = s->buffers.find(buffer);
    BX_ENSURE(bufferIter != s->buffers.end());
    glDeleteTextures(1, &bufferIter->second);

    s->buffers.erase(buffer);
    s_createInfoCache->bufferCreateInfos.erase(buffer);
    s->bufferHandlePool.Destroy(buffer);
}

ShaderHandle Graphics::CreateShader(const ShaderCreateInfo& createInfo)
{
    BX_ENSURE(ValidateShaderCreateInfo(createInfo));

    ShaderHandle shaderHandle = s->shaderHandlePool.Create();
    s_createInfoCache->shaderCreateInfos.insert(std::make_pair(shaderHandle, createInfo));

    GLenum type = ShaderTypeToGl(createInfo.shaderType);

    String name = createInfo.name.IsSome() ? createInfo.name.Unwrap() : "Unnamed";
    s->shaders.emplace(std::make_pair(shaderHandle, std::move(Shader(name, type, createInfo.src))));

    return shaderHandle;
}

void Graphics::DestroyShader(ShaderHandle& shader)
{
    BX_ENSURE(shader);

    auto& shaderIter = s->shaders.find(shader);
    BX_ENSURE(shaderIter != s->shaders.end());

    s->shaders.erase(shader);
    s_createInfoCache->shaderCreateInfos.erase(shader);
    s->shaderHandlePool.Destroy(shader);
}

GraphicsPipelineHandle Graphics::CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo)
{
    BX_ENSURE(ValidateGraphicsPipelineCreateInfo(createInfo));

    GraphicsPipelineHandle graphicsPipelineHandle = s->graphicsPipelineHandlePool.Create();
    s_createInfoCache->graphicsPipelineCreateInfos.insert(std::make_pair(graphicsPipelineHandle, createInfo));

    auto& vertShaderIter = s->shaders.find(createInfo.vertexShader);
    BX_ENSURE(vertShaderIter != s->shaders.end());
    auto& fragShaderIter = s->shaders.find(createInfo.fragmentShader);
    BX_ENSURE(fragShaderIter != s->shaders.end());

    String name = createInfo.name.IsSome() ? createInfo.name.Unwrap() : "Unnamed";
    s->graphicsPipelines.emplace(std::make_pair(graphicsPipelineHandle, std::move(ShaderProgram(name, List<Shader*>{ &vertShaderIter->second, &fragShaderIter->second }))));

    return graphicsPipelineHandle;
}

void Graphics::DestroyGraphicsPipeline(GraphicsPipelineHandle& graphicsPipeline)
{
    BX_ENSURE(graphicsPipeline);

    s->graphicsPipelines.erase(graphicsPipeline);
    s_createInfoCache->graphicsPipelineCreateInfos.erase(graphicsPipeline);
    s->graphicsPipelineHandlePool.Destroy(graphicsPipeline);
}

ComputePipelineHandle Graphics::CreateComputePipeline(const ComputePipelineCreateInfo& createInfo)
{
    BX_ENSURE(ValidateComputePipelineCreateInfo(createInfo));

    ComputePipelineHandle computePipelineHandle = s->computePipelineHandlePool.Create();
    s_createInfoCache->computePipelineCreateInfos.insert(std::make_pair(computePipelineHandle, createInfo));

    auto& shaderIter = s->shaders.find(createInfo.shader);
    BX_ENSURE(shaderIter != s->shaders.end());

    String name = createInfo.name.IsSome() ? createInfo.name.Unwrap() : "Unnamed";
    s->computePipelines.emplace(std::make_pair(computePipelineHandle, std::move(ShaderProgram(name, List<Shader*>{ &shaderIter->second }))));

    return computePipelineHandle;
}

void Graphics::DestroyComputePipeline(ComputePipelineHandle& computePipeline)
{
    BX_ENSURE(computePipeline);

    s->computePipelines.erase(computePipeline);
    s_createInfoCache->computePipelineCreateInfos.erase(computePipeline);
    s->computePipelineHandlePool.Destroy(computePipeline);
}

BindGroupLayoutHandle Graphics::GetBindGroupLayout(GraphicsPipelineHandle graphicsPipeline, u32 bindGroup)
{

}

BindGroupLayoutHandle Graphics::GetBindGroupLayout(ComputePipelineHandle computePipeline, u32 bindGroup)
{

}

BindGroupHandle Graphics::CreateBindGroup(const BindGroupCreateInfo& createInfo)
{

}

void Graphics::DestroyBindGroup(BindGroupHandle& bindGroup)
{

}

RenderPassHandle Graphics::BeginRenderPass(const RenderPassDescriptor& descriptor)
{
    // TODO: support multiple color attachments


}

void Graphics::SetGraphicsPipeline(GraphicsPipelineHandle graphicsPipeline)
{

}

void Graphics::SetVertexBuffer(u32 slot, const BufferSlice& bufferSlice)
{

}

void Graphics::SetIndexBuffer(const BufferSlice& bufferSlice, IndexFormat format)
{

}

void Graphics::SetBindGroup(u32 index, BindGroupHandle bindGroup)
{

}

void Graphics::Draw(u32 vertexCount, u32 firstVertex, u32 instanceCount, u32 firstInstance)
{

}

void Graphics::DrawIndexed(u32 indexCount, u32 firstIndex, u32 baseVertex, u32 instanceCount, u32 firstInstance)
{

}

void Graphics::EndRenderPass(RenderPassHandle& renderPass)
{

}

void Graphics::WriteBufferPtr(BufferHandle buffer, u64 offset, const void* data)
{

}

void Graphics::FlushBufferWrites()
{

}

void Graphics::WriteTexturePtr(TextureHandle texture, const u8* data, const ImageDataLayout& dataLayout, const Extend3D& size)
{

}

void Graphics::FlushTextureWrites()
{

}
