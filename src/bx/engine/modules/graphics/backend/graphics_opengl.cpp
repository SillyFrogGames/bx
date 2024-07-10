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
#include "bx/engine/modules/graphics/backend/opengl/shader.hpp"
#include "bx/engine/modules/graphics/backend/opengl/validation.hpp"
using namespace Gl;

#ifdef BX_WINDOW_GLFW_BACKEND
#include "bx/engine/modules/window/backend/window_glfw.hpp"
#endif

struct State : NoCopy
{
    
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
    BX_ENSURE(ValidateTextureCreateInfo(createInfo));


}

HTexture Graphics::CreateTextureWithDataPtr(const TextureCreateInfo& createInfo, const void* data)
{

}

void Graphics::DestroyTexture(HTexture& texture)
{

}

HTextureView Graphics::CreateTextureView(HTexture texture)
{

}

void Graphics::DestroyTextureView(HTextureView& textureView)
{

}

HSampler Graphics::CreateSampler(const SamplerCreateInfo& create)
{

}

void Graphics::DestroySampler(HSampler& sampler)
{

}

HBuffer Graphics::CreateBuffer(const BufferCreateInfo& createInfo)
{

}

HBuffer Graphics::CreateBufferWithDataPtr(const BufferCreateInfo& createInfo, const void* data)
{

}

void Graphics::DestroyBuffer(HBuffer& buffer)
{

}

HShader Graphics::CreateShader(const ShaderCreateInfo& createInfo)
{

}

void Graphics::DestroyShader(HShader& shader)
{

}

HGraphicsPipeline Graphics::CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo)
{

}

void Graphics::DestroyGraphicsPipeline(HGraphicsPipeline& graphicsPipeline)
{

}

HComputePipeline Graphics::CreateComputePipeline(const ComputePipelineCreateInfo& createInfo)
{

}

void Graphics::DestroyComputePipeline(HComputePipeline& graphicsPipeline)
{

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
