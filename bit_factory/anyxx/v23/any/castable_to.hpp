#pragma once

#include <bit_factory/anyxx/any_concept.hpp>

namespace anyxx {

/// \brief Register a model class for a specific any_ interface. Must
/// be in global namespace.
/// \tparam T The model class with fully qualified name
/// \tparam Trait type template.
/// \tparam Args... Optional template parameters for the Trait.
///
/// See also \ref casts.
template <typename T, template <typename, typename...> typename Trait, typename... Args>
struct castable_to {
    constexpr castable_to() {
        bind_v_table_to_meta_data<typename Trait<Args...>::v_table_t, T>();
    }
};

}  // namespace anyxx
