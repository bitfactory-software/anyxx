#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx_std.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

#ifdef __cpp_lib_ranges_fold

//namespace anyxx {
//
//template <typename A>
//concept is_any_self_forward_range =
//    is_any<A> && std::ranges::forward_range<A> &&
//    std::same_as<std::ranges::range_value_t<A>, self>;
//
//template <typename Any, typename AnyForwardRange>
//  requires is_any_self_forward_range<AnyForwardRange>
//struct translate_jacket_param<Any, AnyForwardRange const&> {
//  using type =
//      anyxx::any_forward_range<Any, Any,
//                               typename AnyForwardRange::erased_data_t,
//                               typename AnyForwardRange::dispatch_t> const&;
//};
//template <typename AnyConstObserver, typename AnyMutableObserver,
//          typename AnyForwardRange>
//  requires is_any_self_forward_range<AnyForwardRange>
//struct translate_v_table_param<AnyConstObserver, AnyMutableObserver,
//                               typename AnyForwardRange const&> {
//  using type =
//      anyxx::any_forward_range<Any, Any, typename anyxx::erased_data_t,
//                               typename AnyForwardRange::dispatch_t> const&;
//};
//
//template <typename Concrete, typename AnyForwardRange>
//  requires is_any_self_forward_range<AnyForwardRange>
//struct v_table_to_map<Concrete, AnyForwardRange const&> {
//  static Concrete const& forward(auto const& any_range) {
//    return any_range | std::views::transform([](auto const& any) {
//             *unerase_cast<Concrete>(any);
//           });
//  }
//};
//template <typename T>
//struct translate_map_param<T, anyxx::any_forward_range<anyxx::self, anyxx::self,
//                                                       anyxx::value> const&> {
//  using type =
//      anyxx::any_forward_range<T, T, anyxx::const_observer,
//                               typename AnyForwardRange::dispatch_t> const&;
//};
//template <typename Traited>
//struct forward_trait_to_map<
//    Traited,
//    anyxx::any_forward_range<anyxx::self, anyxx::self, anyxx::value> const&> {
//  template <typename Sig>
//  static Traited const& forward(Sig&& sig) {
//    return sig.erased_data_.value_;
//  }
//};
//
//}  // namespace anyxx

                        //(anyxx::any_forward_range<anyxx::self, anyxx::self,
                        //                          anyxx::value> const&),
                        //const,


namespace example_2b {

TRAIT(monoid,
      (ANY_OP_DEFAULTED(anyxx::self, +, op, (anyxx::self const&), const,
                        [&x](auto const& r) {
                          auto self = anyxx::trait_as<monoid>(x);
                          return self | (std::vector{anyxx::trait_as<monoid>(
                                            r)});  // NOLINT
                        }),
       ANY_OP_DEFAULTED(anyxx::self, |, concat,
                        (std::vector<monoid_trait<T>> const&), const,
                        [&x](const auto& r) {
                          auto self = anyxx::trait_as<monoid>(x);
                          return std::ranges::fold_right(
                              r, self, [&](auto const& m1, auto const& m2) {
                                return m1 + m2;
                              });
                        }),
       ANY_OP_DEFAULTED(bool, ==, equal, (anyxx::self const&), const,
                        ([&x](auto const& r) { return x == r; }))))

}  // namespace example_2b

ANY_MODEL_MAP((int), example_2b::monoid) {
  static monoid_trait<int> concat(int self, auto const& r) {
    return monoid_trait<int>{std::ranges::fold_right(
        r, self, [&](int m1, int m2) { return m1 + m2; })};
  };
};

ANY_MODEL_MAP((std::string), example_2b::monoid) {
  static monoid_trait<std::string> op(std::string const& self,
                                      std::string const& r) {
    return self + r;
  };
};

namespace example_2b {

template <typename M, typename R>
void test_monoid(monoid_trait<M> const& m, R r)
  requires std::ranges::range<R> &&
           std::same_as<typename R::value_type, monoid_trait<M>>
{
  using type_1 = decltype(m + (monoid_trait<M>{}) + m);
  using type_2 = decltype(m + (m + (monoid_trait<M>{})));
  static_assert(std::same_as<type_1, type_2>);
  static_assert(std::same_as<type_1, monoid_trait<M>>);
  auto c1 = m + monoid_trait<M>{} + m == m + m + monoid_trait<M>{};
  CHECK(c1);
  auto c2 = (m | r) ==
            std::ranges::fold_right(
                r, m, [&](auto const& m1, auto const& m2) { return m1 + m2; });
  CHECK(c2);
}

}  // namespace example_2b

TEST_CASE("example 2b monoid ") {
  using namespace example_2b;
  using namespace std::string_literals;
  using namespace anyxx;
  test_monoid(trait_as<monoid>(1), std::vector<monoid_trait<int>>{{2}, {3}});
  test_monoid(trait_as<monoid>("1"s),
              std::vector<monoid_trait<std::string>>{{"2"s}, {"3"s}});
}

#endif