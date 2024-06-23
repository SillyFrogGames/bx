#pragma once

#include <bx/framework/components/transform.hpp>
#include <bx/framework/components/camera.hpp>

class TransformGizmo
{
public:
	static bool Edit(f32 rect[4], const Camera& camera, Transform& trx, bool showControls, bool allowHotkeys);
};