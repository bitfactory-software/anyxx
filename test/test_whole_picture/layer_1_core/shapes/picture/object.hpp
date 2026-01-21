#pragma once

#include <bit_factory/anyxx.hpp>
#include <test/test_whole_picture/layer_0_architecture/draw/picture.hpp>
#include <test/test_whole_picture/layer_1_core/core.hpp>
#include <test/test_whole_picture/layer_1_core/surface/object.hpp>

ANY_MEMBERS_COUNT_FWD(CORE_EXPORT, whole_picture::core::shapes, picture)

namespace whole_picture::core::shapes {

struct picture : anyxx::members<picture> {
  picture(architecture::point top_left_, surface content_)
      : top_left(top_left_), content(content_) {}
  architecture::point top_left;
  core::surface content;
};

}  // namespace whole_picture::core::shapes

ANY_META_CLASS_FWD(CORE_EXPORT, whole_picture::core::shapes::picture)
ANY_DISPATCH_FOR_FWD(CORE_EXPORT, whole_picture::core::shapes::picture,
                     whole_picture::architecture, any_shape)
ANY_MODEL_MAP((whole_picture::core::shapes::picture),
              whole_picture::architecture::any_surface) {
  void write(auto& self, point p, char ch) { self.content.write(p, ch); };
};

ANY_MODEL_MAP((whole_picture::core::shapes::picture),
              whole_picture::architecture::any_shape) {
  using picture = whole_picture::core::shapes::picture;
  void draw(picture const& self,
            architecture::mutable_observed_surface const& surface) const {
    architecture::draw::picture(surface, self.top_left, self.content);
  };
  [[nodiscard]] auto size(picture const& self) const {
    return self.content.get_size();
  };
  [[nodiscard]] architecture::point top_left(picture const& self) const {
    return self.top_left;
  };
};
