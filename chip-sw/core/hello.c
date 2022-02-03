#include "mmio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <ee290c_devices.h>

struct ee290c_uart uart;

int main(int argc, char** argv)
{  
  char hello_uart_string[] = "Hello from UART\n";
  
  ee290c_uart_init(EE90C_UART_DEFAULT_ADDR, &uart);
  
  ee290c_uart_puts(&uart, hello_uart_string);
 
  printf("Hello world!\n");
 
  for (volatile int i = 0; i < 50000; i++)
  {
    asm("");
  }
  
  return 0;
}
