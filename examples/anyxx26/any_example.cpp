#include <catch2/catch_test_macros.hpp>
#include <bit_factory/v26/anys/any.hpp>

using namespace anyxx26;

TEST_CASE("anyxx26 std any equivalents") {
    {
        auto a1 = any_copyable{ 42 };
        if(auto p = unerase_cast<int>(a1)){
            CHECK(*p == 42);
        } else{
            CHECK(false);
        }
        if(unerase_cast<double>(a1)){
            CHECK(false);
        } else{
            CHECK(true);
        }
        any_copyable a2 = a1;
        if(auto p = unerase_cast<int>(a2)){
            CHECK(*p == 42);
        } else{
            CHECK(false);
        }
        a1 = std::string{ "hello" };
        if(auto p = unerase_cast<std::string>(a1)){
            CHECK(*p == "hello");
        } else {
            CHECK(false);
        }
        if(auto p = unerase_cast<int>(a2)){
            CHECK(*p == 42);
        } else{
            CHECK(false);
        }
    }
    {
        auto a1 = any_moveable{ std::make_unique<int>(42) };
        if(auto p = unerase_cast<std::unique_ptr<int>>(a1)){
            CHECK(*p->get() == 42);
        } else{
            CHECK(false);
        }
        any_moveable a2 = std::move(a1);
        if(auto p = unerase_cast<std::unique_ptr<int>>(a2)){
            CHECK(*p->get() == 42);
        } else{
            CHECK(false);
        }
		static_assert(!std::copy_constructible<any_moveable>);
    }
}

