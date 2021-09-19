#include "typer/extra/hash.hpp"

#include "example.hpp"
#include "catch.hpp"

#include <unordered_set>

TEST_CASE("hash")
{
    std::hash<std::string> s;
    auto hash = typer::hash<demo::User>{};

    demo::User u1{1, "Zak"};
    demo::User u2{2, "Dana"};
    CHECK(hash(u1) == hash(u1));
    CHECK(hash(u1) != hash(u2));
}

struct UserComparer
{
    bool operator()(const demo::User& lhs, const demo::User& rhs) const
    {
        return lhs.id == rhs.id && lhs.name == rhs.name;
    }
};

TEST_CASE("using hash with unordered_set")
{
    std::unordered_set<demo::User, typer::hash<demo::User>, UserComparer> set;
    set.insert(demo::User{1, "Zak"});
}

#include "typer/extra/equal_op.hpp" // to get equal op

// unfortunately std::hash do not support specialization with enable_if, so we
// have two options: use typer::hash<T> as template argument in containers or
// specialize concrete type with inheritance
namespace std
{
template <>
struct hash<demo::User> : typer::hash<demo::User>
{
};
} // namespace std
