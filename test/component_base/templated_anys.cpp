#include "templated_anys.hpp"

#include <map>  // NOLINT
#include <string>

ANY_META_CLASS(std::map<int, double>)
ANY_META_CLASS(std::map<std::string, int>)
ANY_META_CLASS(std::map<std::string, double>)

namespace{
anyxx::register_trait<std::map<std::string, int>,
                      test::component_base::mutable_map, std::string, int>
    __;
anyxx::register_trait<std::map<int, double>, test::component_base::map, int,
                      double>
    __;
anyxx::register_trait<std::map<int, double>, test::component_base::mutable_map,
                      int, double>
    __;
}
