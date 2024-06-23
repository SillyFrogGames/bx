#pragma once

#include <bx/engine/core/ecs.hpp>
#include <bx/engine/core/math.hpp>

class Attributes : public Component<Attributes>
{
public:
	Attributes();

private:
	template <typename T>
	friend class Serial;

	template <typename T>
	friend class Inspector;
};