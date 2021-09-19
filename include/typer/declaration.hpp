#pragma once
#include <tuple>

namespace typer
{

namespace detail
{
    template <typename Tuple, typename F, std::size_t... Indices>
    constexpr void for_each_impl(Tuple&& tuple, F&& f,
                                 std::index_sequence<Indices...>)
    {
        using swallow = int[];
        (void)swallow{1, (f(std::get<Indices>(std::forward<Tuple>(tuple))),
                          void(), int{})...};
    }

    //! Tuple for each implementation
    template <typename Tuple, typename F>
    constexpr void for_each(Tuple&& tuple, F&& f)
    {
        constexpr std::size_t N =
            std::tuple_size<std::remove_reference_t<Tuple>>::value;
        for_each_impl(std::forward<Tuple>(tuple), std::forward<F>(f),
                      std::make_index_sequence<N>{});
    }
} // namespace detail

//-----------------------------------------------------------------------------
template <class... Args>
struct tuple : std::tuple<Args...>
{
    constexpr tuple(Args&&... args)
        : std::tuple<Args...>{std::forward<Args>(args)...}
    {
    }
    constexpr tuple(std::tuple<Args...>&& args)
        : std::tuple<Args...>{std::forward<std::tuple<Args...>&>(args)}
    {
    }
    constexpr std::size_t size() const
    {
        return std::tuple_size<std::tuple<Args...>>();
    }

    template <class T>
    constexpr tuple<Args..., T> push_back(T&& val)
    {
        return std::tuple_cat((std::tuple<Args...>)(*this),
                              std::make_tuple(std::forward<T>(val)));
    }

    template <class Functor>
    constexpr void for_each(Functor f) const
    {
        detail::for_each((std::tuple<Args...>)(*this), f);
    }
};

//-----------------------------------------------------------------------------

struct member_base // used in traits detection
{
};

template <class Object, class Type, class... Tags>
struct member : member_base
{
    using type = Type;
    using object_type = Object;
    constexpr member(const char* name, Type Object::*member, Tags&&... tags)
        : name_{name}
        , member_{member}
        , tags_{std::forward<Tags>(tags)...}
    {
    }

    constexpr const char* name() const { return name_; }

    constexpr const type& access(const object_type& obj) const
    {
        return obj.*member_;
    }
    type& access(object_type& obj) { return obj.*member_; }

    constexpr tuple<Tags...> tags() const { return tags_; }

private:
    const char* name_;
    type object_type::*member_;
    tuple<Tags...> tags_;
};

//-----------------------------------------------------------------------------
struct meta_object_data_base // used in traits detection
{
};

template <class Type, class Members = tuple<>, class Bases = tuple<>,
          class Tags = tuple<>>
struct meta_object_data : meta_object_data_base
{
    using type = Type;
    constexpr meta_object_data(const char* name, Members members = {},
                               Bases bases = {}, Tags tags = {})
        : name{name}
        , members{members}
        , bases{bases}
        , tags{tags}
    {
    }
    const char* name{};
    Members members;
    Bases bases;
    Tags tags;
};

//-----------------------------------------------------------------------------
//! A wrapper class to hold the type
template <class T>
struct type_wrapper
{
    using type = T;
};

//-----------------------------------------------------------------------------
//! Used to define the meta object
template <class Type, class Members = tuple<>, class Bases = tuple<>,
          class Tags = tuple<>>
class meta_object_builder
{
    const char* name_{};
    Members members_;
    Bases bases_;
    Tags tags_;

public:
    using type = Type;

    constexpr meta_object_builder(const char* name, Members members = {},
                                  Bases bases = {}, Tags tags = {})
        : name_{name}
        , members_{members}
        , bases_{bases}
        , tags_{tags}
    {
    }
    [[nodiscard]] constexpr auto build()
    {
        return meta_object_data<Type, Members, Bases, Tags>{name_, members_,
                                                            bases_, tags_};
    }

    template <class T, class... MTags>
    [[nodiscard]] constexpr auto add_member(const char* name, T Type::*member,
                                            MTags&&... tags)
    {
        return create_object(members_.push_back(create_member(
                                 name, member, std::forward<MTags>(tags)...)),
                             bases_, tags_);
    }

    template <class T>
    [[nodiscard]] constexpr auto add_base()
    {
        return create_object(members_, bases_.push_back(type_wrapper<T>{}),
                             tags_);
    }

    template <class T>
    [[nodiscard]] constexpr auto add_tag(T&& val)
    {
        return create_object(members_, bases_,
                             tags_.push_back(std::forward<T>(val)));
    }

private:
    template <class Members_, class Bases_, class Tags_>
    constexpr auto create_object(Members_ members, Bases_ bases, Tags_ tags)
        -> meta_object_builder<Type, Members_, Bases_, Tags_>
    {
        return {name_, members, bases, tags};
    }

    template <class T, class... MTags>
    constexpr auto create_member(const char* name, T Type::*member_ptr,
                                 MTags&&... tags) -> member<Type, T, MTags...>
    {
        return {name, member_ptr, tags...};
    }
};
//-----------------------------------------------------------------------------
template <class T, class... Tags>
constexpr auto build_meta_object(const char* name, Tags&&... tags)
{
    return meta_object_builder<T>(name, {}, {}, {tags...});
}
//-----------------------------------------------------------------------------

//! Performs a lookup of the meta object.
//! A user can specialize it to add 3rd party types
template <class T, class = void>
struct lookup_meta_object;

//! Default specialization performs an ADL lookup to find build_meta_object in
//! type's namespace
template <class T>
struct lookup_meta_object<
    T, std::void_t<decltype(build_meta_object((T**)nullptr))>>
{
    constexpr auto operator()()
    {
        // perform ADL lookup
        return build_meta_object((T**)nullptr);
    }
};
//-----------------------------------------------------------------------------
// Enforce compile time computation by declarting constexpr variable
//! A meta object for type T
template <class T>
constexpr auto meta_object_v = lookup_meta_object<T>{}();

//-----------------------------------------------------------------------------
template <class T>
using meta_object = decltype(meta_object_v<T>);

} // namespace typer