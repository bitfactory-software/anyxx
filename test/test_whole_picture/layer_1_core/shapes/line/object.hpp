#pragma once

#include <bit_factory/anyxx.hpp>
#include <test/test_whole_picture/layer_0_architecture/draw/line.hpp>
#include <test/test_whole_picture/layer_1_core/core.hpp>

namespace whole_picture::core::shapes {

struct line {
  line(architecture::point p1_, architecture::point p2_) : p1(p1_), p2(p2_) {}
  architecture::point p1 = {0, 0}, p2 = {0, 0};
};

}  // namespace whole_picture::core

ANY_META_CLASS_FWD(CORE_EXPORT, whole_picture::core::shapes::line)
ANY_MODEL_FWD(CORE_EXPORT, whole_picture::core::shapes::line, whole_picture::architecture, shape)
ANY_DISPATCH_FOR_FWD(CORE_EXPORT, whole_picture::core::shapes::line, whole_picture::architecture, shape)

ANY_MODEL_MAP((whole_picture::core::shapes::line), whole_picture::architecture::shape) {
    void draw(whole_picture::core::shapes::line const& self,
        architecture::mutable_observed_surface const& surface) const {
        architecture::draw::line(surface, self.p1, self.p2);
    };
    [[nodiscard]] auto size(whole_picture::core::shapes::line const& self) const {
        return architecture::abs(self.p1 - self.p2) +
            architecture::size{ .cx = 1, .cy = 1 };
    };
    [[nodiscard]] architecture::point top_left(whole_picture::core::shapes::line const& self) const {
        auto min = architecture::min(self.p1, self.p2);
        if(self.p1.x > self.p2.x) min.x++;
        if(self.p1.y > self.p2.y) min.y++;
        return min;
    };
};
