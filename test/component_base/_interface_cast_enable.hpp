
#include <string>

#include <virtual_void/interface/base.hpp>
#include <virtual_void/interface/declare_macro.hpp>

#include <virtual_void/data/has_type_info/observer.hpp>
#include <virtual_void/data/has_type_info/shared_const.hpp>
#include <virtual_void/data/has_type_info/unique.hpp>
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

namespace test_query_interface {

ERASED_INTERFACE(to_string_i, (INTERFACE_CONST_METHOD(std::string, to_string)))
ERASED_INTERFACE(get_value_i, (INTERFACE_CONST_METHOD(double, get_value)))
ERASED_INTERFACE_(set_value_i, get_value_i,
                  (INTERFACE_METHOD(void, set_value, double)))

COMPONENT_BASE_EXPORT void enable_casts();

COMPONENT_BASE_EXPORT to_string_i<virtual_void::data::has_type_info::const_observer> get_to_string_i_co();
COMPONENT_BASE_EXPORT to_string_i<virtual_void::data::has_type_info::shared_const> get_to_string_i_sc(double v);
COMPONENT_BASE_EXPORT to_string_i<virtual_void::data::has_type_info::unique> get_to_string_i_u(double v);
COMPONENT_BASE_EXPORT virtual_void::data::has_type_info::shared_const sc_X(double v);

}

VV_TYPEID_OF_DECLARE(COMPONENT_BASE_EXPORT, test_query_interface::to_string_i<virtual_void::data::has_type_info::const_observer>);
VV_TYPEID_OF_DECLARE(COMPONENT_BASE_EXPORT, test_query_interface::to_string_i<virtual_void::data::has_type_info::unique>);

VV_TYPEID_OF_DECLARE(COMPONENT_BASE_EXPORT, test_query_interface::get_value_i<virtual_void::data::has_type_info::const_observer>);
VV_TYPEID_OF_DECLARE(COMPONENT_BASE_EXPORT, test_query_interface::get_value_i<virtual_void::data::has_type_info::mutable_observer>);
VV_TYPEID_OF_DECLARE(COMPONENT_BASE_EXPORT, test_query_interface::get_value_i<virtual_void::data::has_type_info::shared_const>);
VV_TYPEID_OF_DECLARE(COMPONENT_BASE_EXPORT, test_query_interface::get_value_i<virtual_void::data::has_type_info::unique>);

VV_TYPEID_OF_DECLARE(COMPONENT_BASE_EXPORT, test_query_interface::set_value_i<virtual_void::data::has_type_info::mutable_observer>);
VV_TYPEID_OF_DECLARE(COMPONENT_BASE_EXPORT, test_query_interface::set_value_i<virtual_void::data::has_type_info::unique>);
