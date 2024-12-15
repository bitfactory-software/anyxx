
#include <string>

#include <virtual_void/interface/base.hpp>
#include <virtual_void/interface/declare_macro.hpp>

#include <virtual_void/data/has_i_table/observer.hpp>
#include <virtual_void/data/has_i_table/shared_const.hpp>
#include <virtual_void/data/has_i_table/unique.hpp>
#include <virtual_void/virtual_void.hpp>
#include <virtual_void/typeid_of_macro.hpp>

using namespace virtual_void;
using namespace virtual_void::interface;

#ifndef COMPONENT_BASE_EXPORT
#define COMPONENT_BASE_EXPORT
#endif

#ifdef COMPONENT_BASE_SHARED
#ifdef _MSVC_LANG
#undef COMPONENT_BASE_EXPORT
#define COMPONENT_BASE_EXPORT __declspec(dllexport)
#pragma message ("component_base -> dll")
#endif
#endif

namespace test::component_base {

ERASED_INTERFACE(to_string_i, (INTERFACE_CONST_METHOD(std::string, to_string)))
ERASED_INTERFACE(get_value_i, (INTERFACE_CONST_METHOD(double, get_value)))
ERASED_INTERFACE_(set_value_i, get_value_i,
                  (INTERFACE_METHOD(void, set_value, double)))


COMPONENT_BASE_EXPORT to_string_i<virtual_void::data::has_i_table::const_observer> get_to_string_i_co();
COMPONENT_BASE_EXPORT to_string_i<virtual_void::data::has_i_table::shared_const> get_to_string_i_sc(double v);
COMPONENT_BASE_EXPORT to_string_i<virtual_void::data::has_i_table::unique> get_to_string_i_u(double v);
COMPONENT_BASE_EXPORT virtual_void::data::has_i_table::shared_const sc_X(double v);
COMPONENT_BASE_EXPORT virtual_void::data::has_i_table::unique u_X(double v);

}

