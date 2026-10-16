#pragma once

#include <bit_factory/anyxx/errors/error.hpp>
#include <bit_factory/anyxx/proxies/proxy_concept.hpp>
#include <map>
#include <functional>
#include <string>
#include <type_traits>
#include <concepts>
#include <utility>

namespace anyxx {

class unknown_factory_key_error : public error {
  using error::error;
};
/// \brief A key type for factory registration.
template <typename Tag>
struct key {
  const char* label;
  friend auto operator<=>(key, key) = default;
};
template <typename T>
struct is_key_impl : std::false_type {};
template <typename T>
struct is_key_impl<key<T>> : std::true_type {};
template <typename T>
concept is_key = is_key_impl<T>::value;

/// \brief A class template to implement a factory for \ref any
/// objects.
template <typename Any, typename Key, typename... Args>
  requires proxy_trait<typename Any::proxy_t>::is_owner
class factory {
  using constructor_t = std::function<Any(Args...)>;
  std::map<Key, constructor_t> factory_map_;

  auto register_impl(Key key, auto const& construct) {
    factory_map_[key] = [construct](Args... args) -> Any {
      return Any{std::in_place, construct(std::forward<Args>(args)...)};
    };
  }

  auto construct_impl(Key key, Args... args) {
    if (auto found = factory_map_.find(key); found != factory_map_.end())
      return found->second(std::forward<Args>(args)...);
    if constexpr (std::same_as<Key, std::string>) {
      throw unknown_factory_key_error{key};
    } else if constexpr (is_key<Key>) {
      throw unknown_factory_key_error{key.label};
    } else {
      throw unknown_factory_key_error{std::to_string(key)};
    }
  };

 public:
  auto register_(Key const& key, auto const& construct) {
    register_impl(key, construct);
    return nullptr;
  }
  auto construct(auto key, Args&&... args) {
    return construct_impl(key, std::forward<Args>(args)...);
  }
};


}  // namespace anyxx