#pragma once

#include <virtual_void/interface/extension_method.hpp>

namespace virtual_void::interface {

template <is_interface FIRST, is_interface SECOND, typename R, typename... ARGS>
class multi_method;

template <is_interface FIRST, is_interface SECOND, typename R, typename... ARGS>
  requires has_extension_methods_enabled<FIRST> &&
           has_extension_methods_enabled<SECOND>
class multi_method<FIRST, SECOND, R(ARGS...)> {
 public:
  using interface1_t = FIRST;
  using v_table1_t = extended_v_table<FIRST>;
  using interface2_t = FIRST;
  using v_table2_t = extended_v_table<SECOND>;

  using CONSTNESS1 = typename nth_t<0, ARGS...>;
  using VOID1 = void_t<CONSTNESS1>;
  using observer_interface1_t = interface1_t::template type_for<VOID1>;

  using CONSTNESS2 = typename nth_t<1, ARGS...>;
  using VOID2 = void_t<CONSTNESS2>;
  using observer_interface2_t = interface2_t::template type_for<VOID2>;

  template <typename CLASS>
  using class1_param_t = self_pointer<VOID1>::template type<CLASS>;
  template <typename CLASS>
  using class2_param_t = self_pointer<VOID2>::template type<CLASS>;

  using erased_function_t =
      typename translate_erased_function<R, ARGS...>::type;
  //
  //  template <typename... ARGS>
  //  struct default_function_impl;
  //  template <is_constness CONSTNESS, typename... OTHER_ARGS>
  //  struct default_function_impl<CONSTNESS, OTHER_ARGS...> {
  //    using type = R (*)(observer_interface_t const&, OTHER_ARGS...);
  //    static R noop(observer_interface_t const&, OTHER_ARGS...) {
  //      if constexpr (std::same_as<R, void>) {
  //        return;
  //      } else {
  //        return {};
  //      }
  //    }
  //  };
  //  using default_function = default_function_impl<ARGS...>;
  //
 private:
  std::size_t index1_ = extension_method_count_of<v_table1_t> ++;
  std::size_t index2_ = extension_method_count_of<v_table2_t> ++;
  std::vector<std::vector<void(*)()>> dispatch_matrix_;
  std::size_t dispatch_dimension_size_1_ = 0;
  std::size_t dispatch_dimension_size_2_ = 0;

  ////    default_function::type default_ = default_function::noop;
  //
 public:
  multi_method(multi_method const&) = delete;
  // multi_method(default_function::type f = default_function::noop)
  //     : default_(f) {}
  multi_method() = default;
  //  template <typename... OTHER_ARGS>
  //  auto operator()(observer_interface_t const& m, OTHER_ARGS&&... args) const
  //  {
  //    auto v_table = get_v_table(m)->extension_method_table;
  //    if (v_table->size() <= index_)
  //      return default_(m, std::forward<OTHER_ARGS>(args)...);
  //    auto target = v_table->at(index_);
  //    if (!target) return default_(m, std::forward<OTHER_ARGS>(args)...);
  //
  //    auto erased_function = reinterpret_cast<erased_function_t>(target);
  //    return (erased_function)(get_void_data_ptr(m),
  //                             std::forward<OTHER_ARGS>(args)...);
  //  }
  //  template <typename CLASS, typename... OTHER_ARGS>
  //  auto operator()(CLASS const* p, OTHER_ARGS&&... args) const {
  //    return (*this)(observer_interface_t{*p},
  //    std::forward<OTHER_ARGS>(args)...);
  //  }
  template <typename CLASS1, typename CLASS2, typename FUNCTION>
  auto define(FUNCTION f) {
    auto fp = ensure_function_ptr2<CLASS1, class1_param_t, CLASS2,
                                   class2_param_t, R, ARGS...>(f);
    auto v_table1 =
        runtime::extension_method_table_instance<v_table1_t, CLASS1>();
    auto v_table2 =
        runtime::extension_method_table_instance<v_table2_t, CLASS2>();

    // runtime::insert_function(v_table, index_, fp);
    return fp;
  }
};

}  // namespace virtual_void::interface
