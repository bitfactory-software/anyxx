#include <cmath>
#include <concepts>
#include <iostream>
#include <string>
#include <vector>

#include <catch.hpp>

#include <virtual_void/interface/declare_macro.hpp>

#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/has_no_meta/unique_ptr.hpp>
#include <virtual_void/data/has_no_meta/value.hpp>
#include <virtual_void/interface/call_operator.hpp>
#include <virtual_void/interface/subscript_operator.hpp>

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void::interface;
using namespace virtual_void::data;
using namespace virtual_void::data::has_no_meta;

namespace {
using map_t = std::map<int, std::string>;
using vector_t = std::vector<std::string>;
}  // namespace

VV_RUNTIME_STATIC(type_info, map_t)
VV_RUNTIME_STATIC(type_info, vector_t)

template <typename VV>
using map_i = subscript_operator<VV, std::string&(int), mutable_>;

template <typename VV>
using const_vector_i = subscript_operator<VV, std::string const&(int), const_>;

// 'operator []' is ambiguous
// template <typename VV>
// using mutable_vector_i =
//    overloaded_subscript_operator<VV, std::string&(int), mutable_, std::string
//    const&(int), const_ >;

namespace {
template <template <typename> typename MAP, typename OBSERVER>
concept mutable_subscribeable =
    requires(map_t map, int i, std::string s) { MAP<OBSERVER>{map}[i] = s; };
template <template <typename> typename MAP, typename OBSERVER>
concept const_subscribeable =
    requires(map_t map, int i, std::string s) { MAP<OBSERVER>{map}[i] == s; };
}  // namespace

static_assert(!mutable_subscribeable<const_vector_i, const_observer>);
static_assert(const_subscribeable<const_vector_i, const_observer>);
static_assert(!mutable_subscribeable<const_vector_i, mutable_observer>);
static_assert(const_subscribeable<const_vector_i, mutable_observer>);

static_assert(!mutable_subscribeable<map_i, const_observer>);
static_assert(!const_subscribeable<map_i, const_observer>);
static_assert(mutable_subscribeable<map_i, mutable_observer>);
static_assert(const_subscribeable<map_i, mutable_observer>);

TEST_CASE("mutable subscript_operator with mutable_observer") {
  map_t map{{0, "hallo"}};
  map_i<mutable_observer> m{map};
  REQUIRE(m[0] == "hallo");
  m[1] = "world";
  REQUIRE(m[1] == "world");
  REQUIRE(map[1] == "world");
}

TEST_CASE("const subscript_operator with const_observer") {
  vector_t vector{"hallo"};
  const_vector_i<const_observer> v{vector};
  REQUIRE(v[0] == "hallo");
}
TEST_CASE("const subscript_operator with mutable_observer") {
  vector_t vector{"hallo"};
  const_vector_i<mutable_observer> v{vector};
  REQUIRE(v[0] == "hallo");
}

TEST_CASE("mutable subscript_operator (vector) with mutable_observer") {
  vector_t vector{"hallo", ""};
  map_i<mutable_observer> v{vector};
  REQUIRE(v[0] == "hallo");
  v[1] = "world";
  REQUIRE(v[1] == "world");
  REQUIRE(vector[1] == "world");
}
