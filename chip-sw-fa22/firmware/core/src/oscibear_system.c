
#include "oscibear_hal.h"

#include "main.h"


__attribute__((weak)) void GPIO_IRQn_Handler() {

}

void UserSoftware_IRQn_Handler() {}

void SupervisorSoftware_IRQn_Handler() {}

void HypervisorSoftware_IRQn_Handler() {}

void MachineSoftware_IRQn_Handler() {
  {
    char str[32];
    sprintf(str, "machine software irq\n");
    HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
  }
}

void UserTimer_IRQn_Handler() {}

void SupervisorTimer_IRQn_Handler() {}

void HypervisorTimer_IRQn_Handler() {}

void MachineTimer_IRQn_Handler() {
  {
    char str[32];
    sprintf(str, "machine timer irq\n");
    HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
  }
}

void UserExternal_IRQn_Handler() {}

void SupervisorExternal_IRQn_Handler() {}

void HypervisorExternal_IRQn_Handler() {}

void MachineExternal_IRQn_Handler() {
  uint32_t irq_source = HAL_PLIC_claimIRQ(0);

  {
    char str[32];
    sprintf(str, "machine external irq: %d\n", irq_source);
    HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
  }
  switch (irq_source) {
    case 2:                 // GPIO interrupt
      GPIO_IRQn_Handler();
      GPIOA->HIGH_IE = 0b0;
      break;
    case 6:               // baseband RX error interrupt
      //   sprintf(str, "** RX Error Message: %u\n", baseband_rxerror_message());
      break;
    case 7:               // baseband RX start interrupt
      //   sprintf(str, "** RX Start\n");
      break;
    case 8:               // baseband RX finish interrupt
      //   sprintf(str, "** Bytes Read: %u\n", baseband_rxfinish_message());
      break;
    case 9:               // baseband RX error interrupt
      //   sprintf(str, "TX Operation Failed. Error message: %u\n", baseband_txerror_message());
      break;
    case 10:              // baseband TX finish interrupt
      //   sprintf(str, "TX Operation Finished. Check above for any errors.\n");
      break;
  }
      
  HAL_PLIC_completeIRQ(0, irq_source);
}

void system_init(void) {
  // TODO: these should really go into main(), but putting here temporarily for ease of testing
  HAL_init();

  UART_InitTypeDef UART_init_config;
  UART_init_config.baudrate = 10000;

  HAL_UART_init(UART0, &UART_init_config);
  HAL_GPIO_init(GPIOA, GPIO_PIN_0);
  return;
}

void trap_handler(void) {
  {
    char str[32];
    sprintf(str, "traphandler\n");
    HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
  }
  uint32_t m_cause;
  asm volatile("csrr %0, mcause" : "=r"(m_cause));

  switch (m_cause) {
    case 0x00000000:      // instruction address misaligned
      break;
    case 0x00000001:      // instruction access fault
      break;
    case 0x00000002:      // illegal instruction
      break;
    case 0x00000003:      // breakpoint
      break;
    case 0x00000004:      // load address misaligned
      break;
    case 0x00000005:      // load access fault
      break;
    case 0x00000006:      // store/AMO address misaligned
      break;
    case 0x00000007:      // store/AMO access fault
      break;
    case 0x00000008:      // environment call from U-mode
      break;
    case 0x00000011:      // environment call from M-mode
      break;
    case 0x80000003:      // machine software interrupt
      MachineSoftware_IRQn_Handler();
      CLINT->MSIP = 0;
      HAL_CORE_clearIRQ(MachineSoftware_IRQn);
      break;
    case 0x80000007:      // machine timer interrupt
      MachineTimer_IRQn_Handler();
      HAL_CLINT_setTimerInterrupt(0xFFFFFFFFFFFFFFFF);
      HAL_CORE_clearIRQ(MachineTimer_IRQn);
      break;
    case 0x8000000B:      // machine external interrupt
      MachineExternal_IRQn_Handler();
      HAL_CORE_clearIRQ(MachineExternal_IRQn);
      break;
  }
}

