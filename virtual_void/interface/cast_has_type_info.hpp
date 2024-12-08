#pragma once

#undef interface

#include <virtual_void/data/has_type_info/observer.hpp>
#include <virtual_void/data/has_type_info/shared_const.hpp>
#include <virtual_void/data/has_type_info/unique.hpp>
#include <virtual_void/data/has_type_info/value.hpp>
#include <virtual_void/interface/cast.hpp>
#include <virtual_void/utillities/VV_EXPORT.hpp>

#define VV_FIND_COPY_DECLARE(to, from)                                 \
  namespace virtual_void::interface {                                  \
  template <>                                                          \
  VV_EXPORT extern find_copy_factory_method<virtual_void::data::to,    \
                                            virtual_void::data::from>& \
  find_copy<virtual_void::data::to, virtual_void::data::from>();       \
  }

VV_FIND_COPY_DECLARE(has_type_info::const_observer,
                     has_type_info::const_observer)
VV_FIND_COPY_DECLARE(has_type_info::mutable_observer,
                     has_type_info::mutable_observer)
VV_FIND_COPY_DECLARE(has_type_info::shared_const, has_type_info::shared_const)
VV_FIND_COPY_DECLARE(has_type_info::unique, has_type_info::unique)
VV_FIND_COPY_DECLARE(has_type_info::unique, has_type_info::shared_const)
VV_FIND_COPY_DECLARE(has_type_info::const_observer, has_type_info::unique)
VV_FIND_COPY_DECLARE(has_type_info::mutable_observer, has_type_info::unique)

#define VV_FIND_MOVE_DECLARE(to, from)                                 \
  namespace virtual_void::interface {                                  \
  template <>                                                          \
  VV_EXPORT extern find_move_factory_method<virtual_void::data::to,    \
                                            virtual_void::data::from>& \
  find_move<virtual_void::data::to, virtual_void::data::from>();       \
  }

VV_FIND_MOVE_DECLARE(has_type_info::unique, has_type_info::unique)
VV_FIND_MOVE_DECLARE(has_type_info::shared_const, has_type_info::unique)
