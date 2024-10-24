#pragma once

#include <assert.h>

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "../../erased/data/has_m_table/has_m_table.h"
#include "../../erased/lifetime/shared_const_trait.h"
#include "../../erased/virtual_void.h"

namespace virtual_void::m_table {
using shared_const_data_ptr =
    erased::data::shared_const_ptr<erased::data::with_m_table>;
}
namespace virtual_void::erased {
template <>
struct data_trait<m_table::shared_const_data_ptr>
    : shared_const_trait<data::has_m_table> {};
}  // namespace virtual_void::erased
namespace virtual_void::m_table {

static_assert(erased::is_data_pointer<shared_const_data_ptr>);

using shared_const = erased::virtual_void<shared_const_data_ptr>;
template <typename T>
using typed_shared_const =
    erased::virtual_typed<T const, shared_const_data_ptr>;

template <typename T, typename... ARGS>
auto make_shared_const(ARGS&&... args) {
  return erased::data::make_shared_const<erased::data::holder<const T, erased::data::has_m_table>>(
      std::forward<ARGS>(args)...);
}

static_assert(erased::is_virtual_void<shared_const>);
static_assert(erased::is_virtual_void<typed_shared_const<int>>);

}  // namespace virtual_void::m_table