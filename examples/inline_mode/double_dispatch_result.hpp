#pragma once

#include <ostream>
#include <typeinfo>

namespace {
enum class encounter_action {
  snoops_at,
  chases,
  nestle_to,
  hisses_at,
  strolls_with,
  strokes,
  shakes_hands_with
};

struct encounter_result {
  std::type_info const* left;
  encounter_action action;
  std::type_info const* right;
  friend auto operator<=>(encounter_result const&,
                          encounter_result const&) = default;
};

std::ostream& operator<<(std::ostream& os, encounter_result const& result) {
  os << result.left->name() << "-";
  switch (result.action) {
    case (encounter_action::snoops_at):
      os << "snoops_at";
      break;
    case (encounter_action::chases):
      os << "chases";
      break;
    case (encounter_action::nestle_to):
      os << "nestle_to";
      break;
    case (encounter_action::hisses_at):
      os << "hisses_at";
      break;
    case (encounter_action::strolls_with):
      os << "strolls_with";
      break;
    case (encounter_action::strokes):
      os << "strokes";
      break;
    case (encounter_action::shakes_hands_with):
      os << "shakes_hands_with";
      break;
    default:
      os << "UNKNOWN";
  }
  return os << "-" << result.right->name();
}

inline void show(std::string_view label, std::vector<encounter_result> const& results) {
  std::cout << "+++" << label << "\n";
  for (auto const& r : results) std::cout << r << "\n";
  std::cout << "--" << label << "\n";
}

}  // namespace
