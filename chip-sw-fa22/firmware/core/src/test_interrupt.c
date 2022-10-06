#include "main.h"

extern char str[512];

void testInterrupt() {
  uint8_t payload[]  = {0x1, 0x17, 0x54, 0x45, 0x53, 0x54, 0x20, 0x44, 0x41, 0x54, 0x41, 0x20, 0x46, 0x4f, 0x52, 0x20, 0x42, 0x41, 0x53, 0x45, 0x42, 0x41, 0x4e, 0x44, 0x21};
  GPIOA->HIGH_IE = 0b1;
  // GPIOA->LOW_IE = 0b1;

  // for (uint16_t i=6; i<11; i+=1) {
  for (uint16_t i=1; i<50; i+=1) {
    HAL_PLIC_enable(0, i);
    HAL_PLIC_setPriority(i, 5); // Set all the priorities to 5 for now
  }

  uint64_t target_tick = HAL_getTick() + (6000 * MTIME_FREQ);
  HAL_CLINT_setTimerInterrupt(target_tick);

  HAL_CORE_enableIRQ(MachineSoftware_IRQn);
  HAL_CORE_enableIRQ(MachineTimer_IRQn);
  HAL_CORE_enableIRQ(MachineExternal_IRQn);
  
  HAL_CORE_enableGlobalInterrupt();

  while (1) {
    BLE_send(payload, 12);
    
    sprintf(str, "tick: %lu\ttarget_tick: %lu\n", (uint32_t)HAL_getTick(), (uint32_t)target_tick);
    HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);

    HAL_delay(1000);
  }
}
