#include "typer/introspection.hpp"

#include "json.hpp"// nlohmann/json lib

#include "example.hpp"
#include "catch.hpp"

#include <iostream>
using namespace std::literals::string_literals;

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


TEST_CASE("json")
{
    demo::User user{1, "Bob"};
    nlohmann::json json = user;
    CHECK(R"({"id":1,"name":"Bob"})"s == json.dump());
    std::cout << json.dump();

}