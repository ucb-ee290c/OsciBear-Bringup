#include "baseband.h"
#include "mmio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int main(void)
{
  printf("Writing MMIO: Tuning Registers\n");

  // Setting Trim MMIO
  baseband_tuning_set(BASEBAND_TRIM_G0, 255, 8, 0); // FF
  printf("Value read for trim_g0 %x\n", reg_read8(BASEBAND_TRIM_G0));
  baseband_tuning_set(BASEBAND_TRIM_G1, 253, 8, 0); // FD
  printf("Value read for trim_g1 %x\n", reg_read8(BASEBAND_TRIM_G1));
  baseband_tuning_set(BASEBAND_TRIM_G2, 251, 8, 0); // FB
  baseband_tuning_set(BASEBAND_TRIM_G3, 249, 8, 0); // F9
  baseband_tuning_set(BASEBAND_TRIM_G4, 247, 8, 0); // F7
  baseband_tuning_set(BASEBAND_TRIM_G5, 245, 8, 0); // F5
  baseband_tuning_set(BASEBAND_TRIM_G6, 243, 8, 0); // F3
  baseband_tuning_set(BASEBAND_TRIM_G7, 241, 8, 0); // F1
  printf("Value read for trim_g7 %x\n", reg_read8(BASEBAND_TRIM_G7));

  // Setting Mixer MMIO
  baseband_tuning_set(BASEBAND_MIXER_R1_R0, 15, 4, 4); // R1 = F
  baseband_tuning_set(BASEBAND_MIXER_R1_R0,  3, 4, 0); // R0 = 3
  printf("Value read for mixer r1 r0 %x\n", reg_read8(BASEBAND_MIXER_R1_R0));
  baseband_tuning_set(BASEBAND_MIXER_R3_R2,  8, 4, 4); // R3 = 8
  baseband_tuning_set(BASEBAND_MIXER_R3_R2, 15, 4, 0); // R2 = F

  // Setting I_VGA MMIO (currently only value)
  // Writing overflow value to test
  baseband_tuning_set(BASEBAND_I_VGA_ATTEN_VALUE, 65535, 10, 0); // 3FF
  printf("Value read for I VGA Atten %x\n", reg_read16(BASEBAND_I_VGA_ATTEN_VALUE));

  // Setting I_Filter MMIO
  baseband_tuning_set(BASEBAND_I_FILTER_R1_R0,  3, 4, 0); // R0 = 3
  baseband_tuning_set(BASEBAND_I_FILTER_R1_R0, 15, 4, 4); // R1 = F
  printf("Value read for I filter r1 r0 %x\n", reg_read8(BASEBAND_I_FILTER_R1_R0));
  baseband_tuning_set(BASEBAND_I_FILTER_R3_R2, 15, 4, 0); // R3 = F
  baseband_tuning_set(BASEBAND_I_FILTER_R3_R2,  8, 4, 4); // R2 = 8
  baseband_tuning_set(BASEBAND_I_FILTER_R5_R4,  5, 4, 0); // R4 = 5
  baseband_tuning_set(BASEBAND_I_FILTER_R5_R4, 11, 4, 4); // R5 = B
  baseband_tuning_set(BASEBAND_I_FILTER_R7_R6,  1, 4, 0); // R6 = 1
  baseband_tuning_set(BASEBAND_I_FILTER_R7_R6, 13, 4, 4); // R7 = D
  baseband_tuning_set(BASEBAND_I_FILTER_R9_R8, 15, 4, 0); // R8 = F
  baseband_tuning_set(BASEBAND_I_FILTER_R9_R8, 15, 4, 4); // R9 = F

  // Setting Q_VGA MMIO (currently only value)
  // Writing overflow value to test
  baseband_tuning_set(BASEBAND_Q_VGA_ATTEN_VALUE, 65535, 10, 0); // 3FF (10 bits)
  printf("Value read for Q VGA Atten %x\n", reg_read16(BASEBAND_Q_VGA_ATTEN_VALUE));

  // Setting Q_Filter MMIO
  baseband_tuning_set(BASEBAND_Q_FILTER_R1_R0,  3, 4, 0); // R0 = 3
  baseband_tuning_set(BASEBAND_Q_FILTER_R1_R0, 15, 4, 4); // R1 = F
  printf("Value read for Q filter r1 r0 %x\n", reg_read8(BASEBAND_Q_FILTER_R1_R0));
  baseband_tuning_set(BASEBAND_Q_FILTER_R3_R2, 15, 4, 0); // R3 = F
  baseband_tuning_set(BASEBAND_Q_FILTER_R3_R2,  8, 4, 4); // R2 = 8
  baseband_tuning_set(BASEBAND_Q_FILTER_R5_R4,  5, 4, 0); // R4 = 5
  baseband_tuning_set(BASEBAND_Q_FILTER_R5_R4, 11, 4, 4); // R5 = B
  baseband_tuning_set(BASEBAND_Q_FILTER_R7_R6,  1, 4, 0); // R6 = 1
  baseband_tuning_set(BASEBAND_Q_FILTER_R7_R6, 13, 4, 4); // R7 = D
  baseband_tuning_set(BASEBAND_Q_FILTER_R9_R8, 15, 4, 0); // R8 = F
  baseband_tuning_set(BASEBAND_Q_FILTER_R9_R8, 15, 4, 4); // R9 = F

  // Setting DAC MMIO
  baseband_tuning_set(BASEBAND_DAC_T0, 63, 6, 0); // 3F
  printf("Value read for DAC t0 %x\n", reg_read16(BASEBAND_DAC_T0));
  baseband_tuning_set(BASEBAND_DAC_T1, 61, 6, 0); // 3D
  baseband_tuning_set(BASEBAND_DAC_T2, 59, 6, 0); // 3B
  baseband_tuning_set(BASEBAND_DAC_T3, 57, 6, 0); // 39

  // Setting Enable_RX MMIO
  // Writing overflow value to test
  baseband_tuning_set(BASEBAND_ENABLE_RX_I, 255, 5, 0); // 1F (5 bits)
  printf("Value read for EN RX I %x\n", reg_read8(BASEBAND_ENABLE_RX_I));
  baseband_tuning_set(BASEBAND_ENABLE_RX_Q, 253, 5, 0); // 1D (5 bits)

  // Setting MUX_DBG
  // Writing overflow value to test
  baseband_tuning_set(BASEBAND_MUX_DBG_IN,  65535, 10, 0); // 3FF (10 bits)
  printf("Value read for MUX DBG IN %x\n", reg_read16(BASEBAND_MUX_DBG_IN));
  baseband_tuning_set(BASEBAND_MUX_DBG_OUT, 65535, 10, 0); // 3FF (10 bits)
  
  printf("Done writing MMIO\n");

  return 0;
}
