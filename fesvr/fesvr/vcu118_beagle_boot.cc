#include <unistd.h>
#include <assert.h>
#include "tsi_vcu118.h"

int main(int argc, char** argv)
{
  std::vector<std::string> args(argv + 1, argv + argc);

  // setup the test
  printf("Beagle Boot Test\n");

  tsi_vcu118_t htif(args);

  // bring rocket out of reset
  printf("Bring Rocket out of reset\n");
  htif.memif().write_uint32(SCR_BASE + SCR_RS_RST, 0);

  // bring boom out of reset
  printf("Bring BOOM out of reset\n");
  htif.memif().write_uint32(SCR_BASE + SCR_BH_RST, 0);

  printf("Beagle Boot Test Done\n");

  return 0;
}
