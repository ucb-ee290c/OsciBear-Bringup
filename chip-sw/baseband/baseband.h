#include "mmio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Address map
#define BASEBAND_INST 0x8000
#define BASEBAND_ADDITIONAL_DATA 0x8004

#define BASEBAND_STATUS0 0x8008
#define BASEBAND_STATUS1 0x800C
#define BASEBAND_STATUS2 0x8010
#define BASEBAND_STATUS3 0x8014
#define BASEBAND_STATUS4 0x8018

#define BASEBAND_TRIM_G0 0x801C
#define BASEBAND_TRIM_G1 0x801D
#define BASEBAND_TRIM_G2 0x801E
#define BASEBAND_TRIM_G3 0x801F
#define BASEBAND_TRIM_G4 0x8020
#define BASEBAND_TRIM_G5 0x8021
#define BASEBAND_TRIM_G6 0x8022
#define BASEBAND_TRIM_G7 0x8023

#define BASEBAND_MIXER_R1_R0 0x8024
#define BASEBAND_MIXER_R3_R2 0x8025

#define BASEBAND_I_VGA_ATTEN_VALUE 0x8026
#define BASEBAND_I_VGA_ATTEN_RESET 0x8028
#define BASEBAND_I_VGA_ATTEN_USE_AGC 0x8029
#define BASEBAND_I_VGA_ATTEN_SAMPLE_WINDOW 0x802A
#define BASEBAND_I_VGA_ATTEN_IDEAL_P2P 0x802B
#define BASEBAND_I_VGA_ATTEN_GAIN 0x802C
#define BASEBAND_I_FILTER_R1_R0 0x802D
#define BASEBAND_I_FILTER_R3_R2 0x802E
#define BASEBAND_I_FILTER_R5_R4 0x802F
#define BASEBAND_I_FILTER_R7_R6 0x8030
#define BASEBAND_I_FILTER_R9_R8 0x8031

#define BASEBAND_Q_VGA_ATTEN_VALUE 0x8032
#define BASEBAND_Q_VGA_ATTEN_RESET 0x8034
#define BASEBAND_Q_VGA_ATTEN_USE_AGC 0x8035
#define BASEBAND_Q_VGA_ATTEN_SAMPLE_WINDOW 0x8036
#define BASEBAND_Q_VGA_ATTEN_IDEAL_P2P 0x8037
#define BASEBAND_Q_VGA_ATTEN_GAIN 0x8038
#define BASEBAND_Q_FILTER_R1_R0 0x8039
#define BASEBAND_Q_FILTER_R3_R2 0x803A
#define BASEBAND_Q_FILTER_R5_R4 0x803B
#define BASEBAND_Q_FILTER_R7_R6 0x803C
#define BASEBAND_Q_FILTER_R9_R8 0x803D

#define BASEBAND_I_DCO_USE_DCO 0x803E
#define BASEBAND_I_DCO_RESET 0x803F
#define BASEBAND_I_DCO_GAIN 0x8040

#define BASEBAND_Q_DCO_USE_DCO 0x8041
#define BASEBAND_Q_DCO_RESET 0x8042
#define BASEBAND_Q_DCO_GAIN 0x8043

#define BASEBAND_DAC_T0 0x8044
#define BASEBAND_DAC_T1 0x8045
#define BASEBAND_DAC_T2 0x8046
#define BASEBAND_DAC_T3 0x8047

#define BASEBAND_ENABLE_DEBUG 0x8048

#define BASEBAND_MUX_DBG_IN 0x804A
#define BASEBAND_MUX_DBG_OUT 0x804C

#define BASEBAND_ENABLE_RX_I 0x804E
#define BASEBAND_ENABLE_RX_Q 0x804F

#define BASEBAND_IMAGE_REJECTION_OP 0x8048

#define BASEBAND_LUT_CMD 0x8050

#define BASEBAND_RXERROR_MESSAGE 0x8054
#define BASEBAND_RXFINISH_MESSAGE 0x8058
#define BASEBAND_TXERROR_MESSAGE 0x805C

// Instruction macro
#define BASEBAND_INSTRUCTION(primaryInst, secondaryInst, data) ((primaryInst & 0xF) + ((secondaryInst & 0xF) << 4) + ((data & 0xFFFFFF) << 8))

// Primary instructions
#define BASEBAND_CONFIG 0
#define BASEBAND_SEND 1
#define BASEBAND_RECEIVE 2
#define BASEBAND_RECEIVE_EXIT 3
#define BASEBAND_DEBUG 15

// Secondary instructions
#define BASEBAND_CONFIG_CRC_SEED 0
#define BASEBAND_CONFIG_ACCESS_ADDRESS 1
#define BASEBAND_CONFIG_CHANNEL_INDEX 2

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
  reg_write32(BASEBAND_INST, BASEBAND_INSTRUCTION(BASEBAND_DEBUG, -1, byte_size));

  // TODO: Determine cycles to wait. Printing for now
  printf("Sent DEBUG instruction with data at address %.8x. Waiting...\n", addr);

  // Word-aligned address (word is 4 bytes, 32 bits)
  int mismatch = -1;
  int fail = 0;
  uint32_t res_addr = addr + ((byte_size & ~3) + ((byte_size % 4) > 0 ? 4 : 0)); 
  printf("Output bytes at address %.8x: ", res_addr);
  for (int i = 0; i < byte_size; i++) {
    printf("%.2x ", (unsigned)*(unsigned char*)(res_addr + i));
    // Checking match
    if ((unsigned)*(unsigned char*)(addr + i) != (unsigned)*(unsigned char*)(res_addr + i)) {
      fail += 1;
      if (mismatch == -1) mismatch = i;
    }
  }
  printf("\n");
  if (fail) {
    printf("FAILED TEST. %d bytes are mismatched in output. Index of first mismatch: %d\n", fail, mismatch);
  } else {
    printf("PASSED TEST. All input bytes match output bytes.\n");
  }
}
