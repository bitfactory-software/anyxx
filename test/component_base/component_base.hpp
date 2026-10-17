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

TRAIT(to_string_i, anyxx::dynamic_deletable,
      (FN(std::string, to_string, (), const)))
TRAIT(get_value_i, anyxx::dynamic_smart_ptr, (FN(double, get_value, (), const)))
TRAIT(set_value_i, get_value_i, (FN(void, set_value, (double), )))

COMPONENT_BASE_EXPORT
anyxx::any<anyxx::cref, to_string_i> get_to_string_i_co();
COMPONENT_BASE_EXPORT anyxx::any<anyxx::shared, to_string_i> get_to_string_i_sc(
    double v);
COMPONENT_BASE_EXPORT anyxx::any<anyxx::unique, to_string_i> get_to_string_i_u(
    double v);
COMPONENT_BASE_EXPORT anyxx::any<anyxx::shared, anyxx::dynamic_deletable> sc_X(
    double v);
COMPONENT_BASE_EXPORT anyxx::any<anyxx::unique, anyxx::dynamic_deletable> u_X(
    double v);

}  // namespace test::component_base
