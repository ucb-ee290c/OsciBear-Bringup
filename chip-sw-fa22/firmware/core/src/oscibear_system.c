
#include "oscibear_hal.h"

#include "main.h"

void UserSoftware_IRQn_Handler() {}

void SupervisorSoftware_IRQn_Handler() {}

void HypervisorSoftware_IRQn_Handler() {}

void MachineSoftware_IRQn_Handler() {
  char str[32];
  sprintf(str, "machine software irq\n");
  HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
}

void UserTimer_IRQn_Handler() {}

void SupervisorTimer_IRQn_Handler() {}

void HypervisorTimer_IRQn_Handler() {}

void MachineTimer_IRQn_Handler() {
  char str[32];
  sprintf(str, "machine timer irq\n");
  HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
}

void UserExternal_IRQn_Handler() {}

void SupervisorExternal_IRQn_Handler() {}

void HypervisorExternal_IRQn_Handler() {}

void MachineExternal_IRQn_Handler() {
  // uint32_t irq_source = HAL_PLIC_claimIRQ(0);
  uint32_t irq_source = -1;

      
  char str[32];
  sprintf(str, "machine external irq: %d\n", irq_source);
  HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
  
  // switch (irq_source) {

  // }

  // if (irqSource == 6) {
  //   sprintf(str, "** RX Error Message: %u\n", baseband_rxerror_message());
  // }
  // if (irqSource == 7) {
  //   sprintf(str, "** RX Start\n");
  // }
  // if (irqSource == 8) {
  //   sprintf(str, "** Bytes Read: %u\n", baseband_rxfinish_message());
  // }
  // if (irqSource == 9) {
  //   sprintf(str, "TX Operation Failed. Error message: %u\n", baseband_txerror_message());
  // }
  // if (irqSource == 10) {
  //   sprintf(str, "TX Operation Finished. Check above for any errors.\n");
  // }

  // plic_complete_irq(0, irqSource);
  // HAL_PLIC_completeIRQ(0, irq_source);
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
      HAL_CORE_clearIRQ(MachineSoftware_IRQn);
      break;
    case 0x80000007:      // machine timer interrupt
      MachineTimer_IRQn_Handler();
      HAL_CORE_clearIRQ(MachineTimer_IRQn);
      break;
    case 0x8000000B:      // machine external interrupt
      MachineExternal_IRQn_Handler();
      HAL_CORE_clearIRQ(MachineExternal_IRQn);
      break;
  }
}

