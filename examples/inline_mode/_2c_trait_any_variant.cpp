#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <format>
#include <sstream>
#include <string>
#include <variant>

namespace example_2c {

struct custom {
  std::string answer;
};

ANY(value, (ANY_METHOD_DEFAULTED(std::string, to_string, (), const,
                                 [x]() { return std::format("{}", x); }),
            ANY_METHOD_DEFAULTED(void, from_string, (std::string_view), ,
                                 [&x](std::string_view sv) {
                                   std::stringstream ss{std::string{sv},
                                                        std::ios_base::in};
                                   ss >> x;
                                 })))

using vany_value = anyxx::vany_type<value, anyxx::shared_const, anyxx::rtti,
                                    bool, int, double, std::string>;

using vany_values_t = std::vector<vany_value>;

}  // namespace example_2c

ANY_MODEL_MAP((example_2c::custom), example_2c::value) {
  static std::string to_string(const custom& x) {
    return "{" + x.answer + "}";
  };
  static void from_string(custom & x, std::string_view sv) {
    x.answer = sv.substr(1, sv.length() - 2);
  };
};

namespace example_2c {}  // namespace example_2c

TEST_CASE("example 2c trait any variant") {
  using namespace example_2c;
  using namespace std::string_literals;
  vany_value vv3{std::string{"hello"}};
  vany_value vv4{std::in_place_type<example_2c::value<anyxx::shared_const>>,
                 std::in_place_type<custom>, "42"};
  static_assert(
      anyxx::constructibile_for<example_2c::value<anyxx::shared_const>,
                                vany_value::erased_data_t>);
  auto v6 =
      example_2c::value<anyxx::shared_const>(std::in_place_type<custom>, "43");
  vany_value vv6{v6};
  vany_value vv5{
      example_2c::value<anyxx::shared_const>{std::in_place_type<custom>, "42"}};
  auto custom_value =
      example_2c::value<anyxx::shared_const>{std::in_place_type<custom>, "42"};
  vany_value vv1{custom_value};
  vany_value vv_custom_43 =
      example_2c::value<anyxx::shared_const>{std::in_place_type<custom>, "43"};
  vany_values_t vany_values = {
      example_2c::value<anyxx::shared_const>{std::in_place, custom{"42"}}};

  CHECK(vany_value{true}.to_string() == "true");
  CHECK(vv_custom_43.to_string() == "{43}");
}
