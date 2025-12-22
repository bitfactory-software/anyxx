#include <catch2/catch_test_macros.hpp>
#include <bit_factory/anyxx.hpp>

using namespace anyxx;

TEST_CASE("hook 1") {
  hook<std::string(std::string const&)> the_hook;

      the_hook.insert([](auto super, std::string const& in) -> std::string {
        CHECK(!super);
        return in + "-0-";
      });

  CHECK(the_hook("hallo") == "hallo-0-");

      the_hook.insert([](auto super, std::string const& in) -> std::string {
        auto intermediate = in + "-1-";
        if (super) return super(intermediate);
        return intermediate;
      });

  CHECK(the_hook("hallo") == "hallo-1--0-");
}

TEST_CASE("hook 2") {
  using hook_t = hook<std::string(std::string const&)>;
  hook_t the_hook;

  hook_t::connection connection1 =
      the_hook.insert([](auto super, std::string const& in) -> std::string {
        CHECK(!super);
        return in + "-0-";
      });

  CHECK(the_hook("hallo") == "hallo-0-");

  hook_t::connection connection2 =
      the_hook.insert([](auto super, std::string const& in) -> std::string {
        auto intermediate = in + "-1-";
        if (super) return super(intermediate);
        return intermediate;
      });

  CHECK(the_hook("hallo") == "hallo-1--0-");

  connection1.close();
  CHECK(the_hook("hallo") == "hallo-1-");
}
