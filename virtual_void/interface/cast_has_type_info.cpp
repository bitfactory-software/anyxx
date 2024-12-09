#include <virtual_void/interface/cast_has_type_info.hpp>

using namespace virtual_void;
using namespace virtual_void::interface;
using namespace virtual_void::data;

using cast_domain_t = virtual_void::open_method::via_type_info::domain;



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

VV_FIND_COPY_IMPLEMENT(has_type_info::const_observer,
                       has_type_info::const_observer)
VV_FIND_COPY_IMPLEMENT(has_type_info::mutable_observer,
                       has_type_info::mutable_observer)
VV_FIND_COPY_IMPLEMENT(has_type_info::shared_const, has_type_info::shared_const)
VV_FIND_COPY_IMPLEMENT(has_type_info::unique, has_type_info::unique)
VV_FIND_COPY_IMPLEMENT(has_type_info::unique, has_type_info::shared_const)
VV_FIND_COPY_IMPLEMENT(has_type_info::const_observer, has_type_info::unique)
VV_FIND_COPY_IMPLEMENT(has_type_info::mutable_observer, has_type_info::unique)

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

VV_FIND_MOVE_IMPLEMENT(has_type_info::unique, has_type_info::unique)
VV_FIND_MOVE_IMPLEMENT(has_type_info::shared_const, has_type_info::unique)
