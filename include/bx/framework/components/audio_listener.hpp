#pragma once

#include <bx/engine/core/ecs.hpp>
#include <bx/engine/core/math.hpp>

class AudioListener : public Component<AudioListener>
{
public:
	AudioListener();

private:
	template <typename T>
	friend class Serial;

	template <typename T>
	friend class Inspector;
};