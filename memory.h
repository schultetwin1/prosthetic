#ifndef __EMU_MEMORY__
#define __EMU_MEMORY__

#include <cinttypes>
#include <map>
#include <vector>

using std::vector;
using std::map;

class Memory {
 public:
  Memory(const char* filename, unsigned rom_size, unsigned ram_size);
  Memory& operator=(Memory const&) = delete;

  uint32_t Read32(unsigned addr) const;
  void Write32(unsigned addr, uint32_t data);
  uint16_t Read16(unsigned addr) const;
  void Write16(unsigned addr, uint16_t data);
  uint8_t Read8(unsigned addr) const;
  void Write8(unsigned addr, uint8_t data);

  int applyMap(unsigned oldaddr, unsigned newaddr, unsigned size);
  void removeMap(int id);

 private:
  struct MemoryMap {
    unsigned oldaddr;
    unsigned newaddr;
    unsigned size;
  };

  enum AccessType {
    MEM_READ,
    MEM_WRITE
  };

  map<unsigned long, uint8_t> mem;
  vector<MemoryMap> maps;

  unsigned applyMapToAddr(unsigned addr) const;
  void checkAccess(unsigned addr, unsigned alignment, AccessType type) const;
  void writeHalfWord(unsigned addr, uint16_t halfword);
  uint16_t readHalfWord(unsigned addr) const;
  void writeWord(unsigned addr, uint32_t word);
  uint32_t readWord(unsigned addr) const;
};
#endif
