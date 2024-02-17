cmake_minimum_required (VERSION 3.1)

project(Demo)

# Dummy lib
set (DUMMY_SRCS
	"src/Dummy.cpp"
)

add_library (Dummy SHARED ${DUMMY_SRCS})
target_link_libraries (Dummy Framework)
target_include_directories (Dummy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions (Dummy PUBLIC BUILD_DLL)

# Dummy editor lib
set (DUMMY_EDITOR_SRCS ${DUMMY_SRCS}
)

add_library (DummyEditor SHARED ${DUMMY_EDITOR_SRCS})
target_link_libraries (DummyEditor FrameworkEditor)
target_include_directories (DummyEditor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions (DummyEditor PUBLIC BUILD_DLL)

# Output of DLLs
set_target_properties(Dummy
    PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

set_target_properties(DummyEditor
    PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib
)