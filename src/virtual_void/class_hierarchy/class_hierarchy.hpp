#pragma once

#include <map>
#include <typeindex>
#include <vector>

#include "../utillities/overload.hpp"
#include "../utillities/type_list.hpp"
#include "../virtual_void.hpp"

namespace virtual_void::class_hierarchy {

template <typename CLASS>
struct class_;

struct base {
  using bases_ = type_list<>;
};
template <typename... BASES>
struct bases {
  using bases_ = type_list<BASES...>;
};

template <class CLASS>
concept is_registered_class =
    requires(CLASS) { typename class_<CLASS>::bases_; };

template <typename CLASS, bool deep = true>
constexpr void visit_class(auto visitor) {
  visitor.template operator()<CLASS>();
  if constexpr (is_registered_class<CLASS>) {
    using bases = class_<CLASS>::bases_;
    bases::for_each([&]<typename BASE>() {
      visitor.template operator()<CLASS, BASE>();
      if constexpr (deep) {
        visit_class<BASE, deep>(visitor);
      }
    });
  }
}

template <typename CLASSES, bool deep = true>
constexpr void visit_classes(auto visitor) {
  CLASSES::for_each(
      [&]<typename CLASS>() { visit_class<CLASS, deep>(visitor); });
}

using bases_t = std::vector<const std::type_info*>;
struct class_with_bases {
  const std::type_info* self;
  bases_t bases;
};
using classes_with_bases = std::map<std::type_index, class_with_bases>;

constexpr auto declare_visitor(classes_with_bases& registry) {
  return overload{[&]<typename C> { registry[typeid(C)].self = &typeid(C); },
                  [&]<typename C, typename B> {
                    registry[typeid(C)].bases.emplace_back(&typeid(B));
                  }};
}
template <typename CLASS, bool deep = true>
constexpr nullptr_t declare(classes_with_bases& registry) {
  class_hierarchy::visit_class<CLASS, deep>(declare_visitor(registry));
  return {};
}
template <typename CLASSES, bool deep = true>
constexpr nullptr_t declare_classes(classes_with_bases& registry) {
  class_hierarchy::visit_classes<CLASSES, deep>(declare_visitor(registry));
  return {};
}
template <typename CLASS>
constexpr auto declare_deep(classes_with_bases& registry) {
  return declare<CLASS, true>(registry);
}
template <typename CLASS>
constexpr auto declare_shallow(classes_with_bases& registry) {
  return declare<CLASS, false>(registry);
}

constexpr void visit_hierarchy(const std::type_index& self,
                               const classes_with_bases& classes_with_bases,
                               auto visitor);
inline constexpr void visit_bases(const bases_t& bases,
                                  const classes_with_bases& classes_with_bases,
                                  auto visitor) {
  for (auto base : bases) visit_hierarchy(*base, classes_with_bases, visitor);
}
inline constexpr void visit_hierarchy(
    const std::type_index& self, const classes_with_bases& classes_with_bases,
    auto visitor) {
  auto found = classes_with_bases.find(self);
  if (found == classes_with_bases.end())
    throw std::runtime_error("class not registered.");
  visitor(*found->second.self);
  visit_bases(found->second.bases, classes_with_bases, visitor);
}

}  // namespace virtual_void::class_hierarchy