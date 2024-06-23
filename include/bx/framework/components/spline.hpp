#pragma once

#include <bx/engine/core/ecs.hpp>
#include <bx/engine/core/math.hpp>

class Spline : public Component<Spline>
{
public:
	Spline();

private:
	template <typename T>
	friend class Serial;

	template <typename T>
	friend class Inspector;
};