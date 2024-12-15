#pragma once

#define VV_IS_A(class_, interface_) \
  virtual_void::interface::is_a<class_, interface_##_v_table>();

