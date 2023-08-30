
#include "main.h"

volatile int done_status = 0;
char str[512];


// Command functions
// Load and send <bytes> bytes of data from address <addr>
void ble_send(uint32_t addr, uint32_t bytes) {
  reg_write32(BASEBAND_ADDITIONAL_DATA, addr);
  reg_write32(BASEBAND_INST, BASEBAND_INSTRUCTION(BASEBAND_SEND, 0, bytes));
}

// Configure baseband constant <target> (from secondary instruction set) to value <value>
void ble_configure(uint8_t target, uint32_t value) {
  reg_write32(BASEBAND_ADDITIONAL_DATA, value);
  reg_write32(BASEBAND_INST, BASEBAND_INSTRUCTION(BASEBAND_CONFIG, target, 0));
}

// Try and receive data. Any found data will be stored at address <addr>
void ble_receive(uint32_t addr) {
  reg_write32(BASEBAND_ADDITIONAL_DATA, addr);
  reg_write32(BASEBAND_INST, BASEBAND_INSTRUCTION(BASEBAND_RECEIVE, 0, 0));
}

// Try and receive data. Any found data will be stored at address <addr>
void ble_receive_exit() {
  reg_write32(BASEBAND_INST, BASEBAND_INSTRUCTION(BASEBAND_RECEIVE_EXIT, 0, 0));
}

uint32_t baseband_status0() {
  return reg_read32(BASEBAND_STATUS0);
}

uint32_t baseband_status1() {
  return reg_read32(BASEBAND_STATUS1);
}

uint32_t baseband_status2() {
  return reg_read32(BASEBAND_STATUS2);
}

uint32_t baseband_status3() {
  return reg_read32(BASEBAND_STATUS3);
}

uint32_t baseband_status4() {
  return reg_read32(BASEBAND_STATUS4);
}

uint32_t baseband_rxerror_message() {
  return reg_read32(BASEBAND_RXERROR_MESSAGE);
}

uint32_t baseband_txerror_message() {
  return reg_read32(BASEBAND_TXERROR_MESSAGE);
}

uint32_t baseband_rxfinish_message() {
  return reg_read32(BASEBAND_RXFINISH_MESSAGE);
}

// LUT Control
#define LUT_LOFSK 0
#define LUT_LOCT 1
#define LUT_AGCI 2
#define LUT_AGCQ 3
#define LUT_DCOIFRONT 4
#define LUT_DCOQFRONT 5

#define LUT_COMMAND(lut, address, value) ((lut & 0xF) + ((address & 0x3F) << 4) + ((value & 0x3FFFFF) << 10))

void baseband_set_lut(uint8_t lut, uint8_t address, uint32_t value) {
  reg_write32(BASEBAND_LUT_CMD, LUT_COMMAND(lut, address, value));
}

// Function written for writing to tuning MMIO, but technically can be used for all MMIO although unchecked
// For partial write (4 bits), offset is used to shift data to correct position
void baseband_tuning_set(uint32_t addr, uint32_t data, uint32_t bit_size, uint32_t offset) {
  // For partial writes, need to read, mask, write-back
  if (bit_size == 4) {
    uint8_t temp = (reg_read8(addr) & ~(15 << offset)) | (data << offset);
    // Debug print
    // printf("%x\n", temp);
    reg_write8(addr, temp);
  } else if (bit_size <= 8) {
    reg_write8(addr, data);
  } else {
    reg_write16(addr, data);
  }
}

// Function that tests (send + check) the baseband debug command
void baseband_debug(uint32_t addr, size_t byte_size) {
  // Sending baseband DEBUG instruction
  reg_write32(BASEBAND_ADDITIONAL_DATA, addr);
  
  // INST = data, 1111 1111 
  reg_write32(BASEBAND_INST, BASEBAND_INSTRUCTION(BASEBAND_DEBUG, -1, byte_size));

  // TODO: Determine cycles to wait. Printing for now
  sprintf(str, "Sent DEBUG instruction with data at address %.8x. Waiting...\n", addr);
  HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);

  // Word-aligned address (word is 4 bytes, 32 bits)
  int mismatch = -1;
  int fail = 0;
  uint32_t res_addr = addr + ((byte_size & ~3) + ((byte_size % 4) > 0 ? 4 : 0)); 
  sprintf(str, "Output bytes at address %.8x: ", res_addr);
  HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);

  for (int i = 0; i < byte_size; i++) {
    sprintf(str, "%.2x ", (unsigned)*(unsigned char*)(res_addr + i));
    HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
    // Checking match
    if ((unsigned)*(unsigned char*)(addr + i) != (unsigned)*(unsigned char*)(res_addr + i)) {
      fail += 1;
      if (mismatch == -1) mismatch = i;
    }
  }
  if (fail) {
    sprintf(str, "FAILED TEST. %d bytes are mismatched in output. Index of first mismatch: %d\n", fail, mismatch);
  } else {
    sprintf(str, "PASSED TEST. All input bytes match output bytes.\n");
  }
  HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
}



typedef struct plic_context_control
{
  uint32_t priority_threshold;
  uint32_t claim_complete;
} plic_context_control_t __attribute__ ((aligned (0x1000)));


uint32_t *const plic_enables      =               (uint32_t* const) 0xc002000; // Context [0..15871], sources bitmap registers [0..31]
uint32_t *const plic_priorities   =               (uint32_t* const) 0xc000000; // priorities [0..1023]
plic_context_control_t *const plic_ctx_controls = (plic_context_control_t* const) 0xc200000; // Priority threshold & claim / complete for context [0..15871]

void plic_set_bit(uint32_t* const target, uint32_t index)
{
  uint32_t reg_index = index >> 5;
  uint32_t bit_index = index & 0x1F;
  *target |= (1 << bit_index);
}

void plic_enable_for_hart(uint32_t hart_id, uint32_t irq_id) {
  uint32_t* base = plic_enables + 32 * hart_id;
  plic_set_bit(base, irq_id);
}

void plic_set_priority(uint32_t irq_id, uint32_t priority) {
  plic_priorities[irq_id] = priority;
}

uint32_t plic_claim_irq(uint32_t hart_id) {
  return plic_ctx_controls[hart_id].claim_complete;
}

void plic_complete_irq(uint32_t hart_id, uint32_t irq_id){
  plic_ctx_controls[hart_id].claim_complete = irq_id;
}

#define ADC_BUF_SIZE 2048
//uint8_t adc_i_buf[ADC_BUF_SIZE];
uint8_t adc_q_buf[ADC_BUF_SIZE];

void configure_rx_chain() 
{
  // set bit 0 of BASEBAND_TRIM_G7 to 1
  // this is the bit that enables the mux_dbg_out
  *(uint8_t *)BASEBAND_TRIM_G7 = 0b1;

  // Definitively turns off the Analog Test 2 as an input
  uint8_t analog_test_2_input_en = 0b0;
  uint8_t rfclock_sel = 0b1;
  uint8_t pll_sign = 0b1;
  uint8_t lo_enb = 0b1; // Active low 
  uint8_t cc_enable = 0b0;
  uint8_t ref_sel = 0b0;
  uint8_t bb_trim_g0 = *(uint8_t *)BASEBAND_TRIM_G0;

  uint8_t set_trim_g0 =  (rfclock_sel << 5) | (analog_test_2_input_en << 4) | (pll_sign << 3) | (lo_enb << 0);
  *(uint8_t *)BASEBAND_TRIM_G0 = set_trim_g0;

  // Enable the DCOC
  *(uint8_t *)BASEBAND_I_DCO_USE_DCO = 0b0;
  *(uint8_t *)BASEBAND_Q_DCO_USE_DCO = 0b0;
  // Set the current DACS T0 T1 T2 and T3
  // Current DACs are used to tune a DC offset
  *(uint8_t *)BASEBAND_DAC_T0 = 0b0;
  *(uint8_t *)BASEBAND_DAC_T1 = 0b0; 
  *(uint8_t *)BASEBAND_DAC_T2 = 0b0;
  *(uint8_t *)BASEBAND_DAC_T3 = 0b0;
  
  uint32_t mux_dbg_out = (0b1 << 3) | (0b11111 << 16) | (0b11111 << 24);
  *(uint32_t *)BASEBAND_MUX_DBG_OUT = mux_dbg_out;
  // mux_dbg_in 10 bits
  // mux_dbg_in[0] - Into 1st VGA I
  // mux_dbg_in[1] - Into BPF I
  // mux_dbg_in[2] - Into 2nd VGA I
  // mux_dbg_in[3] - Into ADC I
  // mux_dbg_in[4] - Into 1st VGA Q
  // mux_dbg_in[5] - Into BPF Q
  // mux_dbg_in[6] - Into 2nd VGA Q
  // mux_dbg_in[7] - Into ADC Q
  uint16_t mux_dbg_in = (0b1 << 0);
  *(uint16_t*)BASEBAND_MUX_DBG_IN = mux_dbg_in;

  uint8_t mixer_r0 = 0b11; // 4 bits r0 IN
  uint8_t mixer_r1 = 0b11; // 4 bits r1 IP
  *(uint8_t *)BASEBAND_MIXER_R1_R0 = (mixer_r1 << 4) | mixer_r0 ; // gain for mixer
  uint8_t mixer_r2 = 0b11; // 4 bits r2 QN
  uint8_t mixer_r3 = 0b11; // 4 bits r3 QP 
  *(uint8_t *)BASEBAND_MIXER_R3_R2 = (mixer_r3 << 4) | mixer_r2; // gain for mixer
  *(uint16_t *)BASEBAND_I_VGA_ATTEN_VALUE = (0b1001111100);  // Set 1st stage VGA tuning bit to 1, otherwise nothing works
  *(uint16_t *)BASEBAND_Q_VGA_ATTEN_VALUE = (0b1001111100);  // Set 1st stage VGA tuning bit to 1, otherwise nothing works

  
  // For the BPF there are 5 unique sets of resistor trim values
  // r0 r1 - High pass filter zero
  // r2 r3 - High pass filter zero
  // r4 r5 - 
  // r6 r9
  // r7 r8
  

}

void mixer_sweep_loop()
{
  // sweep mixer r0 r1 through 0b0000 to 0b1111
  /*
  while(1) {
    for(int i = 0; i < 16; i++) {
      mixer_r0_r1 = i | (i << 4);
      *(uint8_t *)BASEBAND_MIXER_R1_R0 = mixer_r0_r1;
      mixer_r2_r3 = i | (i << 4);
      *(uint8_t *)BASEBAND_MIXER_R3_R2 = mixer_r2_r3;;
      sprintf(str, "current iteration: %d\n", i);
      HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
      HAL_delay(1000);
    }
  }
  */
}

void adc_test_loop()
{
  uint8_t state = 0;
  uint32_t adc_i_data = 0;
  uint32_t adc_q_data = 0;
  uint16_t i;
  uint16_t img_reject_out = 0; // 11 bits
  while(1) {
    
    HAL_GPIO_writePin(GPIOA, GPIO_PIN_0, 1);
    for(i = 0; i<ADC_BUF_SIZE;i++)
    {
      //adc_i_data = (uint8_t)(((*(uint32_t *)BASEBAND_STATUS1) >> 24) & 0xFF);
      adc_q_data = (uint8_t)(((*(uint32_t *)BASEBAND_STATUS3) >> 24) & 0xFF);
      //adc_i_buf[i] = adc_i_data;
      adc_q_buf[i] = adc_q_data;
      //HAL_delay(1);
    }
    HAL_GPIO_writePin(GPIOA, GPIO_PIN_0, 0);
    //sprintf(str, "\nI:\n");
    //HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
    HAL_GPIO_writePin(GPIOA, GPIO_PIN_1, 1);
    HAL_UART_transmit(UART0, adc_q_buf, ADC_BUF_SIZE, 0);
    HAL_GPIO_writePin(GPIOA, GPIO_PIN_1, 0);
    //HAL_delay(200);
    /*
    sprintf(str, "\nQ:\n"); 
    HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
    HAL_UART_transmit(UART0, adc_q_buf, ADC_BUF_SIZE, 0);
    HAL_delay(200);
    */
    //img_reject_out = (uint16_t)(((*(uint32_t *)BASEBAND_STATUS0) >> 16) & 0x7FF);
    //sprintf(str, "\nimg_rjct_out: %d\n", img_reject_out);
    //HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
  }
  
}

int main() {
  HAL_init();

  UART_InitTypeDef UART_init_config;
  UART_init_config.baudrate = 100000;
  

  HAL_UART_init(UART0, &UART_init_config);
  HAL_GPIO_init(GPIOA, GPIO_PIN_0);
  HAL_GPIO_init(GPIOA, GPIO_PIN_1);
  HAL_GPIO_writePin(GPIOA, GPIO_PIN_0, 0);
  HAL_GPIO_writePin(GPIOA, GPIO_PIN_1, 1);

  sprintf(str, "Hi :) \n");
  HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);

  // HAL_delay(2000);

  // Payload is <header><data>, where data is "OSCIBEAR"
  // 79   83   67   73   66   69   65   82
  //uint8_t payload[]  = {0x79, 0x83, 0x67, 0x73, 0x69, 0x62, 0x65, 0x61, 0x72};
  // PDU Type 4 bits
  // RFU 2 bits
  // TxAdd 1 bit
  // RxAdd 1 bit
  // Length 6 bits
  // AdvA 6 bytes
  // AdvData 0-31 bytes

  // Create a payload from a string
  uint8_t payload[128];
  uint8_t payload_len = 0;
  uint8_t pdu_rfu_txrxadd;

  //pdu_rfu_txrxadd = 0b01000010;
  pdu_rfu_txrxadd = 0b01000001;
  payload[payload_len++] = pdu_rfu_txrxadd;
  

  char *payload_str = "HOTCHIPS23";
  uint8_t pdu_len = strlen(payload_str) + 8; // payload_len + 8 should be 18
  // Advertiser Address
  payload[payload_len++] = pdu_len;
  payload[payload_len++] = 0x47; // G
  payload[payload_len++] = 0x4F; // O
  payload[payload_len++] = 0x42; // B
  payload[payload_len++] = 0x45; // E
  payload[payload_len++] = 0x41; // A
  payload[payload_len++] = 0x52; // R
  // Advertiser Data
  // Length
  payload[payload_len++] = 11;
  // Type
  payload[payload_len++] = 0xFF; // manufacturer specific
  while(*payload_str) {
    payload[payload_len++] = (uint8_t)*payload_str++;
  }

  ble_configure(BASEBAND_CONFIG_CHANNEL_INDEX, 38);
  //  baseband_set_lut(uint8_t lut, uint8_t address, uint32_t value)
  // Initialize all the LO LUT entries with a linspace of values
  // LOCT vals made up of coarse lower 5 bits and fine upper 3 bits
  uint8_t coarse = 2;
  uint8_t fine = 0;
  uint8_t loct = (fine % 8) << 5 | (coarse % 32);
  for (int i = 0; i < 40; i++) {
    baseband_set_lut(LUT_LOCT, i, (uint8_t)loct);
  }
  

  
  // for(int i = 0; i<255; i++)
  // {
  //   for (int j = 0; j < 40; j++) {
  //     baseband_set_lut(LUT_LOCT, j, (uint8_t)i);
  //   }
  //   // print the current value of the LUT
  //   sprintf(str, "current value: %d\r\n", i);
  //   HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
  //   HAL_delay(1000);
  // }
  
  //baseband_set_lut(LUT_LOCT, (uint8_t)0, (uint8_t)0);
  // Set the LUT_LOFSK values to a linear ramp from 15 to 0
  // uint8_t val = 0;
  // for (int i = 0; i < 64; i++) {
  //   val = (63 - i)/4;
  //   baseband_set_lut(LUT_LOFSK, i, val);
  // }

  for(int i = 0; i < 32; i++)
  {
    baseband_set_lut(LUT_LOFSK, i, 255);
  }
  for(int i = 32; i < 64; i++)
  {
    baseband_set_lut(LUT_LOFSK, i, 0);
  }

  // for (int i = 0; i < 64; i++) {
  //   baseband_set_lut(LUT_LOFSK, i, 255);
  // }

  //while(1);
  //void ble_configure(uint8_t target, uint32_t value)
  

  //*(uint8_t*)BASEBAND_TRIM_G0 = 0b00000000;
  //*(uint8_t*)BASEBAND_TRIM_G0 = 0b00000000;

  ///// TEST THE RX CHAIN
  //configure_rx_chain();

  //ble_receive(0x80004000);
  //adc_test_loop();

  /*
  // Receiver testing 
  // uint8_t toggle = 0;
  while(1)
  {
    
    ble_receive(0x80004000);

  }
  */
  
  


  // switch back and forth between 0 and 1 in bit 9
  //#define BASEBAND_I_VGA_ATTEN_VALUE 0x8026
  

  /*
  for(int i = 0; i < 63; i++) {
    baseband_set_lut(LUT_LOCT, i, 0x00);
  }
  */

  // Interrupts for baseband TX are 9, 10
  // plic_enable_for_hart(0, 9);
  // plic_enable_for_hart(0, 10);
  // plic_set_priority(9, 5); // Set all the priorities to 5 for now
  // plic_set_priority(10, 5);
  
  // HAL_CORE_enableIRQ(MachineExternal_IRQn);
  // HAL_CORE_enableInterrupt();

  //uint32_t timer = 0;
  //  uint16_t cur_channel = 1;

  //sprintf(str, "Beginning transmit\n");
  //HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
  *(uint8_t*)BASEBAND_TRIM_G0 = 0b00000000;
  uint8_t tx_ctrl_status = 0;
  HAL_GPIO_writePin(GPIOA, GPIO_PIN_0, 0);
  while (1) {

    //sprintf(str, "Sending payload to baseband...\n");
    //HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
    ble_send((uint32_t) payload, payload_len);
    HAL_GPIO_writePin(GPIOA, GPIO_PIN_0, 1);
    /*
    // Channel switching 
    if(timer > 200000) {
      timer = 0;
      //void ble_configure(uint8_t target, uint32_t value)
      baseband_config(BASEBAND_CONFIG_CHANNEL_INDEX, )
    }
    */
    tx_ctrl_status = (*(uint32_t *)BASEBAND_STATUS0 >> 11) & 0x3;
    while(tx_ctrl_status != 0) {
      // write 
      // Poll the tx_ctrl_status
      // set tuning trim G0 0th bit 1
      tx_ctrl_status = (*(uint32_t *)BASEBAND_STATUS0 >> 11) & 0x3;
    }
    //*(uint8_t*)BASEBAND_TRIM_G0 = 0b00000001;
    HAL_GPIO_writePin(GPIOA, GPIO_PIN_0, 0);
    sprintf(str, "\nDone.\n");
    HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);  
    // while (done_status != 1) {
    //   sprintf(str, "*");
    // }
    // sprintf(str, "\nDone.\n");
    // HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);

    HAL_delay(1);
    //*(uint8_t*)BASEBAND_TRIM_G0 = 0b00000000;
  }
}
