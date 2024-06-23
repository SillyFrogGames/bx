#include "bx/framework/resources/shader.hpp"
#include "bx/framework/resources/shader.serial.hpp"

#include <bx/engine/core/file.hpp>
#include <bx/engine/modules/graphics.hpp>

#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cstring>
#include <fstream>
#include <sstream>

template<>
bool Resource<Shader>::Save(const String& filename, const Shader& data)
{
    std::ofstream stream(File::GetPath(filename));
    if (stream.fail())
        return false;

    cereal::JSONOutputArchive archive(stream);
    archive(data);

    return true;
}

template<>
bool Resource<Shader>::Load(const String& filename, Shader& data)
{
    std::ifstream stream(File::GetPath(filename));
    if (stream.fail())
        return false;

    std::stringstream ss;
    ss << stream.rdbuf();

    data.SetSource(ss.str());

    ShaderInfo shaderInfo;

    shaderInfo.shaderType = ShaderType::VERTEX;
    shaderInfo.source = data.m_source.c_str();
    data.m_vertex = Graphics::CreateShader(shaderInfo);

    shaderInfo.shaderType = ShaderType::PIXEL;
    shaderInfo.source = data.m_source.c_str();
    data.m_pixel = Graphics::CreateShader(shaderInfo);

    return true;
}

template<>
void Resource<Shader>::Unload(const Shader& data)
{
    Graphics::DestroyShader(data.m_vertex);
    Graphics::DestroyShader(data.m_pixel);
}