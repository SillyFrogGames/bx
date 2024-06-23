#pragma once

#include "bx/framework/resources/texture.hpp"

#include <bx/engine/core/serial.serial.hpp>
#include <bx/engine/core/math.serial.hpp>
#include <bx/engine/core/resource.serial.hpp>
#include <bx/engine/containers/string.serial.hpp>
#include <bx/engine/containers/list.serial.hpp>
#include <bx/engine/containers/hash_map.serial.hpp>
#include <bx/engine/containers/tree.serial.hpp>

template <>
class Serial<Texture>
{
public:
	template<class Archive>
	static void Save(Archive& ar, const Texture& data)
	{
		ar(cereal::make_nvp("channels", data.channels));
		ar(cereal::make_nvp("width", data.width));
		ar(cereal::make_nvp("height", data.height));
		ar(cereal::make_nvp("depth", data.depth));
		ar(cereal::make_nvp("pixels", data.pixels));
	}

	template<class Archive>
	static void Load(Archive& ar, Texture& data)
	{
		ar(cereal::make_nvp("channels", data.channels));
		ar(cereal::make_nvp("width", data.width));
		ar(cereal::make_nvp("height", data.height));
		ar(cereal::make_nvp("depth", data.depth));
		ar(cereal::make_nvp("pixels", data.pixels));
	}
};
REGISTER_SERIAL(Texture);
