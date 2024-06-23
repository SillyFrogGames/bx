#include "Editor/Gizmos/SplineGizmo.hpp"

void SplineGizmo::Edit(Spline& spl)
{
	// TODO: Implement this
}

/*
#include "glm/gtc/matrix_transform.hpp"
#include "ImGuizmo.h"
#include "LevelEditor.h"

//Components
#include "Framework/Components/Spline.h"
#include "Framework/Components/Transform.h"

//Commands
#include "Commands/SplineChanged.h"

//Undo Redo Variables

static bool g_splineChanged = false;
static Spline g_oldSpline;

SplineGizmo::SplineGizmo()
{
	//Rect Defaults
	m_winPosX = 0.f;
	m_winPosY = 0.f;
	m_winWidth = 0.f;
	m_winHeight = 0.f;

	//Selection Defaults
	m_selectedID = -1;


	m_drawList = nullptr;
	m_mousePos = glm::vec2(0);

}

SplineGizmo::~SplineGizmo()
{
	m_drawList = nullptr;
}

void SplineGizmo::ManipulateSpline(Spline& splineComp, const glm::mat4& view, const glm::mat4& projection, Transform& transComp)
{
	//Set Mouse Position
	ImGuiIO& io = ImGui::GetIO();
	m_mousePos = glm::vec2(io.MousePos.x, io.MousePos.y);

	//Set DrawList
	m_drawList = ImGui::GetBackgroundDrawList();


	//Behind Camera
	glm::vec4 camSpacePosition;
	camSpacePosition = TransformPoint(glm::vec4(0, 0, 0, 0), (projection * view * transComp.GetModel()));
	if (camSpacePosition.z < 0.001f)
	{
		return;
	}


	//Can Draw Gizmo?
	if (!m_drawList)
	{
		return;
	}

	//Clicked out of range deselect point
	if (IsClicked(0))
	{
		m_selectedID = -1;
	}

	for (int id = 0; id < splineComp.GetPoints().size(); id++)
	{
		int nextId = id + 1;
		//Check if there are more than 1 points 
		if (splineComp.GetPoints().size() > (nextId))
		{
			glm::vec2 p1 = WorldToScreen(splineComp.GetPoint(id) + transComp.GetPosition(), view, projection);
			glm::vec2 p2 = WorldToScreen(splineComp.GetPoint(nextId) + transComp.GetPosition(), view, projection);

			m_drawList->AddLine(ImVec2(p1.x, p1.y), ImVec2(p2.x, p2.y), ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1)), 1.0f);

			//Insert Point if Clicked on line
			if (IsDoubleClicked(0))
			{
				//Check if mouse is close or over line
				if (SDFLine(p1, p2, m_mousePos) <= 8.f)
				{
					glm::vec3 newPoint = ScreenToWorld(m_mousePos, view, projection, splineComp.GetPoint(id).z + transComp.GetPosition().z) - transComp.GetPosition();
					splineComp.InsertPoint(nextId, newPoint);
					CommandHistory::Add(new SplinePointInsert(m_currentEntity, nextId, newPoint));
					return;
				}
			}
		}
		else if (splineComp.GetIsClosed())
		{
			//If spline is closed check again but only from last to first
			glm::vec2 firstPoint = WorldToScreen(splineComp.GetPoint(splineComp.GetPoints().size() - 1) + transComp.GetPosition(), view, projection);
			glm::vec2 lastPoint = WorldToScreen(splineComp.GetPoint(0) + transComp.GetPosition(), view, projection);
			m_drawList->AddLine(ImVec2(firstPoint.x, firstPoint.y), ImVec2(lastPoint.x, lastPoint.y), ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1)), 1.0f);

			if (IsDoubleClicked(0))
			{
				//Check if mouse is close or over line
				if (SDFLine(firstPoint, lastPoint, m_mousePos) <= 8.f)
				{
					glm::vec3 newPoint = ScreenToWorld(m_mousePos, view, projection, splineComp.GetPoint(id).z + transComp.GetPosition().z) - transComp.GetPosition();
					splineComp.PushBackPoint(newPoint);
					CommandHistory::Add(new SplinePointAdded(m_currentEntity, newPoint));
					return;
				}
			}
		}


		if (DrawGizmo(id, splineComp.GetPoints().at(id) + transComp.GetPosition(), view, projection))
		{
			//Select Point
			if (IsClicked(0))
			{
				m_selectedID = id;
				g_oldSpline = splineComp;
				return;
			}

			//Remove Point
			if (IsClicked(1))
			{
				CommandHistory::Add(new SplinePointDeleted(m_currentEntity, id, splineComp.GetPoint(id)));
				splineComp.RemovePoint(id);
				m_selectedID = -1;
				return;
			}

			//Move Point
			if (IsHoldingDown(id, 0))
			{
				//Draw Movement Text
				std::string posDeltaString = "PosX: " + std::to_string(splineComp.GetPoint(id).x + transComp.GetPosition().x) + " PosY: " + std::to_string(splineComp.GetPoint(id).y + transComp.GetPosition().y);
				m_drawList->AddText(ImVec2(m_mousePos.x, m_mousePos.y), ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), posDeltaString.c_str());

				float depth = splineComp.GetPoint(id).z + transComp.GetPosition().z;
				splineComp.SetPoint(id, ScreenToWorld(m_mousePos, view, projection, depth) - transComp.GetPosition());

				g_splineChanged = true;
			}
			else
			{
				m_selectedID = -1;
				if (g_splineChanged)
				{
					g_splineChanged = false;
					CommandHistory::Add(new SplineChanged(m_currentEntity, g_oldSpline, splineComp));
				}

			}
		}
	}
	return;
}

bool SplineGizmo::DrawGizmo(int curID, const glm::vec3& point, const glm::mat4& view, const glm::mat4& projection)
{
	glm::vec2 pointPos = WorldToScreen(point, view, projection);
	glm::vec2 mouseDelta = GetMouseDeltaFromPoint(glm::vec2(pointPos.x, pointPos.y));
	float mouseDistance = glm::length(mouseDelta);

	if (curID == m_selectedID)
	{
		m_drawList->AddCircleFilled(ImVec2(pointPos.x, pointPos.y), 12.5f, ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.6f, 0, 1)), 16);
		return true;
	}
	else
	{
		if (mouseDistance <= 8.0f)
		{
			m_drawList->AddCircleFilled(ImVec2(pointPos.x, pointPos.y), 12.5f, IM_COL32_WHITE, 16);
			return true;
		}
		else
		{
			m_drawList->AddCircleFilled(ImVec2(pointPos.x, pointPos.y), 8.f, IM_COL32_WHITE, 16);
		}
	}
	return false;
}


///Input
bool SplineGizmo::IsClicked(ImGuiMouseButton mouseID)
{
	if (ImGui::IsMouseClicked(mouseID) && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive())
	{
		return true;
	}
	return false;
}

bool SplineGizmo::IsHoldingDown(int pointID, ImGuiMouseButton mouseID)
{
	if (pointID == m_selectedID && ImGui::IsMouseDown(mouseID) && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive())
	{
		return true;
	}
	return false;
}

bool SplineGizmo::IsDoubleClicked(ImGuiMouseButton mouseID)
{
	if (ImGui::IsMouseDoubleClicked(mouseID) && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive())
	{
		return true;
	}
	return false;
}


///WorldToScreen
//Referenced from ImGuizmo
glm::vec4 SplineGizmo::TransformPoint(const glm::vec4& point, const glm::mat4& matrix)
{
	glm::vec4 out;

	out.x = point.x * matrix[0][0] + point.y * matrix[1][0] + point.z * matrix[2][0] + matrix[3][0];
	out.y = point.x * matrix[0][1] + point.y * matrix[1][1] + point.z * matrix[2][1] + matrix[3][1];
	out.z = point.x * matrix[0][2] + point.y * matrix[1][2] + point.z * matrix[2][2] + matrix[3][2];
	out.w = point.x * matrix[0][3] + point.y * matrix[1][3] + point.z * matrix[2][3] + matrix[3][3];

	return out;
}

//Based on ImGuizmo
glm::vec2 SplineGizmo::WorldToScreen(const glm::vec3& worldPos, const glm::mat4& matV, const glm::mat4& matP)
{
	glm::vec4 trans;
	glm::vec4 worldPosT = glm::vec4(worldPos, 1);

	glm::vec2 position = glm::vec2(m_winPosX, m_winPosY);
	glm::vec2 size = glm::vec2(m_winWidth, m_winHeight);

	trans = TransformPoint(worldPosT, (matP * matV));
	trans *= 0.5f / trans.w;
	trans += glm::vec4(0.5f, 0.5f, 0, 0);
	trans.y = 1.f - trans.y;
	trans.x *= size.x;
	trans.y *= size.y;
	trans.x += position.x;
	trans.y += position.y;
	return glm::vec2(trans.x, trans.y);
}


///ScreenToWorld
//Referenced from Last Block
glm::vec3 SplineGizmo::ScreenToWorld(const glm::vec2& pos, const glm::mat4& view, const glm::mat4& projection, float depth)
{
	glm::vec3 origin = LevelEditor::GetCamera().GetPosition();
	glm::vec3 direction = ScreenWorldRay(pos, view, projection);
	glm::vec3 normal = glm::vec3(0.f, 0.f, -1.0f);

	float denom = dot(normal, direction);

	if (abs(denom) > 0.0001f)
	{
		float t = dot((glm::vec3(0.f, 0.f, depth) - origin), normal) / denom;
		if (t >= 0)
		{
			return origin + (direction * t);
		}
	}
	return origin;
}

glm::vec3 SplineGizmo::ScreenWorldRay(const glm::vec2& pos, const glm::mat4& view, const glm::mat4& projection)
{
	float hScreenWidth = ImGui::GetMainViewport()->Size.x * 0.5f;
	float hScreenHeight = ImGui::GetMainViewport()->Size.y * 0.5f;

	glm::mat4 invPV = glm::inverse(projection * view);
	glm::vec4 nearP = glm::vec4((pos.x - hScreenWidth) / hScreenWidth, -1 * (pos.y - hScreenHeight) / hScreenHeight, -1, 1.0f);
	glm::vec4 farP = glm::vec4((pos.x - hScreenWidth) / hScreenWidth, -1 * (pos.y - hScreenHeight) / hScreenHeight, 1, 1.0f);
	glm::vec4 nearResult = invPV * nearP;
	glm::vec4 farResult = invPV * farP;
	nearResult /= nearResult.w;
	farResult /= farResult.w;
	glm::vec3 dir = glm::vec3(farResult - nearResult);
	return glm::normalize(dir);
}

//Referenced from ImGuizmo
void SplineGizmo::SetRect(float x, float y, float width, float height)
{
	m_winPosX = x;
	m_winPosY = y;
	m_winWidth = width;
	m_winHeight = height;
}

float SplineGizmo::SDFLine(glm::vec2 a, glm::vec2 b, glm::vec2 p)
{
	glm::vec2 ba = b - a;
	glm::vec2 pa = p - a;
	float h = dot(ba, pa) / dot(ba, ba);
	if (h < 0)
		h = 0;
	if (h > 1)
		h = 1;

	return glm::length(pa - ba * h);
}

glm::vec2 SplineGizmo::GetMouseDeltaFromPoint(glm::vec2 point)
{
	ImGuiIO& io = ImGui::GetIO();
	glm::vec2 mouseDelta;
	mouseDelta.x = io.MousePos.x - point.x;
	mouseDelta.y = io.MousePos.y - point.y;
	return mouseDelta;
}
*/