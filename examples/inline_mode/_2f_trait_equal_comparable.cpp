// Walkthrough to implement c++ 0x concept_map with ``Any++`` for static AND
// dynmaic polymorhism
//
// In C++20, concepts and concept maps provide a powerful way to define
// interfaces and their implementations. They where primarily designed
// for static polymorphism. With `Any++`, we can extend this idea to support
// both static and dynamic polymorphism.
//
// As an illustrative example, we choose to provide a `equal_comparable` trait.
// This means, we want a way to provide an "equal"(==) and a "not equal"(!=)
// operator for any type, regardless of whether the type itself provides these
// operators.
// Whereby it should be enough for the application to provide only one of the
// two operators. The other should be automatically provided.
//
// Why would you use this?
// It allows you to write generic code that can work with any type that models
// `equal_comparable`, without requiring those types to inherit from a common
// base class or implement specific interfaces. This can be particularly useful
// when working with third-party types or when you want to add functionality to
// existing types without modifying their definitions.
//

#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

// The library namespace, where the trait and an associated algorithm are
// defined.
namespace lib_2f {

// The names for the functions in the model map map are "eq" and "ne".
// These functions are then used to implement the operators == and !=
// in the external interface.
TRAIT_(equal_comparable, anyxx::dynamic_value,
       // anyxx::self is a magic type that represents the type of the object for
       // whitch the trait is called. It is used to smooth the differences
       // between static and dynamic dispatch, so the implemention can be
       // written against the actual type of the object ref/const qualified as
       // specified. If the mapped type provies already an operator,
       // ANY_OP_MAP_NAMED_FRIEND use it as the default implementation.
       (ANY_OP_MAP_NAMED_FRIEND(bool, ==, eq, (anyxx::self const&), const),
        // We can use the == operator of the external interface to
        // provide a default implementation for the != operator.
        ANY_OP_DEF(public, bool, !=, ne, (anyxx::self const&), const,
                   [&x](auto const& r) { return !Map{}.eq(x, r); })))

// TRAIT automatically defines the is_equal_comparable_model concept.
// Here we use this concept to check that some basic types model the trait:
static_assert(is_equal_comparable_model<int>);
static_assert(is_equal_comparable_model<double>);
static_assert(is_equal_comparable_model<bool>);
static_assert(is_equal_comparable_model<std::string>);

// This is an algorithm that use the == and != operators on objects of any
// type that models the equal_comparable trait.
// Because the Proxy type of the trait is a template parameter, this algorithm
// can be used both for static AND dynamic dispatch!
template <anyxx::is_proxy Proxy>
void test_equal_comparable_(anyxx::any<equal_comparable, Proxy> const& a,
                            anyxx::any<equal_comparable, Proxy> const& b) {
  CHECK((a == b) == (b == a));
  CHECK((a != b) == (b != a));
  CHECK((a == b) != (b != a));
  CHECK((a != b) != (b == a));
}

// This is a convenience wrapper for the test algorithm above in the static
// dispatch case. Because it is guarded by the concept
// is_equal_comparable_model provided by the library, any missuse should get a
// nice compiler error.
template <is_equal_comparable_model T>
void test_equal_comparable(T const& a, T const& b) {
  using namespace anyxx;
  test_equal_comparable_<using_cref<T>>(a, b);
}

}  // namespace lib_2f

namespace app_2f {

// A type that does not provide operator== ...
struct a_type {
  std::string name;
};
// ... and for which we do not provide a model map, so the provided concept is
// not satisfied:
static_assert(!lib_2f::is_equal_comparable_model<app_2f::a_type>);

}  // namespace app_2f

namespace app_2f {

// An other type that does not provide operator== ...
struct b_type {
  std::string name;
};

}  // namespace app_2f

// ... but for which we provide a model map, so the provided concept is
// satisfied:
ANY_MODEL_MAP((app_2f::b_type), lib_2f::equal_comparable) {
  static auto eq(app_2f::b_type const& self, app_2f::b_type const& r) {
    return self.name == r.name;
  };
};
static_assert(lib_2f::is_equal_comparable_model<app_2f::b_type>);

// Now we can use the algorithm defined in the library
// 1. Static dispatch usage:
TEST_CASE("equal_comparable static") {
  lib_2f::test_equal_comparable(1, 1);
  lib_2f::test_equal_comparable(app_2f::b_type{"A"}, app_2f::b_type{"B"});
}
// 2. Dynamic dispatch usage.
TEST_CASE("equal_comparable dynamic") {
  using any_equal_comparable =
      anyxx::any<lib_2f::equal_comparable, anyxx::val<>>;
  std::vector<std::pair<any_equal_comparable, any_equal_comparable>> v{
      {app_2f::b_type{"A"}, app_2f::b_type{"A"}},
      {app_2f::b_type{"A"}, app_2f::b_type{"B"}},
      {3, 3},
      {3, 4},
      {3.14, 3.14},
      {3.14, 2.71},
      {"hello", "hello"},
      {"hello", "world"},
  };
  for (auto const& [a, b] : v) {
    lib_2f::test_equal_comparable_(a, b);
  }

  {
    // This should throw, because the types of a and b are not the same.
    // This behaviour is enforced by the \ref anyxx::self keyword type.
    using namespace anyxx;
    using namespace lib_2f;
    any<equal_comparable, val<>> a{42};
    any<equal_comparable, val<>> b{3.14};
    CHECK_THROWS_AS(a == b, type_mismatch_error);
  }
}
