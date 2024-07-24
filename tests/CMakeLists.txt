set(BX_TEST_SRCS
    main.cpp
    handle_map_test.cpp
    hash_map_test.cpp
)

add_executable (BxTests ${BX_TEST_SRCS})
target_link_libraries (BxTests bx gtest)

add_test (NAME bx_tests COMMAND BxTests)