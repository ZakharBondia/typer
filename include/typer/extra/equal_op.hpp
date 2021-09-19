#pragma once

#include "equal.hpp"

// Declaring it global to avoid ambiguity with namespaces
template <class T, class = typer::enable_if_supported_t<T>>
bool operator==(const T& lhs, const T& rhs)
{
    return typer::is_equal(lhs, rhs);
}

template <class T, class = typer::enable_if_supported_t<T>>
bool operator!=(const T& lhs, const T& rhs)
{
    return !typer::is_equal(lhs, rhs);
}