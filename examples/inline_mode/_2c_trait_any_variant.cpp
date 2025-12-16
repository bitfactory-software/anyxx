#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
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
                                     [&x](std::string_view sv) {
                                       std::stringstream ss{std::string{sv},
                                                            std::ios_base::in};
                                       ss >> x;
                                     })))

using vany_value = anyxx::vany_type<any_value, anyxx::shared_const, anyxx::rtti,
                                    bool, int, double, std::string>;

using vany_values_t = std::vector<vany_value>;

}  // namespace example_2c

ANY_MODEL_MAP((example_2c::custom), example_2c::any_value) {
  static std::string to_string(const custom& x) {
    return "{" + x.answer + "}";
  };
  static void from_string(custom & x, std::string_view sv) {
    x.answer = sv.substr(1, sv.length() - 2);
  };
};

namespace example_2c {}  // namespace example_2c

TEST_CASE("example 2ca trait any variant") {
  using namespace example_2c;
  using namespace std::string_literals;
  using namespace anyxx;
  vany_value vv3{std::string{"hello"}};
  vany_value vv4{std::in_place_type<any_value<anyxx::shared_const>>,
                 std::in_place_type<custom>, "42"};
  static_assert(
      constructibile_for<any_value<shared_const>, vany_value::erased_data_t>);
  auto v6 = any_value<shared_const>(std::in_place_type<custom>, "43");
  vany_value vv6{v6};
  vany_value vv5{any_value<shared_const>{std::in_place_type<custom>, "42"}};
  auto custom_value = any_value<shared_const>{std::in_place_type<custom>, "42"};
  vany_value vv1{custom_value};
  vany_value vv_custom_43 =
      any_value<shared_const>{std::in_place_type<custom>, "43"};
  vany_values_t vany_values = {
      any_value<shared_const>{std::in_place, custom{"42"}}};

  CHECK(vany_value{true}.to_string() == "true");
  CHECK(vv_custom_43.to_string() == "{43}");
}

TEST_CASE("example 2cb trait any variant single open dispatch") {
  using namespace example_2c;
  using namespace std::string_literals;
  using namespace anyxx;
  vany_value vv1{std::string{"hello"}};
  vany_value vv2{int{42}};
  vany_value vv3{
      any_value<shared_const>{std::in_place_type<custom>, "Hello world!"}};

  vany_dispatch<
      dispatch<void(virtual_<any_value<shared_const>>, std::ostream&)>,
      overloads{
          [&](const std::string& s, std::ostream& os) {
            os << "String: " << s << ", ";
          },
          [&](int i, std::ostream& os) { os << "Int: " << i << ", "; },
          [&](double d, std::ostream& os) { os << "Double: " << d << ", "; },
          [&](bool b, std::ostream& os) {
            os << "Bool: " << std::boolalpha << b << ", ";
          }}>
      vany_stream;
  vany_stream.define<custom>(
      [](const custom& c, std::ostream& os) { os << "Custom: " << c.answer; });

  std::stringstream ss;
  vany_stream(vv1, ss);
  vany_stream(vv2, ss);
  vany_stream(vany_value{true}, ss);
  vany_stream(vv3, ss);
  CHECK(ss.str() == "String: hello, Int: 42, Bool: true, Custom: Hello world!");
}
