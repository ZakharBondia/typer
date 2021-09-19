#pragma once
#include "declaration.hpp"

#include <type_traits>

namespace typer
{

//! Whether introspection is is_supported for type T
template <class T, class = void>
struct is_supported : std::false_type
{
};

template <class T>
struct is_supported<
    T, std::void_t<decltype(std::declval<lookup_meta_object<T>>()())>>
    : std::true_type
{
};

template <class T>
constexpr bool is_supported_v = is_supported<T>::value;

//-----------------------------------------------------------------------------
//! Whether the given class is a valid meta object
template <class T>
struct is_meta_object : std::is_base_of<meta_object_data_base, T>
{
};
template <class T>
constexpr bool is_meta_object_v = is_meta_object<T>::value;

//-----------------------------------------------------------------------------
//! Whether the given class is a valid meta member. Meta members belong to meta
//! object and provide meta data about object's members
template <class T>
struct is_meta_member : std::is_base_of<member_base, T>
{
};

template <class T>
constexpr bool is_meta_member_v = is_meta_member<T>::value;

//-----------------------------------------------------------------------------
//! Returns the string name of the member
template <class MM>
[[nodiscard]] constexpr const char* name_of(MM member)
{
    static_assert(is_meta_member_v<MM>,
                  "This function is only used for meta members");
    return member.name();
}

//-----------------------------------------------------------------------------
template <class T>
[[nodiscard]] constexpr const char* type_name()
{
    static_assert(is_supported_v<T>, "Type reflection is not is_supported");
    return ::typer::meta_object_v<T>.name;
};
//-----------------------------------------------------------------------------
namespace detail
{
    template <class T, class D = void>
    struct type_of_impl
    {
        static_assert(std::is_same_v<T, void>,
                      "This trait is only for meta objects and members");
        // using type = void;
    };

    template <class T>
    struct type_of_impl<T, std::enable_if_t<is_meta_object_v<T>>>
    {
        using type = typename T::type;
    };

    template <class T>
    struct type_of_impl<T, std::enable_if_t<is_meta_member_v<T>>>
    {
        using type = typename T::type;
    };
} // namespace detail

//! Type meta object or member belongs to
template <class M>
using type_of = typename detail::type_of_impl<M>::type;

//-----------------------------------------------------------------------------
//! Members count of type T
template <class T>
constexpr std::size_t members_size()
{
    static_assert(is_supported_v<T>, "Type reflection is not is_supported");
    std::size_t count = 0;
    auto& mo = meta_object_v<T>;
    // invoke on each base class
    mo.bases.for_each([&](auto type_wrapper) {
        using Base = typename decltype(type_wrapper)::type;
        count += members_size<Base>();
    });
    // invoke on itself
    count += mo.members.size();
    return count;
}

//-----------------------------------------------------------------------------
//! Pass each member of T to the functor \arg func
template <class T, class F>
void for_each_member_of(F&& func)
{
    static_assert(is_supported_v<T>, "Type reflection is not is_supported");
    auto& mo = meta_object_v<T>;
    // invoke on each base class
    mo.bases.for_each([&](auto type_wrapper) {
        using Base = typename decltype(type_wrapper)::type;
        // recursive
        for_each_member_of<Base>(std::forward<F>(func));
    });
    // invoke on itself
    mo.members.for_each(std::forward<F>(func));
}

//! Pass each direct member of T to the functor \arg func. The direct member
//! means only member defined in concrete class ignoring all members comming
//! from base classes
template <class T, class F>
void for_each_direct_member_of(F&& func)
{
    static_assert(is_supported_v<T>, "Type reflection is not is_supported");
    meta_object_v<T>.members.for_each(std::forward<F>(func));
}

//-----------------------------------------------------------------------------
//! Provides direct member access
template <class T, class MM>
auto&& access(T& obj, MM&& member)
{
    return member.access(obj);
}

//-----------------------------------------------------------------------------
//! SFINAE shortcut enabling only types with introspection
template <class T, class ResType = void>
using enable_if_supported_t =
    typename std::enable_if<is_supported_v<T>, ResType>::type;

template <class T, class ResType = void>
using enable_if_not_supported_t =
    typename std::enable_if<!is_supported_v<T>, ResType>::type;

} // namespace typer
