#pragma once

#include <virtual_void/interface/extension_method.hpp>

namespace virtual_void::interface {

template <is_interface INTERFACE>
struct virtual_ {
  using type = INTERFACE;
};

template <typename ARG>
struct translate_erased_function_param {
  using type = ARG;
};
template <is_interface INTERFACE>
struct translate_erased_function_param<virtual_<INTERFACE>> {
  using type = typename INTERFACE::void_t;
};

template <typename RET, typename... ARGS>
struct translate_erased_function {
  using type = RET (*)(typename translate_erased_function_param<ARGS>::type...);
};

template <std::size_t COUNT, typename... ARGS>
constexpr std::size_t multi_method_dimension_count = COUNT;
template <std::size_t COUNT, is_interface INTERFACE, typename... ARGS>
constexpr std::size_t
    multi_method_dimension_count<COUNT, virtual_<INTERFACE>, ARGS...> =
        multi_method_dimension_count<COUNT + 1, ARGS...>;

template <typename R, typename... CLASSES>
struct ensure_function_ptr_from_functor_t {
  template <typename FUNCTOR, typename... ARGS>
  struct striped_virtuals {
    static R function(CLASSES*... classes, ARGS... args) {
      return FUNCTOR{}(classes..., args...);
    };
  };

  template <typename FUNCTOR, is_interface INTERFACE, typename... ARGS>
  struct striped_virtuals<FUNCTOR, virtual_<INTERFACE>, ARGS...>
      : striped_virtuals<FUNCTOR, ARGS...> {};

  template <typename... ARGS>
  static auto instance(
      auto functor)  // if functor is a templated operator() from a
                     // stateless function object, instantiate it now!;
  {
    using functor_t = decltype(functor);
    if constexpr (std::is_pointer_v<functor_t>) {
      return functor;
    } else {
      return striped_virtuals<functor_t, ARGS...>::function;
    }
  }
};

template <typename... METHOD_ARGS>
struct args_to_tuple {
  template <typename T, typename... ACTUAL_ARGS>
  auto operator()(T&& dispatch_args, ACTUAL_ARGS&&... actual_args) {
    return std::tuple_cat(
        std::forward<T>(dispatch_args),
        std::make_tuple(std::forward<ACTUAL_ARGS>(actual_args)...));
  }
};
template <is_interface INTERFACE, typename... METHOD_ARGS>
struct args_to_tuple<virtual_<INTERFACE>, METHOD_ARGS...> {
  template <typename T, typename ACTUAL_ARG, typename... ACTUAL_ARGS>
  auto operator()(T&& dispatch_args, ACTUAL_ARG&& dispatch_arg,
                  ACTUAL_ARGS&&... actual_args) {
    return args_to_tuple<METHOD_ARGS...>{}(
        std::tuple_cat(std::forward<T>(dispatch_args),
                       std::make_tuple(get_void_data_ptr(dispatch_arg))),
        std::forward<ACTUAL_ARGS>(actual_args)...);
  }
};

template <typename DISPATCH, typename... ARGS>
struct dispatch_matrix {
  using type = DISPATCH;
};
template <typename DISPATCH, is_interface INTERFACE, typename... ARGS>
struct dispatch_matrix<DISPATCH, virtual_<INTERFACE>, ARGS...> {
  using type = typename dispatch_matrix<std::vector<DISPATCH>, ARGS...>::type;
};

template <typename R, typename... ARGS>
struct multi_method;
template <typename R, typename... ARGS>
struct multi_method<R(ARGS...)> {
  using erased_function_t =
      typename translate_erased_function<R, ARGS...>::type;

  static constexpr std::size_t dimension_count =
      multi_method_dimension_count<0, ARGS...>;

  using dispatch_matrix_t = dispatch_matrix<erased_function_t, ARGS...>::type;
  dispatch_matrix_t dispatch_matrix_;

  using dispatch_indices = std::array<std::size_t, dimension_count>;

  template <std::size_t DIM, typename... ARGS>
  struct dispatch_access {
    auto define(auto fp, auto& matrix) {
      matrix = reinterpret_cast<erased_function_t>(fp);
      return fp;
    }
    template <typename DISPATCH_MATRIX, typename DISPATCH_ARGS_TUPLE>
    auto invoke(DISPATCH_MATRIX const& target,
                DISPATCH_ARGS_TUPLE&& dispatch_duple_args, auto&&...) const {
      return std::apply(target,
                        std::forward<DISPATCH_ARGS_TUPLE>(dispatch_duple_args));
    }
  };

  template <std::size_t DIM, is_interface INTERFACE, typename... ARGS>
  struct dispatch_access<DIM, virtual_<INTERFACE>, ARGS...>
      : dispatch_access<DIM + 1, ARGS...> {
    using interface_t = INTERFACE;
    using v_table_t = typename interface_t::v_table_t;
    using next_t = dispatch_access<DIM + 1, ARGS...>;

    std::size_t index_ = extension_method_count_of<v_table_t> ++;
    std::size_t dispatch_dimension_size_ = 0;

    template <typename CLASS, typename... CLASSES>
    auto define(auto fp, auto& matrix) {
      auto extension_method_table =
          runtime::extension_method_table_instance<v_table_t, CLASS>();
      auto dispatch_index =
          *runtime::get_multi_method_index_at(extension_method_table, index_)
               .or_else([&] -> std::optional<std::size_t> {
                 runtime::set_multi_method_index_at(
                     extension_method_table, index_, dispatch_dimension_size_);
                 return dispatch_dimension_size_++;
               });
      if (matrix.size() <= dispatch_index) matrix.resize(dispatch_index + 1);
      return next_t::template define<CLASSES...>(fp, matrix[dispatch_index]);
    }

    template <typename DISPATCH_MATRIX, typename DISPATCH_ARGS_TUPLE,
              typename... ACTUAL_ARGS>
    auto invoke(DISPATCH_MATRIX const& target,
                DISPATCH_ARGS_TUPLE&& dispatch_args_tuple,
                INTERFACE const& interface,
                ACTUAL_ARGS&&... actual_args) const {
      auto extension_method_table =
          get_v_table(interface)->extension_method_table;
      auto dispatch_dim =
          runtime::get_multi_method_index_at(extension_method_table, index_);
      assert(dispatch_dim);
      return next_t::invoke(
          target[*dispatch_dim],
          std::forward<DISPATCH_ARGS_TUPLE>(dispatch_args_tuple),
          std::forward<ACTUAL_ARGS>(actual_args)...);
    }
  };

  dispatch_access<0, ARGS...> dispatch_access_;

  template <typename... CLASSES>
  auto define(auto f) {
    auto fp = ensure_function_ptr_from_functor_t<
        R, CLASSES...>::template instance<ARGS...>(f);
    return dispatch_access_.template define<CLASSES...>(fp, dispatch_matrix_);
  };

  template <typename... ACTUAL_ARGS>
  auto operator()(ACTUAL_ARGS&&... actual_args) {
    auto dispatch_args_tuple = args_to_tuple<ARGS...>{}(
        std::tuple<>{}, std::forward<ACTUAL_ARGS>(actual_args)...);
    return dispatch_access_.invoke(dispatch_matrix_, dispatch_args_tuple,
                                   std::forward<ACTUAL_ARGS>(actual_args)...);
  }
};

}  // namespace virtual_void::interface
