#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <map>  // NOLINT
#include <string>

using namespace anyxx;
using namespace anyxx;

struct X {
  std::string s_;
  [[nodiscard]] std::string to_string() const { return s_; }
};

namespace {
ANY(any_to_tstring, (ANY_METHOD(std::string, to_string, (), const)))

ANY_TEMPLATE(((KEY), (VALUE)), any_map,
             (ANY_METHOD(VALUE const&, at, (KEY), const),
              ANY_METHOD(std::size_t, size, (), const)))

ANY_TEMPLATE_(((KEY), (VALUE)), any_mutable_map, any_map, ((KEY), (VALUE)),
              (ANY_METHOD_OVERLOAD(VALUE&, at, (KEY), ),
               ANY_OP(VALUE&, [], (KEY), )))

ANY_TEMPLATE(((KEY), (VALUE)), any_recursive_map,
             (ANY_METHOD(VALUE, at, (KEY), const),
              ANY_METHOD(std::size_t, size, (), const)))

ANY_TEMPLATE(((KEY), (VALUE)), any_mutable_recursive_map,
             (ANY_METHOD(VALUE, at, (KEY), ),
              ANY_METHOD(std::size_t, size, (), const)))

ANY_TEMPLATE(((KEY)), any_map_to_tstring,
             (ANY_METHOD(any_to_tstring<const_observer>, at, (KEY), const)))

ANY_MODEL_MAP((int), any_to_tstring) {
  auto to_string(int const& x) -> std::string { return std::to_string(x); };
};
ANY_MODEL_MAP((double), any_to_tstring) {
  auto to_string(double const& x) -> std::string { return std::to_string(x); };
};
}  // namespace

template <typename KEY, typename VALUE>
void test_any_map_template(any_map<const_observer, KEY, VALUE> map_i) {
  REQUIRE(map_i.size() == 2);
  REQUIRE(map_i.at("one") == 1);
  REQUIRE(map_i.at("two") == 2);
}

TEST_CASE("any template test") {
  std::map<std::string, int> map_string_to_int = {{"one", 1}, {"two", 2}};

  auto test_any_map_lambda =
      [](any_map<const_observer, std::string, int> map_i) {
        REQUIRE(map_i.size() == 2);
        REQUIRE(map_i.at("one") == 1);
        REQUIRE(map_i.at("two") == 2);
      };
  test_any_map_lambda(map_string_to_int);

  test_any_map_template<std::string, int>(map_string_to_int);

  auto test_any_map_to_tstring_lambda =
      [](any_map_to_tstring<const_observer, std::string> map_i) {
        REQUIRE(map_i.at("one").to_string() == "1");
        REQUIRE(map_i.at("two").to_string() == "2");
      };
  test_any_map_to_tstring_lambda(map_string_to_int);
}

TEST_CASE("any template test2") {
  std::map<std::string, double> map_string_to_int = {{"one", 1}, {"two", 2}};

  auto test_any_map_to_tstring_lambda =
      [](any_map_to_tstring<const_observer, std::string> map_i) {
        REQUIRE(map_i.at("one").to_string() == "1.000000");
        REQUIRE(map_i.at("two").to_string() == "2.000000");
      };
  test_any_map_to_tstring_lambda(map_string_to_int);
}

TEST_CASE("any template test3") {
  std::map<int, std::map<std::string, std::map<int, double>>> map = {
      {1, {{"one", {{1, 3.14}, {2, 6.28}}}, {"two", {{3, 3.333}}}}},
      {2, {{"one", {{4, 4.14}, {5, 4.28}}}, {"two", {{6, 4.333}}}}}};

  auto test_map_lambda =
      [](any_recursive_map<
          const_observer, int,
          any_recursive_map<const_observer, std::string,
                            any_map<const_observer, int, double>>>
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
          mutable_observer, int,
          any_mutable_recursive_map<
              mutable_observer, std::string,
              any_mutable_map<mutable_observer, int, double>>>
             map_i) {
        auto x = map_i.at(1);
        auto y = x.at("one");
        auto z = y.at(1);
        REQUIRE(z == 3.14);
        REQUIRE(map_i.at(1).at("one").at(1) == 3.14);
        REQUIRE(map_i.at(2).at("one").at(4) == 4.14);
        map_i.at(2).at("one").at(4) = 8.28;
      };
  test_map_lambda_mutate(map);
  REQUIRE(map[2]["one"][4] == 8.28);
}

#define __ANY_TEMPLATE_MODEL__(class_, t, all, interface_namespace_,          \
                               interface_)                                    \
  template <>                                                                 \
      interface_##_v_table _detail_ANYXX_V_TABLE_TEMPLATE_FORMAL_ARGS(t) *    \
      interface_namespace_::_detail_ANYXX_MAKE_V_TABLE_FUNCTION_NAME(         \
          interface_)<_detail_ANYXX_TEMPLATE_ARGS(all)>() {                   \
    static interface_##_v_table _detail_ANYXX_V_TABLE_TEMPLATE_FORMAL_ARGS(t) \
        v_table{std::in_place_type<_detail_REMOVE_PARENS(class_)>};           \
    return &v_table;                                                          \
  }

namespace {
ANY_TEMPLATE_MODEL_MAP((std::map<int, double>), any_map, ((int), (double))) {
  double const& at(std::map<int, double> const& x, int i) { return x.at(i); };
};
}  // namespace

TEST_CASE("any template test4") {
  std::map<int, double> map = {{3, 3.333}, {6, 4.333}};

  any_map<const_observer, int, double> any_mutable_map{map};
  REQUIRE(any_mutable_map.at(3) == 3.333);
}