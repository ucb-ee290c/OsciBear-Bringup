#include <unistd.h>
#include <assert.h>
#include "tsi_vcu118.h"

int main(int argc, char** argv)
{
  std::vector<std::string> args(argv + 1, argv + argc);

  // setup the test
  printf("Start test\n");

  tsi_vcu118_t htif(args);

  // John: For debug only
  if (strcmp(argv[1], "+mem") == 0) {
    // write to random addresses

    uint32_t writeArray[4] = { 0xDEADBEEF, 0xABCDEF01, 0x01234567, 0x89ABCDEF };
    htif.memif().write(0x80000000, 16, writeArray);

    // read from random address
    uint32_t readArray[4];
    htif.memif().read( 0x80000000, 16,  readArray);

    // check the array
    for (int i = 0; i < 4; ++i) {
      printf("Asserting that write word 0x%08x == read word 0x%08x\n", writeArray[i], readArray[i]);
      assert(writeArray[i] == readArray[i]);
    }
  } else {
    uint32_t nwords = 32;
    uint32_t bootrom = 0x10000;
    printf("Reading the first %d words of the boot ROM (address 0x%06d) for debug...\n", nwords, bootrom);
    uint32_t readArray[32];
    htif.memif().read( bootrom, 4*nwords, readArray);
    for (uint32_t i = 0; i < nwords; i++) {
      printf("0x%06x: 0x%08x\n", (bootrom + i*4), readArray[i]);
    }
  }

  return 0;
}
