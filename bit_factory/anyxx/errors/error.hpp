#pragma once

#include <source_location>
#include <sstream>
#include <stdexcept>
#include <string>

namespace anyxx {

class error : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;

  static std::string message(std::string_view what,
                             const std::source_location location) {
    std::stringstream s;
    s << "file: " << location.file_name() << '(' << location.line() << ':'
      << location.column() << ") `" << location.function_name() << "`: " << what
      << '\n';
    return s.str();
  }

  error(std::string_view what, const std::source_location location)
      : std::runtime_error(message(what, location)) {}
};

}  // namespace anyxx