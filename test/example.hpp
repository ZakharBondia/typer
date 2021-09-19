#pragma once

#include "typer/declaration.hpp"

#include <string>

namespace demo
{
struct User
{
    int id;
    std::string name;
};

constexpr auto build_meta_object(User**)
{
    return typer::build_meta_object<User>("User")
        .add_member("id", &User::id)
        .add_member("name", &User::name)
        .build();
}

struct DerivedUser : User
{
    std::string address;
};

constexpr auto build_meta_object(DerivedUser**)
{
    return typer::build_meta_object<DerivedUser>("DerivedUser")
        .add_base<User>()
        .add_member("address", &DerivedUser::address)
        .build();
}
} // namespace demo