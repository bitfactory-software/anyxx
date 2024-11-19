#include <cmath>
#include <iostream>
#include <string>
#include <vector>

import virtual_void;
//#include "virtual_void/perfect_typeid_hash/index_table.hpp"

#include <catch.hpp>

TEST_CASE("build perfect typeid hash index") {
  using entry_t = std::pair<perfect_typeid_hash::type_id, const char*>;
  using table_t = std::vector<entry_t>;
  table_t elements = {{&typeid(int), "int"},
                      {&typeid(std::string), "std::string"},
                      {&typeid(entry_t), "entry_t"}};

  auto not_found = "not found";
  perfect_typeid_hash::index_table<const char*> index_table(elements,
                                                            not_found);

  REQUIRE(index_table[&typeid(float)] == not_found);

  for (auto element : elements)
    REQUIRE(index_table[element.first] == element.second);
}

TEST_CASE("typeid perfect typeid hash index inital_sparse_base") {
  std::cout << "typeid perfect typeid hash index inital_sparse_base"
            << std::endl;
  for (int i = 1; i <= 6; ++i) {
    std::size_t size = (std::size_t)std::pow(10.0, i);
    std::cout
        << "| " << size << " | "
        << perfect_typeid_hash::index_table<nullptr_t>::inital_sparse_base(size)
        << " | "<< std::endl;
    }
}

TEST_CASE("typeid perfect typeid hash index table.size()") {
  std::cout << "typeid perfect typeid hash index table.size()"
            << std::endl;
  for (int sparse_base = 4; sparse_base <= 20; ++sparse_base) {
    std::cout
        << " | " << sparse_base << " | "
        << perfect_typeid_hash::index_table<nullptr_t>::size_for_sparse_base(
               sparse_base)
        << " | "<< std::endl;
  }
}
