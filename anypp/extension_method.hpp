#pragma once

#include <vector>
#include <anypp/any_base.hpp>
#include <anypp/hook.hpp>
#include <anypp/meta_data.hpp>

namespace anypp {

#ifdef VV_DLL_MODE
template <typename EXTENDED_V_TABLE>
std::size_t& extension_method_count_of();
#else
template <typename EXTENDED_V_TABLE>
std::size_t& extension_method_count_of() {
  static std::size_t count = 0;
  return count;
}
#endif

template <is_any ANY>
struct virtual_ {
  using type = ANY;
};

template <typename ARG>
struct translate_erased_function_param {
  using type = ARG;
};
template <is_any ANY>
struct translate_erased_function_param<virtual_<ANY>> {
  using type = typename ANY::void_t;
};

template <typename RET, typename... ARGS>
struct translate_erased_function {
  using type = RET (*)(typename translate_erased_function_param<ARGS>::type...);
};

template <std::size_t COUNT, typename... ARGS>
constexpr std::size_t extension_method_dimension_count = COUNT;
template <std::size_t COUNT, is_any ANY, typename... ARGS>
constexpr std::size_t
    extension_method_dimension_count<COUNT, virtual_<ANY>, ARGS...> =
        extension_method_dimension_count<COUNT + 1, ARGS...>;

template <typename R, typename... CLASSES>
struct ensure_function_ptr_from_functor_t {
  template <typename FUNCTOR, typename... ARGS>
  struct striped_virtuals {
    static R function(CLASSES*... classes, ARGS... args) {
      return FUNCTOR{}(classes..., args...);
    };
  };

  template <typename FUNCTOR, is_any ANY, typename... ARGS>
  struct striped_virtuals<FUNCTOR, virtual_<ANY>, ARGS...>
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
template <is_any ANY, typename... METHOD_ARGS>
struct args_to_tuple<virtual_<ANY>, METHOD_ARGS...> {
  template <typename T, typename ACTUAL_ARG, typename... ACTUAL_ARGS>
  auto operator()(T&& dispatch_args, ACTUAL_ARG&& dispatch_arg,
                  ACTUAL_ARGS&&... actual_args) {
    return args_to_tuple<METHOD_ARGS...>{}(
        std::tuple_cat(std::forward<T>(dispatch_args),
                       std::make_tuple(get_void_data_ptr(dispatch_arg))),
        std::forward<ACTUAL_ARGS>(actual_args)...);
  }
};

template <typename R, typename... ARGS>
struct extension_method_default {
  template <is_any... ANYPPS>
  struct inner {
    template <typename... ARGS>
    struct implemenation {
      struct type {
        using function_t = hook<R(ANYPPS const&..., ARGS...)>;
        static auto function() {
          return [](auto super, ANYPPS const&..., ARGS... args) -> R {
            return R{};
          };
        }
      };
    };
    template <is_any ANY, typename... ARGS>
    struct implemenation<virtual_<ANY>, ARGS...>
        : implemenation<ARGS...> {};
    template <typename... ARGS>
    using type = typename implemenation<ARGS...>::type;
  };

  template <typename... ARGS>
  struct outer {
    template <is_any... ANYPPS>
    using type = inner<ANYPPS...>::template type<ARGS...>;
  };
  template <is_any ANY, typename... ARGS>
  struct outer<virtual_<ANY>, ARGS...> {
    template <is_any... ANYPPS>
    using type = outer<ARGS...>::template type<ANY, ANYPPS...>;
  };

  using type = outer<ARGS...>::template type<>;
};

template <typename DISPATCH, typename... ARGS>
struct dispatch_matrix {
  using type = DISPATCH;
};
template <typename DISPATCH, is_any ANY, typename... ARGS>
struct dispatch_matrix<DISPATCH, virtual_<ANY>, ARGS...> {
  using type = typename dispatch_matrix<std::vector<DISPATCH>, ARGS...>::type;
};

template <typename R, typename... ARGS>
struct extension_method;
template <typename R, typename... ARGS>
struct extension_method<R(ARGS...)> {
  using erased_function_t =
      typename translate_erased_function<R, ARGS...>::type;

  static constexpr std::size_t dimension_count =
      extension_method_dimension_count<0, ARGS...>;

  using dispatch_matrix_t = dispatch_matrix<erased_function_t, ARGS...>::type;
  dispatch_matrix_t dispatch_matrix_;

  using dispatch_indices = std::array<std::size_t, dimension_count>;

  using extension_method_default_t =
      typename extension_method_default<R, ARGS...>::type;
  extension_method_default_t::function_t extension_method_default_hook_;
  extension_method_default_t::function_t::connection default_connection_ =
      extension_method_default_hook_.insert(
          extension_method_default_t::function());

  template <bool MULTIDIM, std::size_t DIM, typename... ARGS>
  struct dispatch_access;

  template <std::size_t DIM, typename... ARGS>
  struct dispatch_access<true, DIM, ARGS...> {
    auto define(auto fp, auto& matrix) {
      matrix = reinterpret_cast<erased_function_t>(fp);
      return fp;
    }
    template <typename DISPATCH_MATRIX, typename DISPATCH_ARGS_TUPLE>
    std::optional<R> invoke(DISPATCH_MATRIX const& target,
                            DISPATCH_ARGS_TUPLE&& dispatch_args_tuple,
                            auto&&...) const {
      if (!target) return {};
      return std::apply(target,
                        std::forward<DISPATCH_ARGS_TUPLE>(dispatch_args_tuple));
    }
  };

  template <std::size_t DIM, is_any ANY, typename... ARGS>
  struct dispatch_access<true, DIM, virtual_<ANY>, ARGS...>
      : dispatch_access<true, DIM + 1, ARGS...> {
    using interface_t = ANY;
    using v_table_t = typename interface_t::v_table_t;
    using next_t = dispatch_access<true, DIM + 1, ARGS...>;

    std::size_t index_ = extension_method_count_of<v_table_t>()++;
    std::size_t dispatch_dimension_size_ = 0;

    template <typename CLASS, typename... CLASSES>
    auto define(auto fp, auto& matrix) {
      auto extension_method_table =
          extension_method_table_instance<v_table_t, CLASS>();
      auto dispatch_index =
          *get_multi_method_index_at(extension_method_table, index_)
               .or_else([&] -> std::optional<std::size_t> {
                 set_multi_method_index_at(
                     extension_method_table, index_, dispatch_dimension_size_);
                 return dispatch_dimension_size_++;
               });
      if (matrix.size() <= dispatch_index) matrix.resize(dispatch_index + 1);
      return next_t::template define<CLASSES...>(fp, matrix[dispatch_index]);
    }

    template <typename DISPATCH_MATRIX, typename DISPATCH_ARGS_TUPLE,
              typename... ACTUAL_ARGS>
    std::optional<R> invoke(DISPATCH_MATRIX const& target,
                            DISPATCH_ARGS_TUPLE&& dispatch_args_tuple,
                            ANY const& any,
                            ACTUAL_ARGS&&... actual_args) const {
      auto extension_method_table =
          get_v_table(any)->extension_method_table;
      auto dispatch_dim =
          get_multi_method_index_at(extension_method_table, index_);
      if (!dispatch_dim) return {};
      if (target.size() < *dispatch_dim + 1) return {};
      return next_t::invoke(
          target[*dispatch_dim],
          std::forward<DISPATCH_ARGS_TUPLE>(dispatch_args_tuple),
          std::forward<ACTUAL_ARGS>(actual_args)...);
    }
  };

  template <is_any ANY, typename... ARGS>
  struct dispatch_access<false, 0, virtual_<ANY>, ARGS...> {
    using interface_t = ANY;
    using v_table_t = typename interface_t::v_table_t;
    std::size_t index_ = extension_method_count_of<v_table_t>()++;

    template <typename CLASS>
    auto define(auto fp, auto&) {
      auto v_table =
          extension_method_table_instance<v_table_t, CLASS>();
      insert_function(v_table, index_, fp);
      return fp;
    }
    template <typename DISPATCH_MATRIX, typename DISPATCH_ARGS_TUPLE,
              typename... ACTUAL_ARGS>
    std::optional<R> invoke(DISPATCH_MATRIX const&,
                            DISPATCH_ARGS_TUPLE&& dispatch_args_tuple,
                            ANY const& any,
                            ACTUAL_ARGS&&... actual_args) const {
      auto v_table = get_v_table(any)->extension_method_table;
      auto target = get_function(v_table, index_);
      if (!target) return {};
      auto erased_function = reinterpret_cast<erased_function_t>(target);
      return std::apply(erased_function,
                        std::forward<DISPATCH_ARGS_TUPLE>(dispatch_args_tuple));
    }
  };

  dispatch_access<(dimension_count > 1), 0, ARGS...> dispatch_access_;

 public:
  template <typename... CLASSES>
  auto define(auto f) {
    auto fp = ensure_function_ptr_from_functor_t<
        R, CLASSES...>::template instance<ARGS...>(f);
    return dispatch_access_.template define<CLASSES...>(fp, dispatch_matrix_);
  };
  template <typename... ACTUAL_ARGS>
  auto operator()(ACTUAL_ARGS&&... actual_args) const {
    auto dispatch_args_tuple = args_to_tuple<ARGS...>{}(
        std::tuple<>{}, std::forward<ACTUAL_ARGS>(actual_args)...);
    return *dispatch_access_
                .invoke(dispatch_matrix_, dispatch_args_tuple,
                        std::forward<ACTUAL_ARGS>(actual_args)...)
                .or_else([&]() -> std::optional<R> {
                  return std::invoke(extension_method_default_hook_,
                                     std::forward<ACTUAL_ARGS>(actual_args)...);
                });
  }
  auto& get_extension_method_default_hook() {
    return extension_method_default_hook_;
  };
};

}  // namespace anypp

#ifdef VV_DLL_MODE

#define VV_EXTENSION_METHOD_COUNT_FWD(export_, ns_, c_)      \
  namespace ns_ {                                            \
  struct c_;                                                 \
  }                                                          \
  namespace anypp {                        \
  template <>                                                \
  export_ std::size_t& extension_method_count_of<ns_::c_>(); \
  }

#define VV_EXTENSION_METHOD_COUNT_IMPL(ns_, c_)                                \
  template <>                                                                  \
  std::size_t& anypp::extension_method_count_of<ns_::c_>() { \
    static std::size_t count = 0;                                              \
    return count;                                                              \
  }

#define VV_EXTENSION_TABLE_INSTANCE_FWD(export_, class_, interface_namespace_) \
  namespace anypp {                                          \
  template <>                                                                  \
  export_ anypp::extension_method_table_t*                     \
  anypp::extension_method_table_instance<interface_##_v_table, \
                                                         class_>(); \
  }

#define VV_EXTENSION_TABLE_INSTANCE(class_, interface_namespace_, interface_) \
  template <>                                                                  \
  anypp::extension_method_table_t*                             \
  anypp::extension_method_table_instance<interface_##_v_table, \
                                                         class_>() {           \
    return extension_method_table_instance_implementation<                     \
        interface_##_v_table, class_>();                                       \
  }

#else

#define VV_EXTENSION_METHOD_COUNT_FWD(...)
#define VV_EXTENSION_METHOD_COUNT_IMPL(...)
#define VV_EXTENSION_TABLE_INSTANCE_FWD(...)
#define VV_EXTENSION_TABLE_INSTANCE(...)

#endif
