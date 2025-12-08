#pragma once

#include <test/test_whole_picture/layer_0_architecture/architecture.hpp>

namespace whole_picture::collision::fallback {

bool intersect(architecture::shape<anyxx::const_observer> const& lhs,
               architecture::shape<anyxx::const_observer> const& rhs);

}  // namespace whole_picture::collision::fallback
