#include <virtual_void/interface/conversions_for_has_type_info.hpp>

using namespace virtual_void;
using namespace virtual_void::interface;
using namespace virtual_void::data;


VV_FIND_COPY_IMPLEMENT(has_type_info::const_observer,
                       has_type_info::const_observer)
VV_FIND_COPY_IMPLEMENT(has_type_info::mutable_observer,
                       has_type_info::mutable_observer)
VV_FIND_COPY_IMPLEMENT(has_type_info::shared_const, has_type_info::shared_const)
VV_FIND_COPY_IMPLEMENT(has_type_info::unique, has_type_info::unique)
VV_FIND_COPY_IMPLEMENT(has_type_info::unique, has_type_info::shared_const)
VV_FIND_COPY_IMPLEMENT(has_type_info::const_observer, has_type_info::unique)
VV_FIND_COPY_IMPLEMENT(has_type_info::mutable_observer, has_type_info::unique)

VV_FIND_MOVE_IMPLEMENT(has_type_info::unique, has_type_info::unique)
VV_FIND_MOVE_IMPLEMENT(has_type_info::shared_const, has_type_info::unique)
