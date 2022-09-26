#include <unistd.h>
#include <assert.h>
#include "tsi_vcu118.h"

int main(int argc, char** argv)
{
  std::vector<std::string> args(argv + 1, argv + argc);

  tsi_vcu118_t htif(args);

  uint64_t r_addr = 0;
  uint64_t w_addr = 0;
  uint32_t w_data = 0;

  for (auto& arg : args) {
      if (!arg.find("+r=")) {
          r_addr = std::stoull(arg.c_str() + strlen("+r="), 0, 16);

          assert(r_addr > 0);

          uint32_t r_data = htif.memif().read_uint32(r_addr);
          printf("READ: Got 0x%x from 0x%lx\n", r_data, r_addr);
      }
      else if (!arg.find("+w=")) {
          w_addr = std::stoull(arg.substr(strlen("+w="), arg.find(",") - strlen("+w=")), 0, 16);
          w_data = std::stoull(arg.c_str() + arg.find(",") + 1, 0, 16);

          assert(w_addr > 0);

          htif.memif().write_uint32(w_addr, w_data);
          printf("WRITE: Wrote 0x%x to 0x%lx\n", w_data, w_addr);
      }
  }

  return 0;
}
