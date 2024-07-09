#include "bx/framework/resources/material.hpp"
#include "bx/framework/resources/material.serial.hpp"

#include <bx/engine/core/file.hpp>
#include <bx/engine/modules/graphics.hpp>

#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cstring>
#include <fstream>
#include <sstream>

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

    HTexture albedoTexture = data.m_textures["Albedo"].GetData().GetTexture();
    HTextureView albedoTextureView = Graphics::CreateTextureView(albedoTexture); // TODO: handle leak! don't care atm

    BindGroupCreateInfo createInfo{};
    createInfo.name = Optional<String>::Some("Material Bind Group");
    createInfo.entries = {
        // TODO: 3 is a bit weird, emulate bind GROUPS on opengl
        BindGroupEntry(3, BindingResource::TextureView(albedoTextureView))
    };

    data.m_bindGroup = Graphics::CreateBindGroup(createInfo);

    return true;
}

template<>
void Resource<Material>::Unload(Material& data)
{
    Graphics::DestroyBindGroup(data.m_bindGroup);
}

BindGroupLayoutDescriptor Material::GetBindGroupLayout()
{
    return BindGroupLayoutDescriptor(Material::SHADER_BIND_GROUP, {
        BindGroupLayoutEntry(3, ShaderStageFlags::FRAGMENT, BindingTypeDescriptor::Sampler())           // layout (binding = 3) uniform sampler2D Albedo;
    });
}