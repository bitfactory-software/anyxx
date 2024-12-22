#pragma once

#include <any>
#include <catch.hpp>
#include <iostream>
#include <virtual_void/data/has_meta_runtime/shared_const.hpp>
#include <virtual_void/data/has_meta_runtime/unique.hpp>
#include <virtual_void/open_method/algorithm.hpp>
#include <virtual_void/open_method/via_type_info/factory.hpp>

#include "class_hierarchy_test_hierarchy.hpp"

using namespace virtual_void;

namespace {

TEST_CASE("typeid factory") {
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
    meta::visit_classes<classes>(
        overload{[&]<typename C> { test.template operator()<C>(); },
                 [&]<typename C, typename B> {}});
  }

  {
    auto factory = open_method::via_type_info::factory<
        data::has_meta_runtime::shared_const()>{};
    using classes = type_list<D, C1, C2>;
    open_method::fill_with_overloads(
        classes{}, factory,
        []<typename T>() -> data::has_meta_runtime::shared_const {
          return erased_in_place<data::has_meta_runtime::shared_const, T>(
              typeid(T).name());
        });
    factory.seal_for_runtime();
    auto test = [&]<typename T>() {
      auto cv = factory(typeid(T));
      auto type_info = get_std_type_info(*get_meta(cv));
      std::cout << type_info->name() << std::endl;
      std::cout << typeid(T).name() << std::endl;
      REQUIRE(type_info == &typeid(T));
      auto tp = static_cast<const T*>(get_data(cv));
      REQUIRE(tp->data == typeid(T).name());
    };
    meta::visit_classes<classes>(
        overload{[&]<typename C> { test.template operator()<C>(); },
                 [&]<typename C, typename B> {}});
  }
  {
    auto factory =
        open_method::via_type_info::factory<data::has_meta_runtime::unique()>{};
    using classes = type_list<D, C1, C2>;
    open_method::fill_with_overloads(
        classes{}, factory, []<typename T>() -> data::has_meta_runtime::unique {
          return erased_in_place<data::has_meta_runtime::unique, T>(
              typeid(T).name());
        });
    factory.seal_for_runtime();
    auto test = [&]<typename T>() {
      auto cv = factory(typeid(T));
      auto type_info = get_std_type_info(*get_meta(cv));
      REQUIRE(type_info == &typeid(T));
      auto tp = static_cast<const T*>(get_data(cv));
      REQUIRE(tp->data == typeid(T).name());
    };
    meta::visit_classes<classes>(
        overload{[&]<typename C> { test.template operator()<C>(); },
                 [&]<typename C, typename B> {}});
  }
}

}  // namespace
