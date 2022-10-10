
#include "oscibear_hal_baseband.h"

void BLE_send(uint8_t *data, uint16_t size) {
  BASEBAND->ADDITIONAL_DATA = (uint32_t)data;
  BASEBAND->INST = BASEBAND_INSTRUCTION(BASEBAND_SEND, 0, size);
}

uint32_t BASEBAND_getRXErrorMessage() {
  return BASEBAND->RXERROR_MESSAGE;
}

uint32_t BASEBAND_getTXErrorMessage() {
  return BASEBAND->TXERROR_MESSAGE;
}

uint32_t BASEBAND_getRXCompleteMessage() {
  return BASEBAND->RXFINISH_MESSAGE;
}
