#include "example.hpp"
#include "typer/introspection.hpp"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <vector>
#include <iostream>

using namespace std::literals::string_literals;

struct UnknownType
{
};


TEST_CASE("is_supported")
{
    CHECK(typer::is_supported_v<demo::User>);
    CHECK(typer::is_supported_v<demo::DerivedUser>);
    CHECK(!typer::is_supported_v<UnknownType>);

    CHECK(typer::is_supported<demo::User>());
    CHECK(typer::is_supported<demo::DerivedUser>());
    CHECK(!typer::is_supported<UnknownType>());
}

TEST_CASE("is_meta_object")
{
    CHECK(!typer::is_meta_object<demo::User>());
    CHECK(typer::is_meta_object<decltype(typer::meta_object_v<demo::User>)>());
    CHECK(typer::is_meta_object<typer::meta_object<demo::User>>());
}

TEST_CASE("name_of")
{
    auto user_meta = typer::meta_object_v<demo::User>;

    CHECK("id"s == typer::name_of(std::get<0>(user_meta.members)));
    CHECK("name"s == typer::name_of(std::get<1>(user_meta.members)));
}

TEST_CASE("for_each_member_of")
{
    auto get_member_names = [](auto* ptr) {
        std::vector<std::string> actual_names;
        using T = std::remove_pointer_t<decltype(ptr)>;
        typer::for_each_member_of<T>([&](const auto& member) {
            std::cout << typer::type_name<T>() << typer::name_of(member) << 
            "\n";
            actual_names.push_back(typer::name_of(member));
        });
        return actual_names;
    };
    CHECK(std::vector<std::string>{{"id", "name"}} ==
          get_member_names((demo::User*)nullptr));
          
    CHECK(std::vector<std::string>{{"id", "name", "address"}} ==
          get_member_names((demo::DerivedUser*)nullptr));
}
TEST_CASE("members_size")
{
    CHECK(3 == typer::members_size<demo::DerivedUser>());
}

TEST_CASE("type_name")
{
    CHECK("User"s == typer::type_name<demo::User>());
    CHECK("DerivedUser"s == typer::type_name<demo::DerivedUser>());
}

TEST_CASE("type_of")
{
    using U = typer::type_of<typer::meta_object<demo::User>>;
    using D = typer::type_of<typer::meta_object<demo::DerivedUser>>;
    auto id_mem = std::get<0>(typer::meta_object_v<demo::User>.members);
    auto name_mem = std::get<1>(typer::meta_object_v<demo::User>.members);
    using I = typer::type_of<decltype(id_mem)>;
    using S = typer::type_of<decltype(name_mem)>;

    static_assert(std::is_same_v<U, demo::User>);
    static_assert(std::is_same_v<D, demo::DerivedUser>);
    static_assert(std::is_same_v<I, int>);
    static_assert(std::is_same_v<S, std::string>);
}

TEST_CASE("access")
{
    auto id_mem = std::get<0>(typer::meta_object_v<demo::User>.members);
    auto name_mem = std::get<1>(typer::meta_object_v<demo::User>.members);

    demo::User user{0, "Zak"};
    CHECK(0 == typer::access(user, id_mem));
    CHECK("Zak" == typer::access(user, name_mem));

    typer::access(user, id_mem) = 1;
    typer::access(user, name_mem) = "Dana"s;
    CHECK(1 == user.id);
    CHECK("Dana" == user.name);
}