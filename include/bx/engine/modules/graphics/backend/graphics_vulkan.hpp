#pragma once

#include "bx/engine/modules/graphics.hpp"

struct ImGui_ImplVulkan_InitInfo;

class GraphicsVulkan
{
public:
    static ImGui_ImplVulkan_InitInfo ImGuiInitInfo();
};