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

TRAIT(stringable, anyxx::dynamic_copyable,
      (FN(std::string, to_string, (), const)))
template <typename Proxy = val<>>
using any_to_string = any<Proxy, stringable>;

TRAIT_TEMPLATE(((KEY), (VALUE)), map, anyxx::dynamic_copyable, (),
               (FN_EXACT(VALUE const&, at, (KEY const&), const),
                FN(std::size_t, size, (), const)))
template <typename Key, typename Value, typename Proxy = val<>>
using any_map = any<Proxy, map, Key, Value>;

TRAIT_TEMPLATE(((KEY), (VALUE)), mutable_map, map, ((KEY), (VALUE)),
               (FN_EXACT(VALUE&, at, (KEY const&), ), ))
template <typename Key, typename Value, typename Proxy = val<>>
using any_mutable_map = any<Proxy, mutable_map, Key, Value>;

TRAIT_TEMPLATE(((KEY), (VALUE)), recursive_map, anyxx::dynamic_copyable, (),
               (FN(VALUE, at, (KEY const&), const),
                FN(std::size_t, size, (), const)))

TRAIT_TEMPLATE(((KEY), (VALUE)), mutable_recursive_map, anyxx::dynamic_copyable,
               (), (FN(VALUE, at, (KEY const&), )))

TRAIT_TEMPLATE(((KEY)), map_to_string, anyxx::dynamic_copyable, (),
               (FN((any_to_string<cref>), at, (KEY const&), const)))

ANY_MODEL_MAP((int), stringable) {
  auto to_string(int const& x) -> std::string { return std::to_string(x); };
};
ANY_MODEL_MAP((double), stringable) {
  auto to_string(double const& x) -> std::string { return std::to_string(x); };
};
}  // namespace

template <typename KEY, typename VALUE>
void any_map_template_f(any_map<KEY, VALUE, cref> const& map_i) {
  REQUIRE(map_i.size() == 2);
  REQUIRE(map_i.at("one") == 1);
  REQUIRE(map_i.at("two") == 2);
}

TEST_CASE("any template usage simple") {
  using namespace anyxx;
  std::map<std::string, int> map_string_to_int = {{"one", 1}, {"two", 2}};

  auto any_map_lambda = [](any<cref, map, std::string, int> map_i) {
    REQUIRE(map_i.size() == 2);
    REQUIRE(map_i.at("one") == 1);
    REQUIRE(map_i.at("two") == 2);
  };
  any_map_lambda(map_string_to_int);

  any_map_template_f<std::string, int>(map_string_to_int);

  auto any_map_to_string_lambda =
      [](any<cref, map_to_string, std::string> map_i) {
        REQUIRE(map_i.at("one").to_string() == "1");
        REQUIRE(map_i.at("two").to_string() == "2");
      };
  any_map_to_string_lambda(map_string_to_int);
}

TEST_CASE("nested(recursive) any template") {
  std::map<int, std::map<std::string, std::map<int, double>>> m = {
      {1, {{"one", {{1, 3.14}, {2, 6.28}}}, {"two", {{3, 3.333}}}}},
      {2, {{"one", {{4, 4.14}, {5, 4.28}}}, {"two", {{6, 4.333}}}}}};

  auto lambda =
      [](any<cref, recursive_map, int,
             any<cref, recursive_map, std::string, any<cref, map, int, double>>>
             map_i) {
        auto x = map_i.at(1);
        auto y = x.at("one");
        auto z = y.at(1);
        REQUIRE(z == 3.14);
        REQUIRE(map_i.at(1).at("one").at(1) == 3.14);
        REQUIRE(map_i.at(2).at("one").at(4) == 4.14);
      };
  lambda(m);

  auto lambda_mutate = [](any<mutref, mutable_recursive_map, int,
                              any<mutref, mutable_recursive_map, std::string,
                                  any<mutref, mutable_map, int, double>>>
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
  lambda_mutate(m);
  REQUIRE(m[2]["one"][4] == 8.28);
}

namespace {
ANY_TEMPLATE_MODEL_MAP((std::map<int, double>), map, ((int), (double))) {
  double const& at(std::map<int, double> const& x, int i) { return x.at(i); };
};
}  // namespace

TEST_CASE("any template with model map") {
  std::map<int, double> map = {{3, 3.333}, {6, 4.333}};

  any_map<int, double, cref> any_mutable_map{map};
  REQUIRE(any_mutable_map.at(3) == 3.333);
}