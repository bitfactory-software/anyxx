#include <string>
#include <anypp/observer.hpp>
#include <anypp/shared_const.hpp>
#include <anypp/unique.hpp>
#include <anypp/any_base.hpp>
#include <anypp/any_meta_class.hpp>

using namespace anypp;

#ifndef ANY_DLL_MODE
#pragma message("ERROR! usage of component_base needs #define ANY_DLL_MODE")
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

ANY_V_TABLE_INSTANCE_FWD(COMPONENT_BASE_EXPORT, test::component_base::X, test::component_base,
                        get_value_i)
ANY_V_TABLE_INSTANCE_FWD(COMPONENT_BASE_EXPORT, test::component_base::X, test::component_base,
                        set_value_i)
ANY_V_TABLE_INSTANCE_FWD(COMPONENT_BASE_EXPORT, test::component_base::X, test::component_base,
                        to_string_i)

namespace test::component_base {

ANY(to_string_i, (ANY_CONST_METHOD(std::string, to_string)))
ANY(get_value_i, (ANY_CONST_METHOD(double, get_value)))
ANY_(set_value_i, get_value_i, (ANY_METHOD(void, set_value, double)))

COMPONENT_BASE_EXPORT
to_string_i<anypp::const_observer> get_to_string_i_co();
COMPONENT_BASE_EXPORT to_string_i<anypp::shared_const>
get_to_string_i_sc(double v);
COMPONENT_BASE_EXPORT to_string_i<anypp::unique> get_to_string_i_u(
    double v);
COMPONENT_BASE_EXPORT anypp::shared_const sc_X(double v);
COMPONENT_BASE_EXPORT anypp::unique u_X(double v);

}  // namespace test::component_base
