#pragma once

#include <map>
#include <typeindex>
#include <vector>
#include <virtual_void/meta/archetype.hpp>
#include <virtual_void/meta/class.hpp>
#include <virtual_void/meta/interface.hpp>
#include <virtual_void/meta/table.hpp>
#include <virtual_void/data/has_meta_runtime/unique.hpp>
#include <virtual_void/utillities/overload.hpp>
#include <virtual_void/utillities/type_list.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::meta {

template <typename ARCHETYPE>
struct models {
  using archetype = ARCHETYPE;
};

struct models_no_archetype : models<archetype_unspecified> {};

template <typename CLASS>
struct class_ : models_no_archetype {};

template <typename CLASS>
using archetype_for_class = class_<CLASS>::archetype;

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

class type_info {
  const std::type_info& type_info_;
  archetype& archetype_;
  using copy_construct_t = auto(const_void) -> data::has_meta_runtime::unique;
  copy_construct_t* copy_construct_;

  table<virtual_void::interface::base_v_table*, nullptr> i_table_;
  table<void (*)(), nullptr> m_table_;

 public:
  template <typename CLASS>
  constexpr type_info(std::in_place_type_t<CLASS>)
      : type_info_(typeid_of<CLASS>()),
        archetype_(runtime<archetype, archetype_for_class<CLASS>>()),
        copy_construct_(+[](const_void from) {
          return erased<data::has_meta_runtime::unique>(
              *static_cast<CLASS const*>(from));
        }) {}

  constexpr operator const std::type_info&() const{ return get_type_info(); }
  constexpr const std::type_info& get_type_info() const { return type_info_; }
  constexpr archetype& get_archetype() const { return archetype_; }
  constexpr int get_archetype_index() const {
    return get_archetype().get_archetype_index();
  }
  auto copy_construct(const_void from) { return copy_construct_(from); }

  auto& get_i_table() { return i_table_; }
  auto& get_m_table() { return m_table_; }
  auto& get_i_table() const { return i_table_; }
  auto& get_m_table() const { return m_table_; }
};

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

constexpr auto fill_runtime_registry(classes_with_bases& registry) {
  return overload{
      [&]<typename C> { registry[typeid_of<C>()].self = &typeid_of<C>(); },
      [&]<typename C, typename B> {
        registry[typeid_of<C>()].bases.emplace_back(&typeid_of<B>());
      }};
}
template <typename CLASS, bool deep = true>
constexpr nullptr_t declare(classes_with_bases& registry) {
  meta::visit_class<CLASS, deep>(fill_runtime_registry(registry));
  return {};
}
template <typename CLASSES, bool deep = true>
constexpr nullptr_t declare_classes(classes_with_bases& registry) {
  meta::visit_classes<CLASSES, deep>(fill_runtime_registry(registry));
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

template <typename V_TABLE>
int archetype_index_in_v_table(archetype& archetype);
template <>
inline int archetype_index_in_v_table<base_v_table>(archetype& archetype) {
  return -1;
}
template <typename V_TABLE>
int archetype_index_in_v_table(archetype& archetype) {
  using base_t = V_TABLE::v_table_base_t;
  auto index = archetype_index_in_v_table<base_t>(archetype);
  auto& interface_meta = runtime<meta::interface, V_TABLE>();
  if (index < 0)
    return interface_meta.register_archetype(archetype);
  else
    return interface_meta.register_archetype_at(archetype, index);
}

template <typename CLASS, typename V_TABLE>
struct is_a {
  constexpr is_a() {
    auto& type_info = runtime<meta::type_info, CLASS>();
    auto& i_table = type_info.get_i_table();
    auto& archetype = type_info.get_archetype();
    auto i_table_idx = archetype_index_in_v_table<V_TABLE>(archetype);
    using uneraser = static_cast_uneraser<CLASS>;
    auto v_table_ptr = V_TABLE::template imlpementation<uneraser>();
    i_table.register_target(i_table_idx, v_table_ptr);
  }
};

}  // namespace virtual_void::meta

