#define ANYXX_26
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

template <typename Self>
struct evaluable : copyable<Self> {
  std::string to_string() const;
  // static std::string to_string(Self const& self) {
  //   return std::format("{}", self);
  // }
  using dispatch_table[[= v_table_data]] = anyxx::dispatch_table;
};

template <>
struct evaluable<bool> {
  static std::string to_string(bool const& self) {
    return std::format("{}", self);
  }
};
template <>
struct evaluable<int> {
  static std::string to_string(int const& self) {
    return std::format("{}", self);
  }
};
template <>
struct evaluable<double> {
  static std::string to_string(double const& self) {
    return std::format("{}", self);
  }
};
template <typename... T>
struct evaluable<std::variant<T...>> {
    static std::string to_string(std::variant<T...> const& self) {
        return std::visit([](auto&& arg) { return as<evaluable>(arg).to_string(); }, self);
    }
};


using value = vany<evaluable, bool, int, double>;
using bool_int_double = vany_type_trait<value>::concrete_variant;
static_assert(std::is_same_v<bool_int_double, std::variant<bool, int, double>>);

template <typename Self>
struct node : anyxx::deleteable<Self> {
  value evaluate() const;
  using dispatch_table[[= v_table_data]] = anyxx::dispatch_table;
};

//VANY_DISPACH_DECLARE(
//     , evaluate_plus,
//     (value(virtual_<any<evaluable>>, virtual_<any<evaluable>>)),
//     ([]<no_any L, no_any R>(L&& l, R&& r) -> value
//{ return std::forward<L>(l) + std::forward<R>(r);
// }));
//struct plus {
//  any<node> left_, right_;
//  value evaluate() const {
//    return evaluate_plus(left_.evaluate(), right_.evaluate());
//  }
//};

// VANY_DISPACH_DECLARE(
//     , evaluate_times,
//     (value(virtual_<any<evaluable>>, virtual_<any<evaluable>>)),
//     ([]<no_any L, no_any R>(L&& l, R&& r) -> value
//{
//     return std::forward<L>(l) * std::forward<R>(r);
// },
//[]<no_any R>(bool l, R&& r) -> value
//{
//     return static_cast<int>(l) * std::forward<R>(r);
// },
//[]<no_any L>(L&& l, bool r) -> value
//{
//     return std::forward<L>(l) * static_cast<int>(r);
// },
//[](bool l, bool r) -> value
//{ return l && r; }));
// struct times {
//   any<node> left_, right_;
//   value evaluate() const {
//     return evaluate_times(left_.evaluate(), right_.evaluate());
//   }
// };

struct leave {
  value v_;
  value evaluate() const { return v_; }
};

}  // namespace sample_12
}  // namespace

TEST_CASE("talk_2611_vany_dispatchXXX") {
  using namespace sample_12;
  std::stringstream ss;
  CHECK(evaluable<bool>::to_string(true) == "true");
  CHECK(as<evaluable>(true).to_string() == "true");
  value v1{true};
  any<evaluable> v2{std::in_place_type<bool_int_double>, bool_int_double{42}};
  CHECK(v1.to_string() == "true");
  CHECK(v2.to_string() == "42");
  // CHECK(ss.str() == "true\n42\n3.14\n");
}

// VANY_DISPACH(sample_12, evaluate_plus)
// VANY_DISPACH(sample_12, evaluate_times)
//
// TEST_CASE("talk_2612_vany_open_dispatch core") {
//   using namespace sample_12;
//   any<node> expr{times{leave{2}, plus{leave{3}, leave{4}}}};
//   auto v = expr.evaluate();
//   CHECK(v.to_string() == "14");
//
//   any<node> expr1{times{leave{2.1}, plus{leave{3.5}, leave{4}}}};
//   auto v1 = expr1.evaluate();
//   CHECK(v1.to_string() == "15.75");
//
//   any<node> expr2{times{leave{false}, plus{leave{3.5}, leave{4}}}};
//   auto v2 = expr2.evaluate();
//   CHECK(v2.to_string() == "0");
//
//   any<node> expr3{times{leave{true}, plus{leave{3.5}, leave{4}}}};
//   auto v3 = expr3.evaluate();
//   CHECK(v3.to_string() == "7.5");
// }
//
// namespace {
// namespace sample_12 {
// template <>
// struct evaluable<std::string> {
//   std::string as_string(std::string const& self) { return self; };
// };
// auto __ = evaluate_plus.define<std::string, std::string>(
//     [](const auto& lhs, const auto& rhs) -> value { return lhs + rhs; });
// auto __ = evaluate_plus.define<bool_int_double, std::string>(
//     [](const auto& lhs, const auto& rhs) -> value {
//       return as<evaluable>(lhs).to_string() + rhs;
//     });
// auto __ = evaluate_plus.define<std::string, bool_int_double>(
//     [](const auto& lhs, const auto& rhs) -> value {
//       return lhs + as<evaluable>(rhs).to_string();
//     });
// }  // namespace sample_12
// }  // namespace
//
// TEST_CASE("talk_2612_vany_open_dispatch string ext") {
//   using namespace sample_12;
//   any<node> expr{plus{leave{2}, plus{leave{3}, leave{"4"s}}}};
//
//   auto v = expr.evaluate();
//   CHECK(v.to_string() == "234");
//
//   any<node> expr1{times{leave{3}, leave{"4"s}}};
//   CHECK_THROWS_AS(expr1.evaluate(), no_default_function_error);
//
//   any<node> expr2{plus{leave{"Hello, "s}, leave{"world"s}}};
//   auto v2 = expr2.evaluate();
//   CHECK(v2.to_string() == "Hello, world"s);
// }
