#include <anyxx/anyxx.hpp>
#include <catch.hpp>
#include <cmath>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>

using namespace Catch::Matchers;

using namespace anyxx;

namespace example {

class db {
 public:
  ANY(any_object, (ANY_CONST_METHOD(std::string, type)))
  ANY(any_serializeable, (ANY_CONST_METHOD(void, serialize, std::ostream&)))
  anyxx::factory<any_object, std::string, std::istream&> deserialize;
  struct referenceable {
    int id;
    any_object<shared_const> object;
  };
  std::vector<referenceable> query();
};

}  // namespace example

TEST_CASE("example XX/ riunning object table") {}
