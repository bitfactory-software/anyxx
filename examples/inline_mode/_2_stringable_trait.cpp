#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <format>
#include <iostream>
#include <string>

namespace anyxx {
template <typename Value>
struct trait_base {
  Value value_;
};
}  // namespace anyxx

#define _detail_ANYXX_TRAIT_FUNCTION_H(l) _detail_ANYXX_TRAIT_FUNCTION l
#define _detail_ANYXX_TRAIT_FUNCTION(overload, type, name, name_ext,           \
                                     exact_const, const_, ...)                 \
  auto name(T const_& x __VA_OPT__(                                            \
      , _detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) -> type {                  \
    using namespace std;                                                       \
    return name_ext(x __VA_OPT__(, _detail_PARAM_LIST(a, _sig, __VA_ARGS__))); \
  };
#define _detail_ANYXX_TRAIT_FUNCTIONS(...)                         \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_TRAIT_FUNCTION_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__))

#define _detail_ANYXX_TRAIT_METHOD_H(l) _detail_ANYXX_TRAIT_METHOD l
#define _detail_ANYXX_TRAIT_METHOD(overload, type, name, name_ext,    \
                                   exact_const, const_, ...)          \
  overload type name_ext(                                             \
      __VA_OPT__(_detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) const_ { \
    return trait{}.name(base_t::value_ __VA_OPT__(, ) __VA_OPT__(     \
        _detail_PARAM_LIST(a, _sig, __VA_ARGS__)));                   \
  }
#define _detail_ANYXX_TRAIT_METHODS(...)                         \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_TRAIT_METHOD_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__))

#define TRAIT_META_FUNCTION(tpl1, tpl2, tpl3, n, BASE, btpl, l)               \
                                                                              \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(tpl1)>                          \
  struct n;                                                                   \
                                                                              \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(tpl2)>                          \
  struct n##_trait_default {                                                  \
    _detail_ANYXX_TRAIT_FUNCTIONS(l)                                          \
  };                                                                          \
                                                                              \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(tpl2)>                          \
  struct n##_trait : n##_trait_default<_detail_ANYXX_TEMPLATE_ARGS(tpl2)> {}; \
                                                                              \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(tpl1)>                          \
  struct n : BASE<_detail_ANYXX_BASE_TEMPLATE_ACTUAL_ARGS(btpl)> {            \
    using value_t = ErasedData;                                               \
    using trait = n##_trait<_detail_ANYXX_TEMPLATE_ARGS(tpl3)>;               \
    using base_t = BASE<_detail_ANYXX_BASE_TEMPLATE_ACTUAL_ARGS(btpl)>;       \
    using base_t::value_;                                                     \
    n(ErasedData v) : base_t(std::move(v)) {}                                 \
    ErasedData& operator=(ErasedData v) {                                     \
      base_t::value_ = std::move(v);                                          \
      return *this;                                                           \
    }                                                                         \
    ~n() = default;                                                           \
    n() = default;                                                            \
    n(n const&) = default;                                                    \
    n(n&&) = default;                                                         \
    n& operator=(n const&) = default;                                         \
    n& operator=(n&&) = default;                                              \
                                                                              \
    _detail_ANYXX_TRAIT_METHODS(l)                                            \
  };

#define TRAIT_(n, BASE, l) \
  TRAIT_META_FUNCTION((ErasedData), (T), (ErasedData), n, BASE, (), l)

#define TRAIT(n, ...) TRAIT_(n, ::anyxx::trait_base, __VA_ARGS__)

#define TRAIT_TEMPLATE_(t, n, BASE, btpl, l)                         \
  TRAIT_META_FUNCTION(_add_head((ErasedData), t), _add_head((T), t), \
                      _add_head((ErasedData), t), n, BASE, btpl, l)

#define TRAIT_TEMPLATE(t, n, l) \
  TRAIT_TEMPLATE_(t, n, ::anyxx::trait_base, (), l)

using namespace anyxx;

namespace example_2 {

TRAIT(stringable, (ANY_METHOD(std::string, to_string, (), const)))

template <>
struct stringable_trait<bool> {
  static std::string to_string(bool const& value) {
    return value ? "wahr" : "falsch";
  };
};

template <>
struct stringable_trait<double> {
  static std::string to_string(const double& value) {
    return std::format("{:6.2}", value);
  }
};

template <typename V>
void print_impl(stringable<V> const& s) {
  std::cout << s.to_string() << "\n";
}
template <typename V>
void print(V const& s) {
    return print_impl(stringable<V>{s});
}


}  // namespace example_2

TEST_CASE("example 2 ") {
  using namespace example_2;
  print(true);
  print(3.14);
  print(42);
}
