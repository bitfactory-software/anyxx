#include <virtual_void/meta/archetype.hpp>

using namespace virtual_void;
using namespace virtual_void::meta;

int meta::next_archetype_index_value() {
  static int i = 0;
  return i++;
};

template <>
meta::archetype_t& meta::archetype_instance<archetype_unspecified>() {
  return archetype_implementation<archetype_unspecified>();
};
