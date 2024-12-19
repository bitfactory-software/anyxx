#include <virtual_void/meta/i_table.hpp>

using namespace virtual_void;


int meta::next_i_table_index_value() {
  static int i = 0;
  return i++;
};
