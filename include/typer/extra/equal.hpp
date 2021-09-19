#pragma once

#include "../introspection.hpp"

namespace typer
{

// Fallback to equal operator
template <class T>
constexpr auto is_equal(const T& lhs, const T& rhs)
    -> enable_if_not_supported_t<T, bool>
{
    return lhs == rhs;
}

//! Whether each data members of lhs and rhs are equal
template <class T>
constexpr auto is_equal(const T& lhs, const T& rhs)
    -> enable_if_supported_t<T, bool>
{
    bool res = true;
    typer::for_each_member_of<T>([&](const auto& member) {
        if (!res)
            return;
        res &= is_equal(typer::access(lhs, member), typer::access(rhs, member));
    });
    return res;
}
} // namespace typer