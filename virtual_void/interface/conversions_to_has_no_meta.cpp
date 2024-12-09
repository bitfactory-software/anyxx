#include <virtual_void/interface/conversions_to_has_no_meta.hpp>

using namespace virtual_void;
using namespace virtual_void::interface;
using namespace virtual_void::data;

VV_FIND_COPY_IMPLEMENT(has_no_meta::const_observer,
                       has_type_info::const_observer)
VV_FIND_COPY_IMPLEMENT(has_no_meta::mutable_observer,
                       has_type_info::mutable_observer)
VV_FIND_COPY_IMPLEMENT(has_no_meta::const_observer, has_type_info::shared_const)
VV_FIND_COPY_IMPLEMENT(has_no_meta::mutable_observer, has_type_info::unique)
