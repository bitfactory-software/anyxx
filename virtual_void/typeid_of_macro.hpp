#pragma once

#define VV_TYPEID_OF_DECLARE(export_macro, type) \
  template <>                                    \
  export_macro const std::type_info& virtual_void::typeid_of<type>();

#define VV_TYPEID_OF_DEFINE(type)                         \
  template <>                                             \
  const std::type_info& virtual_void::typeid_of<type>() { \
    return typeid(type);                                  \
  }
