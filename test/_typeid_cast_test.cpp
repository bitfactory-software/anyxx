#pragma once

#include <any>
#include <iostream>
#include <typeinfo>

#include "class_hierarchy_test_hierarchy.hpp"
#include <catch.hpp>

#include <virtual_void/data/has_meta_runtime/observer.hpp>
#include <virtual_void/data/has_meta_runtime/observer.hpp>
#include <virtual_void/open_method/algorithm.hpp>
#include <virtual_void/open_method/via_type_info/declare.hpp>
#include <virtual_void/typeid_cast/cast.hpp>

using namespace TestDomain;
using namespace virtual_void;
using namespace virtual_void::open_method;

namespace {
template <typename CLASSES>
void run_cast_test(const auto& castMethod, auto make_dispatch_var) {
  using namespace TestDomain;

  virtual_void::meta::visit_classes<CLASSES>(virtual_void::overload{
      [&]<typename TOP> {
        const TOP top;
        virtual_void::is_erased_data auto c_typed_void =
            make_dispatch_var(top);

        virtual_void::meta::visit_class<TOP>(virtual_void::overload{
            [&]<typename X> {
              auto static_cast_result = static_cast<const X*>(&top);
              auto type_id_cast_result =
                  virtual_void::typeid_cast::cast_to<const X>(castMethod,
                                                              c_typed_void);
              REQUIRE(static_cast_result == type_id_cast_result);
            },
            [&]<typename, typename> {}});
      },
      [&]<typename, typename> {}});
}


TEST_CASE("typeid_cast_test via_type_info") {
  using classes = virtual_void::type_list<D, C1, C2>;

  via_type_info::domain typeidTestDomain;

  typeid_cast::const_cast_method<via_type_info::declare> typeid_const_cast(
      typeidTestDomain);
  std::cout << typeid(typeid_const_cast).name() << std::endl;
  declare_classes(classes{}, typeidTestDomain);
  virtual_void::typeid_cast::fill_const_cast_for(classes{}, typeid_const_cast);
  via_type_info::seal_for_runtime(typeidTestDomain);

  run_cast_test<classes>(typeid_const_cast, [](auto& top) {
    return erased<data::has_meta_runtime::const_observer>(top);
  });
}

}  // namespace
