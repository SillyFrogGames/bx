#include "bx/framework/resources/material.hpp"
#include "bx/framework/resources/material.serial.hpp"

#include <bx/engine/core/file.hpp>
#include <bx/engine/modules/graphics.hpp>

#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cstring>
#include <fstream>
#include <sstream>

void Material::BuildPipeline()
{
    if (m_pipeline != INVALID_GRAPHICS_HANDLE)
    {
        m_pipeline = INVALID_GRAPHICS_HANDLE;
        Graphics::DestroyPipeline(m_pipeline);
    }

    if (m_resources != INVALID_GRAPHICS_HANDLE)
    {
        m_resources = INVALID_GRAPHICS_HANDLE;
        Graphics::DestroyResourceBinding(m_resources);
    }

    if (!m_shader)
        return;

    const auto& shaderData = m_shader.GetData();

    // Build graphic components
    PipelineInfo pipelineInfo;

    pipelineInfo.numRenderTargets = 1;
    pipelineInfo.renderTargetFormats[0] = Graphics::GetColorBufferFormat();
    pipelineInfo.depthStencilFormat = Graphics::GetDepthBufferFormat();

    pipelineInfo.topology = PipelineTopology::TRIANGLES;
    pipelineInfo.faceCull = PipelineFaceCull::CCW;
    pipelineInfo.depthEnable = true;

    LayoutElement layoutElems[] =
    {
        LayoutElement { 0, 0, 3, GraphicsValueType::FLOAT32, false, 0, 0 },
        LayoutElement { 1, 0, 4, GraphicsValueType::FLOAT32, false, 0, 0 },
        LayoutElement { 2, 0, 3, GraphicsValueType::FLOAT32, false, 0, 0 },
        LayoutElement { 3, 0, 3, GraphicsValueType::FLOAT32, false, 0, 0 },
        LayoutElement { 4, 0, 2, GraphicsValueType::FLOAT32, false, 0, 0 },
        LayoutElement { 5, 0, 4, GraphicsValueType::INT32, false, 0, 0 },
        LayoutElement { 6, 0, 4, GraphicsValueType::FLOAT32, false, 0, 0 }
    };

    pipelineInfo.layoutElements = layoutElems;
    pipelineInfo.numElements = 7;

    pipelineInfo.vertShader = shaderData.GetVertex();
    pipelineInfo.pixelShader = shaderData.GetPixel();

    m_pipeline = Graphics::CreatePipeline(pipelineInfo);

    ResourceBindingElement resourceElems[] =
    {
        ResourceBindingElement { ShaderType::PIXEL, "Albedo", 1, ResourceBindingType::TEXTURE, ResourceBindingAccess::DYNAMIC }
    };

    ResourceBindingInfo resourceBindingInfo;
    resourceBindingInfo.resources = resourceElems;
    resourceBindingInfo.numResources = 1;

    m_resources = Graphics::CreateResourceBinding(resourceBindingInfo);
}

template<>
bool Resource<Material>::Save(const String& filename, const Material& data)
{
    // Serialize data
    std::ofstream stream(File::GetPath(filename));
    if (stream.fail())
        return false;

    cereal::JSONOutputArchive archive(stream);
    archive(cereal::make_nvp("material", data));

    return true;
}

template<>
bool Resource<Material>::Load(const String& filename, Material& data)
{
    // Deserialize data
    std::ifstream stream(File::GetPath(filename));
    if (stream.fail())
        return false;

    cereal::JSONInputArchive archive(stream);
    archive(cereal::make_nvp("material", data));

    data.BuildPipeline();
    return true;
}

template<>
void Resource<Material>::Unload(const Material& data)
{
    Graphics::DestroyPipeline(data.m_pipeline);
}