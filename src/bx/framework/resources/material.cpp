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

    return true;
}

template<>
void Resource<Material>::Unload(Material& data)
{
}