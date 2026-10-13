#pragma once

/*! \file bit_factory/anyxx.hpp
    \brief C++ header only library for external polymorphism.

    for Microsoft C++, you must enable the C-Preprocessor with this flag:
    /Zc:preprocessor (see CMakeLists.txt for example)
*/

#include <bit_factory/anyxx/any_access.hpp>
#include <bit_factory/anyxx/any_concept.hpp>
#include <bit_factory/anyxx/errors/error.hpp>
#include <bit_factory/anyxx/extensions/hook.hpp>
#include <vector>
#include <algorithm>
#include <cassert>
#include <typeinfo>

namespace anyxx {

using dispatch_table_function_t = void (*)();
using dispatch_table_dispatch_index_t = std::size_t;
using dispatch_table_entry_t = unsigned long long;
using dispatch_table_t = std::vector<dispatch_table_entry_t>;
template <typename AnyVTable, typename Class>
dispatch_table_t* dispatch_table_instance_implementation() {
  static dispatch_table_t dispatch_table;
  return &dispatch_table;
}
#ifdef ANY_DLL_MODE
template <typename AnyVTable, typename Class>
dispatch_table_t* dispatch_table_instance();
#else
template <typename AnyVTable, typename Class>
dispatch_table_t* dispatch_table_instance() {
  return dispatch_table_instance_implementation<AnyVTable, Class>();
}
#endif

#define ANY_OPEN_DISPATCH                                    \
  ANY_V_TABLE_DATA(anyxx::dispatch_table_t*, dispatch_table, \
                   anyxx::dispatch_table_instance<v_table_t, Concrete>())

void insert_function(dispatch_table_t* table, std::size_t index, auto fp) {
  if (table->size() <= index) table->resize(index + 1);
  auto& entry = table->at(index);
  entry = reinterpret_cast<unsigned long long>(fp);
}
inline dispatch_table_function_t get_function(dispatch_table_t* table,
                                              std::size_t index) {
  if (table->size() <= index) return {};
  return reinterpret_cast<dispatch_table_function_t>(table->at(index));
}

inline dispatch_table_dispatch_index_t get_multi_dispatch_index_at(
    dispatch_table_t* table, std::size_t index) {
  if (table->size() <= index) return {};
  if (auto const entry = table->at(index))
    return static_cast<dispatch_table_dispatch_index_t>(entry);
  else
    return {};
}
inline void set_multi_dispatch_index_at(
    dispatch_table_t* table, std::size_t index_multi_dispatch,
    dispatch_table_dispatch_index_t
        dispatch_index_of_class_in_dispatch_matrix) {
  if (table->size() <= index_multi_dispatch)
    table->resize(index_multi_dispatch + 1);
  auto& entry = table->at(index_multi_dispatch);
  entry = dispatch_index_of_class_in_dispatch_matrix;
}

template <typename VTable>
concept is_open_dispatch_v_table = requires(VTable* v_table) {
  { v_table->dispatch_table } -> std::convertible_to<dispatch_table_t*>;
};

#ifdef ANY_DLL_MODE
template <typename AnyVTable>
std::size_t& dispatchs_count();
#else
/// \brief A counter for dispatch tables per v-table type.
template <typename AnyVTable>
std::size_t& dispatchs_count() {
  static std::size_t count = 0;
  return count;
}
#endif

/// \brief A tag to indicate that the corresponding function
/// parameter is an
/// \ref any used for dispatch.
///
/// \tparam Any The \ref any used for dispatch
template <is_any Any>
  requires(Any::is_dyn && is_open_dispatch_v_table<typename Any::v_table_t>)
struct virtual_ {
  using type = Any;
};

template <typename Arg>
struct translate_erased_function_param {
  using type = Arg;
};
template <is_any Any>
struct translate_erased_function_param<virtual_<Any>> {
  using type = typename Any::void_t;
};

template <typename RET, typename... Args>
struct translate_erased_function {
  using type = RET (*)(typename translate_erased_function_param<Args>::type...);
};

template <std::size_t I, typename First, typename... Args>
auto arg_n(First first, Args... args) {
  if constexpr (I == 0) {
    return first;
  } else {
    return arg_n<I - 1>(std::forward<Args>(args)...);
  }
}

template <std::size_t COUNT, typename... Args>
constexpr std::size_t dispatch_dimension_count = COUNT;
template <std::size_t COUNT, is_any Any, typename... Args>
constexpr std::size_t dispatch_dimension_count<COUNT, virtual_<Any>, Args...> =
    dispatch_dimension_count<COUNT + 1, Args...>;

template <typename R, typename... Classes>
struct ensure_function_ptr_from_functor_t {
  template <typename FUNCTOR, typename... Args>
  struct striped_virtuals {
    static R function(Classes&... classes, Args... args) {
      return FUNCTOR{}(classes..., args...);
    };
  };
  template <typename FUNCTOR, is_any Any, typename... Args>
  struct striped_virtuals<FUNCTOR, virtual_<Any>, Args...>
      : striped_virtuals<FUNCTOR, Args...> {};

  template <typename... Args>
  static auto instance(auto functor)  // if functor is a templated operator()
                                      // from a stateless function object,
                                      // instantiate it now!;
  {
    using functor_t = decltype(functor);
    if constexpr (std::is_pointer_v<functor_t>) {
      return functor;
    } else {
      return striped_virtuals<functor_t, Args...>::function;
    }
  }
};

template <typename... DispatchArgs>
struct args_to_tuple {
  template <typename T, typename... ActualArgs>
  auto operator()(T&& dispatch_args, ActualArgs&&... actual_args) {
    return std::tuple_cat(
        std::forward<T>(dispatch_args),
        std::make_tuple(std::forward<ActualArgs>(actual_args)...));
  }
};
template <is_any Any, typename... DispatchArgs>
struct args_to_tuple<virtual_<Any>, DispatchArgs...> {
  template <typename T, typename ACTUAL_ARG, typename... ActualArgs>
  auto operator()(T&& dispatch_args, ACTUAL_ARG&& dispatch_arg,
                  ActualArgs&&... actual_args) {
    return args_to_tuple<DispatchArgs...>{}(
        std::tuple_cat(std::forward<T>(dispatch_args),
                       std::make_tuple(get_proxy_ptr(dispatch_arg))),
        std::forward<ActualArgs>(actual_args)...);
  }
};

template <std::size_t FirstN, typename... Ts>
auto get_tuple_head(std::tuple<Ts...> from) {
  return [&]<std::size_t... I>(std::index_sequence<I...>) {
    return std::make_tuple(std::get<I>(from)...);
  }(std::make_index_sequence<FirstN>{});
}
template <std::size_t FromN, typename... Ts>
auto get_tuple_tail(std::tuple<Ts...> from) {
  return [&]<std::size_t... I>(std::index_sequence<I...>) {
    return std::make_tuple(std::get<I>(from)...);
  }(std::make_index_sequence<std::tuple_size_v<std::tuple<Ts...>> - FromN>{});
}

template <typename R, typename... FArgs>
struct dispatch_function;
template <typename R, typename... FArgs>
struct dispatch_function<R, std::tuple<FArgs...>> {
  using type = R (*)(FArgs...);
};

class no_default_function_error : public error {
  using error::error;
};

template <typename R, typename... OuterArgs>
struct dispatch_function_types {
  template <is_any... Anys>
  struct inner {
    template <typename... Args>
    struct implemenation {
      struct type {
        using function_t = hook<R(Anys const&..., Args...)>;
        static auto function() {
          return []([[maybe_unused]] auto super,
                    [[maybe_unused]] Anys const&... anys,
                    [[maybe_unused]] Args... args) -> R {
            if constexpr (std::same_as<R, void>) {
              return;
            } else {
              if constexpr (std::is_default_constructible_v<R>) {
                return R{};
              } else {
                throw no_default_function_error("no default function");
              }
            }
          };
        }
      };
    };
    template <is_any Any, typename... Args>
    struct implemenation<virtual_<Any>, Args...> : implemenation<Args...> {};
    template <typename... Args>
    using type = typename implemenation<Args...>::type;
  };

  template <typename... Args>
  struct outer {
    template <is_any... Anys>
    using type = inner<Anys...>::template type<Args...>;
  };
  template <is_any Any, typename... Args>
  struct outer<virtual_<Any>, Args...> {
    template <is_any... Anys>
    using type = outer<Args...>::template type<Any, Anys...>;
  };

  using default_type = outer<OuterArgs...>::template type<>;
};

template <typename F, typename... Args>
struct dispatch_matrix {
  using type = F;
};
template <typename DispatchMatrix, is_any Any, typename... Args>
struct dispatch_matrix<DispatchMatrix, virtual_<Any>, Args...> {
  using type =
      typename dispatch_matrix<std::vector<DispatchMatrix>, Args...>::type;
};

/// \brief Open dispatch method. Solves the expression problem. See
/// \ref dispatch_sig "dispatch<R(Args...)>" for details.
///
/// \tparam R Return type.
/// \tparam ...Args Parameter types. The parameters for open dispatch
/// must be the first and braced as \ref virtual_.
template <typename R, typename... Args>
class dispatch;
/// \anchor dispatch_sig
/// \brief Open dispatch method. Solves the expression problem.
/// \tparam R The return type.
/// \tparam Args The parameter types. The signature of the dispatch
/// is R(Args...).
///
/// The open dispatch is managed via a singleton object of this
/// class.
///
/// To declare and define a singleton, use the \ref ANY_SINGLETON and
/// \ref ANY_SINGLETON_DECLARE macros.
///
/// Each trait of the \ref any tagged as virtual must have open
/// dispatch enabled. To enable open dispatch, declare a struct named
/// 'trait_name'_has_open_dispatch.
/// \code
/// struct node_has_open_dispatch {};
/// ANY(node<>, , )
/// // open dispatch evaluate, returns int, dispatched via an
/// any_node; dispatch<int(virtual_<any_node<>>)> evaluate;
/// \endcode
/// Examples are listed here: \ref dispatch
template <typename R, typename... Args>
class dispatch<R(Args...)> {
 public:
  using erased_function_t =
      typename translate_erased_function<R, Args...>::type;

  static constexpr std::size_t dimension_count =
      dispatch_dimension_count<0, Args...>;

  using dispatch_matrix_t = dispatch_matrix<erased_function_t, Args...>::type;
  dispatch_matrix_t dispatch_matrix_;

  using default_ = typename dispatch_function_types<R, Args...>::default_type;
  default_::function_t dispatch_default_hook_;
  default_::function_t::connection default_connection_ =
      dispatch_default_hook_.insert(default_::function());

  enum class kind { single, multiple };
  template <kind Kind, std::size_t Dimension, typename... DispatchArgs>
  struct dispatch_access;

  template <typename... DispatchArgs>
  struct dispatch_access<kind::multiple, dimension_count, DispatchArgs...> {
    auto define(auto fp, auto& matrix) {
      matrix = reinterpret_cast<erased_function_t>(fp);
      return fp;
    }
    template <typename F, typename ArgsTuple>
    std::optional<R> invoke(F const& target, ArgsTuple&& dispatch_args_tuple,
                            auto&&...) const {
      if (!target) return {};
      auto typed_target = reinterpret_cast<
          typename dispatch_function<R, std::decay_t<ArgsTuple>>::type>(target);
      return std::apply(typed_target,
                        std::forward<ArgsTuple>(dispatch_args_tuple));
    }
  };

  template <std::size_t Dimension, is_any Any, typename... DispatchArgs>
  struct dispatch_access<kind::multiple, Dimension, virtual_<Any>,
                         DispatchArgs...>
      : dispatch_access<kind::multiple, Dimension + 1, DispatchArgs...> {
    using interface_t = Any;
    using v_table_t = typename interface_t::v_table_t;
    using next_t =
        dispatch_access<kind::multiple, Dimension + 1, DispatchArgs...>;

    // index 0 is for the 'wildcard' functions
    std::size_t index_ = 1 + dispatchs_count<v_table_t>()++;
    std::size_t dispatch_dimension_size_ = 1;

    template <typename Class>
    std::size_t get_dispatch_index() {
      if constexpr (std::same_as<Any, Class>) {
        return 0;
      } else {
        auto dispatch_table = dispatch_table_instance<v_table_t, Class>();
        if (auto index = get_multi_dispatch_index_at(dispatch_table, index_))
          return index;
        else
          set_multi_dispatch_index_at(dispatch_table, index_,
                                      dispatch_dimension_size_);
        return dispatch_dimension_size_++;
      }
    }

    template <typename Class, typename... Classes>
    auto define(auto fp, auto& matrix) {
      auto dispatch_index = get_dispatch_index<Class>();
      if (matrix.size() <= dispatch_index) matrix.resize(dispatch_index + 1);
      return next_t::template define<Classes...>(fp, matrix[dispatch_index]);
    }

    template <typename DispatchMatrix, typename ArgsTuple,
              typename... ActualArgs>
    std::optional<R> invoke(DispatchMatrix const& target,
                            ArgsTuple&& dispatch_args_tuple, Any const& any,
                            ActualArgs&&... actual_args) const {
      auto dispatch_table = get_v_table(any)->dispatch_table;
      auto dispatch_dim = get_multi_dispatch_index_at(dispatch_table, index_);
      if (dispatch_dim && target.size() > dispatch_dim)
        if (auto found =
                next_t::invoke(target[dispatch_dim],
                               std::forward<ArgsTuple>(dispatch_args_tuple),
                               std::forward<ActualArgs>(actual_args)...))
          return found;

      if (target.size())
        return next_t::invoke(
            target[0],
            std::tuple_cat(get_tuple_head<Dimension>(dispatch_args_tuple),
                           std::make_tuple(&any),
                           get_tuple_tail<Dimension + 1>(dispatch_args_tuple)),
            std::forward<ActualArgs>(actual_args)...);
      return {};
    }
  };

  template <is_any Any, typename... AccessArgs>
  struct dispatch_access<kind::single, 0, virtual_<Any>, AccessArgs...> {
    using interface_t = Any;
    using v_table_t = typename interface_t::v_table_t;
    std::size_t index_ = dispatchs_count<v_table_t>()++;

    template <typename CLASS>
    auto define(auto fp, auto&) {
      auto v_table = dispatch_table_instance<v_table_t, CLASS>();
      insert_function(v_table, index_, fp);
      return fp;
    }

    template <typename... Other>
    R invoke(default_::function_t const& default_, Any const& any,
             Other&&... other) const {
      auto v_table = get_v_table(any)->dispatch_table;
      auto target = get_function(v_table, index_);
      if (!target)
        return std::invoke(default_, any, std::forward<Other>(other)...);
      auto erased_function = reinterpret_cast<erased_function_t>(target);
      return std::invoke(erased_function, get_proxy_ptr(any),
                         std::forward<Other>(other)...);
    }
  };

  static const constexpr kind dispatch_kind =
      (dimension_count > 1) ? kind::multiple : kind::single;
  dispatch_access<dispatch_kind, 0, Args...> dispatch_access_;

 public:
  /// \brief Register a dispatch target for the model provided in the
  /// type parameter.
  /// \tparam Classes The models for which the dispatch target is
  /// registered.
  /// \param f The dispatch target.
  /// \return Dummy value, useful for initializing an auto unnamed
  /// variable at (static) namespace scope.
  template <typename... Classes>
  auto define(auto f) {
    auto fp = ensure_function_ptr_from_functor_t<
        R, Classes...>::template instance<Args...>(f);
    return dispatch_access_.template define<Classes...>(fp, dispatch_matrix_);
  };
  /// \brief Invoke the dispatch.
  /// \tparam ActualArgs The deduced argument types for the dispatch
  /// call.
  /// \param actual_args Arguments fulfilling the signature of this
  /// dispatch.
  /// \return The result of the dispatched function call.
  ///
  /// The called function is determined by the \ref any objects
  /// marked with \ref virtual_ in the class instantiation.
  template <typename... ActualArgs>
  auto operator()(ActualArgs&&... actual_args) const {
    if constexpr (dispatch_kind == kind::multiple) {
      auto dispatch_args_tuple = args_to_tuple<Args...>{}(
          std::tuple<>{}, std::forward<ActualArgs>(actual_args)...);
      return *dispatch_access_
                  .invoke(dispatch_matrix_, dispatch_args_tuple,
                          std::forward<ActualArgs>(actual_args)...)
                  .or_else([&]() -> std::optional<R> {
                    return std::invoke(
                        dispatch_default_hook_,
                        std::forward<ActualArgs>(actual_args)...);
                  });
    } else {
      return dispatch_access_.invoke(dispatch_default_hook_,
                                     std::forward<ActualArgs>(actual_args)...);
    }
  }
  auto& get_dispatch_default_hook() { return dispatch_default_hook_; };
};

template <size_t At, typename Tuple, size_t... Is>
auto make_tuple_from_elements(Tuple&& tuple, std::index_sequence<Is...>) {
  return std::forward_as_tuple(
      std::get<Is + At>(std::forward<Tuple>(tuple))...);
}
template <size_t At, size_t N, typename Tuple>
auto make_tuple_from_elements_at(Tuple&& tuple) {
  return make_tuple_from_elements<At>(std::forward<Tuple>(tuple),
                                      std::make_index_sequence<N>{});
}

}  // namespace anyxx

#ifdef ANY_DLL_MODE

#define ANY_DISPATCH_COUNT_FWD(export_, ns_, any_)             \
  namespace ns_ {}                                             \
  namespace anyxx {                                            \
  template <>                                                  \
  export_ std::size_t& dispatchs_count<ns_::any_##_v_table>(); \
  }

#define ANY_DISPATCH_COUNT(ns_, any_)                          \
  template <>                                                  \
  std::size_t& anyxx::dispatchs_count<ns_::any_##_v_table>() { \
    static std::size_t count = 0;                              \
    return count;                                              \
  }

#define ANY_DISPATCH_FOR_FWD(export_, class_, interface_namespace_, \
                             interface_)                            \
  namespace anyxx {                                                 \
  template <>                                                       \
  export_ dispatch_table_t* dispatch_table_instance<                \
      interface_namespace_::interface_##_v_table, class_>();        \
  }

#define ANY_DISPATCH_FOR(class_, interface_namespace_, interface_) \
  template <>                                                      \
  anyxx::dispatch_table_t* anyxx::dispatch_table_instance<         \
      interface_namespace_::interface_##_v_table, class_>() {      \
    return dispatch_table_instance_implementation<                 \
        interface_namespace_::interface_##_v_table, class_>();     \
  }

#else

/// \addtogroup dispatch_macros DISPATCH_ macros
/// \brief Macros to define static runtime data for open dispatch.
/// Only neccessary for DLL scenarios.
///
/// Name conventions:
/// _FWD: macro to declare the function signature only. To be used in
/// header files.
///
/// ANY_DISPATCH_COUNT macros declare/define the dispatch counter for
/// a specific any. This dispatch counter is used to assign unique
/// indices to each dispatch.
///
/// ANY_DISPATCH_FOR macros declare/define the dispatch table
/// instance for a any. This is necessary once for each model class
/// that participates in open dispatch.
///
///  @{

/// \def ANY_DISPATCH_COUNT_FWD
/// \brief Declare access to the dispatch counter for a specific \ref
/// any. Must be placed in global namespace.
/// \param export_ To supply an export macro in a DLL scenario.
/// \param ns_ Namespace of the \ref any.
/// \param any_ Name of the \ref any (without any_ prefix).
#define ANY_DISPATCH_COUNT_FWD(...)
/// \def ANY_DISPATCH_COUNT
/// \brief Define the dispatch counter for a specific \ref any. Must
/// be placed in global namespace.
/// \param ns_ Namespace of the \ref any.
/// \param any_ Name of the \ref any (without any_ prefix).
#define ANY_DISPATCH_COUNT(...)
/// \def ANY_DISPATCH_FOR_FWD
/// \brief Declare access to the dispatch table instance function for
/// a model. Must be placed in global namespace.
/// \param export_ To supply an export macro in a DLL scenario.
/// \param class_ The model class with fully qualified name.
/// \param interface_namespace_ Namespace of the \ref any.
/// \param interface_ Name of the \ref any (without any_ prefix).
#define ANY_DISPATCH_FOR_FWD(...)
/// \def ANY_DISPATCH_FOR
/// \brief Define the dispatch table instance function for a model.
/// Must be placed in global namespace
/// \param class_ The model class with fully qualified name.
/// \param interface_namespace_ Namespace of the \ref any.
/// \param interface_ Name of the \ref any (without any_ prefix).
#define ANY_DISPATCH_FOR(...)

///  @}

#endif
