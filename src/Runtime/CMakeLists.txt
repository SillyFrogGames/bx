cmake_minimum_required (VERSION 3.1)

set (GAME_RUNTIME_SRCS
	"GameMain.cpp"
)

add_executable (Game ${GAME_RUNTIME_SRCS})
target_link_libraries (Game Framework)

set (EDITOR_RUNTIME_SRCS
	"EditorMain.cpp"
)

add_executable (Editor ${EDITOR_RUNTIME_SRCS})
target_link_libraries (Editor FrameworkEditor)