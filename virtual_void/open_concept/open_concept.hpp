#pragma once

#include <vector>
#include <virtual_void/data/has_no_meta/observer.hpp>
#include <virtual_void/utillities/ensure_function_ptr.hpp>
#include <virtual_void/utillities/translate_erased_function.hpp>
#include <virtual_void/utillities/type_list.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::open_concept {

template <typename INTERFACE_NAME>
std::size_t extension_method_count_of = 0;

using v_table_function_t = void (*)();
using v_table_t = std::vector<v_table_function_t>;

void insert_function(v_table_t& v_table, std::size_t index, auto fp) {
  if (v_table.size() <= index) v_table.resize(index + 1);
  v_table[index] = reinterpret_cast<v_table_function_t>(fp);
}

template <typename INTERFACE_NAME, typename CLASS_NAME>
v_table_t& v_table_instance() {
  static v_table_t v_table;
  return v_table;
}

template <typename INTERFACE_NAME, is_erased_data ERASED_DATA>
class model {
 public:
  using erased_data_t = ERASED_DATA;
  using void_t = typename erased_data_trait<ERASED_DATA>::void_t;
  using base_t = model<INTERFACE_NAME, ERASED_DATA>;

 protected:
  erased_data_t erased_data_ = {};
  v_table_t* v_table_ = nullptr;

 public:
  model() = default;
  model(erased_data_t virtual_void, v_table_t* v_table)
      : erased_data_(std::move(virtual_void)), v_table_(v_table) {}
  template <typename CONSTRUCTED_WITH>
  model(CONSTRUCTED_WITH&& constructed_with)
    requires erased_constructibile_for<CONSTRUCTED_WITH, ERASED_DATA, base_t>
      : erased_data_(erased<erased_data_t>(
            std::forward<CONSTRUCTED_WITH>(constructed_with))) {
    v_table_ = &v_table_instance<INTERFACE_NAME,
                                 std::remove_cvref_t<CONSTRUCTED_WITH>>();
  }
  template <typename CONSTRUCTED_WITH>
  model(CONSTRUCTED_WITH const* constructed_with)
    requires erased_constructibile_for<CONSTRUCTED_WITH, ERASED_DATA, base_t>
      : erased_data_(erased<erased_data_t>(
            std::forward<CONSTRUCTED_WITH const&>(*constructed_with))) {
    v_table_ = &v_table_instance<INTERFACE_NAME,
                                 std::remove_cvref_t<CONSTRUCTED_WITH>>();
  }
  template <typename OTHER>
  model(OTHER const& other)
    requires(std::derived_from<OTHER, model<ERASED_DATA>>)
      : erased_data_(get_virtual_void(other)), v_table_(get_v_table(other)) {}
  template <typename OTHER>
  model(OTHER&& other)
    requires(std::derived_from<OTHER, model<ERASED_DATA>>)
      : erased_data_(std::move(other.erased_data_)),
        v_table_(get_v_table(other)) {}
  template <typename OTHER>
  model& operator=(OTHER&& other)
    requires(std::derived_from<OTHER, model<ERASED_DATA>>)
  {
    erased_data_ = std::move(other.erased_data_);
    v_table_ = get_v_table(other);
    return *this;
  }
  model(model const&) = default;
  model(model&& rhs) noexcept
      : erased_data_(std::move(rhs.erased_data_)), v_table_(rhs.v_table_) {}
  model& operator=(model const& other) = default;

  template <typename INTERFACE_NAME, is_erased_data OTHER>
  friend class model;
  template <typename INTERFACE_NAME, typename R, typename... ARGS>
  friend class extension_method;

  template <typename INTERFACE_NAME, is_erased_data ERASED_DATA>
  friend inline auto& get_virtual_void(
      model<INTERFACE_NAME, ERASED_DATA> const& interface);
  template <typename INTERFACE_NAME, is_erased_data ERASED_DATA>
  friend inline auto move_virtual_void(
      model<INTERFACE_NAME, ERASED_DATA>&& interface);
  template <typename INTERFACE_NAME, is_erased_data ERASED_DATA>
  friend inline auto get_interface_data(
      model<INTERFACE_NAME, ERASED_DATA> const& interface);
  template <typename INTERFACE_NAME, is_erased_data ERASED_DATA>
  friend inline auto& get_v_table(
      model<INTERFACE_NAME, ERASED_DATA> const& interface);
};

template <typename INTERFACE_NAME, is_erased_data ERASED_DATA>
auto& get_virtual_void(model<INTERFACE_NAME, ERASED_DATA> const& m) {
  return m.erased_data_;
}
template <typename INTERFACE_NAME, is_erased_data ERASED_DATA>
auto move_virtual_void(model<INTERFACE_NAME, ERASED_DATA>&& m) {
  return std::move(m.erased_data_);
}
template <typename INTERFACE_NAME, is_erased_data ERASED_DATA>
auto get_interface_data(model<INTERFACE_NAME, ERASED_DATA> const& m) {
  return get_data(get_virtual_void(m));
}
template <typename INTERFACE_NAME, is_erased_data ERASED_DATA>
inline auto& get_v_table(model<INTERFACE_NAME, ERASED_DATA> const& m) {
  return *m.v_table_;
}

template <typename INTERFACE_NAME, typename R, typename... ARGS>
class extension_method;

template <typename INTERFACE_NAME, typename R, typename... ARGS>
class extension_method<INTERFACE_NAME, R(ARGS...)> {
 public:
  using interface_t = INTERFACE_NAME;
  std::size_t index = extension_method_count_of<INTERFACE_NAME> ++;
  using CONSTNESS = typename first_t<ARGS...>;
  using dispatch_t = void_t<CONSTNESS>;
  template <typename CLASS>
  using class_param_t = self_pointer<dispatch_t>::template type<CLASS>;
  using erased_function_t =
      typename translate_erased_function<R, ARGS...>::type;

 private:
  erased_function_t default_;
  static erased_function_t make_default_noop() {
    return +[]<typename... ARGS>(ARGS...) -> R {
      if constexpr (std::same_as<R, void>) {
        return;
      } else {
        return {};
      }
    };
  }

 public:
  extension_method(extension_method const&) = delete;
  extension_method(erased_function_t f = make_default_noop())
      : default_(f) {}
  template <is_erased_data ERASED_DATA, typename... OTHER_ARGS>
  auto operator()(model<INTERFACE_NAME, ERASED_DATA> const& m,
                  OTHER_ARGS&&... args) const {
    if (m.v_table_->size() <= index)
      return default_(nullptr, std::forward<OTHER_ARGS>(args)...);
    auto target = m.v_table_->at(index);
    if (!target) return default_(nullptr, std::forward<OTHER_ARGS>(args)...);

    auto erased_function = reinterpret_cast<erased_function_t>(target);
    return (erased_function)(get_interface_data(m),
                             std::forward<OTHER_ARGS>(args)...);
  }
  template <typename CLASS, typename... OTHER_ARGS>
  auto operator()(CLASS const* p, OTHER_ARGS&&... args) const {
    return (*this)(model<INTERFACE_NAME, data::has_no_meta::const_observer>{*p},
                   std::forward<OTHER_ARGS>(args)...);
  }
  template <typename CLASS, typename FUNCTION>
  auto define(FUNCTION f) {
    auto fp = ensure_function_ptr<CLASS, class_param_t, R, ARGS...>(f);
    auto& v_table = v_table_instance<INTERFACE_NAME, CLASS>();
    insert_function(v_table, index, fp);
    return fp;
  }
};

}  // namespace virtual_void::open_concept