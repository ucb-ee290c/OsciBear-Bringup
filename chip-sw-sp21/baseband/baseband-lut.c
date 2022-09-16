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

  if (irqSource != 9 && irqSource != 10) printf("Unexpected TX interrupt.");
  if (irqSource == 9) printf("** TX Operation Failed. Error message: %u\n", baseband_txerror_message());
  if (irqSource == 10) {
    printf("** TX Operation Finished. Check above for any errors.\n");
    done_status = 1;
  }
  
  plic_complete_irq(0, irqSource);
}

int main(void)
{
  // Payload is <header><data>, where data is "TEST DATA FOR BASEBAND!"
  uint8_t payload[]  = {0x1, 0x17, 0x54, 0x45, 0x53, 0x54, 0x20, 0x44, 0x41, 0x54, 0x41, 0x20, 0x46, 0x4f, 0x52, 0x20, 0x42, 0x41, 0x53, 0x45, 0x42, 0x41, 0x4e, 0x44, 0x21};

  // Interrupts for baseband TX are 9, 10
  plic_enable_for_hart(0, 9);
  plic_enable_for_hart(0, 10);
  plic_set_priority(9, 5); // Set all the priorities to 5 for now
  plic_set_priority(10, 5);
  clint_connect_interrupt(INT_CODE_MACHINE_EXTERNAL, &plic_handler);  
  clint_machine_interrupt_enable(INT_CODE_MACHINE_EXTERNAL);
  mstatus_enable_interrupt();

  // LUT test to map i -> i for all LUT types.
  for (int l = LUT_LOFSK; l <= LUT_DCOQFRONT; l++) {
    printf("Mapping ADDR I -> VALUE I for CONTROL LUT %d.\n", l);
    for (int i = 0; i < 64; i++) {
      baseband_set_lut((uint8_t) l, (uint8_t) i, (uint32_t) i);
    }
  }
  printf("Done. Check waveforms to ensure that LUT registers were properly set.\n");

  printf("Sending payload to baseband TX...\n");
  ble_send((uint32_t) payload, sizeof(payload));
  while (done_status != 1) {
    printf("*");
  }
  printf("\nDone. Check waveforms to ensure that LOCT and LOFSK output matches LUTs.\n");

  return 0;
}