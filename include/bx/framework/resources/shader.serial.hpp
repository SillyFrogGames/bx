#pragma once

#include "bx/framework/resources/shader.hpp"

#include <bx/engine/core/serial.serial.hpp>
#include <bx/engine/core/math.serial.hpp>
#include <bx/engine/core/resource.serial.hpp>
#include <bx/engine/containers/string.serial.hpp>
#include <bx/engine/containers/list.serial.hpp>
#include <bx/engine/containers/hash_map.serial.hpp>
#include <bx/engine/containers/tree.serial.hpp>

template <>
class Serial<Shader>
{
public:
	template<class Archive>
	static void Save(Archive& ar, const Shader& data)
	{
		//ar(cereal::make_nvp("source", data.m_source));
	}

	template<class Archive>
	static void Load(Archive& ar, Shader& data)
	{
		//ar(cereal::make_nvp("source", data.m_source));
		//data.Build();
	}
};
REGISTER_SERIAL(Shader);