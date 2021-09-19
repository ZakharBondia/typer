#include "typer/extra/equal_op.hpp"
#include "example.hpp"
#include "catch.hpp"

TEST_CASE("equal")
{
    demo::User u1{1, "Zak"};
    demo::User u2{2, "Dana"};
    CHECK(typer::is_equal(u1, u1));
    CHECK(typer::is_equal(u2, u2));
    CHECK(!typer::is_equal(u1, u2));
}
TEST_CASE("equal operator")
{
    demo::User u1{1, "Zak"};
    demo::User u2{2, "Dana"};
    bool res = u1 == u1;
    CHECK(res);
    CHECK(u1 == u1);
    CHECK(u1 != u2);
}

namespace demo
{
TEST_CASE("equal in type's namespace")
{
    User u1{1, "Zak"};
    User u2{2, "Dana"};
    bool res = u1 == u1;
    CHECK(res);
    CHECK(u1 == u1);
    CHECK(u1 != u2);
}
} // namespace demo

namespace another_ns
{
template <class T>
bool compare(const T& lhs, const T& rhs)
{
    return lhs == rhs;
}

TEST_CASE("equal in 3rd namespace")
{
    demo::User u1{1, "Zak"};
    demo::User u2{2, "Dana"};
    bool res = u1 == u1;
    CHECK(res);
    CHECK(u1 == u1);
    CHECK(u1 != u2);
}
TEST_CASE("equal with using namespace")
{
    using namespace demo;
    User u1{1, "Zak"};
    User u2{2, "Dana"};
    bool res = u1 == u1;
    CHECK(res);
    CHECK(u1 == u1);
    CHECK(u1 != u2);
}
} // namespace another_ns

TEST_CASE("equal evaluated in another ns")
{
    demo::User u1{1, "Zak"};
    demo::User u2{2, "Dana"};

    another_ns::compare(u1, u2);
}

// FIXME investigate why std::equal_to is not able to lookup our equal op
#if 0
TEST_CASE("equal operator")
{    
    demo::User u1{1, "Zak"};
    demo::User u2{2, "Dana"};
    std::equal_to<demo::User> equal_to;
    CHECK(equal_to(u1, u2));
}
#endif
