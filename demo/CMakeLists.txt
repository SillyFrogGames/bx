cmake_minimum_required (VERSION 3.1)

# Dummy lib
set (DUMMY_SRCS
	"src/Dummy.cpp"
)

add_library (Dummy STATIC ${DUMMY_SRCS})
target_link_libraries (Dummy Framework)
target_include_directories (Dummy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Dummy editor lib
set (DUMMY_EDITOR_SRCS ${DUMMY_SRCS}
)

add_library (DummyEditor STATIC ${DUMMY_EDITOR_SRCS})
target_link_libraries (DummyEditor FrameworkEditor)
target_include_directories (DummyEditor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)