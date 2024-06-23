#pragma once

#include <bx/engine/containers/string.hpp>

class AssetImporter
{
public:
	static bool ImportTexture(const String& filename);
	static bool ImportModel(const String& filename);
};