#include "bx/framework/systems/renderer.hpp"

#include "bx/framework/components/transform.hpp"
#include "bx/framework/components/camera.hpp"
#include "bx/framework/components/mesh_filter.hpp"
#include "bx/framework/components/mesh_renderer.hpp"
#include "bx/framework/components/animator.hpp"
#include "bx/framework/components/light.hpp"

#include <bx/engine/core/file.hpp>
#include <bx/engine/core/data.hpp>
#include <bx/engine/core/profiler.hpp>
#include <bx/engine/core/resource.hpp>
#include <bx/engine/containers/tree.hpp>
#include <bx/engine/modules/graphics.hpp>
#include <bx/engine/modules/window.hpp>

struct ViewData
{
    Mat4 viewMtx = Mat4::Identity();
    Mat4 projMtx = Mat4::Identity();
    Mat4 viewProjMtx = Mat4::Identity();
};

struct ConstantData
{
    ViewData view;
};

struct LightData
{
    u32 type = 0;
    f32 intensity = 1.0f;

    i32 shadowMapIndex = -1;
    u32 cascadeCount = 0;

    Vec3 position = Vec3(0, 0, 0);
    f32 constant = 1.0f;

    Vec3 direction = Vec3(0, 0, 0);
    f32 linear_cutoff = 0.1f;

    Vec3 color = Vec3(1, 1, 1);
    f32 quadratic_outerCutoff = 0.01f;
};

struct State : NoCopy
{
    HTexture colorTarget = HTexture::null;
    HTexture depthTarget;
};
static std::unique_ptr<State> s;

struct ModelData
{
    Mat4 worldMtx = Mat4::Identity();
    Mat4 meshMtx = Mat4::Identity();
    Vec4i lightIndices = Vec4i(-1, -1, -1, -1);
};


void Renderer::Initialize()
{
    s = std::make_unique<State>();
}

void Renderer::Shutdown()
{
    s.reset();
}

void Renderer::Update()
{
    if (Window::WasResized())
    {
        i32 w, h;
        Window::GetSize(&w, &h);


    }
}

void Renderer::Render()
{
    Graphics::UpdateDebugLines();

    
}