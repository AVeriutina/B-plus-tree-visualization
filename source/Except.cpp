#include "Except.h"

#include <iostream>

namespace Except {

void React() {
  try {
    throw;
  } catch (...) {
    std::cout << "Unknown exeption." << std::endl;
  }
}

}  // namespace Except
