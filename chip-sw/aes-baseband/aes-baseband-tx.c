#include "aes.h"
#include "baseband.h"
#include "aes-baseband.h"
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
  // AES 128bit key
  uint8_t key[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88,0x09, 0xcf,  0x4f, 0x3c};

  // Plaintext
  uint8_t plaintext[16] = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a};

  // Scratch space for ciphertext
  uint8_t fillerspace[16] = {0x0};

  // Payload - init to 0, size is hardcoded
  uint8_t payload[18]  = {0x0};

  // Expected Payload (used to check for correctness)
  uint8_t exp_payload[18] = {0x1, 0x10, 0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60, 0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97};

  // Interrupts for baseband TX are 9, 10
  plic_enable_for_hart(0, 9);
  plic_enable_for_hart(0, 10);
  plic_set_priority(9, 5); // Set all the priorities to 5 for now
  plic_set_priority(10, 5);
  clint_connect_interrupt(INT_CODE_MACHINE_EXTERNAL, &plic_handler);  
  clint_machine_interrupt_enable(INT_CODE_MACHINE_EXTERNAL);
  mstatus_enable_interrupt();

  // TEST DETAILS: Currently just a basic test that encrypts data (AES-ECB) and sends to baseband to TX
  // Real workload would involve AES-CBC + AES-CTR encryption, but for now AES-ECB works fine

  printf("Encrypting plaintext with AES accelerator...\n");
  AES_ECB_ENC_POLL(key, 128, plaintext, fillerspace, 16);

  printf("Packaging ciphertext into packet.\n");
  createPacket(payload, fillerspace, sizeof(fillerspace));

  printf("Sending payload to baseband...\n");
  ble_send((uint32_t) payload, sizeof(payload));
  while (done_status != 1) {
    printf("*");
  }
  printf("\nDone.\n");

  printf("Checking if payload matches expected payload...\n");
  checkBytes(payload, exp_payload, sizeof(exp_payload));

  return 0;
}