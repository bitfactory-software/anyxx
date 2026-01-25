#include "templated_anys.hpp"

#include <map>  // NOLINT
#include <string>

ANY_META_CLASS(std::map<int, double>)
ANY_META_CLASS(std::map<std::string, int>)
ANY_META_CLASS(std::map<std::string, double>)

ANY_REGISTER_MODEL((std::map<std::string, int>),
                   test::component_base::mutable_map, (std::string), (int))
ANY_REGISTER_MODEL((std::map<int, double>), test::component_base::map, (int),
                   (double))
ANY_REGISTER_MODEL((std::map<int, double>), test::component_base::mutable_map,
                   (int), (double))
