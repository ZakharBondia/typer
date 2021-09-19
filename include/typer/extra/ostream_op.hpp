#pragma once

#include "print.hpp"

// Declaring it global to avoid ambiguity with namespaces
template <class T, class = typer::enable_if_supported_t<T>>
std::ostream& operator<<(std::ostream& os, const T& val)
{
    return typer::print(os, val);
}
