#include <gtest/gtest.h>
#include <bx/engine/core/application.hpp>
#include <bx/runtime/runtime.hpp>

void Application::Configure(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
}

bool Application::Initialize()
{
    Runtime::Close();

    int testRet = RUN_ALL_TESTS();
    return testRet == EXIT_SUCCESS;
}

void Application::Shutdown()
{
}