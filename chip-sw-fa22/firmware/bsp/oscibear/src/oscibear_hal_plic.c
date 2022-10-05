
#include "oscibear_hal_plic.h"

void HAL_PLIC_disable(uint32_t hart_id, uint32_t irq_id) {
  uint32_t reg_index = irq_id >> 5;
  uint32_t bit_index = irq_id & 0x1F;
  CLEAR_BITS(PLIC->enables[hart_id], 1 << bit_index);
}

void HAL_PLIC_enable(uint32_t hart_id, uint32_t irq_id) {
  uint32_t reg_index = irq_id >> 5;
  uint32_t bit_index = irq_id & 0x1F;
  SET_BITS(PLIC->enables[hart_id], 1 << bit_index);
}

void HAL_PLIC_setPriority(uint32_t irq_id, uint32_t priority) {
  PLIC->priorities[irq_id] = priority;
}

uint32_t HAL_PLIC_claimIRQ(uint32_t hart_id) {
  return PLIC->context_controls[hart_id].claim_complete;
}

void HAL_PLIC_completeIRQ(uint32_t hart_id, uint32_t irq_id) {
  PLIC->context_controls[hart_id].claim_complete = irq_id;
}




void plic_complete_irq(uint32_t hart_id, uint32_t irq_id){
  // plic_ctx_controls[hart_id].claim_complete = irq_id;
  HAL_PLIC_completeIRQ(hart_id, irq_id);
}
uint32_t plic_claim_irq(uint32_t hart_id) {
  // return plic_ctx_controls[hart_id].claim_complete;
  return HAL_PLIC_claimIRQ(hart_id);
}
void plic_set_bit(uint32_t* const target, uint32_t index) {
  uint32_t reg_index = index >> 5;
  uint32_t bit_index = index & 0x1F;
  *target |= (1 << bit_index);
}
void plic_enable_for_hart(uint32_t hart_id, uint32_t irq_id) {
  // uint32_t* base = plic_enables + 32 * hart_id;
  // plic_set_bit(base, irq_id);
  HAL_PLIC_enable(hart_id, irq_id);
}
void plic_set_priority(uint32_t irq_id, uint32_t priority) {
  // plic_priorities[irq_id] = priority;
  HAL_PLIC_setPriority(irq_id, priority);
}