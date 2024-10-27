#pragma once

#include <typeindex>

#include "../../../class_hierarchy/class_hierarchy.h"
#include "../../../utillities/ensure_function_ptr.h"
#include "../../../utillities/overload.h"
#include "../../../utillities/type_list.h"
#include "../../data/has_m_table/m_table.h"
#include "../../virtual_void.h"
#include "../domain.h"
#include "../table.h"

namespace virtual_void::m_table {

class open_method_base;
using open_methods = std::vector<open_method_base*>;
using m_table_map = std::unordered_map<std::type_info const*,
                                       erased::data::has_m_table::m_table_t*>;

struct domain : virtual_void::open_method::domain<open_method_base> {
  m_table_map m_table_map;
};

class open_method_base : public virtual_void::open_method::table {
  const int m_table_index_ = -1;

 public:
  explicit open_method_base(domain& domain)
      : m_table_index_((int)domain.open_methods.size()) {
    domain.open_methods.push_back(this);
  }
  int m_table_index() const { return m_table_index_; }
};

template <typename DISPATCH, typename VOID>
concept MtableDispatchableVoid = requires(const DISPATCH& void_) {
  { void_.data() } -> std::convertible_to<VOID>;
  {
    void_.m_table()
  } -> std::convertible_to<const erased::data::has_m_table::m_table_t*>;
};

template <typename R, typename... ARGS>
class open_method;

template <typename R, typename... ARGS>
class open_method<R(ARGS...)> : public open_method_base {
  static_assert(std::same_as<first_t<ARGS...>, void*> ||
                std::same_as<first_t<ARGS...>, const void*>);

 public:
  using dispatch_t = typename first_t<ARGS...>;
  template <typename CLASS>
  using class_param_t = self_pointer<dispatch_t>::template type<CLASS>;
  using virtual_void_t = std::pair<const erased::data::has_m_table::m_table_t*, dispatch_t>;
  using erased_function_t = R (*)(ARGS...);

 private:
  table table_;

 public:
  using open_method_base::open_method_base;
  template <typename CLASS, typename FUNCTION>
  auto define(FUNCTION f) {
    auto fp = ensure_function_ptr<CLASS, class_param_t, R, ARGS...>(f);
    return define_erased(typeid(CLASS), fp);
  }
  template <typename... OTHER_ARGS>
  R operator()(erased::data::has_m_table::m_table_t const& m_table, dispatch_t data,
               OTHER_ARGS&&... args) const {
    auto erased_function =
        reinterpret_cast<erased_function_t>(m_table[m_table_index()]);
    return (erased_function)(data, std::forward<OTHER_ARGS>(args)...);
  }
  template <typename DATA, typename... OTHER_ARGS>
  R operator()(const erased::virtual_void<DATA>& virtual_void_,
               OTHER_ARGS&&... args) const {
    return (*this)(*virtual_void_.meta()->get_m_table(), virtual_void_.data(),
                   std::forward<OTHER_ARGS>(args)...);
  }
  template <erased::has_virtual_void_trait DATA, typename... OTHER_ARGS>
  R operator()(const DATA& ptr, OTHER_ARGS&&... args) const {
    erased::virtual_void<DATA> virtual_void_{ptr};
    return (*this)(virtual_void_, std::forward<OTHER_ARGS>(args)...);
  }
  template <typename... OTHER_ARGS>
  R operator()(const virtual_void_t& param, OTHER_ARGS&&... args) const {
    return (*this)(*param.first, param.second,
                   std::forward<OTHER_ARGS>(args)...);
  }
  template <typename POINTER, typename... OTHER_ARGS>
  R operator()(const POINTER& pointer, OTHER_ARGS&&... args) const
    requires MtableDispatchableVoid<POINTER, dispatch_t>
  {
    return (*this)(*pointer.m_table(), pointer.data(),
                   std::forward<OTHER_ARGS>(args)...);
  }
  template <typename POINTER, typename... OTHER_ARGS>
  R call(const POINTER& pointer, OTHER_ARGS&&... args) const
    requires MtableDispatchableVoid<POINTER, dispatch_t>
  {
    return (*this)(pointer, std::forward<OTHER_ARGS>(args)...);
  }
};

template <typename CLASSES>
constexpr nullptr_t declare_classes(m_table_map& registry) {
  class_hierarchy::visit_classes<CLASSES, true>(
      overload{[&]<typename C> { registry[&typeid(C)] = erased::data::has_m_table::m_table_of<C>(); },
               [&]<typename C, typename B> {}});
  return {};
}

template <typename CLASSES>
constexpr nullptr_t declare_classes(CLASSES, domain& domain) {
  declare_classes<CLASSES>(domain.m_table_map);
  declare_classes<CLASSES>(domain.classes);
  return {};
}

template <typename... CLASSES>
constexpr nullptr_t declare_classes(domain& domain) {
  return declare_classes(virtual_void::type_list<CLASSES...>{}, domain);
}

inline void set_m_table(auto type_info, auto m_table,
                        const open_method_base& method) {
  auto target = method.is_defined(*type_info);
  if (!target) target = method.get_default();
  m_table->set_method(method.m_table_index(), target);
}
inline void fix_m_tables(const m_table_map& m_table_map,
                         const open_method_base& method) {
  for (auto [type_info, m_tabele] : m_table_map)
    set_m_table(type_info, m_tabele, method);
}
inline void fix_m_tables(const open_methods& domain,
                         const m_table_map& m_table_map) {
  for (const auto& method : domain) fix_m_tables(m_table_map, *method);
}

inline void fix_m_tables(const domain& domain) {
  for (const auto& method : domain.open_methods)
    fix_m_tables(domain.m_table_map, *method);
}

}  // namespace virtual_void::m_table