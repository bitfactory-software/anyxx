#pragma once

#include <concepts>

#define ANYXX_EXTRACT(...) ANYXX_EXTRACT __VA_ARGS__
#define ANYXX_NOTHING_ANYXX_EXTRACT
#define ANYXX_PASTE(x, ...) x##__VA_ARGS__
#define ANYXX_EVALUATING_PASTE(x, ...) ANYXX_PASTE(x, __VA_ARGS__)
#define ANY_UNPAREN(x) ANYXX_EVALUATING_PASTE(ANYXX_NOTHING_, ANYXX_EXTRACT x)
// usage:
static_assert(std::same_as<ANY_UNPAREN(int), int>);
static_assert(std::same_as<ANY_UNPAREN((int)), int>);

