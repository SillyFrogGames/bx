cmake_minimum_required (VERSION 3.1)

# Engine lib
set (ENGINE_SRCS
	"Platform/Window.cpp"
)

set (ENGINE_LIBS)

add_library (Engine STATIC ${ENGINE_SRCS})
target_link_libraries (Engine Core ${ENGINE_LIBS})
target_include_directories (Engine PUBLIC ${BX_INCLUDE_DIR})

# Engine editor lib
set (ENGINE_EDITOR_SRCS ${ENGINE_SRCS}
)

set (ENGINE_EDITOR_LIBS ${ENGINE_LIBS})

add_library (EngineEditor STATIC ${ENGINE_EDITOR_SRCS})
target_link_libraries (EngineEditor CoreEditor ${ENGINE_EDITOR_LIBS})
target_include_directories (EngineEditor PUBLIC ${BX_INCLUDE_DIR})