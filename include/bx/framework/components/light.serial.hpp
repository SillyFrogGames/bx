#pragma once

#include "bx/framework/components/light.hpp"

#include <bx/engine/core/serial.serial.hpp>
#include <bx/engine/core/math.serial.hpp>

template <>
struct Serial<Light>
{
	template <class Archive>
	static void Save(Archive& ar, Light const& data)
	{
		ar(cereal::make_nvp("intensity", data.m_intensity));
		ar(cereal::make_nvp("constant", data.m_constant));
		ar(cereal::make_nvp("linear", data.m_linear));
		ar(cereal::make_nvp("quadratic", data.m_quadratic));
		ar(cereal::make_nvp("cutoff", data.m_cutoff));
		ar(cereal::make_nvp("outerCutoff", data.m_outerCutoff));
		ar(cereal::make_nvp("color", data.m_color));
	}

	template <class Archive>
	static void Load(Archive& ar, Light& data)
	{
		ar(cereal::make_nvp("intensity", data.m_intensity));
		ar(cereal::make_nvp("constant", data.m_constant));
		ar(cereal::make_nvp("linear", data.m_linear));
		ar(cereal::make_nvp("quadratic", data.m_quadratic));
		ar(cereal::make_nvp("cutoff", data.m_cutoff));
		ar(cereal::make_nvp("outerCutoff", data.m_outerCutoff));
		ar(cereal::make_nvp("color", data.m_color));
	}
};

REGISTER_POLYMORPHIC_SERIAL(ComponentBase, Light)