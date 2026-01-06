#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx_std.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

#ifdef __cpp_lib_ranges_fold

// namespace anyxx {
//
// template <typename A>
// concept is_any_self_forward_range =
//     is_any<A> && std::ranges::forward_range<A> &&
//     std::same_as<std::ranges::range_value_t<A>, self>;
////
// template <typename Any, typename AnyForwardRange>
//   requires is_any_self_forward_range<AnyForwardRange>
// struct translate_jacket_param<Any, AnyForwardRange const&> {
//   using type =
//       anyxx::any_forward_range<typename Any::any_value_t,
//                                typename Any::any_value_t,
//                                typename AnyForwardRange::erased_data_t,
//                                typename AnyForwardRange::dispatch_t> const&;
// };
////
// template <typename AnyConstObserver, typename AnyMutableObserver,
//           typename AnyValue, typename AnyForwardRange>
//   requires is_any_self_forward_range<AnyForwardRange>
// struct translate_v_table_param<AnyConstObserver, AnyMutableObserver,
// AnyValue,
//                                typename AnyForwardRange const&> {
//   using type =
//       anyxx::any_forward_range<AnyValue, AnyValue,
//                                typename AnyForwardRange::erased_data_t,
//                                typename AnyForwardRange::dispatch_t> const&;
// };
////
// template <typename Concrete, typename AnyForwardRange>
//   requires is_any_self_forward_range<AnyForwardRange>
// struct v_table_to_map<Concrete, AnyForwardRange const&> {
//   static Concrete const& forward(auto const& any_range) {
//     return std::views::transform(
//         any_range, [](auto const& any) { *unerase_cast<Concrete>(any); });
//   }
// };
////
// template <typename T, typename AnyForwardRange>
//   requires is_any_self_forward_range<AnyForwardRange>
// struct translate_map_param<T, AnyForwardRange const&> {
//   using type =
//       anyxx::any_forward_range<T, T, typename AnyForwardRange::erased_data_t,
//                                typename AnyForwardRange::dispatch_t> const&;
// };
////
// template <typename Traited, typename AnyForwardRange>
//   requires is_any_self_forward_range<AnyForwardRange>
// struct forward_trait_to_map<Traited, AnyForwardRange const&> {
//   template <typename Sig>
//   static decltype(auto) forward(Sig&& sig) {
//     return std::forward<Sig>(sig);
//     // return std::views::transform(any_range, [](auto const& any) {
//     //          *unerase_cast<Traited>(any);
//     //        });
//   }
// };
////
//}  // namespace anyxx
//
//// ANY_OP_DEFAULTED(anyxx::self, |, concat,
////                  (std::vector<monoid_trait<T>> const&), const,
////                  [&x](const auto& r) {
//
////(anyxx::any_forward_range<anyxx::self, anyxx::self,
////                          anyxx::value> const&),
//// const,
//
// namespace example_2b {
//
// template <typename ErasedData = anyxx::default_erased_data<>::type,
//          typename Dispatch = anyxx::default_member_dispatch<>::type>
// struct monoid;
// template <typename T>
// using monoid_trait = monoid<anyxx::traited<T>, anyxx::trait>;
// template <typename T>
// struct monoid_default_model_map {
//  static auto concat([[maybe_unused]] T const& x,
//                     [[maybe_unused]] anyxx::map_param<
//                         T, anyxx::any_forward_range<anyxx::self, anyxx::self,
//                                                     anyxx::value> const&>
//                         _sig) -> anyxx::map_return<T, anyxx::self> {
//    return [&x](const auto& r) {
//      return anyxx::trait_as<monoid>(x);
//    }(std::forward<decltype(_sig)>(_sig));
//  };
//};
// template <typename T>
// struct monoid_model_map : monoid_default_model_map<T> {};
// template <typename T>
//  requires(anyxx::is_variant<T>)
// struct monoid_model_map<T> {
//  template <typename V>
//  using x_model_map = monoid_model_map<V>;
//  static auto concat([[maybe_unused]] T const& x,
//                     [[maybe_unused]] anyxx::map_param<
//                         T, anyxx::any_forward_range<anyxx::self, anyxx::self,
//                                                     anyxx::value> const&>
//                         _sig) -> anyxx::map_param<T, anyxx::self> {
//    return std::visit(
//        [&]<typename V>(V&& v) {
//          return x_model_map<std::decay_t<V>>::concat(
//              std::forward<V>(v), std::forward<decltype(_sig)>(_sig));
//        },
//        x);
//  };
//};
// struct monoid_v_table_as_static_inline;
// struct monoid_has_open_dispatch;
// template <typename Dispatch>
// struct monoid_v_table;
// template <typename Concrete, typename Dispatch>
// monoid_v_table<Dispatch>* make_monoid_v_table();
// template <typename Dispatch>
// struct monoid_v_table
//    : anyxx::derive_v_table_from<Dispatch>::template type<Dispatch>,
//      anyxx::dispatch_holder<anyxx::is_type_complete<monoid_has_open_dispatch>,
//                             monoid> {
//  using v_table_base_t =
//      typename anyxx::derive_v_table_from<Dispatch>::template type<Dispatch>;
//  using v_table_t = monoid_v_table;
//  static constexpr bool open_dispatch_enabeled =
//      anyxx::is_type_complete<monoid_has_open_dispatch>;
//  using own_dispatch_holder_t =
//      typename anyxx::dispatch_holder<open_dispatch_enabeled, monoid>;
//  using dispatch_t = Dispatch;
//  using any_value_t = monoid<anyxx::value, anyxx::rtti>;
//  using any_const_observer_t = monoid<anyxx::const_observer, anyxx::rtti>;
//  using any_mutable_observer_t = monoid<anyxx::mutable_observer, anyxx::rtti>;
//  static bool static_is_derived_from(const std::type_info& from) {
//    return typeid(v_table_t) == from
//               ? true
//               : v_table_base_t::static_is_derived_from(from);
//  }
//  anyxx::v_table_return<any_value_t, anyxx::self> (*concat)(
//      void const*,
//      [[maybe_unused]] anyxx::v_table_param<
//          any_const_observer_t, any_mutable_observer_t, any_value_t,
//          anyxx::any_forward_range<anyxx::self, anyxx::self,
//                                   anyxx::value> const&>
//          _sig);
//  ;
//  ;
//  monoid_v_table() = default;
//  template <typename Concrete>
//  explicit(false) monoid_v_table(std::in_place_type_t<Concrete> concrete);
//  template <typename Concrete>
//  static auto imlpementation() {
//    if constexpr (std::same_as<anyxx::dyns, Dispatch> ||
//                  !anyxx::is_in_dll_mode) {
//      return anyxx::v_table_instance_inline<v_table_t, Concrete>();
//    } else {
//      return make_monoid_v_table<Concrete, anyxx::rtti>();
//    }
//  }
//};
// template <typename ErasedData, typename Dispatch>
// struct monoid
//    : anyxx::derive_from<Dispatch, monoid_v_table<Dispatch>>::template type<
//          ErasedData, Dispatch> {
//  template <typename... Args>
//  using any_template = monoid<Args...>;
//  using any_t = monoid;
//  using any_const_observer_t = monoid<anyxx::const_observer, anyxx::rtti>;
//  using any_mutable_observer_t = monoid<anyxx::mutable_observer, anyxx::rtti>;
//  using erased_data_t = ErasedData;
//  using dispatch_t = Dispatch;
//  using T = typename
//  anyxx::erased_data_trait<erased_data_t>::static_dispatch_t; using base_t =
//  typename anyxx::derive_from<
//      Dispatch, monoid_v_table<Dispatch>>::template type<ErasedData,
//      Dispatch>;
//  using v_table_base_t = base_t::v_table_t;
//  using v_table_t = monoid_v_table<Dispatch>;
//  using dispatch_t = Dispatch;
//  using any_value_t = monoid<anyxx::value, anyxx::rtti>;
//  using any_const_observer_t = monoid<anyxx::const_observer, anyxx::rtti>;
//  using any_mutable_observer_t = monoid<anyxx::mutable_observer, anyxx::rtti>;
//  template <typename StaticDispatchType>
//  using static_dispatch_map_t = monoid_model_map<StaticDispatchType>;
//  template <typename Concrete>
//  static auto v_table_imlpementation() {
//    static_assert(!anyxx::is_any<Concrete>);
//    return v_table_t::template imlpementation<Concrete>();
//  }
//  using base_t::erased_data_;
//  using base_t::get_v_table_ptr;
//  monoid(erased_data_t erased_data, v_table_t* v_table)
//    requires(!std::same_as<Dispatch, anyxx::dynm>)
//      : base_t(std::move(erased_data), v_table) {}
//  template <typename ConstructedWith>
//  explicit(false) monoid(ConstructedWith&& v)
//    requires anyxx::constructibile_for<ConstructedWith, ErasedData>
//      : base_t(std::forward<ConstructedWith>(v)) {
//    base_t::template init_v_table<ConstructedWith>();
//  }
//  template <typename V>
//  monoid(std::in_place_t, V&& v) : base_t(std::in_place, std::forward<V>(v)) {
//    base_t::template init_v_table<V>();
//  }
//  template <typename T, typename... Args>
//  explicit(false) monoid(std::in_place_type_t<T>, Args&&... args)
//      : base_t(std::in_place_type<T>, std::forward<Args>(args)...) {
//    base_t::template init_v_table<T>();
//  }
//  template <typename Other>
//  explicit(false) monoid(const Other& other)
//    requires(
//        std::derived_from<typename Other::v_table_t, v_table_t> &&
//        anyxx::borrowable_from<erased_data_t, typename Other::erased_data_t>)
//      : base_t(other) {}
//  template <anyxx::is_any Other>
//  explicit(false) monoid(Other&& other) noexcept
//    requires(std::derived_from<typename Other::v_table_t, v_table_t> &&
//             anyxx::moveable_from<erased_data_t, typename
//             Other::erased_data_t>)
//      : base_t(std::forward<Other>(other)) {}
//  anyxx::jacket_param<any_t, anyxx::self> operator|(
//      [[maybe_unused]] anyxx::jacket_param<
//          any_t, anyxx::any_forward_range<anyxx::self, anyxx::self,
//                                          anyxx::value> const&>
//          _sig) const
//    requires(::anyxx::const_correct_call_for_erased_data<void const*,
//                                                         erased_data_t,
//                                                         false>)
//  {
//    if constexpr (std::same_as<anyxx::vany_dispatch, Dispatch>) {
//      using variant_t = erased_data_t;
//      using vany_any_t = std::variant_alternative_t<0, variant_t>;
//      return std::visit(
//          anyxx::overloads{
//              [&]<typename ValueInVariant>(ValueInVariant&& value_in_variant)
//              {
//                return static_dispatch_map_t<std::decay_t<ValueInVariant>>::
//                    concat(std::forward<ValueInVariant>(value_in_variant),
//                           std::forward<decltype(_sig)>(_sig));
//              },
//              [&](vany_any_t const& any) {
//                return any.concat(std::forward<decltype(_sig)>(_sig));
//              }},
//          base_t::erased_data_);
//    } else {
//      if constexpr (std::same_as<anyxx::trait, Dispatch>) {
//        using traited_t = typename erased_data_t::value_t;
//        if constexpr (anyxx::is_variant<traited_t>) {
//          return std::visit(
//              [&]<typename Vx>(Vx&& vx) {
//                return static_dispatch_map_t<std::decay_t<Vx>>::concat(
//                    std::forward<Vx>(vx), std::forward<decltype(_sig)>(_sig));
//              },
//              base_t::erased_data_.value_);
//        } else {
//          if constexpr (std::same_as<void, anyxx::self>) {
//            static_dispatch_map_t<T>::concat(
//                base_t::erased_data_.value_,
//                anyxx::forward_trait_to_map<
//                    traited_t,
//                    anyxx::any_forward_range<anyxx::self, anyxx::self,
//                                             anyxx::value> const&>::
//                    template forward<decltype(_sig)>(
//                        std::forward<decltype(_sig)>(_sig)));
//          } else {
//            return anyxx::jacket_return<anyxx::self>::forward(
//                static_dispatch_map_t<T>::concat(
//                    base_t::erased_data_.value_,
//                    anyxx::forward_trait_to_map<
//                        traited_t,
//                        anyxx::any_forward_range<anyxx::self, anyxx::self,
//                                                 anyxx::value> const&>::
//                        template forward<decltype(_sig)>(
//                            std::forward<decltype(_sig)>(_sig))),
//                *this);
//          }
//        }
//      } else {
//        if constexpr (std::same_as<void, anyxx::self>) {
//          get_v_table_ptr()->concat(
//              anyxx::get_void_data_ptr(base_t::erased_data_),
//              std::forward<decltype(_sig)>(_sig));
//        } else {
//          return anyxx::jacket_return<anyxx::self>::forward(
//              get_v_table_ptr()->concat(
//                  anyxx::get_void_data_ptr(base_t::erased_data_),
//                  std::forward<decltype(_sig)>(_sig)),
//              *this);
//        }
//      }
//    }
//  }
//  ~monoid() = default;
//  monoid() = default;
//  monoid(monoid const&) = default;
//  monoid(monoid&&) = default;
//  monoid& operator=(monoid const&) = default;
//  monoid& operator=(monoid&&) = default;
//  template <anyxx::is_erased_data Other, typename OtherDispatch>
//  friend class anyxx::any_base;
//  template <anyxx::is_any To, anyxx::is_any From>
//  friend To anyxx::unchecked_downcast_to(From from)
//    requires(std::derived_from<To, From>);
//  template <anyxx::is_erased_data Other>
//  using type_for = monoid<Other>;
//};
// template <typename Dispatch>
// template <typename Concrete>
// monoid_v_table<Dispatch>::monoid_v_table(
//    std::in_place_type_t<Concrete> concrete)
//    : v_table_base_t(concrete) {
//  using concept_map = monoid_model_map<Concrete>;
//  concat = [](void const* _vp,
//              [[maybe_unused]] anyxx::v_table_param<
//                  any_const_observer_t, any_mutable_observer_t, any_value_t,
//                  anyxx::any_forward_range<anyxx::self, anyxx::self,
//                                           anyxx::value> const&>
//                  _sig) -> anyxx::v_table_return<any_value_t, anyxx::self> {
//    if constexpr (std::same_as<anyxx::self&, anyxx::self>) {
//      concept_map{}.concat(
//          *anyxx::unchecked_unerase_cast<Concrete>(_vp),
//          anyxx::v_table_to_map<
//              Concrete, anyxx::any_forward_range<anyxx::self, anyxx::self,
//                                                 anyxx::value> const&>::
//              template forward<decltype(_sig)>(
//                  std::forward<decltype(_sig)>(_sig)));
//      return anyxx::handle_self_ref_return<anyxx::self>{}();
//    } else {
//      return concept_map{}.concat(
//          *anyxx::unchecked_unerase_cast<Concrete>(_vp),
//          anyxx::v_table_to_map<
//              Concrete, anyxx::any_forward_range<anyxx::self, anyxx::self,
//                                                 anyxx::value> const&>::
//              template forward<decltype(_sig)>(
//                  std::forward<decltype(_sig)>(_sig)));
//    }
//  };
//  ;
//  ;
//  if constexpr (open_dispatch_enabeled) {
//    own_dispatch_holder_t::set_dispatch_table(
//        ::anyxx::dispatch_table_instance<monoid_v_table, Concrete>());
//  }
//  ::anyxx::set_is_derived_from<Dispatch, v_table_t>(this);
//};
//
//// TRAIT(monoid,
////       (ANY_OP_DEFAULTED(anyxx::self, +, op, (anyxx::self const&), const,
////                         [&x](auto const& r) {
////                           auto self = anyxx::trait_as<monoid>(x);
////                           return self |
///(std::vector{anyxx::trait_as<monoid>( / r)});  // NOLINT / }), /
///ANY_OP_DEFAULTED(anyxx::self, |, concat, / (std::vector<monoid_trait<T>>
///const&), const, /                         [&x](const auto& r) { / auto self =
///anyxx::trait_as<monoid>(x); /                           return
///std::ranges::fold_right( /                               r, self, [&](auto
///const& m1, auto const& m2) { /                                 return m1 +
///m2; /                               }); /                         }), /
///ANY_OP_DEFAULTED(bool, ==, equal, (anyxx::self const&), const, / ([&x](auto
///const& r) { return x == r; }))))
////
//}  // namespace example_2b
//
// ANY_MODEL_MAP((int), example_2b::monoid) {
//  static monoid_trait<int> concat(int self, auto const& r) {
//    return monoid_trait<int>{std::ranges::fold_right(
//        r, self, [&](int m1, int m2) { return m1 + m2; })};
//  };
//};
//
////ANY_MODEL_MAP((std::string), example_2b::monoid) {
////  static monoid_trait<std::string> op(std::string const& self,
////                                      std::string const& r) {
////    return self + r;
////  };
////};
//
// namespace example_2b {
//
// template <typename M, typename R>
// void test_monoid(monoid_trait<M> const& m, R r)
//  requires std::ranges::range<R> &&
//           std::same_as<typename R::value_type, monoid_trait<M>>
//{
//  //using type_1 = decltype(m + (monoid_trait<M>{}) + m);
//  //using type_2 = decltype(m + (m + (monoid_trait<M>{})));
//  //static_assert(std::same_as<type_1, type_2>);
//  //static_assert(std::same_as<type_1, monoid_trait<M>>);
//  //auto c1 = m + monoid_trait<M>{} + m == m + m + monoid_trait<M>{};
//  //CHECK(c1);
//  auto c2 = (m | r) ;
//      //==
//      //      std::ranges::fold_right(
//      //          r, m, [&](auto const& m1, [[maybe_unused]]auto const& m2) {
//      return m1; });// + m2; });
//  //CHECK(c2);
//}
//
//}  // namespace example_2b
////
// TEST_CASE("example 2b monoid ") {
//   using namespace example_2b;
//   using namespace std::string_literals;
//   using namespace anyxx;
//   test_monoid(trait_as<monoid>(1), std::vector<monoid_trait<int>>{{2}, {3}});
//   test_monoid(trait_as<monoid>("1"s),
//               std::vector<monoid_trait<std::string>>{{"2"s}, {"3"s}});
// }
//
#endif