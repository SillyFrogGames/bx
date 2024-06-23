#pragma once

#include "bx/framework/components/audio_listener.hpp"

#include <bx/engine/core/serial.serial.hpp>
#include <bx/engine/core/math.serial.hpp>

template <>
struct Serial<AudioListener>
{
	template <class Archive>
	static void Save(Archive& ar, AudioListener const& data)
	{
	}

	template <class Archive>
	static void Load(Archive& ar, AudioListener& data)
	{
	}
};

REGISTER_POLYMORPHIC_SERIAL(ComponentBase, AudioListener)