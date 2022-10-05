
#include "main.h"

char str[512];

int main() {
  sprintf(str, "start.\n");
  HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);

  test_interrupt();
}
