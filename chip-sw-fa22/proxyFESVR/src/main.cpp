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
    //fesvr.setLoopback(true);
    //fesvr.read(BOOTROM_BASE);
    //fesvr.write(DTIM_BASE, TEST_PATTERN);

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
    for (size_t i = 0; i < 1; i++) {
        content = fesvr.read(BOOTROM_BASE + 4*i);
        printf("Read at BOOTROM_BASE + %lX complete, recieved 0x%X \n", i*4, content);
    }

    printf("Press any key to set flag.");
    getchar();
    printf("Setting flag at DTIM_RET \n");
    fesvr.write(DTIM_RET, 0);
    printf("Trying to read back to confirm results \n");
    content = fesvr.read(DTIM_RET);
    printf("Read %X from DTIM_RET \n", content);

    printf("Toggling GPIO 0 pin high \n");
    fesvr.write(GPIO_BASE + 0xC, 1);
    fesvr.write(GPIO_BASE + 0x8, 1);

    printf("Loading and running... \n");
    //loadElf(&fesvr, "./hello.debug.riscv", DTIM_BASE);
    fesvr.write(DTIM_BASE, 0x80004537);
    fesvr.write(DTIM_BASE + 4, 0x00050513);
    fesvr.write(DTIM_BASE + 8, 0x00F00593);
    fesvr.write(DTIM_BASE + 12, 0x00B52023);
    run(&fesvr);

    content = 0;
    while (content == 0) {
        sleep(1);
        content = fesvr.read(DTIM_RET);
        printf("Poll finish got %X\n", content);
    }

    printf("Finished running, got %X \n", content);
    //printf("%X \n", fesvr.read(DTIM_RET + 4));
}