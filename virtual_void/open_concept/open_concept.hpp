#pragma once

#include <map>
#include <optional>
#include <typeindex>
#include <vector>
#include <virtual_void/data/erased_value.hpp>
#include <virtual_void/utillities/ensure_function_ptr.hpp>
#include <virtual_void/utillities/translate_erased_function.hpp>
#include <virtual_void/utillities/type_list.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::open_concept {

using v_table_function_t = void (*)();
using v_table_t = std::vector<v_table_function_t>;
using remebered_implementations_t =
    std::vector<std::pair<v_table_t&, v_table_function_t>>;

struct extension_method_index;
struct extension_method_head {
  extension_method_head* next = nullptr;
  extension_method_index* index_head = nullptr;

  extension_method_head(extension_method_head*& head);
};
struct extension_method_index {
  std::size_t index = 0;
  extension_method_index* next = nullptr;
  remebered_implementations_t remebered_implementations;

  extension_method_index(extension_method_head& head) : next(head.index_head) {
    head.index_head = this;
  }
};

inline extension_method_head*& get_extension_methods_head() {
  static extension_method_head* head;
  return head;
}
template <typename INTERFACE_NAME>
extension_method_head& get_extension_method_head() {
  static extension_method_head head{get_extension_methods_head()};
  return head;
}

inline extension_method_head::extension_method_head(
    extension_method_head*& head)
    : next(head) {
  head = this;
}

void remember(remebered_implementations_t& remebered_implementations,
              v_table_t& v_table, auto fp) {
  remebered_implementations.push_back(
      {v_table, reinterpret_cast<v_table_function_t>(fp)});
}
inline void insert_function(v_table_t& v_table, std::size_t index,
                            v_table_function_t fp) {
  if (v_table.size() <= index) v_table.resize(index + 1);
  v_table[index] = fp;
}
inline void activate_extension_method_index(extension_method_index& index,
                                            int i) {
  index.index = i;
  for (auto [v_table, fp] : index.remebered_implementations)
    insert_function(v_table, i, fp);
}

inline void activate_extension_methods() {
  static bool run = false;
  if (run) return;
  for (auto head = get_extension_methods_head(); head; head = head->next) {
    auto i = 0;
    for (auto index = head->index_head; index; index = index->next)
      activate_extension_method_index(*index, i++);
  }
  run = true;
}

template <typename INTERFACE_NAME, typename CLASS_NAME>
v_table_t& v_table_instance() {
  static v_table_t v_table;
  return v_table;
}

template <typename INTERFACE_NAME, is_virtual_void VIRTUAL_VOID>
class model {
 public:
  using virtual_void_t = VIRTUAL_VOID;
  using void_t = typename virtual_void_trait<VIRTUAL_VOID>::void_t;
  using base_t = model<INTERFACE_NAME, VIRTUAL_VOID>;

 protected:
  virtual_void_t virtual_void_ = {};
  v_table_t* v_table_ = nullptr;

 public:
  model() = default;
  model(virtual_void_t virtual_void, v_table_t* v_table)
      : virtual_void_(std::move(virtual_void)), v_table_(v_table) {}
  template <typename CONSTRUCTED_WITH>
  model(CONSTRUCTED_WITH&& constructed_with)
    requires erased_constructibile_for<CONSTRUCTED_WITH, VIRTUAL_VOID, base_t>
      : virtual_void_(erased<virtual_void_t>(
            std::forward<CONSTRUCTED_WITH>(constructed_with))) {
    v_table_ = &v_table_instance<INTERFACE_NAME, CONSTRUCTED_WITH>();
  }
  template <typename CONSTRUCT_WITH, typename... ARGS>
  model(std::in_place_type_t<CONSTRUCT_WITH>, ARGS... args)
      : virtual_void_(erased_in_place<virtual_void_t, CONSTRUCT_WITH>(
            std::forward<ARGS>(args)...)) {
    v_table_ = &v_table_instance<INTERFACE_NAME, CONSTRUCT_WITH>();
  }
  template <typename CONSTRUCTED_WITH>
  model(virtual_typed<CONSTRUCTED_WITH, virtual_void_t> const& vt)
      : model(*vt) {}
  template <typename OTHER>
  model(OTHER const& other)
    requires(std::derived_from<OTHER, model<VIRTUAL_VOID>>)
      : virtual_void_(get_virtual_void(other)), v_table_(get_v_table(other)) {}
  template <typename OTHER>
  model(OTHER&& other)
    requires(std::derived_from<OTHER, model<VIRTUAL_VOID>>)
      : virtual_void_(std::move(other.virtual_void_)),
        v_table_(get_v_table(other)) {}
  template <typename OTHER>
  model& operator=(OTHER&& other)
    requires(std::derived_from<OTHER, model<VIRTUAL_VOID>>)
  {
    virtual_void_ = std::move(other.virtual_void_);
    v_table_ = get_v_table(other);
    return *this;
  }
  model(model const&) = default;
  model(model&& rhs) noexcept
      : virtual_void_(std::move(rhs.virtual_void_)), v_table_(rhs.v_table_) {}
  model& operator=(model const& other) = default;

  template <typename INTERFACE_NAME, is_virtual_void OTHER>
  friend class model;
  template <typename INTERFACE_NAME, typename R, typename... ARGS>
  friend class extension_method;

  template <typename INTERFACE_NAME, is_virtual_void VIRTUAL_VOID>
  friend inline auto& get_virtual_void(
      model<INTERFACE_NAME, VIRTUAL_VOID> const& interface);
  template <typename INTERFACE_NAME, is_virtual_void VIRTUAL_VOID>
  friend inline auto move_virtual_void(
      model<INTERFACE_NAME, VIRTUAL_VOID>&& interface);
  template <typename INTERFACE_NAME, is_virtual_void VIRTUAL_VOID>
  friend inline auto get_interface_data(
      model<INTERFACE_NAME, VIRTUAL_VOID> const& interface);
  template <typename INTERFACE_NAME, is_virtual_void VIRTUAL_VOID>
  friend inline auto& get_v_table(
      model<INTERFACE_NAME, VIRTUAL_VOID> const& interface);
};

template <typename INTERFACE_NAME, is_virtual_void VIRTUAL_VOID>
auto& get_virtual_void(model<INTERFACE_NAME, VIRTUAL_VOID> const& m) {
  return m.virtual_void_;
}
template <typename INTERFACE_NAME, is_virtual_void VIRTUAL_VOID>
auto move_virtual_void(model<INTERFACE_NAME, VIRTUAL_VOID>&& m) {
  return std::move(m.virtual_void_);
}
template <typename INTERFACE_NAME, is_virtual_void VIRTUAL_VOID>
auto get_interface_data(model<INTERFACE_NAME, VIRTUAL_VOID> const& m) {
  return get_data(get_virtual_void(m));
}
template <typename INTERFACE_NAME, is_virtual_void VIRTUAL_VOID>
inline auto& get_v_table(model<INTERFACE_NAME, VIRTUAL_VOID> const& m) {
  return *m.v_table_;
}

template <typename INTERFACE_NAME, typename R, typename... ARGS>
class extension_method;

template <typename INTERFACE_NAME, typename R, typename... ARGS>
class extension_method<INTERFACE_NAME, R(ARGS...)>
    : public extension_method_index {
  using interface_t = INTERFACE_NAME;

  using CONSTNESS = typename first_t<ARGS...>;
  using dispatch_t = void_t<CONSTNESS>;
  template <typename CLASS>
  using class_param_t = self_pointer<dispatch_t>::template type<CLASS>;
  using erased_function_t =
      typename translate_erased_function<R, ARGS...>::type;

 public:
  extension_method()
      : extension_method_index(get_extension_method_head<INTERFACE_NAME>()) {}
  template <is_virtual_void VIRTUAL_VOID, typename... OTHER_ARGS>
  auto operator()(model<INTERFACE_NAME, VIRTUAL_VOID> const& m,
                  OTHER_ARGS... args) {
    auto erased_function =
        reinterpret_cast<erased_function_t>(m.v_table_->at(index));
    return (erased_function)(get_interface_data(m),
                             std::forward<OTHER_ARGS>(args)...);
  }
  template <typename CLASS, typename FUNCTION>
  auto define(FUNCTION f) {
    auto fp = ensure_function_ptr<CLASS, class_param_t, R, ARGS...>(f);
    auto& v_table = v_table_instance<INTERFACE_NAME, CLASS>();
    remember(remebered_implementations, v_table, fp);
    return fp;
  }
};

}  // namespace virtual_void::open_concept