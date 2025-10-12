#include <string>
#include <virtual_void/observer.hpp>
#include <virtual_void/shared_const.hpp>
#include <virtual_void/unique.hpp>
#include <virtual_void/base.hpp>
#include <virtual_void/declare_macro.hpp>
#include <virtual_void/virtual_void.hpp>

using namespace virtual_void;
using namespace virtual_void;

#ifndef VV_DLL_MODE
#pragma message("ERROR! usage of component_base needs #define VV_DLL_MODE")
#endif

#ifndef COMPONENT_BASE_EXPORT
#define COMPONENT_BASE_EXPORT
#endif

#ifdef COMPONENT_BASE_SHARED
#ifdef _MSVC_LANG
#undef COMPONENT_BASE_EXPORT
#define COMPONENT_BASE_EXPORT __declspec(dllexport)
#pragma message("component_base -> dll")
#endif
#pragma message("component_base -> import")
#endif

namespace test::component_base {
struct X;
}

VV_V_TABLE_INSTANCE_FWD(COMPONENT_BASE_EXPORT, test::component_base::X, test::component_base,
                        get_value_i)
VV_V_TABLE_INSTANCE_FWD(COMPONENT_BASE_EXPORT, test::component_base::X, test::component_base,
                        set_value_i)
VV_V_TABLE_INSTANCE_FWD(COMPONENT_BASE_EXPORT, test::component_base::X, test::component_base,
                        to_string_i)

namespace test::component_base {

VV_ANY(to_string_i, (VV_CONST_METHOD(std::string, to_string)))
VV_ANY(get_value_i, (VV_CONST_METHOD(double, get_value)))
VV_ANY_(set_value_i, get_value_i, (VV_METHOD(void, set_value, double)))

COMPONENT_BASE_EXPORT
to_string_i<virtual_void::const_observer> get_to_string_i_co();
COMPONENT_BASE_EXPORT to_string_i<virtual_void::shared_const>
get_to_string_i_sc(double v);
COMPONENT_BASE_EXPORT to_string_i<virtual_void::unique> get_to_string_i_u(
    double v);
COMPONENT_BASE_EXPORT virtual_void::shared_const sc_X(double v);
COMPONENT_BASE_EXPORT virtual_void::unique u_X(double v);

}  // namespace test::component_base
