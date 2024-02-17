#pragma once

#include <Core/Plugin.hpp>

class Dummy : public Plugin
{
public:
	const char* Name() override { return "Dummy"; }
	int Version() override { return BX_ENGINE_VERSION; }

	void Initialize() override;
	void Shutdown() override;
};