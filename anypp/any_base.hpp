#pragma once

#include <optional>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <anypp/borrow_erased_data.hpp>
#include <anypp/move_erased_data.hpp>
#include <anypp/trait.hpp>
#include <anypp/meta_data.hpp>

namespace anypp {

template <is_erased_data ERASED_DATA>
class any_base;

template <typename I>
concept is_any_impl = requires(I i, I::erased_data_t ed) {
  typename I::void_t;
  typename I::erased_data_t;
  typename I::v_table_t;
  typename I::trait_t;
  { get_erased_data(i) };
};

template <typename I>
concept is_any = is_any_impl<std::decay_t<I>>;

template <class E>
concept is_typed_any = is_any<E> && requires(E e) {
  typename E::trait_t;
  typename E::value_t;
  { E::is_const } -> std::convertible_to<bool>;
};

template <typename CONSTRUCTED_WITH, typename ERASED_DATA>
concept constructibile_for =
    erased_constructibile_for<CONSTRUCTED_WITH, ERASED_DATA,
                                    any_base<ERASED_DATA>> &&
    !is_any<CONSTRUCTED_WITH> &&
    !is_typed_any<std::remove_cvref_t<CONSTRUCTED_WITH>>;

template <is_erased_data ERASED_DATA>
class any_base {
 public:
  using erased_data_t = ERASED_DATA;
  using trait_t = trait<erased_data_t>;
  using void_t = typename trait_t::void_t;
  using v_table_t = any_base_v_table;

 protected:
  erased_data_t erased_data_ = {};
  v_table_t* v_table_ = nullptr;

  any_base() = default;
  any_base(erased_data_t erased_data, v_table_t* v_table)
      : erased_data_(std::move(erased_data)), v_table_(v_table) {}
  template <typename CONSTRUCTED_WITH>
  any_base(CONSTRUCTED_WITH&& constructed_with)
    requires constructibile_for<CONSTRUCTED_WITH, ERASED_DATA>
      : erased_data_(erased<erased_data_t>(
            std::forward<CONSTRUCTED_WITH>(constructed_with))) {
    using t = unerased<ERASED_DATA, CONSTRUCTED_WITH>;
  }

 public:
  template <is_any OTHER>
  any_base(const OTHER& other)
    requires(std::derived_from<typename OTHER::v_table_t, v_table_t> &&
             borrowable_from<erased_data_t,
                                   typename OTHER::erased_data_t>)
      : erased_data_(borrow_as<ERASED_DATA>(other.erased_data_)),
        v_table_(get_v_table(other)) {}
  template <typename OTHER>
  any_base(OTHER&& other)
    requires(std::derived_from<typename OTHER::v_table_t, v_table_t> &&
             moveable_from<erased_data_t, typename OTHER::erased_data_t>)
      : erased_data_(move_to<ERASED_DATA>(std::move(other.erased_data_))),
        v_table_(get_v_table(other)) {}
  template <typename OTHER>
  any_base& operator=(OTHER&& other)
    requires(std::derived_from<OTHER::v_table_t, v_table_t>)
  {
    erased_data_ = move_to<ERASED_DATA>(std::move(other.erased_data_));
    v_table_ = get_v_table(other);
    return *this;
  }
  any_base(const any_base&) = default;
  // any_base(any_base&) requires(std::is_copy_constructible_v<any_base>) = default;
  any_base(any_base&& rhs) noexcept
      : erased_data_(std::move(rhs.erased_data_)), v_table_(rhs.v_table_) {}
  any_base& operator=(any_base const& other) = default;

  template <is_erased_data OTHER>
  friend class any_base;

  template <is_erased_data ERASED_DATA>
  friend inline auto& get_erased_data(any_base<ERASED_DATA> const& any);
  template <is_erased_data ERASED_DATA>
  friend inline auto move_erased_data(any_base<ERASED_DATA>&& any);
  template <is_erased_data ERASED_DATA>
  friend inline auto get_void_data_ptr(any_base<ERASED_DATA> const& any);
  template <is_any I>
  friend inline auto get_v_table(I const& any);

  template <is_any TO, is_any FROM>
  friend inline TO unchecked_downcast_to(FROM from)
    requires(std::derived_from<TO, FROM>);

 public:
  void operator()() const {}
  void* operator[](void*) const { return {}; }
};

template <typename V_TABLE, is_erased_data ERASED_DATA>
struct interface_t;

template <typename V_TABLE, is_erased_data ERASED_DATA>
using interface_for = typename interface_t<V_TABLE, ERASED_DATA>::type;

template <is_erased_data ERASED_DATA>
auto& get_erased_data(any_base<ERASED_DATA> const& any) {
  return any.erased_data_;
}
template <is_erased_data ERASED_DATA>
auto move_erased_data(any_base<ERASED_DATA>&& any) {
  return std::move(any.erased_data_);
}
template <is_erased_data ERASED_DATA>
auto get_void_data_ptr(any_base<ERASED_DATA> const& any) {
  return get_void_data_ptr(get_erased_data(any));
}

template <is_any Any>
inline const auto& get_runtime(Any const& any) {
  return *get_v_table(any)->meta_data;
}

template <is_erased_data VV>
bool is_derived_from(const std::type_info& from, any_base<VV> const& any) {
  return get_v_table(any)->_is_derived_from(from);
}
template <is_any FROM, is_erased_data VV>
bool is_derived_from(any_base<VV> const& any) {
  return is_derived_from(typeid(FROM::v_table_t), any);
}

template <typename V_TABLE>
void set_is_derived_from(auto v_table) {
  v_table->_is_derived_from = +[](const std::type_info& from) {
    return V_TABLE::static_is_derived_from(from);
  };
}

template <typename TO>
auto unchecked_v_table_downcast_to(any_base_v_table* v_table) {
  return static_cast<TO*>(v_table);
}
template <is_any TO>
auto unchecked_v_table_downcast_to(any_base_v_table* v_table) {
  return unchecked_v_table_downcast_to<typename TO::v_table_t>(v_table);
}

template <is_any Any>
inline auto get_v_table(Any const& any) {
  return unchecked_v_table_downcast_to<Any>(any.v_table_);
}

template <is_any TO, is_any FROM>
TO unchecked_downcast_to(FROM from)
  requires(std::derived_from<TO, FROM>)
{
  return TO{std::move(from.erased_data_),
            unchecked_v_table_downcast_to<TO>(from.v_table_)};
}

template <is_any TO, is_any FROM>
std::optional<TO> downcast_to(FROM from)
  requires(std::derived_from<TO, FROM>)
{
  if (is_derived_from<TO>(from))
    return {unchecked_downcast_to<TO>(std::move(from))};
  return {};
}

template <typename U, is_any Any>
auto unchecked_unerase_cast(Any const& o) {
  return unchecked_unerase_cast<U>(get_erased_data(o));
}
template <typename U, is_any Any>
auto unerase_cast(Any const& o) {
  return unerase_cast<U>(get_erased_data(o), get_runtime(o));
}
template <typename U, is_any Any>
auto unerase_cast(Any const* o) {
  unerase_cast<U>(get_erased_data(*o), get_runtime(o));
  return nullptr;
}

#ifdef ANY_DLL_MODE
template <typename V_TABLE, typename CONCRETE>
V_TABLE* v_table_instance_implementaion();
#else
template <typename V_TABLE, typename CONCRETE>
V_TABLE* v_table_instance_implementaion() {
  static V_TABLE v_table{std::in_place_type<CONCRETE>};
  static auto __ =
      anypp::get_meta_data<CONCRETE>().register_v_table(
          &v_table);
  return &v_table;
}
#endif  // DEBUG

template <template <typename...> typename Any>
constexpr bool has_extension_methods = false;

template <typename I>
concept has_extension_methods_enabled =
    is_any<I> && I::v_table_t::extension_methods_enabled;

template <bool HAS_EXTENSION_METHODS, template <typename...> typename Any>
struct extension_method_holder;
template <template <typename...> typename Any>
struct extension_method_holder<false, Any> {};
template <template <typename...> typename Any>
struct extension_method_holder<true, Any> {
  extension_method_table_t* extension_method_table = nullptr;
};

}  // namespace anypp
