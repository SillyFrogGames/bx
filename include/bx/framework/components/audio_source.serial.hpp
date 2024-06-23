#pragma once

#include "bx/framework/components/audio_source.hpp"

#include <bx/engine/core/serial.serial.hpp>
#include <bx/engine/core/math.serial.hpp>

template <>
struct Serial<AudioSource>
{
	template <class Archive>
	static void Save(Archive& ar, AudioSource const& data)
	{
	}

	template <class Archive>
	static void Load(Archive& ar, AudioSource& data)
	{
	}
};

REGISTER_POLYMORPHIC_SERIAL(ComponentBase, AudioSource)