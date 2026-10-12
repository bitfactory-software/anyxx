#pragma once

#include <bit_factory/anyxx/errors/error.hpp>

namespace anyxx {

class type_mismatch_error : public error {
  using error::error;
};

}  // namespace anyxx