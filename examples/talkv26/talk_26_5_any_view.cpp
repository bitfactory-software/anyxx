#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx/v26/anys/function.hpp>
#include <catch2/catch_test_macros.hpp>
#include <functional>

namespace {

using namespace anyxx;

class container{
    std::vector<int> v{ {1}, {2}, {3}, {4}, {5} };
    std::list<int, 5> l{ {1}, {2}, {3}, {4}, {5} };
    int s{ 1 };
public:
    any<view, bidirectional, int const> get_view(int i) {
        if(i == 0) { return v; } else if(i == 1) { return l; } else { return std::views::single(s); }
    }
};

TEST_CASE("anyxx views") {
    using namespace std::views;
    container c;
    for(auto i : iota(0, 3)) {
        auto it = c.get_view(i).begin();
        ++it; --it;
        CHECK(*it == 1);
    }
}

}
