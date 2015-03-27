#include <cstring>

#include "exceptions.h"

#include "core.h"

static bool is32BitInstr(uint16_t instr) {
  switch (instr >> 11) {
    case 0x1D:
    case 0x1E:
    case 0x1F:
      return true;

    default:
      return false;
  }
}

static unsigned NumOfSetBits(unsigned i) {
  // Stolen from Hackers Delight
  i = i - ((i >> 1) & 0x55555555);
  i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
  return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

Core::Core(const Memory& mem) : mem(mem) {
  reset();
}

void Core::run() {
  while (1) {
    Core::fetch();
  }
}

void Core::reset() {
  memset(registers, 0x00, sizeof(registers));
  registers[SP] = mem.Read32(0);
  registers[PC] = mem.Read32(4);
}

void Core::fetch() {
  uint16_t instr;
  instr = mem.Read16(registers[PC] & ~(0x1));
  if (is32BitInstr(instr)) {
    decode(mem.Read32(registers[PC] & ~(0x1)));
  } else {
    decode(instr);
  }
}

void Core::decode(uint16_t instr) {
  // PUSH = A6 - 167
  uint8_t opcode = instr >> 9;
  switch (opcode) {
    case 0x5A:
      execute_push(instr);
      break;
    case 0x38:
    case 0x3
      execute_ldr(instr);
      break;
    default:
      throw HardFault("Opcode not recognized");
  }
}

void Core::decode(uint32_t instr) {
  throw HardFault("Instr not implemented");
}

void Core::execute_push(uint16_t instr) {
  unsigned num_regs = NumOfSetBits(instr & 0x01FF);
  unsigned addr = registers[SP] - 4 * num_regs;
  for (int i = 0; i < 8; i++) {
    if ((instr >> i) & 0x1) {
      mem.Write32(addr, registers[i]);
      addr += 4;
    }
  }
  if ((instr >> 8) & 0x1) {
    mem.Write32(addr, registers[LR]);
    addr += 4;
  }
  registers[SP] -= 4 * num_regs;
  registers[PC] += 2;
}

void Core::execute_ldr(uint16_t instr) {
  unsigned addr = registers[(instr >> 3) & 0x7];
  registers[instr & 0x7] = mem.Read32(addr);
  if (instr & 0x7 == PC) {
    if (!(registers[PC] & 0x1)) {
      throw HardFault("Loading an ARM address into PC");
    }
  } else {
    registers[PC] += 2;
  }
}
