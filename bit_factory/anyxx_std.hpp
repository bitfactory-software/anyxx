#pragma once

#include <bit_factory/anyxx.hpp>

namespace anyxx {

ANY_TEMPLATE_EX(((ValueType), (Reference)), any_forward_iterator,
                (ANY_OP(anyxx::self &, ++, (), ),
                 ANY_OP(anyxx::self, ++, (int), ),
                 ANY_OP(Reference, *, (), const),
                 ANY_OP_DEFAULTED(bool, ==, equal, (anyxx::self const), const,
                                  ([&x](auto const &r) { return x == r; }))),
                anyxx::value, ,
                (using iterator_category = std::forward_iterator_tag;
                 using difference_type = std::ptrdiff_t;
                 using value_type = ValueType; using reference = Reference;))

ANY_TEMPLATE(
    ((ValueType), (Reference)), any_forward_range,
    (ANY_METHOD((any_forward_iterator<ValueType, Reference>), begin, (), const),
     ANY_METHOD((any_forward_iterator<ValueType, Reference>), end, (), const)),
    anyxx::const_observer, )

}  // namespace anyxx