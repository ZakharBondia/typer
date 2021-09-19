# Typer
An another type reflection library.
WIP

## Goals
- header only
- fully compile time
- macro free
- no dependencies, STL only
- C\++14 (not yet, currently C\++17)

There two core headers declaration.hpp and introspection.hpp that defines all reflaction functionality. 
There are few headers in the extra folder
## Usage 

### Meta object declartion
Declaring the meta object is simple. 
```c++
namespace demo 
{
    struct User
    {
        int id;
        string name;
    };
}
```
It is required to add one just function in the type's namespace
```c++
#include "typer/declaration.hpp"
namespace demo 
{
    constexpr auto build_meta_object(User**)
    {
        return typer::build_meta_object<User>("User")
            .add_member("id", &User::id)
            .add_member("name", &User::name)
            .build();
    }
}
```
Few things to note here:
- "typer/declaration.hpp" contains all you need for meta type declaration
- build_meta_object is a function you need to declare
- return type shall be constexpr auto
- Type** is the argument of the function. Note double star, it prevents implicit pointer casts during meta object lookup
- inside the function just list all class members with their names and bases classes if any
- enjoy the power of reflection

Another example with the base class

```c++
class DerivedUser : public User
{
    friend constexpr auto build_meta_object(DerivedUser**)
    {
        return typer::build_meta_object<DerivedUser>("DerivedUser")
            .add_base<User>()
            .add_member("address", &DerivedUser::address)
            .build();
    }
private:
    string address;
};

```
### Meta object usage
Meta object is stored in the constexpr variable ```typer::meta_object_v<T>```. But you rarely need to access it directly as there are bunch of handy functions that shall be used instead

```c++
cout << typer::type_name<User>(); // prints "User"
cout << typer::members_size<User>(); // prints 2

User user{100, "Zak"};

//cout support defined in "extra/ostream_op.hpp"
cout << user; // prints "{id: 100, name: Zak}"

// inrospection of each member
int index = 0;
typer::for_each_member_of<User>([&user, &index](auto member){
    //pints "1 id: 100"
    //pints "2 name: Zak"
    cout << ++index << typer::name_of(member) << ": " << typer::access(user, mamber); 
});

//access can also be used to write to members
typer::for_each_member_of<User>([&user, &index](auto member){
    if constexpr(typer::name_of(member) == "id"sv)
    {
        typer::access(user, mamber) = 200;
    }
    if constexpr(typer::name_of(member) == "name"sv)
    {
        typer::access(user, mamber) = "Dana";
    }
});
cout << user; // prints "{id: 200, name: Dana}"
```

## Real world example
Below is integration of typer with a popular json library nlohmann/json 
```c++
#include "typer/introspection.hpp"
#include "json.hpp"// nlohmann/json lib

namespace nlohmann
{
// Serializer specialized for types supporting reflection
template <typename T>
struct adl_serializer<T, typer::enable_if_supported_t<T>>
{
    static void to_json(json& j, const T& value) {
        j = json{};
        typer::for_each_member_of<T>([&j, &value](auto member){
            j[typer::name_of(member)] = typer::access(value, member);
        });
    }

    static void from_json(const json& j, T& value) {
        typer::for_each_member_of<T>([&j, &value](auto member){
            using MemberType = typer::type_of<decltype(member)>;
            typer::access(value, member) =
                j[typer::name_of(member)].template get<MemberType>();
        });
    }
};

} // namespace nlohmann
```
That's it now all your types defined with typer can be serialized to json

```c++
    demo::User user{1, "Bob"};
    nlohmann::json json = user;
    std::cout << json.dump();
```



## License
MIT


