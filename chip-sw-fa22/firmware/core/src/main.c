
#include "main.h"


int main() {
  {
    char str[512];
    sprintf(str, "start.\n");
    HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
  }

  // testException();
  // testInterrupt();

  // testAES_ECB_singlePoll();
  // testAES_ECB_singleInt();

  // testBasebandTX();
  testBasebandDigitalLoopback();
}
