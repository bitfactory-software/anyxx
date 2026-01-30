#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace {

namespace simple {

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

struct trait1_has_open_dispatch;

struct trait1_v_table
    : anyxx::base_trait::v_table_t,
      anyxx::dispatch_holder<anyxx::is_type_complete<trait1_has_open_dispatch>,
                             trait1> {
  using v_table_base_t = typename anyxx::base_trait::v_table_t;
  using v_table_t = trait1_v_table;
  using any_value_t = anyxx::any<anyxx::val, trait1>;
  static constexpr bool open_dispatch_enabeled =
      anyxx::is_type_complete<trait1_has_open_dispatch>;
  using own_dispatch_holder_t =
      typename anyxx::dispatch_holder<open_dispatch_enabeled, trait1>;
  static bool static_is_derived_from(const std::type_info& from) {
    return typeid(v_table_t) == from
               ? true
               : v_table_base_t::static_is_derived_from(from);
  }
  anyxx::v_table_return<any_value_t, std::string> (*fn1)(void const*);
  template <typename Concrete>
  explicit(false) trait1_v_table(std::in_place_type_t<Concrete> concrete);
};

struct trait1 : anyxx::base_trait {
  using any_value_t = anyxx::any<anyxx::val, trait1>;
  using base_t = anyxx::base_trait;
  using v_table_base_t = base_t::v_table_t;
  using v_table_t = trait1_v_table;
  template <typename StaticDispatchType>
  using static_dispatch_map_t = trait1_model_map<StaticDispatchType>;
  template <typename Self>
  decltype(auto) fn1(this Self&& self)
    requires(::anyxx::const_correct_call_for_proxy_and_self<
             void const*, typename std::decay_t<Self>::proxy_t,
             std::is_const_v<std::remove_reference_t<Self>>, false>)
  {
    using self_t = std::decay_t<Self>;
    using T = typename self_t::T;
    using proxy_t = typename self_t::proxy_t;
    if constexpr (!self_t::dyn) {
      using traited_t = typename proxy_t::value_t;
      if constexpr (std::same_as<void, std::string>) {
        return static_dispatch_map_t<T>::fn1(get_proxy_value(self));
      } else {
        return anyxx::jacket_return<std::string>::forward(
            static_dispatch_map_t<T>::fn1(get_proxy_value(self)),
            std::forward<Self>(self));
      }
    } else {
      if constexpr (std::same_as<void, std::string>) {
        return get_v_table(self)->fn1(anyxx::get_proxy_ptr(self));
      } else {
        return anyxx::jacket_return<std::string>::forward(
            get_v_table(self)->fn1(anyxx::get_proxy_ptr(self)),
            std::forward<Self>(self));
      }
    }
  }
};

template <typename Concrete>
trait1_v_table::trait1_v_table(std::in_place_type_t<Concrete> concrete)
    : v_table_base_t(concrete) {
  using model_map = trait1_model_map<Concrete>;
  fn1 = [](void const* _vp) -> anyxx::v_table_return<any_value_t, std::string> {
    if constexpr (std::same_as<anyxx::self&, std::string>) {
      model_map{}.fn1(*anyxx::unchecked_unerase_cast<Concrete>(_vp));
      return anyxx::handle_self_ref_return<std::string>{}();
    } else {
      return model_map{}.fn1(*anyxx::unchecked_unerase_cast<Concrete>(_vp));
    }
  };
  if constexpr (open_dispatch_enabeled) {
    own_dispatch_holder_t::set_dispatch_table(
        ::anyxx::dispatch_table_instance<trait1_v_table, Concrete>());
  }
  ::anyxx::set_is_derived_from<v_table_t>(this);
};

std::string cat(std::vector<anyxx::any<anyxx::val, trait1>> const& items) {
  std::string result;
  for (auto const& item : items) {
    result += item.fn1();
  }
  return result;
}

struct model1 {
  std::string s;
  [[nodiscard]] std::string fn1() const { return "model1"; }
};

struct model2 {
  int x;
};

}  // namespace simple

ANY_MODEL_MAP((simple::model2), simple::trait1) {
  static std::string fn1(simple::model2 const& m) {
    return std::to_string(m.x);
  };
};
}  // namespace

TEST_CASE("any via concept simple") {
  using namespace anyxx;
  using namespace simple;

  any<val, trait1> test1{model1{"model1"}};
  CHECK(test1.fn1() == "model1");
  any<val, trait1> test2{model2{42}};
  CHECK(test2.fn1() == "42");

  CHECK(cat({test1, test2}) == "model142");
}