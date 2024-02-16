cmake_minimum_required (VERSION 3.1)

set (RUNTIME_SRCS
	"Main.cpp"
)

add_executable (Game ${RUNTIME_SRCS})
target_link_libraries (Game Framework)

add_executable (Editor ${RUNTIME_SRCS})
target_link_libraries (Editor FrameworkEditor)