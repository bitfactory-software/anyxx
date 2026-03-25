// Walkthrough to implement c++ 0x concept_maps with ``Any++`` for static AND
// dynmaic polymorhism
//
// In C++20, concepts and concept maps provide a powerful way to define
// interfaces and their implementations. They where primarily designed
// for static polymorphism. With `Any++`, we can extend this idea to support
// both static and dynamic polymorphism.
//
// As an illustrative example, we choose to provide a `partial_equality` trait.
// This means, we want a way to provide an "equal"(==) and a "not equal"(!=)
// operator for any type, regardless of whether the type itself provides these
// operators.
// Whereby 'partial equality' means it may not be the case that for every pair
// of objects, either `a == b` or `a != b` is true. This for example the case
// with floating-point comparisons.
//
// Why would you use this?
// It allows you to write generic code that can work with any type that models
// `partial_equality`, without requiring those types to inherit from a common
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

// Our default implementations reference each other, this creates a
// circular dependency, but this allows us to define only one of the
// two operators in a model map, and the other will be automatically
// defined.
// To enforce the interruption of the circular dependency, we
// specify the default definition for == in de default model as
// protected, so it can be only used from a deriving map, as we will se
// later.(*1*)
// The names for the functions in model map map are "eq" and "ne".
// These functions are then used to implement the operators == and !=
// in the external interface of the trait.
TRAIT_EX(partial_equality,
         (ANY_OP_DEF(protected, bool, ==, eq, (anyxx::self const&), const,
                     [&x](auto const& r) {
                       // Because we trait x and r as partial_equality, we will
                       // use the != operator provided by the model map for the
                       // type of x and r. So the circular is broken.
                       return !(trait_as<partial_equality>(x) !=
                                trait_as<partial_equality>(r));
                     }),
          ANY_OP_DEF(public, bool, !=, ne, (anyxx::self const&), const,
                     [&x](auto const& r) {
                       return !(trait_as<partial_equality>(x) ==
                                trait_as<partial_equality>(r));
                     })),
         , , ())

// Here we define the model map for all types that provide already operator==.
template <typename T>
  requires requires(T const& a, T const& b) {
    { a == b } -> std::convertible_to<bool>;
  }
struct partial_equality_model_map<T> : partial_equality_default_model_map<T> {
  static auto eq(T const& self, T const& r) { return self == r; }
  // Because we derive from partial_equality_default_model_map, the default
  // implementation of !=, 'ne' is available and will use the provided
  // operator== via the ne function above.
};

// TRAIT_EX automatically defines the is_partial_equality_model contept.
// Here we check that some basic types model the trait, which is expected
// because they provide operator==.
static_assert(is_partial_equality_model<int>);
static_assert(is_partial_equality_model<double>);
static_assert(is_partial_equality_model<bool>);
static_assert(is_partial_equality_model<std::string>);

// This is a test algorithm that use the == and != operators on objects of any
// type that models the partial_equality trait.
// Because the Proxy type of the trait is a template parameter, this algorithm
// can be used both for static AND dynamic dispatch!
template <anyxx::is_proxy Proxy>
void test_partial_equality_(anyxx::any<Proxy, partial_equality> const& a,
                            anyxx::any<Proxy, partial_equality> const& b) {
  CHECK((a == b) == (b == a));
  CHECK((a != b) == (b != a));
}

// This is a convenience wrapper for the test algorithm above in the static
// dispatch case. Because it is guarded by the concept is_partial_equality_model
// provided by the library, any missuse should get a nice compiler error.
template <is_partial_equality_model T>
void test_partial_equality(T const& a, T const& b) {
  using namespace anyxx;
  test_partial_equality_<anyxx::using_<T>>(a, b);
}

}  // namespace lib_2f

namespace app_2f {

// A type that does not provide operator== ...
struct a_type {
  std::string name;
};
// ... and for which we do not provide a model map, so the provided concept is
// not satisfied:
static_assert(!lib_2f::is_partial_equality_model<app_2f::a_type>);

}  // namespace app_2f

namespace app_2f {

// An other type that does not provide operator== ...
struct b_type {
  std::string name;
};

}  // namespace app_2f

// ... but for which we provide a model map, so the provided concept is
// satisfied:
ANY_MODEL_MAP((app_2f::b_type), lib_2f::partial_equality) {
  // For illustration purposes, we implemet the model in terms of the !=
  // operator.
  using default_map::eq;  //(*1*) We want to use the default implementation of
                          // eq, which is defined 'protected 'in the
                          // default_map. So we bring it into scope via this
                          // using directive.
  static auto ne(app_2f::b_type self, app_2f::b_type r) {
    return self.name != r.name;
  }
};
static_assert(lib_2f::is_partial_equality_model<app_2f::b_type>);

// Now we can use the algorithm defined in the library
// 1. Static dispatch usage:
TEST_CASE("partial_equality static") {
  lib_2f::test_partial_equality(1, 1);
  lib_2f::test_partial_equality(app_2f::b_type{"A"}, app_2f::b_type{"B"});
}
// 2. Dynamic dispatch usage:
TEST_CASE("partial_equality dynamic") {
  using any_partial_equality = anyxx::any<anyxx::val, lib_2f::partial_equality>;
  std::vector<std::pair<any_partial_equality, any_partial_equality>> v{
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
    lib_2f::test_partial_equality_(a, b);
  }
}
