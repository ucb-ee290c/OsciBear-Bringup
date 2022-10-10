
#include "main.h"


void testBasebandTX() {
  uint8_t payload[25]  = {0x1, 0x17, 0x54, 0x45, 0x53, 0x54, 0x20, 0x44, 0x41, 0x54, 0x41, 0x20, 0x46, 0x4f, 0x52, 0x20, 0x42, 0x41, 0x53, 0x45, 0x42, 0x41, 0x4e, 0x44, 0x21};

  for (uint16_t i=1; i<50; i+=1) {
    HAL_PLIC_enable(0, i);
    HAL_PLIC_setPriority(i, 5); // Set all the priorities to 5 for now
  }

  HAL_CORE_enableIRQ(MachineExternal_IRQn);
  
  HAL_CORE_enableGlobalInterrupt();

  while (1) {
    
    char str[256];
    sprintf(str, "Sending payload to baseband...\n");
    HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);

    BLE_send(payload, 25);

    HAL_delay(1000);
  }
}
