#pragma once
#include "../introspection.hpp"

namespace typer
{
// forward declaration of main print function
template <class T, class Stream>
Stream& print(Stream& os, const T& value);

// basic type overload
template <class T, class Stream>
Stream& print(Stream& os, const T& value, std::false_type)
{
    os << value;
    return os;
}

// meta-typed objects overload
template <class T, class Stream>
Stream& print(Stream& os, const T& object, std::true_type)
{
    os << '{';
    auto count = typer::members_size<T>();
    typer::for_each_member_of<T>([&](const auto& member) {
        os << typer::name_of(member) << ": ";
        typer::print(os, typer::access(object, member));
        if (--count != 0)
        {
            os << ", ";
        }
    });
    os << '}';
    return os;
}

//! Prints value to the stream
template <class T, class Stream>
Stream& print(Stream& os, const T& value)
{
    return print(os, value, is_supported<T>{});
}
} // namespace typer
