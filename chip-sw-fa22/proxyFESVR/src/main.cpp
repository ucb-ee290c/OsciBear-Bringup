#include "fesvr.h"
#include <string.h>

#define TEST_PATTERN 0xBBADCAFE

int main() {
    // Configure with OsciBear Memory Parameters & comport
    // FesvrFpgaUart(uint8_t z, uint8_t o, uint8_t a, uint8_t w, unsigned comport);
    // Comport designation see: https://github.com/FlorianBauer/rs232
    FesvrFpgaUart fesvr(4, 4, 32, 8, 17, 2000000);
    uint32_t content;
    printf("Resetting adapter and flushing RXTX\n");
    fesvr.reset();

    // Start by testing the internal logic of driver. 
    printf("fesvr setup complete, testing loopbacks\n");
    fesvr.setLoopback(true);
    fesvr.read(BOOTROM_BASE);
    fesvr.write(DTIM_BASE, TEST_PATTERN);

    printf("Begin tests...\n");
    // fesvr checks return packet, will hang, Ctrl-C to exit. Override will destroy loopback test.
    /*  Usable Functions: 
        int read(size_t addr, size_t *content, size_t size);
        int write(size_t addr, size_t *content, size_t size);
        virtual size_t read(size_t addr);
        virtual int write(size_t addr, size_t content);
        int loadElf(char* file, size_t addr);
        int run(size_t addr);
    */
    fesvr.setLoopback(false);
    //content = fesvr.read(BOOTROM_BASE);
    fesvr.write(DTIM_BASE, TEST_PATTERN);
    //content = fesvr.read(DTIM_BASE);

    /*
    fesvr.loadElf("", DTIM_BASE);
    fesvr.run();
    */
}