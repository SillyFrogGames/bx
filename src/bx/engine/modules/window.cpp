#include "Engine/Modules/Window.hpp"

#include "Engine/Core/Log.hpp"
#include "Engine/Core/Data.hpp"
#include "Engine/Core/Profiler.hpp"

#include <stdlib.h>

static int screenWidth = 1;
static int screenHeight = 1;

int Screen::GetWidth()
{
	return screenWidth;
}

int Screen::GetHeight()
{
	return screenHeight;
}

void Screen::SetWidth(int width)
{
	screenWidth = width;
	Data::SetInt("Width", width, DataTarget::SYSTEM);
}

void Screen::SetHeight(int height)
{
	screenHeight = height;
	Data::SetInt("Height", height, DataTarget::SYSTEM);
}