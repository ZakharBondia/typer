#pragma once

#include "../introspection.hpp"
#include <functional>

namespace typer
{
// Default fallback to std hash
template <class T, class = void>
struct hash : std::hash<T>
{
};

//! A universal hash function calculated by combining hashes of each type
//! member
template <class T>
struct hash<T, enable_if_supported_t<T>>
{
    using argument_type = T;
    using result_type = std::size_t;

    constexpr result_type operator()(const argument_type& object) const noexcept
    {
        result_type seed{0};
        // FIXME check why it doesn't work with ( auto &member)
        typer::for_each_member_of<argument_type>([&](auto member) {
            using member_type = typer::type_of<decltype(member)>;
            // https://www.nullptr.me/2018/01/15/hashing-stdpair-and-stdtuple/
            seed ^= hash<member_type>{}(typer::access(object, member)) +
                    0x9e3779b9 + (seed << 6) + (seed >> 2);
        });
        return seed;
    }
};
} // namespace typer