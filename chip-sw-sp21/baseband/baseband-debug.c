#include "baseband.h"
#include "mmio.h"

#include "ee290c_devices.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void plic_handler()
{
  uint32_t irqSource = plic_claim_irq(0);
  
  printf("** Received PLIC interrupt from source %u\n", irqSource);

  if (irqSource == 6) printf("** RX Error Message: %u\n", baseband_rxerror_message());
  if (irqSource == 8) printf("** Bytes Read: %u\n", baseband_rxfinish_message());
  if (irqSource == 9) printf("** TX Error Message: %u\n", baseband_txerror_message());
  
  plic_complete_irq(0, irqSource);
}

int main(void)
{
  // Data must be in format <header><data>. The header is 2 bytes, with the second byte representing the length of the of the following data.

  // Short sanity test
  uint8_t good_data_one[]  = {0x1, 0x4, 0x11, 0x22, 0x33, 0x44};
  // Spacing data for output
  volatile uint8_t random_spacing_one[sizeof(good_data_one)] = {0x0};

  // char good_data_two[] = "TEST DATA FOR BASEBAND!";
  // NOTE: Cannot use char, will have unecessary bytes in output. Need to use byte array!
  uint8_t good_data_two[]  = {0x1, 0x17, 0x54, 0x45, 0x53, 0x54, 0x20, 0x44, 0x41, 0x54, 0x41, 0x20, 0x46, 0x4f, 0x52, 0x20, 0x42, 0x41, 0x53, 0x45, 0x42, 0x41, 0x4e, 0x44, 0x21};
  volatile uint8_t random_spacing_two[sizeof(good_data_two)] = {0x0};

  uint8_t good_data_three[]  = {0x1, 0xC, 0x41, 0x4e, 0x4f, 0x54, 0x48, 0x45, 0x52, 0x20, 0x54, 0x45, 0x53, 0x54};
  volatile uint8_t random_spacing_three[sizeof(good_data_three)] = {0x0};

  // No longer testing for this case - this is illegal instruction and should never happen (SW Enforced)
  // // The following data is malformed as the size byte in the header is "2", which is smaller than the actual data payload
  // uint8_t bad_data[]  = {0x1, 0x2, 0x54, 0x45, 0x53, 0x54, 0x20, 0x44, 0x41, 0x54, 0x41, 0x20, 0x46, 0x4f, 0x52, 0x20, 0x42, 0x41, 0x53, 0x45, 0x42, 0x41, 0x4e, 0x44, 0x21};
  // volatile uint8_t random_spacing_three[sizeof(bad_data)] = {0x0};
  
  // It seems from the device tree, interrupts for baseband are 6, 7, 8, 9, 10
  for (int i = 6; i < 11; i++)
  {
    plic_enable_for_hart(0, i);
    plic_set_priority(i, 5); // Set all the priorities to 5 for now
  }

  plic_set_priority_threshold(0, 1); // Set Hart 0 priority threshold to 1
  clint_connect_interrupt(INT_CODE_MACHINE_EXTERNAL, &plic_handler);  
  clint_machine_interrupt_enable(INT_CODE_MACHINE_EXTERNAL);
  mstatus_enable_interrupt();
  


  printf("The following DEBUG test should pass:\n");
  baseband_debug((uint32_t) good_data_one, sizeof(good_data_one));
  printf("Done.\n");

  printf("The following DEBUG test should pass:\n");
  baseband_debug((uint32_t) good_data_two, sizeof(good_data_two));
  printf("Done.\n");

  printf("The following DEBUG test should pass:\n");
  baseband_debug((uint32_t) good_data_three, sizeof(good_data_three));
  printf("Done.\n");

  // No longer testing for this case - this is illegal instruction and should never happen (SW Enforced)
  // printf("The following DEBUG test should fail, and only the first 4 bytes should be non-zero (bad header size):\n");
  // baseband_debug((uint32_t) bad_data, sizeof(bad_data));
  // printf("Done.\n");

  // Add more tests if desired

  return 0;
}
