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
    if (stream.fail() || !stream.good())
        return false;
    std::stringstream ss;
    ss << stream.rdbuf();
    data.SetSource(ss.str());

    ShaderCreateInfo vertexCreateInfo{};
    vertexCreateInfo.name = Optional<String>::Some(Log::Format("{} Vertex Shader", filename));
    vertexCreateInfo.shaderType = ShaderType::VERTEX;
    vertexCreateInfo.src = "#define VERTEX\n" + data.m_source; // TODO: remove this cardinal sin
    data.m_vertexShader = Graphics::CreateShader(vertexCreateInfo);

    ShaderCreateInfo fragmentCreateInfo{};
    fragmentCreateInfo.name = Optional<String>::Some(Log::Format("{} Fragment Shader", filename));
    fragmentCreateInfo.shaderType = ShaderType::FRAGMENT;
    fragmentCreateInfo.src = "#define PIXEL\n" + data.m_source;
    data.m_fragmentShader = Graphics::CreateShader(fragmentCreateInfo);

    return true;
}

template<>
void Resource<Shader>::Unload(Shader& data)
{
    Graphics::DestroyShader(data.m_vertexShader);
    Graphics::DestroyShader(data.m_fragmentShader);
}