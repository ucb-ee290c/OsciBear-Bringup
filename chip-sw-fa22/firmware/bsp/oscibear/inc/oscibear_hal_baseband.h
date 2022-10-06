
#ifndef __OSCIBEAR_HAL_BASEBAND_H
#define __OSCIBEAR_HAL_BASEBAND_H

#ifdef __cplusplus
extern "C" {
#endif

#include "oscibear.h"

// Instruction macro
#define BASEBAND_INSTRUCTION(primary_inst, secondary_inst, data) ((READ_BITS(data, 0xFFFFFF) << 8) | (READ_BITS(secondary_inst, 0xF) << 4) | READ_BITS(primary_inst, 0xF))

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

#define LOOPBACK_NONE     0x0
#define LOOPBACK_BASEBAND 0x2

void BLE_send(uint8_t *data, uint16_t size);

uint32_t BASEBAND_getRXErrorMessage();

uint32_t BASEBAND_getTXErrorMessage();

uint32_t BASEBAND_getRXCompleteMessage();

#ifdef __cplusplus
}
#endif

#endif /* __OSCIBEAR_HAL_BASEBAND_H */
