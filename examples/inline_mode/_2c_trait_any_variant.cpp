#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <format>
#include <string>
#include <strstream>
#include <variant>

namespace example_2c {

struct custom {
  std::string answer;
};

ANY(value, (ANY_METHOD_DEFAULTED(std::string, to_string, (), const,
                                 [x]() { return std::format("{}", x); }),
            ANY_METHOD_DEFAULTED(void, from_string, (std::string_view), ,
                                 [&x](std::string_view sv) {
                                   std::strstream ss{std::string(sv),
                                                     std::ios_base::in};
                                   ss >> x;
                                 })))

}  // namespace example_2c

ANY_MODEL_MAP((example_2c::custom), example_2c::value) {
  static std::string to_string(const custom& x) {
    return "{ " + x.answer + "}";
  };
  static void from_string(custom & x, std::string_view sv) {
    x.answer = sv.substr(1, sv.length() - 2);
  };
};

namespace example_2c {

using namespace anyxx;

template <template <typename...> typename any, is_erased_data ErasedData,
          typename Dispatch, typename... Types>
using vany_variant =
    std::variant<any<ErasedData, Dispatch>, any<traited<Types>, trait>...>;

template <template <typename...> typename any, is_erased_data ErasedData,
          typename Dispatch, typename... Types>
using vany =
    any<traited<vany_variant<any, ErasedData, Dispatch, Types...>>, trait>;

using value_vany_variant =
    vany_variant<value, shared_const, rtti, bool, int, double, std::string>;

using vany_value =
    vany<value, shared_const, rtti, bool, int, double, std::string>;

using vany_values_t = std::vector<vany_value>;

}  // namespace example_2c

TEST_CASE("example 2c trait any variant") {
  using namespace anyxx;
  using namespace example_2c;
  using namespace std::string_literals;
  auto custom_value =
      example_2c::value<shared_const>{std::in_place_type<custom>, "42"};
  value_vany_variant vvv = custom_value;
  vany_value vv1 = {vvv};
  vany_value vv2 = value_vany_variant{
      example_2c::value<shared_const>{std::in_place_type<custom>, "43"}};
  //  vany_values_t vany_values = {value<shared:const>{std::in_place, "42"}}
}
