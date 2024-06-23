#include "Framework/Resources/Skeleton.hpp"
#include "Framework/Resources/Skeleton.serial.hpp"

#include <Engine/Core/File.hpp>
#include <Engine/Modules/Graphics.hpp>

#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cstring>
#include <fstream>
#include <sstream>

template<>
bool Resource<Skeleton>::Save(const String& filename, const Skeleton& data)
{
    // Serialize data
    std::ofstream stream(File::GetPath(filename), std::ios::binary);
    if (stream.fail())
        return false;

    cereal::PortableBinaryOutputArchive archive(stream);
    archive(cereal::make_nvp("skeleton", data));

    return true;
}

template<>
bool Resource<Skeleton>::Load(const String& filename, Skeleton& data)
{
    // Deserialize data
    std::ifstream stream(File::GetPath(filename), std::ios::binary);
    if (stream.fail())
        return false;

    cereal::PortableBinaryInputArchive archive(stream);
    archive(cereal::make_nvp("skeleton", data));

    return true;
}

template<>
void Resource<Skeleton>::Unload(const Skeleton& data)
{
}