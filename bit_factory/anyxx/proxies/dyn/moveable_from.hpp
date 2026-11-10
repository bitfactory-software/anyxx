#pragma once

#include <bit_factory/anyxx/proxies/dyn/cow.hpp>
#include <bit_factory/anyxx/proxies/dyn/observer.hpp>
#include <bit_factory/anyxx/proxies/dyn/unique.hpp>
#include <bit_factory/anyxx/proxies/dyn/shared.hpp>
#include <bit_factory/anyxx/proxies/dyn/weak.hpp>
#include <bit_factory/anyxx/proxies/dyn/val.hpp>

namespace anyxx {

template <typename To, typename From>
inline static bool constexpr can_move_to_from = false;

template <typename To, typename From>
concept moveable_from =
    is_proxy<From> && is_proxy<To> && can_move_to_from<To, From>;

template <is_proxy X>
inline static bool constexpr can_move_to_from<X, X> = true;

template <>
inline bool constexpr can_move_to_from<shared, unique> = true;

template <>
inline bool constexpr can_move_to_from<weak, shared> = true;

template <voidness To, voidness From>
  requires const_correct_move_to_from<is_const_void<To>, is_const_void<From>,
                                      is_weak_data<From>>
inline static bool constexpr can_move_to_from<To, From> = true;

template <is_proxy To, is_proxy From>
  requires moveable_from<To, std::decay_t<From>>
void move_to(To& to, auto to_v_table, From&& from, auto from_v_table) {
  return proxy_trait<From>::move_to(to, to_v_table, std::move(from),
                                    from_v_table);
}

}  // namespace anyxx