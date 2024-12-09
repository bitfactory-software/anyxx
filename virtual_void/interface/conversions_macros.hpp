#pragma once

#undef interface

#include <virtual_void/interface/conversion.hpp>
#include <virtual_void/utillities/VV_EXPORT.hpp>

#define VV_FIND_COPY_DECLARE(to, from)                                 \
  namespace virtual_void::interface {                                  \
  template <>                                                          \
  VV_EXPORT extern find_copy_factory_method<virtual_void::data::to,    \
                                            virtual_void::data::from>& \
  find_copy<virtual_void::data::to, virtual_void::data::from>();       \
  }

#define VV_FIND_COPY_IMPLEMENT(to, from)                                      \
  template <>                                                                 \
  find_copy_factory_method<virtual_void::data::to, virtual_void::data::from>& \
  virtual_void::interface::find_copy<virtual_void::data::to,                  \
                                     virtual_void::data::from>() {            \
    static find_copy_factory_method<virtual_void::data::to,                   \
                                    virtual_void::data::from>                 \
        find_copy_method{virtual_void::interface::cast_domain()};             \
    return find_copy_method;                                                  \
  }


#define VV_FIND_MOVE_DECLARE(to, from)                                 \
  namespace virtual_void::interface {                                  \
  template <>                                                          \
  VV_EXPORT extern find_move_factory_method<virtual_void::data::to,    \
                                            virtual_void::data::from>& \
  find_move<virtual_void::data::to, virtual_void::data::from>();       \
  }

#define VV_FIND_MOVE_IMPLEMENT(to, from)                                      \
  template <>                                                                 \
  find_move_factory_method<virtual_void::data::to, virtual_void::data::from>& \
  virtual_void::interface::find_move<virtual_void::data::to,                  \
                                     virtual_void::data::from>() {            \
    static find_move_factory_method<virtual_void::data::to,                   \
                                    virtual_void::data::from>                 \
        find_move_method{virtual_void::interface::cast_domain()};             \
    return find_move_method;                                                  \
  }
