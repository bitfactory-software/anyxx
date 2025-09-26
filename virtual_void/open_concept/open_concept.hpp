#pragma once

#include <vector>
#include <virtual_void/data/has_no_meta/observer.hpp>
#include <virtual_void/meta/base_v_table.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/utillities/ensure_function_ptr.hpp>
#include <virtual_void/utillities/translate_erased_function.hpp>
#include <virtual_void/utillities/type_list.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::open_concept {

template< typename V_TABLE> concept is_extendable_v_table = std::same_as<typename V_TABLE::v_table_base_t, meta::base_v_table>;

template <is_extendable_v_table EXTENDED_V_TABLE>
std::size_t extension_method_count_of = 0;

using open_v_table_t = meta::open_v_table_t;

void insert_function(open_v_table_t& v_table, std::size_t index, auto fp) {
  if (v_table.size() <= index) v_table.resize(index + 1);
  v_table[index] = reinterpret_cast<meta::open_v_table_function_t>(fp);
}

template <is_extendable_v_table EXTENDED_V_TABLE, typename CLASS>
open_v_table_t& v_table_instance() {
  return EXTENDED_V_TABLE::template imlpementation<std::decay_t<CLASS>>()->open_v_table;
}

template <is_extendable_v_table EXTENDED_V_TABLE, typename R, typename... ARGS>
class extension_method;

template <is_extendable_v_table EXTENDED_V_TABLE, typename R, typename... ARGS>
class extension_method<EXTENDED_V_TABLE, R(ARGS...)> {
 public:
  using interface_t = EXTENDED_V_TABLE;
  std::size_t index = extension_method_count_of<EXTENDED_V_TABLE> ++;
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
  template <interface::is_interface MODEL, typename... OTHER_ARGS>
  auto operator()(MODEL const& m,
                  OTHER_ARGS&&... args) const {
    auto& v_table = get_v_table(m)->open_v_table;
    if (v_table.size() <= index)
      return default_(nullptr, std::forward<OTHER_ARGS>(args)...);
    auto target = v_table.at(index);
    if (!target) return default_(nullptr, std::forward<OTHER_ARGS>(args)...);

    auto erased_function = reinterpret_cast<erased_function_t>(target);
    return (erased_function)(get_interface_data(m),
                             std::forward<OTHER_ARGS>(args)...);
  }
  template <typename CLASS, typename FUNCTION>
  auto define(FUNCTION f) {
    auto fp = ensure_function_ptr<CLASS, class_param_t, R, ARGS...>(f);
    auto& v_table = v_table_instance<EXTENDED_V_TABLE, CLASS>();
    insert_function(v_table, index, fp);
    return fp;
  }
};

}  // namespace virtual_void::open_concept