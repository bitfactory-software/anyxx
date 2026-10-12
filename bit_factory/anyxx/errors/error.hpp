#pragma once

#include <stdexcept>

namespace anyxx {

class error : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

}  // namespace anyxx