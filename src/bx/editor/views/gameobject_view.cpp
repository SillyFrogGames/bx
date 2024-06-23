#include "bx/editor/views/gameobject_view.hpp"

#include <bx/framework/gameobject.hpp>

#include <imgui.h>
#include <imgui_internal.h>
#include <implot.h>
#include <IconsFontAwesome5.h>

static String s_selected;

void GameObjectView::Present(bool& show)
{
	const auto& gos = GameObject::GetClasses();
	ImGui::Begin(ICON_FA_ID_BADGE"  GameObjects");

	if (ImGui::BeginListBox("##GameObjectsListBox"))
	{
		for (const auto& name : gos)
		{
			if (ImGui::Selectable(name.c_str(), name == s_selected))
			{
				s_selected = name;
			}
		}
		ImGui::EndListBox();
	}

	if (ImGui::Button("Add"))
	{
		if (!s_selected.empty())
			GameObject::New(Scene::GetCurrent(), s_selected);
	}

	ImGui::End();
}