#include "bx/engine/modules/graphics/backend/graphics_opengl.hpp"

#include "bx/engine/modules/graphics/type_validation.hpp"

#include "bx/engine/core/handle_pool.hpp"
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

    HashMap<HTexture, GLuint> textures;
    HashMap<HTextureView, GLuint> textureViews;
    HashMap<HBuffer, GLuint> buffers;
    HashMap<HShader, Shader> shaders;
    HashMap<HGraphicsPipeline, ShaderProgram> graphicsPipelines;
    HashMap<HComputePipeline, ShaderProgram> computePipelines;
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

const HBuffer& Graphics::EmptyBuffer()
{

}

const HTexture& Graphics::EmptyTexture()
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

HTexture Graphics::CreateTexture(const TextureCreateInfo& createInfo)
{
    CreateTextureWithDataPtr(createInfo, nullptr);
}

HTexture Graphics::CreateTextureWithDataPtr(const TextureCreateInfo& createInfo, const void* data)
{
    BX_ENSURE(ValidateTextureCreateInfo(createInfo));

    HTexture textureHandle = s->textureHandlePool.Create();
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

void Graphics::DestroyTexture(HTexture& texture)
{
    BX_ENSURE(texture);

    auto& textureIter = s->textures.find(texture);
    BX_ENSURE(textureIter != s->textures.end());
    glDeleteTextures(1, &textureIter->second);

    s->textures.erase(texture);
    s_createInfoCache->textureCreateInfos.erase(texture);
    s->textureHandlePool.Destroy(texture);
}

HTextureView Graphics::CreateTextureView(HTexture texture)
{
    BX_ENSURE(texture);

    HTextureView textureViewHandle = s->textureViewHandlePool.Create();
    
    auto& textureIter = s->textures.find(texture);
    BX_ENSURE(textureIter != s->textures.end());
    GLuint texture = textureIter->second;

    s->textureViews.insert(std::make_pair(textureViewHandle, texture));

    return textureViewHandle;
}

void Graphics::DestroyTextureView(HTextureView& textureView)
{
    BX_ENSURE(textureView);

    s->textureViews.erase(textureView);
    s->textureViewHandlePool.Destroy(textureView);
}

HSampler Graphics::CreateSampler(const SamplerCreateInfo& create)
{

}

void Graphics::DestroySampler(HSampler& sampler)
{

}

HBuffer Graphics::CreateBuffer(const BufferCreateInfo& createInfo)
{
    CreateBufferWithDataPtr(createInfo, nullptr);
}

HBuffer Graphics::CreateBufferWithDataPtr(const BufferCreateInfo& createInfo, const void* data)
{
    BX_ENSURE(ValidateBufferCreateInfo(createInfo));

    HBuffer bufferHandle = s->bufferHandlePool.Create();
    s_createInfoCache->bufferCreateInfos.insert(std::make_pair(bufferHandle, createInfo));

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBufferData(buffer, createInfo.size, data, GL_DYNAMIC_DRAW);

    s->buffers.insert(std::make_pair(bufferHandle, buffer));

    return bufferHandle;
}

void Graphics::DestroyBuffer(HBuffer& buffer)
{
    BX_ENSURE(buffer);

    auto& bufferIter = s->buffers.find(buffer);
    BX_ENSURE(bufferIter != s->buffers.end());
    glDeleteTextures(1, &bufferIter->second);

    s->buffers.erase(buffer);
    s_createInfoCache->bufferCreateInfos.erase(buffer);
    s->bufferHandlePool.Destroy(buffer);
}

HShader Graphics::CreateShader(const ShaderCreateInfo& createInfo)
{
    BX_ENSURE(ValidateShaderCreateInfo(createInfo));

    HShader shaderHandle = s->shaderHandlePool.Create();
    s_createInfoCache->shaderCreateInfos.insert(std::make_pair(shaderHandle, createInfo));

    GLenum type = ShaderTypeToGl(createInfo.shaderType);

    String name = createInfo.name.IsSome() ? createInfo.name.Unwrap() : "Unnamed";
    s->shaders.emplace(std::make_pair(shaderHandle, std::move(Shader(name, type, createInfo.src))));

    return shaderHandle;
}

void Graphics::DestroyShader(HShader& shader)
{
    BX_ENSURE(shader);

    auto& shaderIter = s->shaders.find(shader);
    BX_ENSURE(shaderIter != s->shaders.end());

    s->shaders.erase(shader);
    s_createInfoCache->shaderCreateInfos.erase(shader);
    s->shaderHandlePool.Destroy(shader);
}

HGraphicsPipeline Graphics::CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo)
{
    BX_ENSURE(ValidateGraphicsPipelineCreateInfo(createInfo));

    HGraphicsPipeline graphicsPipelineHandle = s->graphicsPipelineHandlePool.Create();
    s_createInfoCache->graphicsPipelineCreateInfos.insert(std::make_pair(graphicsPipelineHandle, createInfo));

    auto& vertShaderIter = s->shaders.find(createInfo.vertexShader);
    BX_ENSURE(vertShaderIter != s->shaders.end());
    auto& fragShaderIter = s->shaders.find(createInfo.fragmentShader);
    BX_ENSURE(fragShaderIter != s->shaders.end());

    String name = createInfo.name.IsSome() ? createInfo.name.Unwrap() : "Unnamed";
    s->graphicsPipelines.emplace(std::make_pair(graphicsPipelineHandle, std::move(ShaderProgram(name, List<Shader*>{ &vertShaderIter->second, &fragShaderIter->second }))));

    return graphicsPipelineHandle;
}

void Graphics::DestroyGraphicsPipeline(HGraphicsPipeline& graphicsPipeline)
{
    BX_ENSURE(graphicsPipeline);

    s->graphicsPipelines.erase(graphicsPipeline);
    s_createInfoCache->graphicsPipelineCreateInfos.erase(graphicsPipeline);
    s->graphicsPipelineHandlePool.Destroy(graphicsPipeline);
}

HComputePipeline Graphics::CreateComputePipeline(const ComputePipelineCreateInfo& createInfo)
{
    BX_ENSURE(ValidateComputePipelineCreateInfo(createInfo));

    HComputePipeline computePipelineHandle = s->computePipelineHandlePool.Create();
    s_createInfoCache->computePipelineCreateInfos.insert(std::make_pair(computePipelineHandle, createInfo));

    auto& shaderIter = s->shaders.find(createInfo.shader);
    BX_ENSURE(shaderIter != s->shaders.end());

    String name = createInfo.name.IsSome() ? createInfo.name.Unwrap() : "Unnamed";
    s->computePipelines.emplace(std::make_pair(computePipelineHandle, std::move(ShaderProgram(name, List<Shader*>{ &shaderIter->second }))));

    return computePipelineHandle;
}

void Graphics::DestroyComputePipeline(HComputePipeline& computePipeline)
{
    BX_ENSURE(computePipeline);

    s->computePipelines.erase(computePipeline);
    s_createInfoCache->computePipelineCreateInfos.erase(computePipeline);
    s->computePipelineHandlePool.Destroy(computePipeline);
}

HBindGroupLayout Graphics::GetBindGroupLayout(HGraphicsPipeline graphicsPipeline, u32 bindGroup)
{

}

HBindGroupLayout Graphics::GetBindGroupLayout(HComputePipeline computePipeline, u32 bindGroup)
{

}

HBindGroup Graphics::CreateBindGroup(const BindGroupCreateInfo& createInfo)
{

}

void Graphics::DestroyBindGroup(HBindGroup& bindGroup)
{

}

HRenderPass Graphics::BeginRenderPass(const RenderPassDescriptor& descriptor)
{

}

void Graphics::SetGraphicsPipeline(HRenderPass renderPass, HGraphicsPipeline graphicsPipeline)
{

}

void Graphics::SetVertexBuffer(HRenderPass renderPass, u32 slot, const BufferSlice& bufferSlice)
{

}

void Graphics::SetIndexBuffer(HRenderPass renderPass, const BufferSlice& bufferSlice, IndexFormat format)
{

}

void Graphics::SetBindGroup(HRenderPass renderPass, u32 index, HBindGroup bindGroup)
{

}

void Graphics::Draw(HRenderPass renderPass, u32 vertexCount, u32 firstVertex, u32 instanceCount, u32 firstInstance)
{

}

void Graphics::DrawIndexed(HRenderPass renderPass, u32 indexCount, u32 firstIndex, u32 baseVertex, u32 instanceCount, u32 firstInstance)
{

}

void Graphics::EndRenderPass(HRenderPass& renderPass)
{

}

void Graphics::WriteBufferPtr(HBuffer buffer, u64 offset, const void* data)
{

}

void Graphics::FlushBufferWrites()
{

}

void Graphics::WriteTexturePtr(HTexture texture, const u8* data, const ImageDataLayout& dataLayout, const Extend3D& size)
{

}

void Graphics::FlushTextureWrites()
{

}
