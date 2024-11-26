#pragma once

#include <any>
#include <iostream>

#include "class_hierarchy_test_hierarchy.hpp"
#include <catch.hpp>

#include "virtual_void/data/has_m_table/shared_const.hpp"
#include "virtual_void/data/has_m_table/unique.hpp"
#include "virtual_void/open_method/algorithm.hpp"
#include "virtual_void/open_method/via_type_info/factory.hpp"

using namespace virtual_void;

namespace {

TEST_CASE("typeid factory") 
{
  using namespace TestDomain;

  {
    auto any_factory = open_method::via_type_info::factory<std::any()>{};
    using classes = type_list<D, C1, C2>;
    open_method::fill_with_overloads(classes{}, any_factory,
                                     []<typename T>() -> std::any {
                                       // std::cout << "construct any
                                       // for " << typeid( T ).name() <<
                                       // std::endl;
                                       return std::any(T());
                                     });
    any_factory.seal_for_runtime();
    auto test = [&]<typename T>() {
      auto a = any_factory(typeid(T));
      REQUIRE(a.type() == typeid(T));
    };
    class_hierarchy::visit_classes<classes>(
        overload{[&]<typename C> { test.template operator()<C>(); },
                 [&]<typename C, typename B> {}});
  }

  {
    auto factory = open_method::via_type_info::factory<
        data::has_m_table::shared_const()>{};
    using classes = type_list<D, C1, C2>;
    open_method::fill_with_overloads(
        classes{}, factory,
        []<typename T>() -> data::has_m_table::shared_const {
          return erased_in_place<data::has_m_table::shared_const, T>(
              typeid(T).name());
        });
    factory.seal_for_runtime();
    auto test = [&]<typename T>() {
      auto cv = factory(typeid(T));
      std::cout << get_meta(cv)->type_info()->name() << std::endl;
      std::cout << typeid(T).name() << std::endl;
      REQUIRE(get_meta(cv)->type_info() == &typeid(T));
      auto tp = static_cast<const T*>(get_data(cv));
      REQUIRE(tp->data == typeid(T).name());
    };
    class_hierarchy::visit_classes<classes>(
        overload{[&]<typename C> { test.template operator()<C>(); },
                 [&]<typename C, typename B> {}});
  }
  {
    auto factory =
        open_method::via_type_info::factory<data::has_m_table::unique()>{};
    using classes = type_list<D, C1, C2>;
    open_method::fill_with_overloads(
        classes{}, factory, []<typename T>() -> data::has_m_table::unique {
          return erased_in_place<data::has_m_table::unique, T>(
              typeid(T).name());
        });
    factory.seal_for_runtime();
    auto test = [&]<typename T>() {
      auto cv = factory(typeid(T));
      REQUIRE(get_meta(cv)->type_info() == &typeid(T));
      auto tp = static_cast<const T*>(get_data(cv));
      REQUIRE(tp->data == typeid(T).name());
    };
    class_hierarchy::visit_classes<classes>(
        overload{[&]<typename C> { test.template operator()<C>(); },
                 [&]<typename C, typename B> {}});
  }
}

}  // namespace
