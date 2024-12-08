#include <virtual_void/interface/cast.hpp>

using namespace virtual_void;

interface::cast_domain_t& interface::cast_domain() {
  static interface::cast_domain_t domain;
  return domain;
}

void interface::seal_casts() { seal_for_runtime(cast_domain()); }
