#include <catch.hpp>

using namespace Catch::Matchers;

#include <virtual_void/hook.hpp>

using namespace virtual_void;

TEST_CASE("hook 1") {
  hook<std::string(std::string const&)> the_hook;

  auto connection1 =
      the_hook.insert([](auto super, std::string const& in) -> std::string {
        CHECK(!super);
        return in + "-0-";
      });

  CHECK(the_hook("hallo") == "hallo-0-");

  auto connection2 =
      the_hook.insert([](auto super, std::string const& in) -> std::string {
        auto intermediate = in + "-1-";
        if (super) return super(intermediate);
        return intermediate;
      });

  CHECK(the_hook("hallo") == "hallo-1--0-");

  connection1.close();
  CHECK(the_hook("hallo") == "hallo-1-");
}
