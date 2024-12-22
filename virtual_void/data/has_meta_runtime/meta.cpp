#pragma once

#include <virtual_void/data/has_meta_runtime/meta.hpp>
#include <virtual_void/meta/class.hpp>

VV_EXPORT std::type_info const*
virtual_void::data::has_meta_runtime::get_std_type_info(
    virtual_void::data::has_meta_runtime::meta const& m) {
  return &m.type_info()->get_type_info();
}
