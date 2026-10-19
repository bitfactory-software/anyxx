#pragma once

#include <bit_factory/anyxx.hpp>
#include <vector>

namespace anyxx {

#ifdef ANY_DLL_MODE
template <typename InObject>
std::size_t& members_count();
#else
template <typename InObject>
std::size_t& members_count() {
  static std::size_t count = 0;
  return count;
}
#endif

/// \brief A class template to implement extension members.
template <typename InObject>
struct members {
  members() : table_(members_count<InObject>()) {}
  using any_value_t = any_<nullable_val<>, dynamic_copyable>;
  std::vector<any_value_t> table_;
  template <typename Member, typename Arg>
  void set(Member member, Arg&& arg) {
    using value_t = typename Member::value_t;
    table_[member.index] =
        any_value_t{std::in_place_type<value_t>, std::forward<Arg>(arg)};
  }
  template <typename Member>
  typename Member::value_t const* get(Member member) const {
    const auto& val = table_[member.index];
    if (!val) return {};
    return &unchecked_any_cast<typename Member::value_t>(val);
  }
  template <typename Member>
  typename Member::value_t* get(Member member) {
    auto& val = table_[member.index];
    if (!val) return {};
    return &unchecked_any_cast<typename Member::value_t>(val);
  }
  template <typename Member>
  typename Member::value_t& operator[](Member member) {
    if (auto val = get(member)) {
      return *val;
    }
    using value_t = typename Member::value_t;
    set(member, value_t());
    return *get(member);
  }
};

template <typename InObject, typename ValueType>
struct member {
  using object_t = InObject;
  using value_t = ValueType;
  std::size_t index = members_count<InObject>()++;
};

}  // namespace anyxx

#ifdef ANY_DLL_MODE

#define ANY_MEMBERS_COUNT_FWD(export_, ns_, c_)  \
  namespace anyxx {                              \
  template <>                                    \
  export_ std::size_t& members_count<ns_::c_>(); \
  }

#define ANY_MEMBERS_COUNT_IMPL(ns_, c_)          \
  template <>                                    \
  std::size_t& anyxx::members_count<ns_::c_>() { \
    static std::size_t count = 0;                \
    return count;                                \
  }

#define ANY_MEMBER_FWD(export_, object_, member_, type_)           \
  export_ anyxx::member<object_, type_>& _inintialize_##member_(); \
  inline const anyxx::member<object_, type_>& member_ =            \
      _inintialize_##member_();

#define ANY_MEMBER_IMPL(ns_, object_, member_, type_)            \
  anyxx::member<object_, type_>& ns_::_inintialize_##member_() { \
    static anyxx::member<object_, type_> instance;               \
    return instance;                                             \
  }

#else

#define ANY_MEMBERS_COUNT_FWD(...)
#define ANY_MEMBERS_COUNT_IMPL(...)
#define ANY_MEMBER_FWD(...)
#define ANY_MEMBER_IMPL(...)

#endif
