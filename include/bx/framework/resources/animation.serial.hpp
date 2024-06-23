#pragma once

#include "bx/framework/resources/animation.hpp"

#include <bx/engine/core/serial.serial.hpp>
#include <bx/engine/core/math.serial.hpp>
#include <bx/engine/core/resource.serial.hpp>
#include <bx/engine/containers/string.serial.hpp>
#include <bx/engine/containers/list.serial.hpp>
#include <bx/engine/containers/hash_map.serial.hpp>
#include <bx/engine/containers/tree.serial.hpp>

template<class Archive>
void serialize(Archive& ar, Animation::PositionKey& data)
{
	ar(cereal::make_nvp("position", data.position));
	ar(cereal::make_nvp("timeStamp", data.timeStamp));
}

template<class Archive>
void serialize(Archive& ar, Animation::RotationKey& data)
{
	ar(cereal::make_nvp("rotation", data.rotation));
	ar(cereal::make_nvp("timeStamp", data.timeStamp));
}

template<class Archive>
void serialize(Archive& ar, Animation::ScaleKey& data)
{
	ar(cereal::make_nvp("scale", data.scale));
	ar(cereal::make_nvp("timeStamp", data.timeStamp));
}

template<class Archive>
void serialize(Archive& ar, Animation::Keyframes& data)
{
	ar(cereal::make_nvp("positionKeys", data.positionKeys));
	ar(cereal::make_nvp("rotationKeys", data.rotationKeys));
	ar(cereal::make_nvp("scaleKeys", data.scaleKeys));
}

template <>
class Serial<Animation>
{
public:
	template<class Archive>
	static void Save(Archive& ar, const Animation& data)
	{
		ar(cereal::make_nvp("name", data.m_name));
		ar(cereal::make_nvp("duration", data.m_duration));
		ar(cereal::make_nvp("ticksPerSecond", data.m_ticksPerSecond));
		ar(cereal::make_nvp("channels", data.m_channels));
	}

	template<class Archive>
	static void Load(Archive& ar, Animation& data)
	{
		ar(cereal::make_nvp("name", data.m_name));
		ar(cereal::make_nvp("duration", data.m_duration));
		ar(cereal::make_nvp("ticksPerSecond", data.m_ticksPerSecond));
		ar(cereal::make_nvp("channels", data.m_channels));
	}
};
REGISTER_SERIAL(Animation);