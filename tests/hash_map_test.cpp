#include <gtest/gtest.h>
#include <bx/engine/containers/handle_map.hpp>

TEST(HashMapTest, BasicAssertions)
{
    struct Impl { int handle = 0; };
    HandleMap<Impl> map;// (1);

    auto h1 = map.Insert(Impl{ 1 });
    auto h2 = map.Insert(Impl{ 2 });
    map.Remove(h1);
    map.Remove(h2);
    auto h3 = map.Insert(Impl{ 3 });
    auto& v3 = map.Get(h3);

    EXPECT_EQ(v3.handle, 3);
}