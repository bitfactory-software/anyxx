#pragma once

#include <map>
#include <optional>
#include <typeindex>
#include <vector>
#include <virtual_void/value.hpp>

namespace virtual_void::open_object {

#ifdef VV_DLL_MODE
template <typename OBJECT_TYPE>
std::size_t& members_count();
#else
template <typename OBJECT_TYPE>
std::size_t& members_count() {
  static std::size_t count = 0;
  return count;
}
#endif

template <typename OBJECT_TYPE>
struct members {
  members() : table_(members_count<OBJECT_TYPE>()) {}
  std::vector<value> table_;
  template <typename OBJECT_MEMBER, typename ARG>
  void set(OBJECT_MEMBER member, ARG&& arg) {
    using value_t = typename OBJECT_MEMBER::value_t;
    table_[member.index] =
        make_value<value_t>(std::forward<ARG>(arg));
  }
  template <typename OBJECT_MEMBER>
  typename OBJECT_MEMBER::value_t const* get(OBJECT_MEMBER member) const {
    const auto& value = table_[member.index];
    if (!value) return {};
    return static_cast<typename OBJECT_MEMBER::value_t const*>(value.get());
  }
  template <typename OBJECT_MEMBER>
  typename OBJECT_MEMBER::value_t* get(OBJECT_MEMBER member) {
    auto& value = table_[member.index];
    if (!value) return {};
    return static_cast<typename OBJECT_MEMBER::value_t*>(value.get());
  }
  template <typename OBJECT_MEMBER>
  typename OBJECT_MEMBER::value_t& operator[](OBJECT_MEMBER member) {
    if (auto value = get(member)) {
      return *value;
    }
    using value_t = typename OBJECT_MEMBER::value_t;
    set(member, value_t());
    return *get(member);
  }
};

template <typename OBJECT_TYPE, typename VALUE_TYPE>
struct member {
  using object_t = OBJECT_TYPE;
  using value_t = VALUE_TYPE;
  std::size_t index = members_count<OBJECT_TYPE>()++;
};

}  // namespace virtual_void::open_object

#ifdef VV_DLL_MODE

#define VV_MEMBERS_COUNT_FWD(export_, ns_, c_)   \
  namespace ns_ {                                \
  struct c_;                                     \
  }                                              \
  namespace virtual_void::open_object {          \
  template <>                                    \
  export_ std::size_t& members_count<ns_::c_>(); \
  }

#define VV_MEMBERS_COUNT_IMPL(ns_, c_)                               \
  template <>                                                        \
  std::size_t& virtual_void::open_object::members_count<ns_::c_>() { \
    static std::size_t count = 0;                                    \
    return count;                                                    \
  }

#define VV_MEMBER_FWD(export_, object_, member_, type_)                     \
  export_ virtual_void::open_object::member<object_, type_>&                \
      _inintialize_##member_();                                             \
  inline const virtual_void::open_object::member<object_, type_>& member_ = \
      _inintialize_##member_();

#define VV_MEMBER_IMPL(ns_, object_, member_, type_)                   \
  virtual_void::open_object::member<object_, type_>&                   \
      ns_::_inintialize_##member_() {                                  \
    static virtual_void::open_object::member<object_, type_> instance; \
    return instance;                                                   \
  }

#else

#define VV_MEMBERS_COUNT_FWD(...)
#define VV_MEMBERS_COUNT_IMPL(...)
#define VV_MEMBER_FWD(...)
#define VV_MEMBER_IMPL(...)

#endif
