#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// cppcheck-suppress-macro performance-unnecessary-value-param
#define CONCEPT_META_FUNCTION(                                                 \
    any_template_params, model_map_template_params, concrete_template_params,  \
    v_table_template_params, static_dispatch_template_params,                  \
    v_model_map_template_params, n, BASE, base_template_params, l, decoration) \
                                                                               \
  _detail_ANYXX_OPTIONAL_TYPENAME_PARAM_LIST(any_template_params) struct n;    \
                                                                               \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(model_map_template_params)>      \
  struct n##_default_model_map {                                               \
    _detail_ANYXX_MAP_FUNCTIONS(l)                                             \
  };                                                                           \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(model_map_template_params)>      \
  struct n##_model_map : n##_default_model_map<_detail_ANYXX_TEMPLATE_ARGS(    \
                             model_map_template_params)> {};                   \
                                                                               \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(model_map_template_params)>      \
    requires(anyxx::is_variant<T>)                                             \
  struct n##                                                                   \
      _model_map<_detail_ANYXX_TEMPLATE_ARGS(model_map_template_params)> {     \
    template <typename V>                                                      \
    using x_model_map = n##_model_map<_detail_ANYXX_TEMPLATE_ARGS(             \
        v_model_map_template_params)>;                                         \
    _detail_ANYXX_MAP_VARIANT_FUNCTIONS(l)                                     \
  };                                                                           \
                                                                               \
  _detail_ANYXX_OPTIONAL_TYPENAME_PARAM_LIST(                                  \
      v_table_template_params) struct n##_v_table                              \
      : BASE##_v_table<_detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(                   \
            base_template_params)> {                                           \
    using v_table_base_t =                                                     \
        BASE##_v_table<_detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(                   \
            base_template_params)>;                                            \
    using v_table_t = n##_v_table;                                             \
                                                                               \
    using any_value_t = anyxx::any<anyxx::value, n>;                           \
    using any_const_observer_t = anyxx::any<const_observer, n>;                \
    using any_mutable_observer_t = anyxx::any<mutable_observer, n>;            \
                                                                               \
    _detail_ANYXX_V_TABLE_FUNCTION_PTRS(l);                                    \
                                                                               \
    n##_v_table() = default;                                                   \
                                                                               \
    template <typename Concrete>                                               \
    explicit(false) n##_v_table(std::in_place_type_t<Concrete> concrete);      \
  };                                                                           \
                                                                               \
  _detail_ANYXX_OPTIONAL_TYPENAME_PARAM_LIST(any_template_params) struct n     \
      : BASE<_detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(base_template_params)> {     \
    using any_value_t = anyxx::any<anyxx::value, n>;                           \
    using any_const_observer_t = anyxx::any<const_observer, n>;                \
    using any_mutable_observer_t = anyxx::any<mutable_observer, n>;            \
                                                                               \
    using base_t =                                                             \
        BASE<_detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(base_template_params)>;      \
                                                                               \
    using v_table_base_t = base_t::v_table_t;                                  \
    using v_table_t =                                                          \
        n##_v_table<_detail_ANYXX_TEMPLATE_ARGS(any_template_params)>;         \
    template <typename StaticDispatchType>                                     \
    using static_dispatch_map_t = n##_model_map<_detail_ANYXX_TEMPLATE_ARGS(   \
        static_dispatch_template_params)>;                                     \
                                                                               \
    _detail_ANYXX_METHODS(l)                                                   \
                                                                               \
        _detail_REMOVE_PARENS(decoration)                                      \
  };                                                                           \
                                                                               \
  _detail_ANYXX_OPTIONAL_TYPENAME_PARAM_LIST(                                  \
      v_table_template_params) template <typename Concrete>                    \
  n##_v_table<_detail_ANYXX_TEMPLATE_ARGS(v_table_template_params)>::          \
      n##_v_table(std::in_place_type_t<Concrete> concrete)                     \
      : v_table_base_t(concrete) {                                             \
    using concept_map =                                                        \
        n##_model_map<_detail_ANYXX_TEMPLATE_ARGS(concrete_template_params)>;  \
                                                                               \
    _detail_ANYXX_V_TABLE_LAMBDAS(l);                                          \
  };

#define __detail_ANYXX_TRAIT_(t, n, BASE, l, decoration)                   \
  CONCEPT_META_FUNCTION(, (T), (Concrete), , (StaticDispatchType), (V), n, \
                        BASE, , l, decoration)

#define TRAIT_EX_(n, BASE, l, decoration) \
  __detail_ANYXX_TRAIT_(, n, BASE, l, decoration)

#define TRAIT_(n, BASE, l) TRAIT_EX_(n, BASE, l, ())

#define TRAIT(n, fns) TRAIT_(n, anyxx::emtpty_trait, fns)
#define TRAIT_EX(n, ...) TRAIT_EX_(n, anyxx::emtpty_trait, __VA_ARGS__)

namespace anyxx {
template <typename...>
struct emtpty_trait_v_table {
  emtpty_trait_v_table() = default;
  template <typename Concrete>
  emtpty_trait_v_table(
      [[maybe_unused]] std::in_place_type_t<Concrete> concrete){};
};
template <typename...>
struct emtpty_trait {
  template <typename...>
  using v_table_t = emtpty_trait_v_table<>;
};
}  // namespace anyxx

using namespace anyxx;

namespace {

TRAIT(trait1, (ANY_METHOD(std::string, fn1, (), const)))

}  // namespace

TEST_CASE("any via concept simple") {
  struct model1 {
    std::string fn1() const { return "model1"; }
  };
  any<value, trait1> test{model1{}};
  CHECK(test.fn1() == "Error: fn1()");
}