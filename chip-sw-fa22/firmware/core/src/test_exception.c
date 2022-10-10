
#include "main.h"


void testLoadAccessFault() {
  volatile uint32_t *addr = 0x90000000;
  uint32_t data = *addr;
}

void testException() {
  testLoadAccessFault();
}