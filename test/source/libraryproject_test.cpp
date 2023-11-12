#include "libraryproject/libraryproject.hpp"

auto main() -> int
{
  auto const result = name();

  return result == "libraryproject" ? 0 : 1;
}
