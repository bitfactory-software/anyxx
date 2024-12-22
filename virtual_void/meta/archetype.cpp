#include <virtual_void/meta/archetype.hpp>

using namespace virtual_void;
using namespace virtual_void::meta;

int meta::next_archetype_index_value() {
  static int i = 0;
  return i++;
};

template<>
archetype& virtual_void::meta::runtime<archetype, archetype_unspecified>(){
  return runtime_implementation<archetype, archetype_unspecified>();
};
