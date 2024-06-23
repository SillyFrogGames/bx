#pragma once

#include <bx/engine/core/math.hpp>
#include <bx/engine/containers/string.hpp>
#include <bx/engine/containers/list.hpp>
#include <bx/engine/containers/hash_map.hpp>

class Animation
{
public:
	struct PositionKey
	{
		Vec3 position;
		f32 timeStamp;
	};

	struct RotationKey
	{
		Quat rotation;
		f32 timeStamp;
	};

	struct ScaleKey
	{
		Vec3 scale;
		f32 timeStamp;
	};

	struct Keyframes
	{
		List<PositionKey> positionKeys;
		List<RotationKey> rotationKeys;
		List<ScaleKey> scaleKeys;
	};

public:
	Animation() {}
	Animation(const String& name, f32 duration, f32 tps, const HashMap<String, Keyframes>& channels)
		: m_name(name)
		, m_duration(duration)
		, m_ticksPerSecond(tps)
		, m_channels(channels)
	{}

	inline const String& GetName() const { return m_name; }
	inline const HashMap<String, Keyframes>& GetChannels() const { return m_channels; }

	inline f32 GetDuration() const { return m_duration; }
	inline f32 GetTicksPerSecond() const { return m_ticksPerSecond; }

private:
	template <typename T>
	friend class Serial;

	template <typename T>
	friend class Resource;

	String m_name;
	f32 m_duration = 0.0f;
	f32 m_ticksPerSecond = 0.0f;
	HashMap<String, Keyframes> m_channels;
};