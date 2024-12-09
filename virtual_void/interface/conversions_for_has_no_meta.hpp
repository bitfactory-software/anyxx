#pragma once

#undef interface

#include <virtual_void/data/has_no_meta/observer.hpp>
#include <virtual_void/data/has_no_meta/shared_const.hpp>
#include <virtual_void/data/has_no_meta/unique.hpp>
#include <virtual_void/data/has_no_meta/value.hpp>
#include <virtual_void/interface/conversions_macros.hpp>

VV_FIND_COPY_DECLARE(has_no_meta::const_observer,
                     has_no_meta::const_observer)
VV_FIND_COPY_DECLARE(has_no_meta::mutable_observer,
                     has_no_meta::mutable_observer)
VV_FIND_COPY_DECLARE(has_no_meta::shared_const, has_no_meta::shared_const)
VV_FIND_COPY_DECLARE(has_no_meta::unique, has_no_meta::unique)
VV_FIND_COPY_DECLARE(has_no_meta::unique, has_no_meta::shared_const)
VV_FIND_COPY_DECLARE(has_no_meta::const_observer, has_no_meta::unique)
VV_FIND_COPY_DECLARE(has_no_meta::mutable_observer, has_no_meta::unique)

VV_FIND_MOVE_DECLARE(has_no_meta::unique, has_no_meta::unique)
VV_FIND_MOVE_DECLARE(has_no_meta::shared_const, has_no_meta::unique)
