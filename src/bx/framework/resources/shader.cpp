#include "bx/framework/resources/shader.hpp"
#include "bx/framework/resources/shader.serial.hpp"

#include <bx/engine/core/file.hpp>
#include <bx/engine/core/guard.hpp>
#include <bx/engine/core/stream.hpp>
#include <bx/engine/containers/hash_set.hpp>
#include <bx/engine/modules/graphics.hpp>

#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

static String ResolveIncludes(const String& source, HashSet<String>& includedFiles)
{
    InputStringStream shaderStream(source);
    OutputStringStream resolvedShader;

    String line;
    while (shaderStream.GetLine(line))
    {
        SizeType includePos = line.find("#include");
        if (includePos != std::string::npos)
        {
            SizeType start = line.find("\"", includePos) + 1;
            SizeType end = line.find("\"", start);
            String includeFile = line.substr(start, end - start);

            if (includedFiles.find(includeFile) != includedFiles.end())
            {
                throw Exception("Circular include detected: " + includeFile);
            }

            includedFiles.insert(includeFile);
            String includedSource = File::ReadTextFile(includeFile);
            resolvedShader << ResolveIncludes(includedSource, includedFiles) << "\n";
            includedFiles.erase(includeFile);
        }
        else
        {
            resolvedShader << line << "\n";
        }
    }

    return resolvedShader.GetString();
}

static String PreprocessShader(const String& source)
{
    HashSet<String> includedFiles;
    return ResolveIncludes(source, includedFiles);
}

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

    String source = PreprocessShader(ss.str());
    data.SetSource(source);

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