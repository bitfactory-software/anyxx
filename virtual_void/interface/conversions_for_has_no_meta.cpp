#include <virtual_void/interface/conversions_for_has_no_meta.hpp>

using namespace virtual_void;
using namespace virtual_void::interface;
using namespace virtual_void::data;

VV_FIND_COPY_IMPLEMENT(has_no_meta::const_observer,
                       has_no_meta::const_observer)
VV_FIND_COPY_IMPLEMENT(has_no_meta::mutable_observer,
                       has_no_meta::mutable_observer)
VV_FIND_COPY_IMPLEMENT(has_no_meta::shared_const, has_no_meta::shared_const)
VV_FIND_COPY_IMPLEMENT(has_no_meta::unique, has_no_meta::unique)
VV_FIND_COPY_IMPLEMENT(has_no_meta::unique, has_no_meta::shared_const)
VV_FIND_COPY_IMPLEMENT(has_no_meta::const_observer, has_no_meta::unique)
VV_FIND_COPY_IMPLEMENT(has_no_meta::mutable_observer, has_no_meta::unique)

VV_FIND_MOVE_IMPLEMENT(has_no_meta::unique, has_no_meta::unique)
VV_FIND_MOVE_IMPLEMENT(has_no_meta::shared_const, has_no_meta::unique)
