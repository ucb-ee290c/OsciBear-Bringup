
#include "oscibear_hal.h"

#include "main.h"


__attribute__((weak)) void HAL_BASEBAND_RXErrorCallback(BASEBAND_TypeDef *BASEBANDx, uint32_t interrupt_id) {
  //   sprintf(str, "** RX Error Message: %u\n", baseband_rxerror_message());
}

__attribute__((weak)) void HAL_BASEBAND_RXStartCallback(BASEBAND_TypeDef *BASEBANDx, uint32_t interrupt_id) {
  //   sprintf(str, "** RX Start\n");
}

__attribute__((weak)) void HAL_BASEBAND_RXCompleteCallback(BASEBAND_TypeDef *BASEBANDx, uint32_t interrupt_id) {
  //   sprintf(str, "TX Operation Finished. Check above for any errors.\n");
}

__attribute__((weak)) void HAL_BASEBAND_TXErrorCallback(BASEBAND_TypeDef *BASEBANDx, uint32_t interrupt_id) {
  // sprintf(str, "TX Operation Failed. Error message: %u\n", baseband_txerror_message());
}

__attribute__((weak)) void HAL_BASEBAND_TXCompleteCallback(BASEBAND_TypeDef *BASEBANDx, uint32_t interrupt_id) {}

__attribute__((weak)) void HAL_GPIO_Callback() {}

void InstructionAddressMisalign_Exception_Handler() {
  while (1) {
    { // debug message
      char str[64];
      sprintf(str, "instruction address misalign exception\n");
      HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
    }
  }
}
void InstructionAccessFault_Exception_Handler() {
  while (1) {
    { // debug message
      char str[64];
      sprintf(str, "instruction access fault exception\n");
      HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
    }
  }
}
void IllegalInstruction_Exception_Handler() {
  while (1) {
    { // debug message
      char str[64];
      sprintf(str, "illegal instruction exception\n");
      HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
    }
  }
}
void Breakpoint_Exception_Handler() {
  while (1) {
    { // debug message
      char str[64];
      sprintf(str, "breakpoint exception\n");
      HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
    }
  }
}
void LoadAddressMisaligne_Exception_handler() {
  while (1) {
    { // debug message
      char str[64];
      sprintf(str, "load address misaligned exception\n");
      HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
    }
  }
}
void LoadAccessFault_Exception_handler() {
  while (1) {
    { // debug message
      char str[64];
      sprintf(str, "load access fault exception\n");
      HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
    }
  }
}
void StoreAMOAddressMisalign_Exception_handler() {
  while (1) {
    { // debug message
      char str[64];
      sprintf(str, "store/AMO address misaligned exception\n");
      HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
    }
  }
}
void StoreAMOAccessFault_Exception_handler() {
  while (1) {
    { // debug message
      char str[64];
      sprintf(str, "store/AMO access fault exception\n");
      HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
    }
  }
}
void EnvironmentCallUMode_Exception_handler() {
  while (1) {
    { // debug message
      char str[64];
      sprintf(str, "environment call from U-mode exception\n");
      HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
    }
  }
}
void EnvironmentCallMMode_Exception_handler() {
  while (1) {
    { // debug message
      char str[64];
      sprintf(str, "environment call from M-mode exception\n");
      HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
    }
  }
}

void UserSoftware_IRQn_Handler() {}
void SupervisorSoftware_IRQn_Handler() {}
void HypervisorSoftware_IRQn_Handler() {}
void MachineSoftware_IRQn_Handler() {
  { // debug message
    char str[32];
    sprintf(str, "machine software irq\n");
    HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
  }
}

void UserTimer_IRQn_Handler() {}
void SupervisorTimer_IRQn_Handler() {}
void HypervisorTimer_IRQn_Handler() {}
void MachineTimer_IRQn_Handler() {
  { // debug message
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

  { // debug message
    char str[32];
    sprintf(str, "machine external irq: %d\n", irq_source);
    HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
  }
  switch (irq_source) {
    case 2:                 // GPIO interrupt
      HAL_GPIO_Callback();
      GPIOA->HIGH_IE = 0b0;
      break;
    case 6:               // baseband RX error interrupt
      HAL_BASEBAND_RXErrorCallback(BASEBAND, irq_source);
      break;
    case 7:               // baseband RX start interrupt
      HAL_BASEBAND_RXStartCallback(BASEBAND, irq_source);
      break;
    case 8:               // baseband RX finish interrupt
      HAL_BASEBAND_RXCompleteCallback(BASEBAND, irq_source);
      break;
    case 9:               // baseband TX error interrupt
      HAL_BASEBAND_TXErrorCallback(BASEBAND, irq_source);
      break;
    case 10:              // baseband TX finish interrupt
      HAL_BASEBAND_RXCompleteCallback(BASEBAND, irq_source);
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
  uint32_t m_cause;
  asm volatile("csrr %0, mcause" : "=r"(m_cause));

  switch (m_cause) {
    case 0x00000000:      // instruction address misaligned
      InstructionAddressMisalign_Exception_Handler();
      break;
    case 0x00000001:      // instruction access fault
      InstructionAccessFault_Exception_Handler();
      break;
    case 0x00000002:      // illegal instruction
      IllegalInstruction_Exception_Handler();
      break;
    case 0x00000003:      // breakpoint
      Breakpoint_Exception_Handler();
      break;
    case 0x00000004:      // load address misaligned
      LoadAddressMisaligne_Exception_handler();
      break;
    case 0x00000005:      // load access fault
      LoadAccessFault_Exception_handler();
      break;
    case 0x00000006:      // store/AMO address misaligned
      StoreAMOAddressMisalign_Exception_handler();
      break;
    case 0x00000007:      // store/AMO access fault
      StoreAMOAccessFault_Exception_handler();
      break;
    case 0x00000008:      // environment call from U-mode
      EnvironmentCallUMode_Exception_handler();
      break;
    case 0x00000011:      // environment call from M-mode
      EnvironmentCallMMode_Exception_handler();
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

