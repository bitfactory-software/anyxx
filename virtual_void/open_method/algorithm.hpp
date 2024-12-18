#pragma once

#include <virtual_void/open_method/domain.hpp>

namespace virtual_void::open_method {

template <typename CLASS>
void fill_with_overload(auto& method, const auto& wrapper) {
  if (!method.template is_defined<CLASS>()) method.template define<CLASS>(wrapper);
}
template <typename TYPE_LIST>
void fill_with_overloads(TYPE_LIST, auto& method, const auto& wrapper) {
  meta::visit_classes<TYPE_LIST>(
      overload{[&]<typename C> { fill_with_overload<C>(method, wrapper); },
               [&]<typename C, typename B> {}});
}
template <typename... CLASSES>
void fill_with_overloads(auto& method, const auto& wrapper) {
  fill_with_overloads(type_list<CLASSES...>{}, method, wrapper);
}

template <typename METHOD>
inline constexpr auto find_declared_in_bases(
    const meta::classes_with_bases& registry,
    const meta::bases_t& bases, const METHOD& method) {
  typename METHOD::dispatch_target_t found = nullptr;
  visit_bases(bases, registry, [&](const std::type_info& base) {
    if (!found) found = method.is_defined(base);
  });
  return found;
}
inline void interpolate(const meta::classes_with_bases& classes,
                        auto* method) {
  for (const auto& [self, class_with_bases] : classes)
    if (!method->is_defined(*class_with_bases.self))
      if (auto found =
              find_declared_in_bases(classes, class_with_bases.bases, *method))
        method->define_erased(*class_with_bases.self, found);
}
inline void interpolate(const auto& domain) {
  for (const auto& method : domain.open_methods)
    interpolate(domain.classes, method);
}
template <typename CLASSES>
auto declare_classes(CLASSES, auto& domain) {
  return meta::declare_classes<CLASSES>(domain.classes);
}
template <typename... CLASSES>
auto declare_classes(auto& domain) {
  return declare_classes(type_list<CLASSES...>{}, domain);
}

}  // namespace virtual_void::open_method