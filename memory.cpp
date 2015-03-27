#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

#include "exceptions.h"
#include "intelhexclass.h"
#include "memory.h"

using std::ifstream;
using std::streampos;
using std::ios;
using std::string;
using std::to_string;

Memory::Memory(const char* filename, unsigned rom_size, unsigned ram_size) 
    : maps(0) {
  intelhex reader;
  ifstream memory_file(filename, ios::in | ios::binary);
  assert(memory_file.is_open());
  memory_file >> reader;
  unsigned long end_address = 0;
  reader.endAddress(&end_address);

  reader.begin();
  while (reader.currentAddress() != end_address) {
    unsigned char data = 0;
    reader.getData(&data);
    mem[reader.currentAddress()] = data;
    reader++;
  }
}

uint32_t Memory::Read32(unsigned addr) const {
  addr = applyMapToAddr(addr);
  checkAccess(addr, 4, MEM_READ);
  return readWord(addr);
}

void Memory::Write32(unsigned addr, uint32_t data) {
  addr = applyMapToAddr(addr);
  checkAccess(addr, 4, MEM_WRITE);
  writeWord(addr, data);
}

uint16_t Memory::Read16(unsigned addr) const {
  addr = applyMapToAddr(addr);
  checkAccess(addr, 2, MEM_READ);
  return readHalfWord(addr);
}

void Memory::Write16(unsigned addr, uint16_t data) {
  addr = applyMapToAddr(addr);
  checkAccess(addr, 2, MEM_WRITE);
  writeHalfWord(addr, data);
}

uint8_t Memory::Read8(unsigned addr) const {
  addr = applyMapToAddr(addr);
  checkAccess(addr, 1, MEM_READ);
  return mem.at(addr);
}

void Memory::Write8(unsigned addr, uint8_t data) {
  addr = applyMapToAddr(addr);
  checkAccess(addr, 1, MEM_WRITE);
  mem[addr] = data;
}

int Memory::applyMap(unsigned oldaddr, unsigned newaddr, unsigned size) {
  // Find idx
  int idx = 0;
  for (idx = 0; idx < (maps.size() && maps[idx].size != 0); idx++)
    ;
  maps.emplace(maps.begin() + idx, MemoryMap{oldaddr, newaddr, size});
  return idx;
}

void Memory::removeMap(int id) {
  maps[id].size = 0;
}

void Memory::checkAccess(unsigned addr, unsigned alignment, AccessType type) const {
  if (addr % alignment != 0) {
    throw HardFault("Unaligned load or store");
  }

  switch (addr & 0xF0000000) {
    // ROM
    case 0x00000000:
    case 0x10000000:
      if (type == MEM_WRITE) {
        throw HardFault("Writing to ROM");
      }
      break;

    // RAM
    case 0x20000000:
    case 0x30000000:
      if (addr >= 0x20002000) {
        throw HardFault("Invalid SRAM Address");
      }
      break;

    // Peripherals
    case 0x40000000:
    case 0x50000000:
      // @TODO
      break;

    // External RAM
    case 0x60000000:
    case 0x70000000:
    case 0x80000000:
    case 0x90000000:
      // @TODO:
      break;

    // External device
    case 0xA0000000:
    case 0xB0000000:
    case 0xC0000000:
    case 0xD0000000:
      // @TODO
      break;

    default:
      //@TODO:
      break;
  }
}

unsigned Memory::applyMapToAddr(unsigned addr) const {
  for (unsigned i = 0; i < maps.size(); i++) {
    if (addr >= maps[i].oldaddr && addr < maps[i].oldaddr + maps[i].size) {
      addr = (addr - maps[i].oldaddr) + maps[i].newaddr;
      return addr;
    }
  }
  return addr;
}

void Memory::writeHalfWord(unsigned addr, uint16_t halfword) {
  mem[addr] = halfword & 0xFF;
  mem[addr + 1] = (halfword >> 8) & 0xFF;
}

uint16_t Memory::readHalfWord(unsigned addr) const {
  uint16_t halfword = mem.at(addr);
  halfword |= (uint16_t)(mem.at(addr + 1)) << 8;
  return halfword;
}

void Memory::writeWord(unsigned addr, uint32_t word) {
  mem[addr] = word & 0xFF;
  mem[addr + 1] = (word >> 8) & 0xFF;
  mem[addr + 2] = (word >> 16) & 0xFF;
  mem[addr + 3] = (word >> 24) & 0xFF;
}

uint32_t Memory::readWord(unsigned addr) const {
  uint32_t word;
  word = mem.at(addr);
  word |= (uint32_t)(mem.at(addr + 1)) << 8;
  word |= (uint32_t)(mem.at(addr + 2)) << 16;
  word |= (uint32_t)(mem.at(addr + 3)) << 24;
  return word;
}
