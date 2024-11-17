#pragma once

#include <any>
#include <iostream>
#include <typeinfo>

import virtual_void;
//#include "virtual_void/data/has_m_table/observer.h"
//#include "virtual_void/data/has_m_table/shared_const.h"
//#include "virtual_void/data/has_m_table/unique.h"
//#include "virtual_void/open_method/algorithm.h"
//#include "virtual_void/open_method/via_m_table/declare.h"

#include <catch.hpp>

using namespace virtual_void;
using namespace virtual_void::data::has_m_table;
using namespace virtual_void::open_method::via_m_table;

#include "class_hierarchy_test_hierarchy.h"

namespace {

using namespace TestDomain;

auto ToString = [](const auto* t) -> std::string { return typeid(*t).name(); };

using to_string_method = declare<std::string(virtual_void::const_)>;

template <typename T>
auto call(const to_string_method& method) {
  T t;
  return method(erased<const_observer>(t));
}

TEST_CASE("m_table open_method") {
  {
    domain testDomain;
    to_string_method toString(testDomain);

    toString.define<A1>(
        +[](const A1* x) -> std::string { return ToString(x); });

    declare_classes<D>(testDomain);
    open_method::interpolate(testDomain);
    REQUIRE(toString.is_defined<D>());
    fix_m_tables(testDomain);
    REQUIRE(call<D>(toString) == typeid(A1).name());  // call for base class
  }
  {
    domain testDomain;
    to_string_method toString(testDomain);

    declare_classes<D>(testDomain);
    open_method::fill_with_overloads<D>(toString, ToString);
    REQUIRE(toString.is_defined<D>());
    fix_m_tables(testDomain);
    REQUIRE(
        call<D>(toString) ==
        typeid(D).name());  // call for function found via fill_with_overloads
  }
  {
    domain testDomain;
    to_string_method toString(testDomain);
    using classes = type_list<D, C1, C2>;
    declare_classes(classes{}, testDomain);
    open_method::fill_with_overloads(classes{}, toString,
                                                   ToString);
    fix_m_tables(testDomain);
    class_hierarchy::visit_classes<classes>(
        overload{[&]<typename C> {
                                 C c;
                                 auto virtual_void = erased<const_observer>(c);
                                 auto u = erased<unique>(C{});
                                 auto expected = typeid(C).name();
                                 auto r = toString(u);
                                 auto r1 = toString(virtual_void);
                                 REQUIRE(r == expected);
                                 REQUIRE(r1 == expected);
                               },
                               [&]<typename C, typename B> {}});
  }
}

}  // namespace
