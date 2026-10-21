#pragma once

#define HAS_OPEN_DISPATCH                                    \
  ANY_V_TABLE_DATA(anyxx::dispatch_table_t*, dispatch_table, \
                   anyxx::dispatch_table_instance<v_table_t, Concrete>())
