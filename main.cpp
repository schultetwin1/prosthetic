#include <iostream>

#include "core.h"
#include "memory.h"

using std::cout;
using std::endl;

#define SIXTYFOUR_KB (16 << 1)
#define EIGHT_KB     (13 << 1)

int main(int argc, char const * const * const argv) {
  if (argc != 2) {
    cout << "usage: ./emu <bin file>" << endl;
  }

  Memory mem(argv[1], SIXTYFOUR_KB, EIGHT_KB);
  // Booting from flash, so apply map
  mem.applyMap(0x0, 0x08000000, SIXTYFOUR_KB);

  Core core(mem);
  core.run();
  return 0;
}
