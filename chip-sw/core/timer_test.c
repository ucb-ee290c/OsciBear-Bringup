#include "mmio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <ee290c_devices.h>

void timer_handler()
{
  printf("Timer IRQ");
  // Clear the interrupt
  clint_set_mtimecmp((uint64_t)-1);
}

int main(int argc, char** argv)
{
  clint_connect_interrupt(INT_CODE_MACHINE_TIMER, &timer_handler);
  
  printf("Start time %llu\n", clint_get_mtime());

  // Interrupt after 5000us
  clint_set_mtimecmp(clint_get_mtime() + 5000);
  
  mstatus_enable_interrupt();
  
  clint_machine_interrupt_enable(INT_CODE_MACHINE_TIMER);
 
  for (volatile int i = 0; i < 10000; i++)
  {
    asm("");
  }
  
  printf("\nEnd time %llu\n", clint_get_mtime());
  
  // Test exception (this one does work)
  //uint32_t addr = 0x80000000;
  //asm ("lw a0, 1(%0)" :: "r"(addr));
  
  return 0;
}
