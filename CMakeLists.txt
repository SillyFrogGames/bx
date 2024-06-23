cmake_minimum_required (VERSION 3.22)
message("CMake Version: ${CMAKE_VERSION}")

project (bx)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

option (ENGINE_BUILD_EDITOR "Build as editor binaries" OFF)
#option (BUILD_TESTS "Build the test binaries" ON)

# Define options for window backend
set (WINDOW_BACKEND "GLFW" CACHE STRING "Choose the window backend: GLFW")
set_property (CACHE WINDOW_BACKEND PROPERTY STRINGS "GLFW")

if (WINDOW_BACKEND STREQUAL "GLFW")
    message(STATUS "Using GLFW as the window backend")
    add_compile_definitions (WINDOW_GLFW_BACKEND)

else ()
    message(FATAL_ERROR "Unknown window backend: ${WINDOW_BACKEND}")
endif ()

# Define options for graphics backend
set (GRAPHICS_BACKEND "OpenGL" CACHE STRING "Choose the graphics backend: OpenGL, OpenGLES, Vulkan")
set_property (CACHE GRAPHICS_BACKEND PROPERTY STRINGS "OpenGL" "OpenGLES" "Vulkan")

if (GRAPHICS_BACKEND STREQUAL "OpenGL")
    message(STATUS "Using OpenGL as the graphics backend")
    add_compile_definitions (GRAPHICS_OPENGL_BACKEND)

elseif (GRAPHICS_BACKEND STREQUAL "OpenGLES")
    message(STATUS "Using OpenGLES as the graphics backend")
    add_compile_definitions (GRAPHICS_OPENGLES_BACKEND)

elseif (GRAPHICS_BACKEND STREQUAL "Vulkan")
    message(STATUS "Using Vulkan as the graphics backend")
    add_compile_definitions (GRAPHICS_VULKAN_BACKEND)

else ()
    message(FATAL_ERROR "Unknown graphics backend: ${GRAPHICS_BACKEND}")
endif ()

# Define options for physics backend
set (PHYSICS_BACKEND "Bullet" CACHE STRING "Choose the physics backend: Bullet")
set_property (CACHE PHYSICS_BACKEND PROPERTY STRINGS "Bullet")

if (PHYSICS_BACKEND STREQUAL "Bullet")
    message(STATUS "Using Bullet as the physics backend")
    add_compile_definitions (PHYSICS_BULLET_BACKEND)

else ()
    message(FATAL_ERROR "Unknown physics backend: ${WINDOW_BACKEND}")
endif ()

# Define options for audio backend
set (AUDIO_BACKEND "PortAudio" CACHE STRING "Choose the audio backend: PortAudio")
set_property (CACHE AUDIO_BACKEND PROPERTY STRINGS "PortAudio")

if (AUDIO_BACKEND STREQUAL "PortAudio")
    message(STATUS "Using PortAudio as the audio backend")
    add_compile_definitions (AUDIO_PORTAUDIO_BACKEND)

else ()
    message(FATAL_ERROR "Unknown audio backend: ${WINDOW_BACKEND}")
endif ()

# TODO: Use Platform.hpp for the future
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	add_compile_definitions (PLATFORM_PC)
endif ()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	add_compile_definitions (PLATFORM_LINUX)
endif ()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	add_compile_definitions (DEBUG_BUILD)

else ()
    add_compile_definitions (RELEASE_BUILD)
endif ()
# ---------------------------------------

add_compile_definitions (ENGINE_PATH="${CMAKE_CURRENT_SOURCE_DIR}")

if (ENGINE_BUILD_EDITOR)
    add_compile_definitions (EDITOR_BUILD)
endif ()

add_subdirectory (extern)

set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
add_subdirectory (wren)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/include)

# Setup common engine sources
set (ENGINE_SRCS
	"src/bx/engine/core/data.cpp"
	"src/bx/engine/core/ecs.cpp"
	"src/bx/engine/core/file.cpp"
	"src/bx/engine/core/input.cpp"
	"src/bx/engine/core/log.cpp"
	"src/bx/engine/core/math.cpp"
	"src/bx/engine/core/memory.cpp"
	"src/bx/engine/core/module.cpp"
	"src/bx/engine/core/object.cpp"
	"src/bx/engine/core/profiler.cpp"
	"src/bx/engine/core/resource.cpp"
	"src/bx/engine/core/thread.cpp"
	"src/bx/engine/core/time.cpp"
	"src/bx/engine/core/uuid.cpp"

	"src/bx/engine/modules/audio.cpp"
	"src/bx/engine/modules/graphics.cpp"
	"src/bx/engine/modules/physics.cpp"
	"src/bx/engine/modules/script.cpp"
	"src/bx/engine/modules/window.cpp"
	"src/bx/engine/modules/imgui.cpp"
	
	"src/bx/framework/components/animator.cpp"
	"src/bx/framework/components/attributes.cpp"
	"src/bx/framework/components/audio_listener.cpp"
	"src/bx/framework/components/audio_source.cpp"
	"src/bx/framework/components/camera.cpp"
	"src/bx/framework/components/character_controller.cpp"
	"src/bx/framework/components/collider.cpp"
	"src/bx/framework/components/light.cpp"
	"src/bx/framework/components/mesh_filter.cpp"
	"src/bx/framework/components/mesh_renderer.cpp"
	"src/bx/framework/components/rigidbody.cpp"
	"src/bx/framework/components/spline.cpp"
	"src/bx/framework/components/transform.cpp"
	"src/bx/framework/resources/animation.cpp"
	"src/bx/framework/resources/material.cpp"
	"src/bx/framework/resources/mesh.cpp"
	"src/bx/framework/resources/shader.cpp"
	"src/bx/framework/resources/skeleton.cpp"
	"src/bx/framework/resources/texture.cpp"
	"src/bx/framework/systems/renderer.cpp"
	"src/bx/framework/systems/dynamics.cpp"
	"src/bx/framework/systems/acoustics.cpp"
	"src/bx/framework/gameobject.cpp"
)

set (ENGINE_LIBS ctti wnaabi cereal fmt glm wren imgui file_embed)

# Setup window backend
if(WINDOW_BACKEND STREQUAL "GLFW")
	set (ENGINE_SRCS ${ENGINE_SRCS}
		"src/bx/engine/modules/window/backend/window_glfw.cpp"
	)

    set (ENGINE_LIBS ${ENGINE_LIBS} glfw imgui_glfw)
endif()

# Setup graphics backend
if(GRAPHICS_BACKEND STREQUAL "OpenGL")
	set (ENGINE_SRCS ${ENGINE_SRCS}
		"src/bx/engine/modules/graphics/backend/graphics_opengl.cpp"
	)

    set (ENGINE_LIBS ${ENGINE_LIBS} glad imgui_opengl3)

elseif(GRAPHICS_BACKEND STREQUAL "Vulkan")
	set (ENGINE_SRCS ${ENGINE_SRCS}
		"src/bx/engine/modules/graphics/backend/graphics_vulkan.cpp"
	)

    set (ENGINE_LIBS ${ENGINE_LIBS} vulkan glslang SPIRV)
endif()

# Setup physics backend
if(PHYSICS_BACKEND STREQUAL "Bullet")
	set (ENGINE_SRCS ${ENGINE_SRCS}
		"src/bx/engine/modules/physics/backend/physics_bullet.cpp"
	)

    set (ENGINE_LIBS ${ENGINE_LIBS} bullet3) #box2d
endif()

# Setup audio backend
if(AUDIO_BACKEND STREQUAL "PortAudio")
	set (ENGINE_SRCS ${ENGINE_SRCS}
		"src/bx/engine/modules/audio/backend/audio_portaudio.cpp"
	)

    set (ENGINE_LIBS ${ENGINE_LIBS} PortAudio)
endif()

# Create engine lib
add_library (Engine STATIC ${ENGINE_SRCS})
target_link_libraries (Engine ${ENGINE_LIBS})
target_include_directories (Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (MSVC)
	target_compile_options(Engine PUBLIC "/ZI")
	target_link_options(Engine PUBLIC "/INCREMENTAL")
endif ()

# Setup editor sources
if (ENGINE_BUILD_EDITOR)
	set (EDITOR_SRCS
		"src/bx/editor/core/asset_importer.cpp"
		"src/bx/editor/core/assets.cpp"
		"src/bx/editor/core/command.cpp"
		"src/bx/editor/core/selection.cpp"
		"src/bx/editor/core/toolbar.cpp"
		"src/bx/editor/core/view.cpp"

		"src/bx/editor/gizmos/spline_gizmo.cpp"
		"src/bx/editor/gizmos/transform_gizmo.cpp"

		"src/bx/editor/views/assets_view.cpp"
		"src/bx/editor/views/console_view.cpp"
		"src/bx/editor/views/data_view.cpp"
		"src/bx/editor/views/entity_view.cpp"
		"src/bx/editor/views/gameobject_view.cpp"
		"src/bx/editor/views/inspector_view.cpp"
		"src/bx/editor/views/profiler_view.cpp"
		"src/bx/editor/views/scene_view.cpp"
		"src/bx/editor/views/settings_view.cpp"
	)

	set (EDITOR_LIBS stb assimp implot ImGuizmo IconFontCppHeaders FontAwesome FreeFonts)

	# Create editor lib
	add_library (Editor STATIC ${EDITOR_SRCS} ${ENGINE_SRCS})
	target_link_libraries (Editor ${EDITOR_LIBS} ${ENGINE_LIBS})
	target_include_directories (Editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

	if (MSVC)
		target_compile_options(Editor PUBLIC "/ZI")
		target_link_options(Editor PUBLIC "/INCREMENTAL")
	endif ()

	# Add editor runtime
	add_executable (EditorRuntime "src/bx/runtime/editor_application.cpp")
	target_link_libraries (EditorRuntime Editor)

else ()
	
	# Add game runtime
	add_executable (GameRuntime "src/bx/runtime/game_application.cpp")
	target_link_libraries (GameRuntime Engine)
endif ()