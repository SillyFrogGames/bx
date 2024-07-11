#include "bx/engine/modules/graphics.hpp"

#include "bx/engine/core/log.hpp"
#include "bx/engine/core/guard.hpp"
#include "bx/engine/core/file.hpp"
#include "bx/engine/core/profiler.hpp"
#include "bx/engine/containers/pool.hpp"

// Debug draw utilities

struct DebugLineData
{
    DebugLineData() {}
	DebugLineData(const Vec3& a, const Vec3& b, u32 c, f32 l)
		: a(a), b(b), color(c), lifespan(l) {}

    Vec3 a = Vec3(0, 0, 0);
    Vec3 b = Vec3(0, 0, 0);
    u32 color = 0;
    f32 lifespan = 0.0f;
};

static List<DebugLineData> g_debugLines;
static List<DebugLineData> g_debugLinesBuffer;

static List<DebugVertex> g_debugVertices;

void Graphics::DebugLine(const Vec3& a, const Vec3& b, u32 color, f32 lifespan)
{
    g_debugLines.emplace_back(a, b, color, lifespan);
}

void Graphics::UpdateDebugLines()
{
    g_debugVertices.clear();

    g_debugLinesBuffer.clear();
    g_debugLinesBuffer.reserve(g_debugLines.size());
    for (auto& line : g_debugLines)
    {
        g_debugVertices.emplace_back(line.a, line.color);
        g_debugVertices.emplace_back(line.b, line.color);
    
        line.lifespan -= Time::GetDeltaTime();
        if (line.lifespan > 0.0f)
            g_debugLinesBuffer.emplace_back(line);
    }
    g_debugLines = g_debugLinesBuffer;
}

void Graphics::DrawDebugLines(const Mat4& viewProj)
{
    DebugDrawAttribs attribs;
    DebugDraw(viewProj, attribs, g_debugVertices);
}

void Graphics::ClearDebugLines()
{
    g_debugLines.clear();
}