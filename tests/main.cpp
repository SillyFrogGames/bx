#include <gtest/gtest.h>
#include <bx/engine/core/application.hpp>

bool Application::Initialize()
{
    return false;
}

void Application::Shutdown()
{
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}