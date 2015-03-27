#ifndef __EMU_EXCEPTIONS__
#define __EMU_EXCEPTIONS__
#include <stdexcept>
#include <string>

using std::runtime_error;
using std::string;

class HardFault : public runtime_error{
 public:
  explicit HardFault(const string& error_message) :
    runtime_error(error_message) {}
};
#endif
