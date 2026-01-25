#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <map>  // NOLINT
#include <string>

using namespace anyxx;

struct X {
  std::string s_;
  [[nodiscard]] std::string to_string() const { return s_; }
};

namespace {

TRAIT(stringable, (ANY_FN(std::string, to_string, (), const)))
template <typename Box = value>
using any_to_string = any<Box, stringable>;

TRAIT_TEMPLATE(((KEY), (VALUE)), map,
               (ANY_FN_EXACT(VALUE const&, at, (KEY const&), const),
                ANY_FN(std::size_t, size, (), const)))
template <typename Key, typename Value, typename Box = value>
using any_map = any<Box, map<Key, Value>>;

TRAIT_TEMPLATE_(((KEY), (VALUE)), mutable_map, map, ((KEY), (VALUE)),
               (ANY_FN_EXACT(VALUE&, at, (KEY const&), ), ))
template <typename Key, typename Value, typename Box = value>
using any_mutable_map = any<Box, mutable_map<Key, Value>>;

ANY_TEMPLATE(((KEY), (VALUE)), recursive_map,
             (ANY_FN(VALUE, at, (KEY const&), const),
              ANY_FN(std::size_t, size, (), const)), )

ANY_TEMPLATE(((KEY), (VALUE)), mutable_recursive_map,
             (ANY_FN(VALUE, at, (KEY const&), )), )

ANY_TEMPLATE(((KEY)), map_to_string,
             (ANY_FN((any_to_string<const_observer>), at, (KEY const&),
                         const)), )

ANY_MODEL_MAP((int), stringable) {
  auto to_string(int const& x) -> std::string { return std::to_string(x); };
};
ANY_MODEL_MAP((double), stringable) {
  auto to_string(double const& x) -> std::string { return std::to_string(x); };
};
}  // namespace

template <typename KEY, typename VALUE>
void test_any_map_template(any_map<KEY, VALUE, const_observer> const& map_i) {
  REQUIRE(map_i.size() == 2);
  REQUIRE(map_i.at("one") == 1);
  REQUIRE(map_i.at("two") == 2);
}

TEST_CASE("any template test") {
  std::map<std::string, int> map_string_to_int = {{"one", 1}, {"two", 2}};

  auto test_any_map_lambda =
      [](any_map<std::string, int, const_observer> map_i) {
        REQUIRE(map_i.size() == 2);
        REQUIRE(map_i.at("one") == 1);
        REQUIRE(map_i.at("two") == 2);
      };
  test_any_map_lambda(map_string_to_int);

  test_any_map_template<std::string, int>(map_string_to_int);

  auto test_any_map_to_string_lambda =
      [](any_map_to_string<std::string, const_observer> map_i) {
        REQUIRE(map_i.at("one").to_string() == "1");
        REQUIRE(map_i.at("two").to_string() == "2");
      };
  test_any_map_to_string_lambda(map_string_to_int);
}

TEST_CASE("any template test2") {
  std::map<std::string, double> map_string_to_int = {{"one", 1}, {"two", 2}};

  auto test_any_map_to_string_lambda =
      [](any_map_to_string<std::string, const_observer> map_i) {
        REQUIRE(map_i.at("one").to_string() == "1.000000");
        REQUIRE(map_i.at("two").to_string() == "2.000000");
      };
  test_any_map_to_string_lambda(map_string_to_int);
}

TEST_CASE("any template test3") {
  std::map<int, std::map<std::string, std::map<int, double>>> map = {
      {1, {{"one", {{1, 3.14}, {2, 6.28}}}, {"two", {{3, 3.333}}}}},
      {2, {{"one", {{4, 4.14}, {5, 4.28}}}, {"two", {{6, 4.333}}}}}};

  auto test_map_lambda =
      [](any_recursive_map<
          int,
          any_recursive_map<std::string, any_map<int, double, const_observer>,
                            const_observer>,
          const_observer>
             map_i) {
        auto x = map_i.at(1);
        auto y = x.at("one");
        auto z = y.at(1);
        REQUIRE(z == 3.14);
        REQUIRE(map_i.at(1).at("one").at(1) == 3.14);
        REQUIRE(map_i.at(2).at("one").at(4) == 4.14);
      };
  test_map_lambda(map);

  auto test_map_lambda_mutate =
      [](any_mutable_recursive_map<
          int,
          any_mutable_recursive_map<
              std::string, any_mutable_map<int, double, mutable_observer>,
              mutable_observer>,
          mutable_observer>
             map_i) {
        auto x = map_i.at(1);
        auto y = x.at("one");
        auto z = y.at(1);
        REQUIRE(z == 3.14);
        {
          auto xx = map_i.at(1).at("one");
        };
        REQUIRE(map_i.at(1).at("one").at(1) == 3.14);
        REQUIRE(map_i.at(2).at("one").at(4) == 4.14);
        map_i.at(2).at("one").at(4) = 8.28;
      };
  test_map_lambda_mutate(map);
  REQUIRE(map[2]["one"][4] == 8.28);
}

namespace {
ANY_TEMPLATE_MODEL_MAP((std::map<int, double>), map, ((int), (double))) {
  double const& at(std::map<int, double> const& x, int i) { return x.at(i); };
};
}  // namespace

TEST_CASE("any template test4") {
  std::map<int, double> map = {{3, 3.333}, {6, 4.333}};

  any_map<int, double, const_observer> any_mutable_map{map};
  REQUIRE(any_mutable_map.at(3) == 3.333);
}