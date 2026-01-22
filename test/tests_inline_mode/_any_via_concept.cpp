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
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(any_template_params)>            \
  struct n;                                                                    \
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
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(v_table_template_params)>        \
  struct n##_v_table;                                                          \
                                                                               \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(v_table_template_params)>        \
  struct n##_v_table                                                           \
      : BASE##_v_table<_detail_ANYXX_TEMPLATE_ARGS(base_template_params)> {    \
    using v_table_base_t =                                                     \
        BASE##_v_table<_detail_ANYXX_TEMPLATE_ARGS(base_template_params)>;     \
    using v_table_t = n##_v_table;                                             \
                                                                               \
    using any = Any;                                                           \
                                                                               \
    using any_value_t = typename any::any_value_t;                             \
    using any_const_observer_t = typename any::any_const_observer_t;           \
    using any_mutable_observer_t = typename any::any_mutable_observer_t;       \
                                                                               \
    _detail_ANYXX_V_TABLE_FUNCTION_PTRS(l);                                    \
                                                                               \
    n##_v_table() = default;                                                   \
                                                                               \
    template <typename Concrete>                                               \
    explicit(false) n##_v_table(std::in_place_type_t<Concrete> concrete);      \
  };                                                                           \
                                                                               \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(any_template_params)>            \
  struct n : BASE<_detail_ANYXX_TEMPLATE_ARGS(base_template_params)> {         \
    using any = Any;                                                           \
                                                                               \
    using T = typename any::T;                                                 \
    using any_value_t = typename any::any_value_t;                             \
    using any_const_observer_t = typename any::any_const_observer_t;           \
    using any_mutable_observer_t = typename any::any_mutable_observer_t;       \
    using any::get_v_table_ptr;                                                \
    using erased_data_t = typename any::erased_data_t;                         \
                                                                               \
    using base_t = BASE<_detail_ANYXX_TEMPLATE_ARGS(base_template_params)>;    \
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
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(v_table_template_params)>        \
  template <typename Concrete>                                                 \
  n##_v_table<_detail_ANYXX_TEMPLATE_ARGS(v_table_template_params)>::          \
      n##_v_table(std::in_place_type_t<Concrete> concrete)                     \
      : v_table_base_t(concrete) {                                             \
    using concept_map =                                                        \
        n##_model_map<_detail_ANYXX_TEMPLATE_ARGS(concrete_template_params)>;  \
                                                                               \
    _detail_ANYXX_V_TABLE_LAMBDAS(l);                                          \
  };

#define __detail_ANYXX_TRAIT_(t, n, BASE, l, decoration)                  \
  CONCEPT_META_FUNCTION(_detail_REMOVE_PARENS(t), (T), (Concrete), (Any), \
                        (StaticDispatchType), (V), n, BASE, (Any), l,     \
                        decoration)

#define TRAIT_EX_(n, BASE, l, decoration) \
  __detail_ANYXX_TRAIT_(((Any)), n, BASE, l, decoration)

#define TRAIT_(n, BASE, l) TRAIT_EX_(n, BASE, l, ())

#define TRAIT(n, ...) TRAIT_(n, anyxx::emtpty_trait, __VA_ARGS__)
#define TRAIT_EX(n, ...) TRAIT_EX_(n, anyxx::emtpty_trait, __VA_ARGS__)

namespace anyxx {
template <typename Any>
struct emtpty_trait_v_table {
  emtpty_trait_v_table() = default;
  template <typename Concrete>
  emtpty_trait_v_table([[maybe_unused]] std::in_place_type_t<Concrete> concrete){};
};
template <typename Any>
struct emtpty_trait {
  template <typename Any>
  using v_table_t = emtpty_trait_v_table<Any>;
};
}  // namespace anyxx

using namespace anyxx;

namespace {

// TRAIT(trait1, (ANY_METHOD(std::string, fn1, (), const)), )
template <typename Any>
struct trait1;
template <typename T>
struct trait1_default_model_map {
  static auto fn1([[maybe_unused]] T const& x)
      -> anyxx::map_return<T, std::string> {
    return x.fn1();
  };
};
template <typename T>
struct trait1_model_map : trait1_default_model_map<T> {};
template <typename T>
  requires(anyxx::is_variant<T>)
struct trait1_model_map<T> {
  template <typename V>
  using x_model_map = trait1_model_map<V>;
  static auto fn1([[maybe_unused]] T const& x) -> decltype(auto) {
    return std::visit(
        anyxx::overloads{[&]<typename V>(V&& v) {
                           return x_model_map<std::decay_t<V>>::fn1(
                               std::forward<V>(v));
                         },
                         [&]<anyxx::is_any Any>([[maybe_unused]] Any&& any) {
                           return std::forward<Any>(any).fn1();
                         }},
        x);
  };
};
template <typename Any>
struct trait1_v_table;
template <typename Any>
struct trait1_v_table : anyxx::emtpty_trait_v_table<Any> {
  using v_table_base_t = anyxx::emtpty_trait_v_table<Any>;
  using v_table_t = trait1_v_table;
  using any = Any;
  using any_value_t = typename any::any_value_t;
  using any_const_observer_t = typename any::any_const_observer_t;
  using any_mutable_observer_t = typename any::any_mutable_observer_t;
  anyxx::v_table_return<any_value_t, std::string> (*fn1)(void const*);
  ;
  ;
  trait1_v_table() = default;
  template <typename Concrete>
  explicit(false) trait1_v_table(std::in_place_type_t<Concrete> concrete);
};
template <typename Any>
struct trait1 : anyxx::emtpty_trait<Any> {
  using self_t = Any;
  decltype(auto) fn1() const
    requires(::anyxx::const_correct_call_for_erased_data<
             void const*, typename Any::erased_data_t, false>)
  {
    using T = typename self_t::T;
    using static_dispatch_map_t = trait1_model_map<T>;
    auto self = static_cast<const self_t&>(*this);

    if constexpr (!anyxx::voidness<T>) {
      if constexpr (std::same_as<void, std::string>) {
        return static_dispatch_map_t::fn1(self->erased_data_.value_);
      } else {
        return anyxx::jacket_return<std::string>::forward(
            static_dispatch_map_t::fn1(self->erased_data_.value_), *this);
      }
    } else {
      if constexpr (std::same_as<void, std::string>) {
        return anyxx::get_v_table(*this)->fn1(anyxx::get_void_data_ptr(*this));
      } else {
        // return anyxx::jacket_return<std::string>::forward(
        //     anyxx::get_v_table(*this)->fn1(anyxx::get_void_data_ptr(*this)),
        //     *this);
        return std::string{"Error: fn1()"};
      }
    }
  }
};
template <typename Any>
template <typename Concrete>
trait1_v_table<Any>::trait1_v_table(std::in_place_type_t<Concrete> concrete)
    : v_table_base_t(concrete) {
  using concept_map = trait1_model_map<Concrete>;
  fn1 = [](void const* _vp) -> anyxx::v_table_return<any_value_t, std::string> {
    if constexpr (std::same_as<anyxx::self&, std::string>) {
      concept_map{}.fn1(*anyxx::unchecked_unerase_cast<Concrete>(_vp));
      return anyxx::handle_self_ref_return<std::string>{}();
    } else {
      return concept_map{}.fn1(*anyxx::unchecked_unerase_cast<Concrete>(_vp));
    }
  };
  ;
  ;
};

}  // namespace

TEST_CASE("any via concept simple") {
  struct model1 {
    std::string fn1() const { return "model1"; }
  };
  any<value, trait1> test{model1{}};
  CHECK(test.fn1() == "Error: fn1()");
}