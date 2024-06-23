#pragma once

#include <bx/engine/core/ecs.hpp>
#include <bx/engine/core/math.hpp>

class AudioSource : public Component<AudioSource>
{
public:
	AudioSource();

private:
	template <typename T>
	friend class Serial;

	template <typename T>
	friend class Inspector;
};