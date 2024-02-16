cmake_minimum_required (VERSION 3.1)

# Framework lib
set (FRAMEWORK_SRCS
	"Components/Transform.cpp"

	"Systems/Renderer.cpp"
)

set (FRAMEWORK_LIBS)

add_library (Framework STATIC ${FRAMEWORK_SRCS})
target_link_libraries (Framework Engine ${FRAMEWORK_LIBS})
target_include_directories (Framework PUBLIC ${BX_INCLUDE_DIR})

# Framework editor lib
set (FRAMEWORK_EDITOR_SRCS ${FRAMEWORK_SRCS}
)

set (FRAMEWORK_EDITOR_LIBS ${FRAMEWORK_LIBS})

add_library (FrameworkEditor STATIC ${FRAMEWORK_EDITOR_SRCS})
target_link_libraries (FrameworkEditor EngineEditor ${FRAMEWORK_EDITOR_LIBS})
target_include_directories (FrameworkEditor PUBLIC ${BX_INCLUDE_DIR})