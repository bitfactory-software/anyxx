#pragma once

#include <any>
#include <iostream>

#include "../include/virtual_void/erased/data/has_m_table/shared_const.h"
#include "../include/virtual_void/erased/data/has_m_table/unique.h"
#include "../include/virtual_void/erased/open_method/algorithm.h"
#include "../include/virtual_void/erased/open_method/via_type_info/factory.h"
#include "class_hierarchy_test_hierarchy.h"
#include "include/catch.hpp"

using namespace virtual_void;

namespace {

TEST_CASE("typeid factory") {
  using namespace TestDomain;

  {
    auto any_factory =
        erased::open_method::via_type_info::factory<std::any()>{};
    using classes = type_list<D, C1, C2>;
    erased::open_method::fill_with_overloads(classes{}, any_factory,
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
    auto factory = erased::open_method::via_type_info::factory<
        erased::data::has_m_table::shared_const()>{};
    using classes = type_list<D, C1, C2>;
    erased::open_method::fill_with_overloads(
        classes{}, factory,
        []<typename T>() -> erased::data::has_m_table::shared_const {
          return erased::data::has_m_table::shared_const{std::in_place_type<T>,
                                                         typeid(T).name()};
        });
    factory.seal_for_runtime();
    auto test = [&]<typename T>() {
      auto cv = factory(typeid(T));
      std::cout << cv.meta()->type_info()->name() << std::endl;
      std::cout << typeid(T).name() << std::endl;
      REQUIRE(cv.meta()->type_info() == &typeid(T));
      auto tp = static_cast<const T*>(cv.data());
      REQUIRE(tp->data == typeid(T).name());
    };
    class_hierarchy::visit_classes<classes>(
        overload{[&]<typename C> { test.template operator()<C>(); },
                 [&]<typename C, typename B> {}});
  }
  {
    auto factory = erased::open_method::via_type_info::factory<
        erased::data::has_m_table::unique()>{};
    using classes = type_list<D, C1, C2>;
    erased::open_method::fill_with_overloads(
        classes{}, factory,
        []<typename T>() -> erased::data::has_m_table::unique {
          return erased::data::has_m_table::unique(std::in_place_type<T>,
                                                   typeid(T).name());
        });
    factory.seal_for_runtime();
    auto test = [&]<typename T>() {
      auto cv = factory(typeid(T));
      REQUIRE(cv.meta()->type_info() == &typeid(T));
      auto tp = static_cast<const T*>(cv.data());
      REQUIRE(tp->data == typeid(T).name());
    };
    class_hierarchy::visit_classes<classes>(
        overload{[&]<typename C> { test.template operator()<C>(); },
                 [&]<typename C, typename B> {}});
  }
}

}  // namespace
