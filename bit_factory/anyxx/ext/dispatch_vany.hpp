#pragma once

#include <bit_factory/anyxx/ext/dispatch.hpp>
#include <bit_factory/anyxx/ext/vany.hpp>

namespace anyxx {

template <typename Vany, typename DynamicDispatch, auto StaticDispatch>
class dispatch_vany {
  DynamicDispatch dynamic_dispatch_;
  constexpr static const std::size_t dimension_count =
      DynamicDispatch::dimension_count;

  template <typename Vany1, typename... Args>
  auto invoke1(Vany1&& vany, Args&&... args) const {
    return std::visit(
        [&]<typename TypedArg>([[maybe_unused]] TypedArg&& arg) {
          return overloads{
              StaticDispatch,
              [&]<is_any Any, typename... Vargs>(Any&& any_, Vargs&&... vargs) {
                return dynamic_dispatch_(std::forward<Any>(any_),
                                         std::forward<Vargs>(vargs)...);
              }}(std::forward<TypedArg>(arg), std::forward<Args>(args)...);
        },
        get_proxy_value(std::forward<Vany1>(vany)));
  }

  template <is_any Vany1, is_any Vany2, typename... Args>
  auto invoke2(Vany1&& vany1, Vany2&& vany2, Args&&... args) const {
    using vany1_t = std::decay_t<Vany1>;
    using vany2_t = std::decay_t<Vany2>;
    using cv1_t = anyxx::vany_type_trait<vany1_t>::concrete_variant;
    using any_v1 = anyxx::vany_type_trait<vany1_t>::any_in_variant;
    using cv2_t = anyxx::vany_type_trait<vany2_t>::concrete_variant;
    using any_v2 = anyxx::vany_type_trait<vany2_t>::any_in_variant;

    auto dispatch_combined = [&]<typename DA1, typename DA2>(DA1&& da1,
                                                             DA2&& da2) {
      auto dyn_case1 = [&]<is_any A1, is_any A2, typename... VAs>(
                           A1&& a1, A2&& a2, VAs&&... vas) {
        return dynamic_dispatch_(std::forward<A1>(a1), std::forward<A2>(a2),
                                 std::forward<VAs>(vas)...);
      };
      auto dyn_case2 = [&]<is_any A1, typename A2, typename... VAs>(
                           A1&& a1, A2 a2, VAs&&... vas)
        requires std::constructible_from<cv2_t, A2>
      {
        return dynamic_dispatch_(std::forward<A1>(a1),
                                 any_v2{std::in_place, cv2_t{std::move(a2)}},
                                 std::forward<VAs>(vas)...);
      };
      auto dyn_case3 = [&]<typename A1, is_any A2, typename... VAs>(
                           A1 a1, A2&& a2, VAs&&... vas)
        requires std::constructible_from<cv1_t, A1>
      {
        return dynamic_dispatch_(any_v1{std::in_place, cv1_t{std::move(a1)}},
                                 std::forward<A2>(a2),
                                 std::forward<VAs>(vas)...);
      };
      auto dyn_case4 = [&]<typename A1, typename A2, typename... VAs>(
                           A1 a1, A2 a2, VAs&&... vas)
        requires(std::constructible_from<cv1_t, A1> &&
                 std::constructible_from<cv2_t, A2>)
      {
        return dynamic_dispatch_(any_v1{std::in_place, cv1_t{std::move(a1)}},
                                 any_v2{std::in_place, cv2_t{std::move(a2)}},
                                 std::forward<VAs>(vas)...);
      };
      return overloads{StaticDispatch, dyn_case1, dyn_case2, dyn_case3,
                       dyn_case4}(std::forward<DA1>(da1),
                                  std::forward<DA2>(da2),
                                  std::forward<Args>(args)...);
    };

    return std::visit(dispatch_combined,
                      get_proxy_value(std::forward<Vany1>(vany1)),
                      get_proxy_value(std::forward<Vany2>(vany2)));
  }

 public:
  template <typename... Classes>
  auto define(auto f) {
    return dynamic_dispatch_.template define<Classes...>(f);
  }

  template <typename... Args>
  auto operator()(Args&&... args) const {
    if constexpr (dimension_count == 1) {
      return invoke1(std::forward<Args>(args)...);
    } else {
      if constexpr (dimension_count == 2) {
        return invoke2(std::forward<Args>(args)...);
      } else {
        static_assert(dimension_count <= 2,
                      "dispatch_vany only supports one and two dimensions");
      }
    }
  }
};

}  // namespace anyxx

/// \addtogroup vany_macros VANY_DISPACH macros
/// \brief Macros to reduce boilerplate for declaring/defining open
/// dispatch with \ref vany_variant. Only neccessary for DLL
/// scenarios
///  @{

/// \brief Declare a singleton object for open \ref vany dispatch.
/// \param export_ To supply an export macro in a DLL scenario.
/// \param name Name of the dispatch.
/// \param vany Type of \ref vany.
/// \param signature Signature of the \ref dispatch.
/// \param static_dispatch Static constexpr visitor for the non-any_
/// members of
///        the \ref vany.
///
/// See also \ref VANY_DISPACH, \ref ANY_SINGLETON_DECLARE.
#define VANY_DISPACH_DECLARE(export_, name, vany, signature, static_dispatch) \
  constexpr static inline auto name##_static_dispatch =                       \
      anyxx::overloads{_detail_REMOVE_PARENS(static_dispatch)};               \
                                                                              \
  using name##_vany = vany;                                                   \
  using name##_dynamic_dispatch =                                             \
      anyxx::dispatch<_detail_REMOVE_PARENS(signature)>;                      \
                                                                              \
  ANY_SINGLETON_DECLARE(                                                      \
      , name,                                                                 \
      anyxx::dispatch_vany<name##_vany, name##_dynamic_dispatch,              \
                           name##_static_dispatch>)

/// \brief Define a \ref vany dispatch singleton object in a source
/// file.
/// \param namespace_ The namespace for the singleton.
/// \param name Name of the singleton.
///
/// See also \ref VANY_DISPACH_DECLARE, \ref ANY_SINGLETON.
#define VANY_DISPACH(namespace_, name) ANY_SINGLETON(namespace_, name);
///  @}


