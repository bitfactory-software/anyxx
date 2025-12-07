#include "templated_anys.hpp"

#include <map>  // NOLINT
#include <string>


//ANY_TEMPLATE_MODEL((std::map<std::string, int>), test::component_base,
//                   any_mutable_map, ((std::string), (int)))
//ANY_TEMPLATE_MODEL((std::map<int, double>), test::component_base, any_map,
//                   ((int), (double)))
//ANY_TEMPLATE_MODEL((std::map<int, double>), test::component_base,
//                   any_mutable_map, ((int), (double)))
