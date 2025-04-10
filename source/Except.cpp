#include "Except.h"

#include <iostream>

namespace Except {

void React() {
  try {
    throw;
  } catch (...) {
    std::cout << "Unknown exception." << std::endl;
  }
}

}  // namespace Except
