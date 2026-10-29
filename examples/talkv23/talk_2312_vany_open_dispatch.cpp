#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx/ext/dispatch.hpp>
#include <bit_factory/anyxx/ext/dispatch_table.hpp>
#include <bit_factory/anyxx/ext/dispatch_vany.hpp>
#include <bit_factory/anyxx/ext/vany.hpp>
#include <catch2/catch_test_macros.hpp>
#include <format>
#include <iostream>
#include <string>

using namespace anyxx;
using namespace std::string_literals;

namespace {
namespace sample_12 {

TRAIT_EX(evaluable, copyable, 
  (FN_IMPL(public, std::string, to_string, (), const, 
    (){ return std::format("{}", self); }), 
   FN(bool, to_bool, (), const),
   FN(int, to_int, (), const),
   FN(double, to_double, (), const)),
   , , (HAS_OPEN_DISPATCH), ())

using value = vany<evaluable, bool, int, double>;

using concrete_values = vany_type_trait<value>::concrete_variant;
template <typename V>
struct concrete_value_trait_map : evaluable_default_trait_map<V> {
  bool to_bool(auto self) { return static_cast<bool>(self); };
  int to_int(auto self) { return static_cast<int>(self); };
  double to_double(auto self) { return static_cast<double>(self); };
};
template <>
struct evaluable_trait_map<bool> : concrete_value_trait_map<bool> {};
template <>
struct evaluable_trait_map<int> : concrete_value_trait_map<int> {};
template <>
struct evaluable_trait_map<double> : concrete_value_trait_map<double> {};
template <typename T>
T value_to(auto const& v) {
  if constexpr (std::same_as<bool, T>) {
    return v.to_bool();
  } else if constexpr (std::same_as<int, T>) {
    return v.to_int();
  } else if constexpr (std::same_as<double, T>) {
    return v.to_double();
  } else if constexpr (std::same_as<std::string, T>) {
    return v.to_string();
  } else {
    static_assert(false);
    return T{};
  }
}

TRAIT_EX(node, anyxx::deleteable, 
    (FN(value, evaluate, (), const)) 
    , , , (HAS_OPEN_DISPATCH), ())

VANY_DISPACH_DECLARE(, evaluate_plus, value,
                     (value(virtual_<any<evaluable>>,
                            virtual_<any<evaluable>>)),
                     ([]<typename L, typename R>(L&& l, R&& r) -> value
                        requires(!anyxx::is_any<L> && !anyxx::is_any<R>)
                      { return std::forward<L>(l) + std::forward<R>(r); }));

VANY_DISPACH_DECLARE(
    , evaluate_times, value,
    (value(virtual_<any<evaluable>>, virtual_<any<evaluable>>)),
    ([]<typename L, typename R>(L&& l, R&& r) -> value
       requires(!anyxx::is_any<L> && !anyxx::is_any<R>)
     { return std::forward<L>(l) * std::forward<R>(r); },
     []<typename R>(bool l, R&& r) -> value
       requires(!anyxx::is_any<R>)
     { return static_cast<int>(l) * std::forward<R>(r); },
     []<typename L>(L&& l, bool r) -> value
       requires(!anyxx::is_any<L>)
     { return std::forward<L>(l) * static_cast<int>(r); },
     [](bool l, bool r) -> value 
     { return l && r; }));

struct plus {
  any<node> left_, right_;
  value evaluate() const {
    return evaluate_plus(left_.evaluate(), right_.evaluate());
  }
};
struct times {
  any<node> left_, right_;
  value evaluate() const {
    return evaluate_times(left_.evaluate(), right_.evaluate());
  }
};

TRAIT_MAP(value, node) {
  value evaluate(value const& self) { return self; };
};

}  // namespace sample_12
}  // namespace

VANY_DISPACH(sample_12, evaluate_plus)
VANY_DISPACH(sample_12, evaluate_times)

TEST_CASE("talk_2312_vany_open_dispatch core") {
  using namespace sample_12;
  any<node> expr{std::in_place_type<times>,
                 any<node>{std::in_place_type<value>, 2},
                 any<node>{std::in_place_type<plus>,
                           any<node>{std::in_place_type<value>, 3},
                           any<node>{std::in_place_type<value>, 4}}};

  auto v = expr.evaluate();
  CHECK(v.to_string() == "14");

  any<node> expr1{std::in_place_type<times>,
                  any<node>{std::in_place_type<value>, 2.1},
                  any<node>{std::in_place_type<plus>,
                            any<node>{std::in_place_type<value>, 3.5},
                            any<node>{std::in_place_type<value>, 4}}};

  auto v1 = expr1.evaluate();
  CHECK(v1.to_string() == "15.75");

  any<node> expr2{std::in_place_type<times>,
                  any<node>{std::in_place_type<value>, false},
                  any<node>{std::in_place_type<plus>,
                            any<node>{std::in_place_type<value>, 3.5},
                            any<node>{std::in_place_type<value>, 4}}};

  auto v2 = expr2.evaluate();
  CHECK(v2.to_string() == "0");

  any<node> expr3{std::in_place_type<times>,
                  any<node>{std::in_place_type<value>, true},
                  any<node>{std::in_place_type<plus>,
                            any<node>{std::in_place_type<value>, 3.5},
                            any<node>{std::in_place_type<value>, 4}}};

  auto v3 = expr3.evaluate();
  CHECK(v3.to_string() == "7.5");
}

namespace {
namespace sample_12 {
TRAIT_MAP(std::string, node) {
  value evaluate(std::string const& self) { return self; };
};
TRAIT_MAP(std::string, evaluable) {
  std::string as_string(std::string const& self) { return self; };
  bool to_bool(auto const& self) { return !self.empty(); };
  int to_int(auto const& self) { return std::atoi(self.c_str()); };
  double to_double(auto const& self) { return std::atof(self.c_str()); };
};
auto __ = evaluate_plus.define<std::string, std::string>(
    [](const auto& lhs, const auto& rhs) -> value { return lhs + rhs; });
auto __ = evaluate_plus.define<concrete_values, std::string>(
    [](const auto& lhs, const auto& rhs) -> value {
      return std::visit(
          [&]<typename LHS>(LHS v) -> value {
            return v + value_to<LHS>(using_as<evaluable>(rhs));
          },
          lhs);
    });
auto __ = evaluate_plus.define<std::string, concrete_values>(
    [](const auto& lhs, const auto& rhs) -> value {
      return std::visit(
          [&]<typename RHS>(RHS v) -> value {
            return value_to<RHS>(using_as<evaluable>(lhs)) + v;
          },
          rhs);
    });
}  // namespace sample_12
}  // namespace

TEST_CASE("talk_2312_vany_open_dispatch string ext") {
  using namespace sample_12;
  any<node> expr{std::in_place_type<plus>,
                 any<node>{std::in_place_type<value>, 2},
                 any<node>{std::in_place_type<plus>,
                           any<node>{std::in_place_type<value>, 3},
                           any<node>{std::in_place_type<std::string>, "4"}}};

  auto v = expr.evaluate();
  CHECK(v.to_string() == "9");

  any<node> expr1{any<node>{
      std::in_place_type<times>, any<node>{std::in_place_type<value>, 3},
      any<node>{std::in_place_type<std::string>, std::string{"4"}}}};
  CHECK_THROWS_AS(expr1.evaluate(), no_default_function_error);

  any<node> expr2{any<node>{
      std::in_place_type<plus>,
      any<node>{std::in_place_type<std::string>, std::string{"Hello, "}},
      any<node>{std::in_place_type<std::string>, std::string{"world"}}}};
  auto v2 = expr2.evaluate();
  CHECK(v2.to_string() == "Hello, world");
}
