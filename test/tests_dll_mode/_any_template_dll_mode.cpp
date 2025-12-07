#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <map>  // NOLINT
#include <string>

#include "test/component_base/templated_anys.hpp"

#ifdef _MSC_VER
#pragma warning(disable : 5046)
#endif

static_assert(anyxx::is_in_dll_mode);

namespace {

template <typename KEY, typename VALUE>
void test_any_map_template(
    test::component_base::any_map<anyxx::const_observer, KEY, VALUE> map_i) {
  REQUIRE(map_i.size() == 2);
  REQUIRE(map_i.at("one") == 1);
  REQUIRE(map_i.at("two") == 2);
}

struct X {
  std::string s_;
  [[nodiscard]] std::string to_string() const { return s_; }
};

namespace template_test {

ANY(any_to_tstring, (ANY_METHOD(std::string, to_string, (), const)))

struct any_map_to_tstring_v_table_is_inline {};
ANY_TEMPLATE(((KEY)), any_map_to_tstring,
             (ANY_METHOD(any_to_tstring<anyxx::const_observer>, at, (KEY),
                         const)))

template <>
struct any_to_tstring_concept_map<int>
    : any_to_tstring_default_concept_map<int> {
  auto to_string(int const& x) -> std::string { return std::to_string(x); };
};
template <>
struct any_to_tstring_concept_map<double>
    : any_to_tstring_default_concept_map<double> {
  auto to_string(double const& x) -> std::string { return std::to_string(x); };
};
template <>
struct any_to_tstring_concept_map<const double>
    : any_to_tstring_concept_map<double> {};

}  // namespace template_test

using namespace test::component_base;
using namespace anyxx;

}  // namespace

ANY_META_CLASS_STATIC(std::map<int, double>)
ANY_META_CLASS_STATIC(std::map<std::string, int>)
ANY_META_CLASS_STATIC(
    std::map<int, std::map<std::string, std::map<int, double>>>)
ANY_META_CLASS_STATIC(std::map<std::string, std::map<int, double>>)

template <>
struct test::component_base::any_map_concept_map<std::map<std::string, int>,
                                                 std::string, int>
    : test::component_base::any_map_default_concept_map<
          std::map<std::string, int>, std::string, int> {
  int const& at(std::map<std::string, int> const& x, std::string i) {
    return x.at(i);
  };
};

#define __ANY_TEMPLATE_MODEL(class_, t, all, interface_namespace_, interface_)                                 \
  template <> \
interface_##_v_table _detail_ANYXX_V_TABLE_TEMPLATE_FORMAL_ARGS(t) *       \
interface_namespace_::_detail_ANYXX_MAKE_V_TABLE_FUNCTION_NAME(  \
              interface_) <_detail_ANYXX_TEMPLATE_ARGS(all)> (){ \
  static interface_##_v_table _detail_ANYXX_V_TABLE_TEMPLATE_FORMAL_ARGS(t) v_table{ \
      std::in_place_type< _detail_REMOVE_PARENS(class_)> }; \
  return &v_table; \
} \

#define ANY_TEMPLATE_MODEL(class_, t, ins, i)                                 \
  __ANY_TEMPLATE_MODEL(class_, t, _add_head(class_, t), ins, i)       \

ANY_TEMPLATE_MODEL((std::map<std::string, int>),((std::string),(int)), test::component_base, any_map)
ANY_TEMPLATE_MODEL((std::map<std::string, int>),((std::string),(int)), test::component_base, any_mutable_map)
ANY_TEMPLATE_MODEL((std::map<int, double>),((int),(double)), test::component_base, any_map)
ANY_TEMPLATE_MODEL((std::map<int, double>),((int),(double)), test::component_base, any_mutable_map)
using KEY1 = test::component_base::any_recursive_map<
             anyxx::const_observer, std::string,
             test::component_base::any_mutable_map<anyxx::const_observer, int,
                                                   double>>;
ANY_TEMPLATE_MODEL((std::map<int, std::map<std::string, std::map<int, double>>>),((int),(KEY1)), test::component_base, any_recursive_map)


static auto __ =
    anyxx::bind_v_table_to_meta_data<any_mutable_map_v_table<std::string, int>,
                                     std::map<std::string, int>>();

static auto __ = anyxx::bind_v_table_to_meta_data<any_map_v_table<int, double>,
                                                  std::map<int, double>>();
static auto __ =
    anyxx::bind_v_table_to_meta_data<any_mutable_map_v_table<int, double>,
                                     std::map<int, double>>();

//template <>
//test::component_base::any_recursive_map_v_table<
//    int, test::component_base::any_recursive_map<
//             anyxx::const_observer, std::string,
//             test::component_base::any_mutable_map<anyxx::const_observer, int,
//                                                   double>>>*
//test::component_base::make_any_recursive_map_v_table<
//    std::map<int, std::map<std::string, std::map<int, double>>>, int,
//    test::component_base::any_recursive_map<
//        anyxx::const_observer, std::string,
//        test::component_base::any_mutable_map<anyxx::const_observer, int,
//                                              double>>>() {
//  static test::component_base::any_recursive_map_v_table<
//      int, test::component_base::any_recursive_map<
//               anyxx::const_observer, std::string,
//               test::component_base::any_mutable_map<anyxx::const_observer, int,
//                                                     double>>>
//      v_table{std::in_place_type<
//          std::map<int, std::map<std::string, std::map<int, double>>>>};
//  return &v_table;
//}

template <>
test::component_base::any_recursive_map_v_table<
    int, test::component_base::any_recursive_map<
             anyxx::const_observer, std::string,
             test::component_base::any_map<anyxx::const_observer, int,
                                                   double>>>*
test::component_base::make_any_recursive_map_v_table<
    std::map<int, std::map<std::string, std::map<int, double>>>, int,
    test::component_base::any_recursive_map<
        anyxx::const_observer, std::string,
        test::component_base::any_map<anyxx::const_observer, int,
                                              double>>>() {
  static test::component_base::any_recursive_map_v_table<
      int, test::component_base::any_recursive_map<
               anyxx::const_observer, std::string,
               test::component_base::any_map<anyxx::const_observer,
                                                     int, double>>>
      v_table{std::in_place_type<
          std::map<int, std::map<std::string, std::map<int, double>>>>};
  return &v_table;
}

template <>
test::component_base::any_mutable_recursive_map_v_table<
    int, test::component_base::any_mutable_recursive_map<
             anyxx::mutable_observer, std::string,
             test::component_base::any_mutable_map<anyxx::mutable_observer, int,
                                                   double>>>*
test::component_base::make_any_mutable_recursive_map_v_table<
    std::map<int, std::map<std::string, std::map<int, double>>>, int,
    test::component_base::any_mutable_recursive_map<
        anyxx::mutable_observer, std::string,
        test::component_base::any_mutable_map<anyxx::mutable_observer, int,
                                              double>>>() {
  static test::component_base::any_mutable_recursive_map_v_table<
      int, test::component_base::any_mutable_recursive_map<
               anyxx::mutable_observer, std::string,
               test::component_base::any_mutable_map<anyxx::mutable_observer,
                                                     int, double>>>
      v_table{std::in_place_type<
          std::map<int, std::map<std::string, std::map<int, double>>>>};
  return &v_table;
}

// static auto __ = anyxx::bind_v_table_to_meta_data<any_map_v_table<int,
// double>,
//                                                   std::map<int, double>>();

template <>
test::component_base::any_recursive_map_v_table<
    std::string,
    test::component_base::any_map<anyxx::const_observer, int, double>>*
test::component_base::make_any_recursive_map_v_table<
    std::map<std::string, std::map<int, double>>, std::string,
    test::component_base::any_map<anyxx::const_observer, int,
                                          double>>() {
  static test::component_base::any_recursive_map_v_table<
      std::string,
      test::component_base::any_map<anyxx::const_observer, int, double>>
      v_table{std::in_place_type<std::map<std::string, std::map<int, double>>>};
  return &v_table;
}

template <>
test::component_base::any_recursive_map_v_table<
    std::string,
    test::component_base::any_mutable_map<anyxx::const_observer, int, double>>*
test::component_base::make_any_recursive_map_v_table<
    std::map<std::string, std::map<int, double>>, std::string,
    test::component_base::any_mutable_map<anyxx::const_observer, int,
                                          double>>() {
  static test::component_base::any_recursive_map_v_table<
      std::string,
      test::component_base::any_mutable_map<anyxx::const_observer, int, double>>
      v_table{std::in_place_type<std::map<std::string, std::map<int, double>>>};
  return &v_table;
}

template <>
test::component_base::any_mutable_recursive_map_v_table<
    std::string, test::component_base::any_mutable_map<anyxx::mutable_observer,
                                                       int, double>>*
test::component_base::make_any_mutable_recursive_map_v_table<
    std::map<std::string, std::map<int, double>>, std::string,
    test::component_base::any_mutable_map<anyxx::mutable_observer, int,
                                          double>>() {
  static test::component_base::any_mutable_recursive_map_v_table<
      std::string, test::component_base::any_mutable_map<
                       anyxx::mutable_observer, int, double>>
      v_table{std::in_place_type<std::map<std::string, std::map<int, double>>>};
  return &v_table;
}

//[](any_mutable_recursive_map<
//    mutable_observer, int,
//    any_mutable_recursive_map<
//        mutable_observer, std::string,
//        any_mutable_map<mutable_observer, int, double>>>

TEST_CASE("any template test") {
  using namespace template_test;
  using namespace anyxx;
  using namespace test::component_base;

  std::map<std::string, int> map_string_to_int = {{"one", 1}, {"two", 2}};

  auto test_any_map_lambda =
      [](any_map<const_observer, std::string, int> map_i) {
        REQUIRE(map_i.size() == 2);
        REQUIRE(map_i.at("one") == 1);
        REQUIRE(map_i.at("two") == 2);
      };
  test_any_map_lambda(map_string_to_int);

  test_any_map_template<std::string, int>(map_string_to_int);

  // auto test_any_map_to_tstring_lambda =
  //     [](any_map_to_tstring<const_observer, std::string> map_i) {
  //       REQUIRE(map_i.at("one").to_string() == "1");
  //       REQUIRE(map_i.at("two").to_string() == "2");
  //     };
  // test_any_map_to_tstring_lambda(map_string_to_int);
}

TEST_CASE("any template test2") {
  using namespace template_test;
  using namespace anyxx;
  using namespace test::component_base;

  std::map<std::string, double> map_string_to_int = {{"one", 1}, {"two", 2}};

  // auto test_any_map_to_tstring_lambda =
  //     [](any_map_to_tstring<const_observer, std::string> map_i) {
  //       REQUIRE(map_i.at("one").to_string() == "1.000000");
  //       REQUIRE(map_i.at("two").to_string() == "2.000000");
  //     };
  // test_any_map_to_tstring_lambda(map_string_to_int);
}

TEST_CASE("any template test3") {
  using namespace template_test;
  using namespace anyxx;
  using namespace test::component_base;

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

TEST_CASE("any template test4") {
  using namespace template_test;
  using namespace anyxx;
  using namespace test::component_base;

  std::map<int, double> map = {{3, 3.333}, {6, 4.333}};

  any_map<const_observer, int, double> any_mutable_map{map};
  REQUIRE(any_mutable_map.at(3) == 3.333);
}

