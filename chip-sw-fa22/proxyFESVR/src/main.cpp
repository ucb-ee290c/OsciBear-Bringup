#include "fesvr.h"
#include <string.h>

#define TEST_PATTERN 0xBBADCAFE

int main() {
    // Configure with OsciBear Memory Parameters
    FesvrFpgaUart fesvr(4, 4, 32, 8, 17);
    uint32_t content;
    
    // Start by testing the internal logic of driver. 
    printf("fesvr setup complete, testing loopbacks\n");
    fesvr.setLoopback(true);
    fesvr.read(BOOTROM_BASE);
    fesvr.write(DTIM_BASE, TEST_PATTERN);

    printf("Begin tests...\n");
    fesvr.setLoopback(false);
    content = fesvr.read(BOOTROM_BASE);
    fesvr.write(DTIM_BASE, TEST_PATTERN);
    content = fesvr.read(DTIM_BASE);

    /*
    fesvr.loadElf("", DTIM_BASE);
    fesvr.run();
    */
}