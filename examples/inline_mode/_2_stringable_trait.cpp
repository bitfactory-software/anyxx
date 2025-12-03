#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <format>
#include <string>

namespace anyxx {
template <typename Value>
struct trait_base {
  Value value_;
  operator Value&() { return value_; }
};

template <typename T>
struct missing_trait_error {
  static constexpr bool not_specialized = false;
};
}  // namespace anyxx

#define TRAIT_ERROR_MESSAGE(name)                               \
  static_assert(anyxx::missing_trait_error<T>::not_specialized, \
                "'" #name                                       \
                "' is missing in the specialization of this trait!");

#define _detail_ANYXX_TRAIT_FUNCTION_H(l) _detail_ANYXX_TRAIT_FUNCTION l
#define _detail_ANYXX_TRAIT_FUNCTION(overload, type, name, name_ext,       \
                                     exact_const, const_, trait_body, ...) \
  auto name([[maybe_unused]] T const_& x __VA_OPT__(                       \
      , _detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) -> type {              \
    _detail_REMOVE_PARENS(trait_body)                                      \
  };
#define _detail_ANYXX_TRAIT_FUNCTIONS(...)                         \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_TRAIT_FUNCTION_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__))

#define _detail_ANYXX_TRAIT_METHOD_H(l) _detail_ANYXX_TRAIT_METHOD l
#define _detail_ANYXX_TRAIT_METHOD(overload, type, name, name_ext,       \
                                   exact_const, const_, trait_body, ...) \
  overload type name_ext(                                                \
      __VA_OPT__(_detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) const_ {    \
    return trait{}.name(base_t::value_ __VA_OPT__(, ) __VA_OPT__(        \
        _detail_PARAM_LIST(a, _sig, __VA_ARGS__)));                      \
  }
#define _detail_ANYXX_TRAIT_METHODS(...)                         \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_TRAIT_METHOD_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__))

#define TRAIT_META_FUNCTION(tpl1, tpl2, tpl3, n, BASE, btpl, l)             \
                                                                            \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(tpl1)>                        \
  struct n;                                                                 \
                                                                            \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(tpl2)>                        \
  struct n##_trait_default {                                                \
    _detail_ANYXX_TRAIT_FUNCTIONS(l)                                        \
  };                                                                        \
                                                                            \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(tpl2)>                        \
  struct n##_trait : n##_trait_default<_detail_ANYXX_TEMPLATE_ARGS(tpl2)> { \
    static constexpr bool is_defined = false;                               \
  };                                                                        \
                                                                            \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(tpl1)>                        \
  struct n : BASE<_detail_ANYXX_BASE_TEMPLATE_ACTUAL_ARGS(btpl)> {          \
    using value_t = ErasedData;                                             \
    using trait = n##_trait<_detail_ANYXX_TEMPLATE_ARGS(tpl3)>;             \
    using base_t = BASE<_detail_ANYXX_BASE_TEMPLATE_ACTUAL_ARGS(btpl)>;     \
    using base_t::value_;                                                   \
    n(ErasedData v) : base_t(std::move(v)) {}                               \
    ErasedData& operator=(ErasedData v) {                                   \
      base_t::value_ = std::move(v);                                        \
      return *this;                                                         \
    }                                                                       \
    ~n() = default;                                                         \
    n() = default;                                                          \
    n(n const&) = default;                                                  \
    n(n&&) = default;                                                       \
    n& operator=(n const&) = default;                                       \
    n& operator=(n&&) = default;                                            \
                                                                            \
    _detail_ANYXX_TRAIT_METHODS(l)                                          \
  };

#define TRAIT_(n, BASE, l) \
  TRAIT_META_FUNCTION((ErasedData), (T), (ErasedData), n, BASE, (), l)

#define TRAIT(n, ...) TRAIT_(n, ::anyxx::trait_base, __VA_ARGS__)

#define TRAIT_TEMPLATE_(t, n, BASE, btpl, l)                         \
  TRAIT_META_FUNCTION(_add_head((ErasedData), t), _add_head((T), t), \
                      _add_head((ErasedData), t), n, BASE, btpl, l)

#define TRAIT_TEMPLATE(t, n, l) \
  TRAIT_TEMPLATE_(t, n, ::anyxx::trait_base, (), l)

#define TRAIT_METHOD(ret, name, params, const_, ...)           \
  ANY_METHOD_(, ret, name, name, false, const_, (__VA_ARGS__), \
              _detail_EXPAND params)

#define TRAIT_METHOD_PURE(ret, name, params, const_)                         \
  ANY_METHOD_(, ret, name, name, false, const_, (TRAIT_ERROR_MESSAGE(name)), \
              _detail_EXPAND params)

namespace example_2 {

using namespace std;
TRAIT(stringable, (TRAIT_METHOD_PURE(std::string, to_string, (), const)))

template <>
struct stringable_trait<bool> {
  static std::string to_string(bool const& value) {
    return value ? "wahr" : "falsch";
  };
};

template <>
struct stringable_trait<double> {
  static std::string to_string(const double& value) {
    return std::format("{:6.3}", value);
  }
};

template <typename V>
std::string print_(stringable<V> s) {
  return s.to_string() + "\n";
}
template <typename V>
auto print(V const& s)
//  requires stringable_trait<V>::is_defined
{
  return print_(stringable<V>{s});
}

}  // namespace example_2

template <class V>
concept is_print_callable = requires(V v) {
  { print(v) } -> std::same_as<std::string>;
};

TEST_CASE("example 2 ") {
  using namespace example_2;
  CHECK(print(true) == "wahr\n");
  CHECK(print(3.14) == "  3.14\n");
  //  static_assert(!is_print_callable<int>);
  print(42);  // remove comment to see the compilation error!
}

// namespace monoid_example {
// TRAIT(monoid,
//       (ANY_METHOD(V, op, (V const&), const), ANY_METHOD(V, concat,
//       (std::ranges::range<), const),
//        ANY_METHOD(V, identity, (), const)))
//
// struct string_monoid {
//   std::string value;
//   auto op(auto s) {
//     std::puts("string_monoid::op()");
//     return s1 + s2;
//   }
//   template <typename Range>
//   auto concat(this auto&& self, Range r) {
//     std::puts("string_monoid::concat()");
//     return std::ranges::fold_right(
//         r, std::string{}, [&](auto m1, auto m2) { return self.op(m1, m2); });
//   }
// };
//
// }  // namespace monoid_example
