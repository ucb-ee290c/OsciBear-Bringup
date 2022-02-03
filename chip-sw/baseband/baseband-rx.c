#include "baseband.h"
#include "mmio.h"

#include "ee290c_devices.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static void phex(volatile uint8_t* str, int len)
{
    unsigned char i;
    for (i = 0; i < len; ++i)
        printf("%02x", str[i]);
    printf("\n");
}

int interrupt_array[20] = {0};

// Interrupt Handler
void plic_handler()
{
  uint32_t irqSource = plic_claim_irq(0);
  interrupt_array[irqSource]++;
  plic_complete_irq(0, irqSource);
}

int main(void)
{
  // Placeholder for received data
  volatile uint8_t received_placeholder[20] = {0x0};

  // Payload is <header><data>, where data is "TEST DATA FOR BASEBAND!"
  uint8_t payload[]  = {0x1, 0x17, 0x54, 0x45, 0x53, 0x54, 0x20, 0x44, 0x41, 0x54, 0x41, 0x20, 0x46, 0x4f, 0x52, 0x20, 0x42, 0x41, 0x53, 0x45, 0x42, 0x41, 0x4e, 0x44, 0x21};
  volatile uint8_t result_placeholder[sizeof(payload)] = {0x0};

  uint8_t msg_buffer[100];
  for (int i = 0; i < 20; i++)
  {
    msg_buffer[i*2] = '0' + interrupt_array[i];
    msg_buffer[i*2+1] = ' ';
  }
  msg_buffer[40] = '\n';
  msg_buffer[41] = '\0';
  printf("%s", msg_buffer);
  for (int i = 6; i < 11; i++)
  {
    plic_enable_for_hart(0, i);
    plic_set_priority(i, 5); // Set all the priorities to 5 for now
  }
  plic_set_priority_threshold(0, 1);
  clint_connect_interrupt(INT_CODE_MACHINE_EXTERNAL, &plic_handler);  
  clint_machine_interrupt_enable(INT_CODE_MACHINE_EXTERNAL);
  mstatus_enable_interrupt();

  printf("Buffer initial contents: ");
  phex(received_placeholder, 20);

  // configure baseband registers
  ble_configure(BASEBAND_CONFIG_ACCESS_ADDRESS, 0x529781e8);
  ble_configure(BASEBAND_CONFIG_CHANNEL_INDEX, 0);
  ble_configure(BASEBAND_CONFIG_CRC_SEED, 0x555555);

  baseband_tuning_set(BASEBAND_I_DCO_USE_DCO, 0, 1, 0);
  baseband_tuning_set(BASEBAND_Q_DCO_USE_DCO, 0, 1, 0);
  baseband_tuning_set(BASEBAND_DAC_T0, 0, 6, 0); 
  baseband_tuning_set(BASEBAND_DAC_T1, 0, 6, 0); 
  baseband_tuning_set(BASEBAND_DAC_T2, 0, 6, 0); 
  baseband_tuning_set(BASEBAND_DAC_T3, 0, 6, 0); 
  baseband_tuning_set(BASEBAND_I_VGA_ATTEN_USE_AGC , 0, 1, 0);
  baseband_tuning_set(BASEBAND_Q_VGA_ATTEN_USE_AGC , 0, 1, 0);
  baseband_tuning_set(BASEBAND_I_VGA_ATTEN_GAIN, 0, 10, 0);
  baseband_tuning_set(BASEBAND_Q_VGA_ATTEN_GAIN, 0, 10, 0);

  printf("Sending RX start command...\n");
  ble_receive((uint32_t) received_placeholder);
  printf("Done.\n");
  while (interrupt_array[8] == 0) {
    for (int i = 0; i < 20; i++)
    {
      msg_buffer[i*2] = '0' + interrupt_array[i];
      msg_buffer[i*2+1] = ' ';
    }
    printf("%s", msg_buffer);
  }
  for (int i = 0; i < 20; i++)
  {
    msg_buffer[i*2] = '0' + interrupt_array[i];
    msg_buffer[i*2+1] = ' ';
  }
  printf("%s", msg_buffer);
  printf("\nDone.\n");
 
  printf("Received %d bytes!\n", baseband_rxfinish_message());
  printf("Received data: ");
  phex(received_placeholder, 20);

  printf("Sending DEBUG command to ensure RX successfully exited...\n");
  baseband_debug((uint32_t) payload, sizeof(payload));
  printf("Done.\n");

  return 0;
}
