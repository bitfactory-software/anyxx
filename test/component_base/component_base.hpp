#include <string>
#include <virtual_void/data/has_meta_runtime/observer.hpp>
#include <virtual_void/data/has_meta_runtime/shared_const.hpp>
#include <virtual_void/data/has_meta_runtime/unique.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/interface/declare_macro.hpp>
#include <virtual_void/meta/i_table.hpp>
#include <virtual_void/virtual_void.hpp>

using namespace virtual_void;
using namespace virtual_void::interface;

#ifndef COMPONENT_BASE_EXPORT
#define COMPONENT_BASE_EXPORT
#endif

#ifdef COMPONENT_BASE_SHARED
#ifdef _MSVC_LANG
#undef COMPONENT_BASE_EXPORT
#define COMPONENT_BASE_EXPORT __declspec(dllexport)
#pragma message("component_base -> dll")
#endif
#endif

namespace test::component_base {

VV_INTERFACE(to_string_i, (VV_CONST_METHOD(std::string, to_string)))
VV_INTERFACE(get_value_i, (VV_CONST_METHOD(double, get_value)))
VV_INTERFACE_(set_value_i, get_value_i, (VV_METHOD(void, set_value, double)))

COMPONENT_BASE_EXPORT
to_string_i<virtual_void::data::has_meta_runtime::const_observer>
get_to_string_i_co();
COMPONENT_BASE_EXPORT to_string_i<virtual_void::data::has_meta_runtime::shared_const>
get_to_string_i_sc(double v);
COMPONENT_BASE_EXPORT to_string_i<virtual_void::data::has_meta_runtime::unique>
get_to_string_i_u(double v);
COMPONENT_BASE_EXPORT virtual_void::data::has_meta_runtime::shared_const sc_X(
    double v);
COMPONENT_BASE_EXPORT virtual_void::data::has_meta_runtime::unique u_X(double v);

}  // namespace test::component_base

VV_INTERFACE_META(COMPONENT_BASE_EXPORT, test::component_base::get_value_i)
VV_INTERFACE_META(COMPONENT_BASE_EXPORT, test::component_base::set_value_i)
VV_INTERFACE_META(COMPONENT_BASE_EXPORT, test::component_base::to_string_i)
