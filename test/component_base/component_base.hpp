#include <bit_factory/anyxx.hpp>
#include <string>

#ifdef __clang__
#pragma GCC diagnostic ignored "-W#pragma-messages"
#endif

#ifndef ANY_DLL_MODE
#pragma message("ERROR! usage of component_base needs #define ANY_DLL_MODE")
#endif

#ifndef COMPONENT_BASE_EXPORT
#define COMPONENT_BASE_EXPORT
#endif

#ifdef COMPONENT_BASE_SHARED
#ifdef _MSC_VER
#undef COMPONENT_BASE_EXPORT
#define COMPONENT_BASE_EXPORT __declspec(dllexport)
#pragma message("component_base -> dll")
#endif
#pragma message("component_base -> import")
#endif
static_assert(anyxx::is_in_dll_mode);

namespace test::component_base {
struct X;
}

namespace test::component_base {

ANY(to_string_i, (ANY_FN(std::string, to_string, (), const)), )
ANY(get_value_i, (ANY_FN(double, get_value, (), const)), )
ANY_(set_value_i, get_value_i, (ANY_FN(void, set_value, (double), )), )

COMPONENT_BASE_EXPORT
any_to_string_i<anyxx::cref> get_to_string_i_co();
COMPONENT_BASE_EXPORT any_to_string_i<anyxx::shared> get_to_string_i_sc(
    double v);
COMPONENT_BASE_EXPORT any_to_string_i<anyxx::unique> get_to_string_i_u(double v);
COMPONENT_BASE_EXPORT anyxx::any<anyxx::shared> sc_X(double v);
COMPONENT_BASE_EXPORT anyxx::any<anyxx::unique> u_X(double v);

}  // namespace test::component_base

