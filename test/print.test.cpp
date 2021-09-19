#include "example.hpp"
#include "typer/extra/print.hpp"
#include "typer/extra/ostream_op.hpp"
#include "catch.hpp"

#include <sstream>
#include <iostream>

using namespace std::literals::string_literals;

TEST_CASE("print to stringstream")
{
    std::stringstream stream;
    typer::print(stream, demo::User{1, "Bob"});
    auto user_str = stream.str();
    CHECK(("{id: 1, name: Bob}"s) == user_str);

    stream.str({});
    typer::print(stream, demo::DerivedUser{{2, "Bob"}, "London"});
    CHECK("{id: 2, name: Bob, address: London}"s == stream.str());
}

TEST_CASE("cout in global namespace")
{
    std::cout << demo::User{1, "Bob"} << "\n";
    std::cout << demo::DerivedUser{{2, "Bob"}, "London"} << "\n";
}

namespace demo
{
TEST_CASE("cout in type's namespace")
{
    std::cout << User{1, "Bob"} << "\n";
    std::cout << DerivedUser{{2, "Bob"}, "London"} << "\n";
}
} // namespace demo

namespace another_ns
{
TEST_CASE("cout in 3rd namespace")
{
    std::cout << demo::User{1, "Bob"} << "\n";
    std::cout << demo::DerivedUser{{2, "Bob"}, "London"} << "\n";
}
TEST_CASE("cout with using namespace")
{
    using namespace demo;
    std::cout << User{1, "Bob"} << "\n";
    std::cout << DerivedUser{{2, "Bob"}, "London"} << "\n";
}
} // namespace another_ns
