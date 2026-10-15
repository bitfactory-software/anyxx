#pragma once

#include <bit_factory/anyxx.hpp>

namespace anyxx {

inline static constexpr std::size_t iterator_val_proxy_size =
    small_object_size * 2u;

struct input_iterator_is_nullable {};
struct input_iterator_val_size {
  static constexpr std::size_t value = iterator_val_proxy_size;
};
struct forward_iterator_is_nullable {};
struct forward_iterator_val_size {
  static constexpr std::size_t value = iterator_val_proxy_size;
};

struct deduce_type {};

template <typename I, typename IteratorCategory>
struct iterator_category_impl {
  using type = std::conditional_t<std::same_as<IteratorCategory, deduce_type>,
                                  typename std::iterator_traits<I>::value_type,
                                  IteratorCategory>;
};
template <typename IteratorCategory>
struct iterator_category_impl<void*, IteratorCategory> {
  using type = std::forward_iterator_tag;
};
template <typename I>
struct difference_type_impl {
  using type = typename std::iterator_traits<I>::difference_type;
};
template <>
struct difference_type_impl<void*> {
  using type = std::ptrdiff_t;
};
template <typename I, typename ValueType>
struct value_type_impl {
  using type = std::conditional_t<std::same_as<ValueType, deduce_type>,
                                  typename std::iterator_traits<I>::value_type,
                                  ValueType>;
};
template <typename ValueType>
struct value_type_impl<void*, ValueType> {
  using type = ValueType;
};
template <typename I, typename Reference>
struct reference_impl {
  using type = std::conditional_t<std::same_as<Reference, deduce_type>,
                                  typename std::iterator_traits<I>::reference,
                                  Reference>;
};
template <typename Reference>
struct reference_impl<void*, Reference> {
  using type = Reference;
};

template <typename T>
concept has_post_increment = requires(T x) {
  { x++ } -> std::same_as<T>;
};

template <typename T>
auto post_increment_def = [](T& self) -> T {
  if constexpr (has_post_increment<T>) {
    return self++;
  } else {
    auto r = self;
    ++self;
    return r;
  }
};

template <typename ValueType, typename Reference, typename UnusedType = void>
struct input_iterator1;
template <typename T, typename ValueType, typename Reference>
struct input_iterator1_default_rep;
struct input_iterator1_is_nullable;
struct input_iterator1_val_size;
template <typename T, typename ValueType, typename Reference>
struct input_iterator1_default_model_map {
  using default_map = input_iterator1_default_model_map;
  using rep_type = T;
  struct deduced_type
      : dynamic_copyable<>::template model_map<T>::deduced_type {
    ;
  };

 public:
  template <typename Map>
  __forceinline auto op_dereference([[maybe_unused]] this Map const& map,
                                    [[maybe_unused]] auto const& x)
      -> anyxx::map_return<T, Reference>
    requires(anyxx::ignore_mapf_concept_with_always_true(requires(T const x) {
      { x.operator*() } -> std::convertible_to<anyxx::map_return<T, Reference>>;
    }))
  {
    using namespace anyxx;
    return ([&x]() -> Reference { return *x; })();
  };
  ;
  ;
};
template <typename T, typename ValueType, typename Reference>
struct input_iterator1_model_map
    : input_iterator1_default_model_map<T, ValueType, Reference> {
  using rep_type =
      anyxx::default_rep<T,
                         input_iterator1_default_rep<T, ValueType, Reference>>;
};
template <typename T, typename ValueType, typename Reference>
  requires(anyxx::is_variant<T>)
struct input_iterator1_model_map<T, ValueType, Reference>
    : input_iterator1_default_model_map<T, ValueType, Reference> {
  using rep_type = T;
  template <typename V>
  using x_model_map = input_iterator1_model_map<V, ValueType, Reference>;
  __forceinline auto op_dereference([[maybe_unused]] T const& x)
      -> decltype(auto) {
    return std::visit(
        anyxx::overloads{[&]<typename V>(V&& v) {
                           return x_model_map<std::decay_t<V>>{}.op_dereference(
                               std::forward<V>(v));
                         },
                         [&]<anyxx::is_any Any>([[maybe_unused]] Any&& any) {
                           return std::forward<Any>(any).op_dereference();
                         }},
        x);
  };
};
struct input_iterator1_has_open_dispatch;
template <typename ValueType, typename Reference>
struct input_iterator1_v_table : dynamic_copyable<>::v_table_t {
  using v_table_base_t = typename dynamic_copyable<>::v_table_t;
  using v_table_t = input_iterator1_v_table;
  using val_is_nullable =
      std::conditional_t<anyxx::is_type_complete<input_iterator1_is_nullable>,
                         std::true_type,
                         typename v_table_base_t::val_is_nullable>;
  static constexpr std::size_t val_proxy_size =
      anyxx::compute_val_proxy_size<input_iterator1_val_size>(
          v_table_base_t::val_proxy_size);
  using any_value_t = anyxx::any<anyxx::val<val_proxy_size, val_is_nullable>,
                                 input_iterator1, ValueType, Reference>;
  static bool static_is_derived_from(const std::type_info& from) {
    if constexpr (anyxx::is_dynamic_castable_v_table<v_table_base_t>) {
      return typeid(v_table_t) == from
                 ? true
                 : v_table_base_t::static_is_derived_from(from);
    } else {
      return false;
    }
  }
  using T = char;
  struct deduced_type : v_table_base_t::deduced_type {
    ;
  };
  anyxx::v_table_return<any_value_t, Reference> (*op_dereferenceconst)(
      void const*);
  ;
  ;
  ;
  ;
  template <typename Concrete, typename Parent = input_iterator1_v_table>
  explicit(false) input_iterator1_v_table(
      std::in_place_type_t<Concrete> concrete,
      std::in_place_type_t<Parent> parent = std::in_place_type<Parent>);
};
template <typename ValueType, typename Reference, typename UnusedType>
struct __declspec(empty_bases) input_iterator1 : dynamic_copyable<> {
  using base_t = dynamic_copyable<>;
  template <typename T>
  using model_map = input_iterator1_model_map<T, ValueType, Reference>;
  using val_is_nullable =
      std::conditional_t<anyxx::is_type_complete<input_iterator1_is_nullable>,
                         std::true_type, typename base_t::val_is_nullable>;
  static constexpr std::size_t val_proxy_size =
      anyxx::compute_val_proxy_size<input_iterator1_val_size>(
          base_t::val_proxy_size);
  using any_value_t = anyxx::any<anyxx::val<val_proxy_size, val_is_nullable>,
                                 input_iterator1, ValueType, Reference>;
  using v_table_base_t = base_t::v_table_t;
  using v_table_t = input_iterator1_v_table<ValueType, Reference>;
  template <typename StaticDispatchType>
  using static_dispatch_map_t =
      input_iterator1_model_map<StaticDispatchType, ValueType, Reference>;
  template <typename M>
  constexpr static bool modeled_by();
  template <typename Self>
  __forceinline decltype(auto) operator*(this Self&& self)
    requires(::anyxx::const_correct_call_for_proxy_and_self<
             void const*, typename std::decay_t<Self>::proxy_t,
             std::is_const_v<std::remove_reference_t<Self>>, false>)
  {
    using self_t = std::decay_t<Self>;
    using T = typename self_t::T;
    using proxy_t = typename self_t::proxy_t;
    using deduced_type = typename self_t::deduced_type;
    //if constexpr (!self_t::is_dyn) {
    //  using traited_t = typename self_t::rep_type;
    //  if constexpr (std::same_as<void, Reference>) {
    //    return static_dispatch_map_t<T>{}.op_dereference(get_proxy_value(self));
    //  } else {
    //    return anyxx::jacket_return<Reference>::forward(
    //        static_dispatch_map_t<T>{}.op_dereference(get_proxy_value(self)),
    //        std::forward<Self>(self));
    //  }
    //} else {
    //  if constexpr (std::same_as<void, Reference>) {
    //    return get_v_table(self)->op_dereferenceconst(
    //        anyxx::get_proxy_ptr(std::forward<self_t const&>(self)));
    //  } else {
        return anyxx::jacket_return<Reference>::forward(
            get_v_table(self)->op_dereferenceconst(
                anyxx::get_proxy_ptr(std::forward<self_t const&>(self))),
            std::forward<Self>(self));
    //  }
    //}
  };
  ;
  ;
};
template <typename T, typename ValueType, typename Reference,
          typename deduced_type =
              input_iterator1_model_map<T, ValueType, Reference>::deduced_type>
concept is_input_iterator1_model =
    requires(T model,
             anyxx::any_trait_class<T, input_iterator1<ValueType, Reference>>
                 trait_class,
             input_iterator1_model_map<T, ValueType, Reference> model_map) {
      requires anyxx::is_type_complete<T>;
      requires requires() {
        {
          model_map.op_dereference(model)
        } -> std::convertible_to<anyxx::map_return<T, Reference>>;
      };
    } &&
    dynamic_copyable<>::template modeled_by<T>();
template <typename ValueType, typename Reference, typename UnusedType>
template <typename T>
constexpr bool input_iterator1<ValueType, Reference, UnusedType>::modeled_by() {
  return is_input_iterator1_model<T, ValueType, Reference>;
};
template <typename ValueType, typename Reference>
template <typename Concrete, typename Parent>
input_iterator1_v_table<ValueType, Reference>::input_iterator1_v_table(
    std::in_place_type_t<Concrete> concrete,
    std::in_place_type_t<Parent> parent)
    : v_table_base_t(concrete, parent) {
  using model_map = input_iterator1_model_map<Concrete, ValueType, Reference>;
  op_dereferenceconst = [](void const* _vp) -> anyxx::v_table_return<any_value_t, Reference> {
    /*if constexpr (std::same_as<anyxx::self&, Reference>) {
      model_map{}.op_dereference(*anyxx::unchecked_unerase_cast<Concrete>(_vp));
      return anyxx::handle_self_ref_return<Reference>{}();
    } else if constexpr (std::same_as<anyxx::self, Reference>) {
      using proxy_t = typename any_value_t::proxy_t;
      return any_value_t{anyxx::erased<proxy_t>(model_map{}.op_dereference(
                             *anyxx::unchecked_unerase_cast<Concrete>(_vp))),
                         anyxx::v_table_instance<Parent, Concrete>()};
    } else*/ {
      auto rr = model_map{}.op_dereference(
          *anyxx::unchecked_unerase_cast<Concrete>(_vp));
      static_assert(std::same_as<decltype(rr), int>);
      static_assert(std::same_as<decltype(rr), anyxx::v_table_return<any_value_t, Reference>>);
      anyxx::v_table_return<any_value_t, Reference> r{rr};
      return r;
    }
  };
  ;
  ;
  ;
  ;
  ::anyxx::set_is_derived_from<v_table_t>(this);
};

TRAIT_TEMPLATE_EX_(
    ((ValueType), (Reference)), input_iterator, dynamic_copyable, (),
    (ANY_OP_MAP_NAMED(anyxx::self&, ++, op_pre_increment, (), ),
     ANY_OP_DEF(public, Reference, *, op_dereference, (), const,
                ([&x]() -> Reference { return *x; })),
     ANY_OP_MAP_NAMED_FRIEND(bool, ==, equal, (anyxx::self const&), const),
     ANY_OP_MAP_NAMED_FRIEND(bool, !=, inequal, (anyxx::self const&), const)),
    ,
    (ANY_TYPE((), iterator_category, std::input_iterator_tag,
              (std::input_iterator_tag)),
     ANY_TYPE((), difference_type, std::ptrdiff_t,
              (difference_type_impl<T>::type)),
     ANY_TYPE((), value_type, ValueType, (value_type_impl<T, ValueType>::type)),
     ANY_TYPE((), reference, Reference, (reference_impl<T, Reference>::type))),
    , ())

template <typename ValueType, typename Reference,
          typename Proxy = nullable_val<iterator_val_proxy_size>>
using any_input_iterator = any<Proxy, input_iterator, ValueType, Reference>;
static_assert(std::input_iterator<any_input_iterator<int, int>>);

TRAIT_TEMPLATE_EX_(((ValueType), (Reference)), forward_iterator, input_iterator,
                   ((ValueType), (Reference)), , ,
                   (ANY_TYPE((), iterator_category, std::forward_iterator_tag,
                             (std::forward_iterator_tag))),
                   , ())

template <typename ValueType, typename Reference,
          typename Proxy = nullable_val<iterator_val_proxy_size>>
using any_forward_iterator = any<Proxy, forward_iterator, ValueType, Reference>;
static_assert(std::forward_iterator<any_forward_iterator<int, int>>);

TRAIT_TEMPLATE_EX_(((ValueType), (Reference)), bidirectional_iterator,
                   forward_iterator, ((ValueType), (Reference)),
                   (ANY_OP_MAP_NAMED(anyxx::self&, --, op_pre_decrement, (), )),
                   ,
                   (ANY_TYPE((), iterator_category,
                             std::bidirectional_iterator_tag,
                             (std::bidirectional_iterator_tag))),
                   , ())

template <typename ValueType, typename Reference,
          typename Proxy = nullable_val<iterator_val_proxy_size>>
using any_bidirectional_iterator =
    any<Proxy, bidirectional_iterator, ValueType, Reference>;
static_assert(
    std::bidirectional_iterator<any_bidirectional_iterator<int, int>>);

TRAIT_TEMPLATE_EX_(
    ((ValueType), (Reference)), random_access_iterator, bidirectional_iterator,
    ((ValueType), (Reference)),
    (ANY_OP_MAP_NAMED_FRIEND(bool, <, less, (anyxx::self const&), const),
     ANY_OP_MAP_NAMED_FRIEND(bool, >, greater, (anyxx::self const&), const),
     ANY_OP_MAP_NAMED_FRIEND(bool, <=, less_equal, (anyxx::self const&), const),
     ANY_OP_MAP_NAMED_FRIEND(bool, >=, greater_equal, (anyxx::self const&),
                             const),
     ANY_OP_MAP_NAMED_FRIEND(anyxx::self&, +=, plus_assign, (std::ptrdiff_t),
                             const),
     ANY_OP_MAP_NAMED_FRIEND(anyxx::self&, -=, minus_assign, (std::ptrdiff_t),
                             const),
     ANY_OP_MAP_NAMED_FRIEND(anyxx::self, +, plus, (std::ptrdiff_t), const),
     ANY_OP_MAP_NAMED_FRIEND(anyxx::self, -, minus, (std::ptrdiff_t), const),
     ANY_FN_DEF(public, std::ptrdiff_t, difference_to, (anyxx::self const&),
                const, [&x](auto const& rhs) { return x - rhs; }),
     ANY_OP_MAP_NAMED(Reference, [], at, (std::ptrdiff_t), const)),
    ,
    (ANY_TYPE((), iterator_category, std::random_access_iterator_tag,
              (std::random_access_iterator_tag))),
    , ())

template <typename ValueType, typename Reference,
          typename Proxy = nullable_val<iterator_val_proxy_size>>
using any_random_access_iterator =
    any<Proxy, random_access_iterator, ValueType, Reference>;

template <typename Value, typename Ref>
auto operator-(any_random_access_iterator<Value, Ref> const& lhs,
               any_random_access_iterator<Value, Ref> const& rhs) {
  return lhs.difference_to(rhs);
}
template <typename Value, typename Ref>
auto operator+(std::ptrdiff_t n,
               any_random_access_iterator<Value, Ref> const& it) {
  return it + n;
}
static_assert(
    std::random_access_iterator<any_random_access_iterator<int, int>>);

TRAIT_TEMPLATE_(((AnyIterator)), range, dynamic_copyable, (),
                (ANY_FN(AnyIterator, begin, (), const),
                 ANY_FN(AnyIterator, end, (), const)))

template <typename AnyIterator, typename Proxy = anyxx::cref>
using any_range = any<Proxy, range, AnyIterator>;

template <typename ValueType, typename Reference = ValueType,
          typename Proxy = anyxx::cref>
using any_forward_range =
    any_range<any_forward_iterator<ValueType, Reference>, Proxy>;

using any_self_forward_range = any_forward_range<self, self>;
template <typename A>
concept is_any_self_forward_range =
    is_any<A> && std::ranges::forward_range<A> &&
    std::same_as<std::ranges::range_value_t<A>, self>;
static_assert(is_any_self_forward_range<any_self_forward_range>);

template <typename AnyForwardRange>
  requires is_any_self_forward_range<AnyForwardRange>
struct translate_sig_map<AnyForwardRange const&> : translate_sig_map<self> {
  template <typename AnyValue>
  using v_table_param =
      any_forward_range<AnyValue, AnyValue,
                        typename AnyForwardRange::proxy_t> const&;
  template <typename Model>
  using concept_arg = anyxx::any_forward_range<Model, Model, anyxx::cref>;
};

template <typename Concrete, typename AnyForwardRange>
  requires is_any_self_forward_range<AnyForwardRange>
struct v_table_to_map<Concrete, AnyForwardRange const&> {
  static auto forward(auto const& any_range) {
    return std::views::transform(any_range, [](auto const& any) {
      return *unerase_cast<Concrete>(any);
    });
  }
};
//
template <typename Traited, typename AnyForwardRange>
  requires is_any_self_forward_range<AnyForwardRange>
struct forward_trait_to_map<Traited, AnyForwardRange const&> {
  static auto forward(auto any_range) {
    return std::views::transform(any_range,
                                 []<typename T>(T const& any) -> Traited {
                                   if constexpr (is_any<T>) {
                                     if constexpr (T::is_dyn) {
                                       return *unerase_cast<Traited>(any);
                                     } else {
                                       return get_proxy_value(any);
                                     }
                                   } else {
                                     return any;
                                   }
                                 });
  }
};

static_assert(
    any_forward_iterator<self, self>::v_table_t::val_is_nullable::value ==
    true);
static_assert(
    std::same_as<any_forward_iterator<self, self>::v_table_t::any_value_t,
                 any<nullable_val<iterator_val_proxy_size>, forward_iterator,
                     self, self>>);
static_assert(
    std::same_as<random_access_iterator<self, self>::v_table_t::any_value_t,
                 any<nullable_val<iterator_val_proxy_size>,
                     random_access_iterator, self, self>>);
static_assert(std::forward_iterator<any_forward_iterator<self, self>>);
static_assert(std::ranges::forward_range<any_forward_range<self, self>>);

template <template <typename...> typename Trait, typename... Ts>
auto trait_range_as(auto&& r) {
  return std::forward<decltype(r)>(r) |
         std::views::transform(
             [&](auto const& x) { return trait_as<Trait, Ts...>(x); });
}

}  // namespace anyxx