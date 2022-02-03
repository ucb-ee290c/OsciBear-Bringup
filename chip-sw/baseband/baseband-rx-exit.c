#include "baseband.h"
#include "mmio.h"

#include "ee290c_devices.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

volatile int done_status = 0;

// Interrupt Handler
void plic_handler()
{
  uint32_t irqSource = plic_claim_irq(0);
  
  printf("** Received PLIC interrupt from source %u\n", irqSource);

  if (irqSource < 6 || irqSource > 10) printf("Unexpected RX/TX interrupt.");
  if (irqSource == 6) printf("** RX Operation Failed. Error message: %u\n", baseband_rxerror_message());
  if (irqSource == 7) printf("** RX Operation Started.\n");
  if (irqSource == 8) {
    printf("** RX Operation Finished. Check above for any errors.\n");
    printf("** Bytes Read: %u\n", baseband_rxfinish_message());
    done_status = 1;
  }
  if (irqSource == 9) printf("** TX Error Message: %u\n", baseband_txerror_message());
  
  plic_complete_irq(0, irqSource);
}

int main(void)
{
  // Placeholder for received data
  volatile uint8_t received_placeholder[12] = {0x0};

  // Payload is <header><data>, where data is "TEST DATA FOR BASEBAND!"
  uint8_t payload[]  = {0x1, 0x17, 0x54, 0x45, 0x53, 0x54, 0x20, 0x44, 0x41, 0x54, 0x41, 0x20, 0x46, 0x4f, 0x52, 0x20, 0x42, 0x41, 0x53, 0x45, 0x42, 0x41, 0x4e, 0x44, 0x21};
  volatile uint8_t result_placeholder[sizeof(payload)] = {0x0};

  for (int i = 6; i < 11; i++)
  {
    plic_enable_for_hart(0, i);
    plic_set_priority(i, 5); // Set all the priorities to 5 for now
  }
  clint_connect_interrupt(INT_CODE_MACHINE_EXTERNAL, &plic_handler);  
  clint_machine_interrupt_enable(INT_CODE_MACHINE_EXTERNAL);
  mstatus_enable_interrupt();

  printf("Sending RX start command...\n");
  ble_receive((uint32_t) received_placeholder);
  printf("Done.\n");
  printf("Random text to increment cycles.\n");
  printf("Sending RX exit command...\n");
  ble_receive_exit();
  while (done_status != 1) {
    printf("*");
  }
  printf("\nDone.\n");

  printf("Sending DEBUG command to ensure RX successfully exited...\n");
  baseband_debug((uint32_t) payload, sizeof(payload));
  printf("Done.\n");

  return 0;
}