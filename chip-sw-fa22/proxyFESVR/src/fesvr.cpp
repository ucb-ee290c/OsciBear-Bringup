#include "tsi.h"
#include "fesvr.h"

#include <stdlib.h>
#include <cstdlib>
#include <stdio.h>

int Fesvr::read(size_t addr, size_t *content, size_t size) {
    for (size_t i = 0; i < size; i++) {
        content[i] = read(addr);
    }
}

int Fesvr::write(size_t addr, size_t *content, size_t size) {
    for (size_t i = 0; i < size; i++) {
        write(addr, content[i]);
    }
}

int Fesvr::loadElf(char* filename, size_t addr) {
    FILE* pFile;
    char buffer[128];

    pFile = fopen(filename, "r");
    if (pFile == NULL) perror("Error opening file");
    else {
        while (!feof(pFile)) {
            size_t txSize = fread(buffer, sizeof(buffer), 1, pFile);
            if (txSize == 0u) break;
            write(addr, ((size_t*)*buffer), txSize);
            addr += 4 * txSize;
        }
        fclose(pFile);
    }
    return 0;
}

int Fesvr::run(size_t addr) {
    write(CLINT_BASE, 0b1u);
}

FesvrFpgaUart::FesvrFpgaUart(uint8_t z, uint8_t o, uint8_t a, uint8_t w, unsigned comport, int brate) {
    port = new TsiFpgaUart(z, o, a, w, comport, brate);

}

size_t FesvrFpgaUart::read(size_t addr) {
    struct TsiPacket tx, rx;
    tx.type = Get;
    tx.size = 2u;
    tx.source = 0u;
    tx.mask = 0b00001111u;
    tx.corrupt = false;
    tx.last = true;
    tx.addr = addr;
    tx.data = 0u;
    port->setLoopback(loopbackEn);
    printf("Read serializing, loopback: %b\n", loopbackEn);
    port->serialize(tx);
    // disable response check, FPGA not working rn
    printf("Read deserializing\n");
    rx = port->deserialize();
    if (loopbackEn) {
        //printf("Read deserializing\n");
        //rx = port->deserialize();
        if (!(rx == tx)) {
            printf("Error: fesvr's driver: Loopback failed.\n");
            //printf("rx: %lx, %lx, %lx, %lx, %lx, %lx, %lx, %lx\n", rx.rawHeader, rx.size, rx.source, rx.mask, rx.corrupt, rx.last, rx.addr, rx.data);
            //printf("tx: %lx, %lx, %lx, %lx, %lx, %lx, %lx, %lx\n", tx.rawHeader, tx.size, tx.source, tx.mask, tx.corrupt, tx.last, tx.addr, tx.data);
        } else {
            printf("Loopback read success.\n");
        }
    } else if (rx.type != AccessAckData) {
        
        printf("Error: PutFullData did not respond with AccessAckData\n!");
    }
    return rx.data;
}

int FesvrFpgaUart::write(size_t addr, size_t content) {
    struct TsiPacket tx, rx;
    tx.type = PutFullData;
    tx.size = 2u;
    tx.source = 0u;
    tx.mask = 0b00001111u;
    tx.corrupt = false;
    tx.last = true;
    tx.addr = addr;
    tx.data = content;
    port->setLoopback(loopbackEn);
    // disable response check, FPGA not working rn
    port->serialize(tx);
    rx = port->deserialize();
    if (loopbackEn) {
        //port->serialize(tx);
        //rx = port->deserialize();
        if (!(rx == tx)) {
            printf("Error: fesvr's driver: Loopback failed.\n");
        } else {
            printf("Loopback write success.\n");
        }
    } else if (rx.type != AccessAck) {
        printf("Error: PutFullData did not respond with AccessAck\n");
    }
    return 0;
}