cmake_minimum_required (VERSION 3.1)

# Core lib
set (CORE_SRCS
	"Module.cpp"
	"Plugin.cpp"
)

set (CORE_LIBS)# ctti wnaabi cereal fmt glm PortAudio glad glfw wren imgui box2d bullet3 file_embed)

add_library (Core STATIC ${CORE_SRCS})
target_link_libraries (Core ${CORE_LIBS})
target_include_directories (Core PUBLIC ${BX_INCLUDE_DIR})

# Core editor lib
set (CORE_EDITOR_SRCS ${CORE_SRCS}
)

set (CORE_EDITOR_LIBS ${CORE_LIBS})

add_library (CoreEditor STATIC ${CORE_EDITOR_SRCS})
target_link_libraries (CoreEditor ${CORE_EDITOR_LIBS})
target_include_directories (CoreEditor PUBLIC ${BX_INCLUDE_DIR})

target_compile_definitions (CoreEditor PUBLIC EDITOR_BUILD)