#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <compare>
#include <concepts>
#include <format>
#include <ios>
#include <iostream>
#include <print>
#include <sstream>
#include <string>
#include <variant>

namespace example_2c {

struct custom {
  std::string answer;
};

struct any_value_has_open_dispatch {};
ANY(any_value, (ANY_METHOD_DEFAULTED(std::string, to_string, (), const,
                                     [x]() { return std::format("{}", x); }),
                ANY_METHOD_DEFAULTED(void, from_string, (std::string_view), ,
                                     [&x](std::string_view sv) -> void {
                                       std::stringstream ss{std::string{sv},
                                                            std::ios_base::in};
                                       ss >> x;
                                     })))

using vany_value = anyxx::make_vany<any_value, anyxx::shared_const, anyxx::rtti,
                                    bool, int, double, std::string>;
using concrete_value = anyxx::vany_type_trait<vany_value>::concrete_variant;
using any_in_variant = anyxx::vany_type_trait<vany_value>::any_in_variant;

static_assert(
    std::same_as<concrete_value, std::variant<bool, int, double, std::string>>);
static_assert(std::same_as<any_in_variant, any_value<anyxx::shared_const>>);

}  // namespace example_2c

ANY_MODEL_MAP((example_2c::custom), example_2c::any_value) {
  static std::string to_string(custom const& x) {
    return "{" + x.answer + "}";
  };
  static void from_string(custom & x, std::string_view sv) {
    x.answer = sv.substr(1, sv.length() - 2);
  };
};

ANY_MODEL_MAP((example_2c::concrete_value), example_2c::any_value) {
  static std::string to_string(concrete_value const& x) {
    return std::visit(
        [&]<typename T>(T const& v) {
          return any_value_model_map<T>::to_string(v);
        },
        x);
  };
  static void from_string(concrete_value & x, std::string_view sv) {
    return std::visit(
        [&]<typename T>(T& v) -> void {
          return any_value_model_map<T>::from_string(v, sv);
        },
        x);
  };
};

namespace example_2c {}  // namespace example_2c

TEST_CASE("example 2ca trait any variant") {
  using namespace example_2c;
  using namespace std::string_literals;
  using namespace anyxx;

  static_assert(
      constructibile_for<any_value<shared_const>, vany_value::erased_data_t>);
  vany_value vv_custom_43 =
      any_value<shared_const>{std::in_place_type<custom>, "43"};
  vany_value vv_custom_42 = {
      any_value<shared_const>{std::in_place, custom{"42"}}};

  CHECK(vany_value{true}.to_string() == "true");
  CHECK(vv_custom_42.to_string() == "{42}");
  CHECK(vv_custom_43.to_string() == "{43}");
}

namespace example_2c {

constexpr static inline auto vany_stream_static_dispatch = anyxx::overloads{
    [](const std::string& s, std::ostream& os) {
      os << "String: " << s << ", ";
    },
    [](int i, std::ostream& os) { os << "Int: " << i << ", "; },
    [](double d, std::ostream& os) { os << "Double: " << d << ", "; },
    [](bool b, std::ostream& os) {
      os << "Bool: " << std::boolalpha << b << ", ";
    }};

extern anyxx::dispatch_vany<
    vany_value,
    anyxx::dispatch<void(anyxx::virtual_<any_value<anyxx::shared_const>>,
                         std::ostream&)>,
    vany_stream_static_dispatch>
    vany_stream;
}  // namespace example_2c

namespace example_2c {

anyxx::dispatch_vany<
    vany_value,
    anyxx::dispatch<void(anyxx::virtual_<any_value<anyxx::shared_const>>,
                         std::ostream&)>,
    vany_stream_static_dispatch>
    vany_stream;

auto __ = vany_stream.define<custom>(
    [](const custom& c, std::ostream& os) { os << "Custom: " << c.answer; });
}  // namespace example_2c

TEST_CASE("example 2cb trait any variant single open dispatch") {
  using namespace example_2c;
  using namespace std::string_literals;
  using namespace anyxx;
  vany_value vv1{std::string{"hello"}};
  vany_value vv2{int{42}};
  vany_value vv3{
      any_value<shared_const>{std::in_place_type<custom>, "Hello world!"}};

  std::stringstream ss;
  vany_stream(vv1, ss);
  vany_stream(vv2, ss);
  vany_stream(vany_value{true}, ss);
  vany_stream(vv3, ss);
  CHECK(ss.str() == "String: hello, Int: 42, Bool: true, Custom: Hello world!");
}

namespace example_2c {

struct compare_equal_types {
  template <typename T>
    requires(!anyxx::is_any<T>)
  constexpr auto operator()(T&& t, T&& u) const -> std::partial_ordering {
    return std::forward<T>(t) <=> std::forward<T>(u);
  }
};

constexpr static inline auto vany_compare_static_dispatch = anyxx::overloads{
    compare_equal_types{},
    [](bool lhs, std::integral auto rhs) -> std::partial_ordering {
      return lhs <=> static_cast<bool>(rhs);
    },
    [](std::integral auto lhs, bool rhs) -> std::partial_ordering {
      return static_cast<bool>(lhs) <=> rhs;
    },
    [](bool lhs, bool rhs) -> std::partial_ordering { return lhs <=> rhs; },
    [](std::integral auto lhs, std::integral auto rhs) -> std::partial_ordering {
      return lhs <=> rhs;
    },
    [](std::floating_point auto lhs, std::floating_point auto rhs)
        -> std::partial_ordering { return lhs <=> rhs; }};

anyxx::dispatch_vany<vany_value,
                     anyxx::dispatch<std::partial_ordering(
                         anyxx::virtual_<any_value<anyxx::shared_const>>,
                         anyxx::virtual_<any_value<anyxx::shared_const>>)>,
                     vany_compare_static_dispatch>
    vany_compare;

auto operator<=>(const vany_value& lhs, const vany_value& rhs) {
  return vany_compare(lhs, rhs);
}
auto operator==(const vany_value& lhs, const vany_value& rhs) {
  return lhs <=> rhs == std::weak_ordering::equivalent;
}
auto operator!=(const vany_value& lhs, const vany_value& rhs) {
  return lhs <=> rhs != std::weak_ordering::equivalent;
}
auto operator<(const vany_value& lhs, const vany_value& rhs) {
  return lhs <=> rhs == std::weak_ordering::less;
}
auto operator>=(const vany_value& lhs, const vany_value& rhs) {
  return lhs <=> rhs != std::weak_ordering::less;
}
auto operator>(const vany_value& lhs, const vany_value& rhs) {
  return lhs <=> rhs == std::weak_ordering::greater;
}
auto operator<=(const vany_value& lhs, const vany_value& rhs) {
  return lhs <=> rhs != std::weak_ordering::greater;
}

auto __ = vany_compare.define<custom, custom>(
    [](const auto& lhs, const auto& rhs) -> std::partial_ordering {
      return lhs.answer <=> rhs.answer;
    });
auto __ = vany_compare.define<concrete_value, custom>(
    [](const auto& lhs, const auto& rhs) -> std::partial_ordering {
      return as_any_value<concrete_value>(lhs).to_string() <=> rhs.answer;
    });
auto __ = vany_compare.define<custom, concrete_value>(
    [](const auto& lhs, const auto& rhs) -> std::partial_ordering {
      return lhs.answer <=> as_any_value<concrete_value>(rhs).to_string() ;
    });
auto __ = vany_compare.define<concrete_value, concrete_value>(
    [](const auto& lhs, const auto& rhs) -> std::partial_ordering {
      return as_any_value<concrete_value>{lhs}.to_string() <=>
             as_any_value<concrete_value>{rhs}.to_string();
    });
}  // namespace example_2c

TEST_CASE("example 2cc trait any variant double dispatch") {
  using namespace example_2c;
  using namespace std::string_literals;
  using namespace anyxx;
  vany_value vv1{"hello"s};
  vany_value vvbt{bool{true}};
  vany_value vvbf{bool{false}};
  vany_value vvi{int{42}};
  vany_value vvi0{int{0}};
  vany_value vvf{double{42.0}};
  vany_value vv3{
      any_value<shared_const>{std::in_place_type<custom>, "Hello world!"}};
  vany_value vv4{
      any_value<shared_const>{std::in_place_type<custom>, "hello"}};

  bool x = vany_compare(vv1, vv1) == std::partial_ordering::equivalent;
  CHECK(x);
  auto y = vv1 <=> vv1;
  CHECK(y == std::weak_ordering::equivalent);
  auto z = vv1 == vv1;
  CHECK(z);
  CHECK(vv3 == vv3);
  CHECK(vv3 != vv1);
  CHECK("hello"s > "Hello world!"s);
  CHECK(vv1 > vv3);
  CHECK(vv3 != vv4);
  CHECK(vv1 == vv4);
  CHECK(vvi == vvf);
  CHECK(vvbt != vvbf);
  CHECK(vvi != vvbf);
  CHECK(vvi == vvbt);
  CHECK(vvi0 == vvbf);
 }
