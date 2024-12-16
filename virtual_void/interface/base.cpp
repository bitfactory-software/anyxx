#include <virtual_void/interface/base.hpp>

template <>
int virtual_void::interface::next_i_table_index_value<virtual_void::const_>() {
  static int i = 0;
  return i++;
};
template <>
int virtual_void::interface::next_i_table_index_value<
    virtual_void::mutable_>() {
  static int i = 0;
  return i++;
};
