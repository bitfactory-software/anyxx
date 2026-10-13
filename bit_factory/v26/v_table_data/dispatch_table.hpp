#pragma once

#include <bit_factory/anyxx/ext/dispatch.hpp>

namespace anyxx26 {

struct dispatch_table {
    using type = anyxx::dispatch_table_t*;
    template<typename Concrete, typename VTable>
    static auto init(VTable*){
        return anyxx::dispatch_table_instance<VTable, Concrete>();
    }
};

}  // namespace anyxx26

