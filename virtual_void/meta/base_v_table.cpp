#include <virtual_void/meta/base_v_table.hpp>

using namespace virtual_void;

meta::base_v_table* meta::base_v_table_imlpementation() {
  static base_v_table v_table{nullptr};
  return &v_table;
}

int meta::next_i_table_index_value() {
  static int i = 0;
  return i++;
};
