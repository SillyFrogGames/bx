#include <gtest/gtest.h>
#include <bx/engine/core/application.hpp>
#include <bx/runtime/runtime.hpp>

bool Application::Initialize(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int testRet = RUN_ALL_TESTS();

    Runtime::Close();
    return testRet == EXIT_SUCCESS;
}

void Application::Shutdown()
{
}