
#include "main.h"

uint8_t *tx_data_addr;
uint8_t *rx_data_addr;
uint16_t payload_size = 25;

uint8_t rx_finish = 0;

void HAL_BASEBAND_RXStartCallback(BASEBAND_TypeDef *BASEBANDx, uint32_t interrupt_id) {
  char str[128];
  sprintf(str, "RX ADDR: %.8x \n", rx_data_addr);
  HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);

  for (uint16_t i=0; i<payload_size; i+=1) {
    sprintf(str, "%.2x ", rx_data_addr[i]);
    HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
  }
  HAL_UART_transmit(UART0, (uint8_t *)"\n", 1, 0);

  rx_finish = 1;
}

void HAL_BASEBAND_RXCompleteCallback(BASEBAND_TypeDef *BASEBANDx, uint32_t interrupt_id) {
  char str[128];
  sprintf(str, "RXCompleteCallback: %lu\n", BASEBAND_getRXCompleteMessage());
  HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
}

void HAL_BASEBAND_TXCompleteCallback(BASEBAND_TypeDef *BASEBANDx, uint32_t interrupt_id) {
}

void testBasebandDigitalLoopback() {
  
  char str[128];

  // set tuning trim G0 0th bit 1
  SET_BITS(BASEBAND->TRIM_G0, 0b1);

  for (uint16_t i=1; i<50; i+=1) {
    HAL_PLIC_enable(0, i);
    HAL_PLIC_setPriority(i, 5); // Set all the priorities to 5 for now
  }

  HAL_CORE_enableIRQ(MachineExternal_IRQn);
  
  HAL_CORE_enableGlobalInterrupt();


  tx_data_addr = 0x80005200;

  rx_data_addr = (uint32_t *)((uint32_t)tx_data_addr + (((uint32_t)payload_size & ~3) + ((payload_size % 4) > 0 ? 4 : 0)));

  for (uint16_t i=0; i<payload_size; i+=1) {
    tx_data_addr[i] = 0xCA;
  }
  tx_data_addr[0] = 0x01;
  tx_data_addr[1] = 0x17;
  tx_data_addr[2] = 0x54;
  tx_data_addr[3] = 0x45;
  tx_data_addr[4] = 0x53;
  tx_data_addr[5] = 0x54;
  tx_data_addr[6] = 0x20;
  tx_data_addr[7] = 0x44;
  tx_data_addr[8] = 0x41;
  tx_data_addr[9] = 0x54;
  tx_data_addr[10] = 0x41;
  tx_data_addr[11] = 0x20;
  tx_data_addr[12] = 0x46;
  tx_data_addr[13] = 0x4f;
  tx_data_addr[14] = 0x52;
  tx_data_addr[15] = 0x20;
  tx_data_addr[16] = 0x42;
  tx_data_addr[17] = 0x41;
  tx_data_addr[18] = 0x53;
  tx_data_addr[19] =  0x45;
  tx_data_addr[20] =  0x42;
  tx_data_addr[21] =  0x41;
  tx_data_addr[22] =  0x4e;
  tx_data_addr[23] =  0x44;
  tx_data_addr[24] =  0x21;
  
  for (uint16_t i=0; i<payload_size; i+=1) {
    rx_data_addr[i] = 0x00;
  }


  // logging TX
  sprintf(str, "TX ADDR: %.8x \n", tx_data_addr);
  HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);

  for (uint16_t i=0; i<payload_size; i+=1) {
    sprintf(str, "%.2x ", tx_data_addr[i]);
    HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
  }
  HAL_UART_transmit(UART0, (uint8_t *)"\n", 1, 0);

  // logging RX
  sprintf(str, "RX ADDR: %.8x \n", rx_data_addr);
  HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);

  for (uint16_t i=0; i<payload_size; i+=1) {
    sprintf(str, "%.2x ", rx_data_addr[i]);
    HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
  }
  HAL_UART_transmit(UART0, (uint8_t *)"\n", 1, 0);

  // send
  BASEBAND->ADDITIONAL_DATA = (uint32_t)tx_data_addr;
  BASEBAND->INST = BASEBAND_INSTRUCTION(BASEBAND_DEBUG, LOOPBACK_BASEBAND, payload_size);


  // add a small delay to let interrupt handler do its job
  HAL_delay(1000);

  while (1) {
    if (rx_finish) {
      HAL_delay(1000);
      // send
      BASEBAND->ADDITIONAL_DATA = (uint32_t)tx_data_addr;
      BASEBAND->INST = BASEBAND_INSTRUCTION(BASEBAND_DEBUG, LOOPBACK_BASEBAND, payload_size);
    }
    
    // sprintf(str, "RX error register: %.8x\n", BASEBAND_getRXErrorMessage());
    // HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
    // sprintf(str, "TX error register: %.8x\n", BASEBAND_getTXErrorMessage());
    // HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
    // sprintf(str, "RX complete register: %.8x\n", BASEBAND_getRXCompleteMessage());
    // HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);


  }
}
