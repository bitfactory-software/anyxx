#pragma once

#include <vector>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/runtime/meta_data.hpp>
#include <virtual_void/utillities/ensure_function_ptr.hpp>
#include <virtual_void/utillities/translate_erased_function.hpp>
#include <virtual_void/utillities/type_list.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::interface {

template <is_interface I>
using extended_v_table = typename I::v_table_t;

template <typename EXTENDED_V_TABLE>
auto extension_method_count_of = std::false_type{};

template <is_interface EXTENDED_INTERACE, typename R, typename... ARGS>
  requires has_extension_methods_enabled<EXTENDED_INTERACE>
class extension_method;

template <is_interface EXTENDED_INTERACE, typename R, typename... ARGS>
  requires has_extension_methods_enabled<EXTENDED_INTERACE>
class extension_method<EXTENDED_INTERACE, R(ARGS...)> {
 public:
  using interface_t = EXTENDED_INTERACE;
  using extended_v_table_t = extended_v_table<EXTENDED_INTERACE>;

  using CONSTNESS = typename first_t<ARGS...>;
  using VOID = void_t<CONSTNESS>;
  using observer_interface_t = interface_t::template type_for<VOID>;
  template <typename CLASS>
  using class_param_t = self_pointer<VOID>::template type<CLASS>;
  using erased_function_t =
      typename translate_erased_function<R, ARGS...>::type;

  template <typename... ARGS>
  struct default_function_impl;
  template <is_constness CONSTNESS, typename... OTHER_ARGS>
  struct default_function_impl<CONSTNESS, OTHER_ARGS...> {
    using type = R (*)(observer_interface_t const&, OTHER_ARGS...);
    static R noop(observer_interface_t const&, OTHER_ARGS...) {
      if constexpr (std::same_as<R, void>) {
        return;
      } else {
        return {};
      }
    }
  };
  using default_function = default_function_impl<ARGS...>;

 private:
  std::size_t index_ = extension_method_count_of<extended_v_table_t> ++;
  default_function::type default_ = default_function::noop;

 public:
  extension_method(extension_method const&) = delete;
  extension_method(default_function::type f = default_function::noop)
      : default_(f) {}
  template <typename... OTHER_ARGS>
  auto operator()(observer_interface_t const& m, OTHER_ARGS&&... args) const {
    auto v_table = get_v_table(m)->extension_method_table;
    if (v_table->size() <= index_)
      return default_(m, std::forward<OTHER_ARGS>(args)...);
    auto target = v_table->at(index_);
    if (!target) return default_(m, std::forward<OTHER_ARGS>(args)...);

    auto erased_function = reinterpret_cast<erased_function_t>(target);
    return (erased_function)(get_void_data_ptr(m),
                             std::forward<OTHER_ARGS>(args)...);
  }
  template <typename CLASS, typename... OTHER_ARGS>
  auto operator()(CLASS const* p, OTHER_ARGS&&... args) const {
    return (*this)(observer_interface_t{*p}, std::forward<OTHER_ARGS>(args)...);
  }
  template <typename CLASS, typename FUNCTION>
  auto define(FUNCTION f) {
    auto fp = ensure_function_ptr<CLASS, class_param_t, R, ARGS...>(f);
    auto v_table =
        runtime::extension_method_table_instance<extended_v_table_t, CLASS>();
    runtime::insert_function(v_table, index_, fp);
    return fp;
  }
};

}  // namespace virtual_void::interface


#define VV_EXTENSION_METHOD_COUNT(interface_) \
template<> \
auto interface::extension_method_count_of<interface_> = 0; \

#define VV_EXTENSION_TABLE_INSTANCE(class_, interface_)               \
  template <>                                                         \
  virtual_void::runtime::extension_method_table_t*                    \
  virtual_void::runtime::extension_method_table_instance<interface_,  \
                                                         class_>() {  \
    return extension_method_table_instance_implementation<interface_, \
                                                          class_>();  \
  }

#define VV_V_TABLE_INSTANCE_STATIC(class, interface_) \
  VV_V_TABLE_INSTANCE_FWD(, class, interface_)        \
  VV_V_TABLE_INSTANCE(, class, interface_)
