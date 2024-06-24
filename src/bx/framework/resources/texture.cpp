#include "bx/framework/resources/texture.hpp"
#include "bx/framework/resources/texture.serial.hpp"

#include <bx/engine/core/file.hpp>
#include <bx/engine/modules/graphics.hpp>

#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cstring>
#include <fstream>
#include <sstream>

template<>
bool Resource<Texture>::Save(const String& filename, const Texture& data)
{
    // TODO: Use astc compression: https://developer.nvidia.com/astc-texture-compression-for-game-assets
    // Possibly using this lib: https://github.com/ARM-software/astc-encoder

    std::ofstream stream(File::GetPath(filename), std::ios::binary);
    if (stream.fail())
        return false;

    cereal::PortableBinaryOutputArchive archive(stream);
    archive(cereal::make_nvp("texture", data));

    return true;
}

template<>
bool Resource<Texture>::Load(const String& filename, Texture& data)
{
    // Deserialize data
    std::ifstream stream(File::GetPath(filename), std::ios::binary);
    if (stream.fail())
        return false;

    cereal::PortableBinaryInputArchive archive(stream);
    archive(cereal::make_nvp("texture", data));

    // Build graphic components
    TextureInfo textureInfo;
    textureInfo.width = data.width;
    textureInfo.height = data.height;
    textureInfo.format = TextureFormat::RGBA8_UNORM;
    textureInfo.flags = TextureFlags::SHADER_RESOURCE;

    BufferData bufferData;
    bufferData.dataSize = static_cast<u32>(data.pixels.size());
    bufferData.pData = data.pixels.data();

    data.m_texture = Graphics::CreateTexture(textureInfo, bufferData);

    return true;
}

template<>
void Resource<Texture>::Unload(const Texture& data)
{
    // TODO
}