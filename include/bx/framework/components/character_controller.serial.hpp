#pragma once

#include "bx/framework/components/character_controller.hpp"

#include <bx/engine/core/serial.serial.hpp>
#include <bx/engine/core/math.serial.hpp>

template <>
struct Serial<CharacterController>
{
	template <class Archive>
	static void Save(Archive& ar, CharacterController const& data)
	{
		ar(cereal::make_nvp("offset", data.m_offset));
		ar(cereal::make_nvp("width", data.m_width));
		ar(cereal::make_nvp("height", data.m_height));
		ar(cereal::make_nvp("stepHeight", data.m_stepHeight));
	}

	template <class Archive>
	static void Load(Archive& ar, CharacterController& data)
	{
		ar(cereal::make_nvp("offset", data.m_offset));
		ar(cereal::make_nvp("width", data.m_width));
		ar(cereal::make_nvp("height", data.m_height));
		ar(cereal::make_nvp("stepHeight", data.m_stepHeight));
	}
};

REGISTER_POLYMORPHIC_SERIAL(ComponentBase, CharacterController)