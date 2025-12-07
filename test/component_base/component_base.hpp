#include <bit_factory/anyxx.hpp>
#include <string>
using namespace anyxx;

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
static_assert(anyxx::is_in_dll_mode);

namespace test::component_base {
struct X;
}

namespace test::component_base {

ANY(to_string_i, (ANY_METHOD(std::string, to_string, (), const)))
ANY(get_value_i, (ANY_METHOD(double, get_value, (), const)))
ANY_(set_value_i, get_value_i, (ANY_METHOD(void, set_value, (double), )))

COMPONENT_BASE_EXPORT
to_string_i<anyxx::const_observer> get_to_string_i_co();
COMPONENT_BASE_EXPORT to_string_i<anyxx::shared_const> get_to_string_i_sc(
    double v);
COMPONENT_BASE_EXPORT to_string_i<anyxx::unique> get_to_string_i_u(double v);
COMPONENT_BASE_EXPORT anyxx::shared_const sc_X(double v);
COMPONENT_BASE_EXPORT anyxx::unique u_X(double v);

}  // namespace test::component_base

ANY_MODEL_FWD(COMPONENT_BASE_EXPORT, test::component_base::X,
              test::component_base, get_value_i)
ANY_MODEL_FWD(COMPONENT_BASE_EXPORT, test::component_base::X,
              test::component_base, set_value_i)
ANY_MODEL_FWD(COMPONENT_BASE_EXPORT, test::component_base::X,
              test::component_base, to_string_i)
