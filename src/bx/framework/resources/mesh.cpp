#include "bx/framework/resources/mesh.hpp"
#include "bx/framework/resources/mesh.serial.hpp"

#include <bx/engine/core/file.hpp>
#include <bx/engine/modules/graphics.hpp>

#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cstring>
#include <fstream>
#include <sstream>

template<>
bool Resource<Mesh>::Save(const String& filename, const Mesh& data)
{
    // TODO: Use some sort of compression (research needs to be done)

    // Serialize data
    std::ofstream stream(File::GetPath(filename), std::ios::binary);
    if (stream.fail())
        return false;

    cereal::PortableBinaryOutputArchive archive(stream);
    archive(cereal::make_nvp("mesh", data));

    return true;
}

template<>
bool Resource<Mesh>::Load(const String& filename, Mesh& data)
{
    // Deserialize data
    std::ifstream stream(File::GetPath(filename), std::ios::binary);
    if (stream.fail())
        return false;

    cereal::PortableBinaryInputArchive archive(stream);
    archive(cereal::make_nvp("mesh", data));

    // Build graphic components
    List<Mesh::Vertex> vertices;
    vertices.resize(data.m_vertices.size());

    for (SizeType i = 0; i < vertices.size(); i++)
    {
        auto& v = vertices[i];
        v.position = data.m_vertices[i];
        v.color = data.m_colors[i];
        v.normal = data.m_normals[i];
        v.tangent = data.m_tangents[i];
        v.uv = data.m_uvs[i];
        v.bones = data.m_bones[i];
        v.weights = data.m_weights[i];
    }

    BufferInfo vbInfo;
    vbInfo.strideBytes = sizeof(Mesh::Vertex);
    vbInfo.type = BufferType::VERTEX_BUFFER;
    vbInfo.usage = BufferUsage::IMMUTABLE;
    vbInfo.access = BufferAccess::READ;

    BufferData vbData;
    vbData.pData = vertices.data();
    vbData.dataSize = static_cast<u32>(vertices.size() * sizeof(Mesh::Vertex));

    data.m_vbuffers = Graphics::CreateBuffer(vbInfo, vbData);

    BufferInfo ibInfo;
    ibInfo.type = BufferType::INDEX_BUFFER;
    ibInfo.usage = BufferUsage::IMMUTABLE;
    ibInfo.access = BufferAccess::READ;

    BufferData ibData;
    ibData.pData = data.m_triangles.data();
    ibData.dataSize = static_cast<u32>(data.m_triangles.size() * sizeof(i32));

    data.m_ibuffer = Graphics::CreateBuffer(ibInfo, ibData);

    return true;
}

template<>
void Resource<Mesh>::Unload(const Mesh& data)
{
    Graphics::DestroyBuffer(data.m_vbuffers);
    Graphics::DestroyBuffer(data.m_ibuffer);
}