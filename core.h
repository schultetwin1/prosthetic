#ifndef __EMU_CORE__
#define __EMU_CORE__
#include "memory.h"

class Core {
 public:
  Core(const Memory& mem);
  void reset();
  void run();

 private:
  enum RegisterNames {
    SP = 13,
    LR = 14,
    PC = 15
  };
  unsigned registers[16];
  void fetch();
  void decode(uint16_t instr);
  void decode(uint32_t instr);

  // All the different executes
  void execute_push(uint16_t instr);
  void execute_ldr(uint16_t instr);

  Memory mem;
};
#endif
