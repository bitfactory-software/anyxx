#pragma once

#define VV_IS_A_MUTABLE(class_, interface_)                      \
  virtual_void::interface::is_a<                                 \
      class_, interface_##_v_table<virtual_void::mutable_void>>();

#define VV_IS_A_CONST(class_, interface_)                        \
  virtual_void::interface::is_a<                                 \
      class_, interface_##_v_table<virtual_void::const_void>>(); \
  virtual_void::interface::is_a<                                 \
      class_, interface_##_v_table<virtual_void::mutable_void>>();
