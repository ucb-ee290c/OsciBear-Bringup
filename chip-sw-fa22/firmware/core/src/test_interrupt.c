#include "main.h"

extern char str[512];

void test_interrupt() {

  HAL_CORE_enableIRQ(MachineSoftware_IRQn);
  HAL_CORE_enableIRQ(MachineTimer_IRQn);
  HAL_CORE_enableIRQ(MachineExternal_IRQn);
  HAL_CORE_enableGlobalInterrupt();

  uint64_t target_tick = HAL_getTick() + (6000 * MTIME_FREQ);
  HAL_CLINT_setTimerInterrupt(target_tick);

  while (1) {
    
    sprintf(str, "tick: %d\ttarget_tick: %d\n", (uint32_t)HAL_getTick(), (uint32_t)target_tick);
    HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);

    HAL_delay(1000);
  }
}
