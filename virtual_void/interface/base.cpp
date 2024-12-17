#include <virtual_void/interface/base.hpp>

using namespace virtual_void;

interface::base_v_table* interface::base_v_table_imlpementation() {
  static base_v_table v_table{nullptr};
  return &v_table;
}

int interface::next_i_table_index_value() {
  static int i = 0;
  return i++;
};
