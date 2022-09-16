#include "ee290c_devices.h"
#include <stdio.h>

void ee290c_uart_init(uint32_t addr, struct ee290c_uart* device)
{
  device->registers = (struct ee290c_uart_registers*) addr;
  device->clk_freq = 20000000; // FIXME: Use a frequency from the DTS?
  device->baud_rate = 115200;
  
  device->registers->txctrl = EE290C_UART_TXCTRL_EN; // Enables TX
  device->registers->rxctrl = EE290C_UART_RXCTRL_EN; // Enables RX
  device->registers->div = device->clk_freq / device->baud_rate - 1;
}

int ee290c_uart_fifo_read(struct ee290c_uart* device, uint8_t* data, const int size)
{
  volatile struct ee290c_uart_registers *uart = device->registers;
  int i;
  
  for (i = 0; i < size; i++)
  {
    uint32_t val = uart->rx;
    if (val & EE290C_UART_RX_EMPTY) break;
    
    data[i] = val & EE290C_UART_DATA;
  }
  
  return i;
}

int ee290c_uart_fifo_write(struct ee290c_uart* device, uint8_t* data, const int size)
{
  volatile struct ee290c_uart_registers *uart = device->registers;
  int i;
  
  for (i = 0; i < size && !(uart->tx & EE290C_UART_TX_FULL); i++)
  {
    uart->tx = data[i];
  }
  
  return i;
}

void ee290c_uart_putc(struct ee290c_uart* device, char c)
{
  int bytes_written = 0;
  while (bytes_written == 0)
  {
    bytes_written += ee290c_uart_fifo_write(device, &c, 1);
  }
}

void ee290c_uart_puts(struct ee290c_uart* device, char* string)
{
  // Probably not efficient, should utilize the UART FIFO
  // But that requries some form of pre known string length
  for (char* c = string; *c != '\0'; c++)
  {
    ee290c_uart_putc(device, *c);
  }
}

// Trap handler & interrupt stuff

void (*clint_handler_vector[16])() = { 0 };

#define MCAUSE_INT_MASK  0x80000000
#define MCAUSE_CODE_MASK 0x7FFFFFFF

uintptr_t handle_trap(uintptr_t cause, uintptr_t epc, uintptr_t regs[32])
{
  if (cause & MCAUSE_INT_MASK) {
    // Handle interrupt
    int code = cause & MCAUSE_CODE_MASK;
    if (clint_handler_vector[code])
    {
      (*clint_handler_vector[code])();
    }
  } else {
    // Handle exception
    printf("Exception encountered (cause = %x, epc = %x)\n", cause & MCAUSE_CODE_MASK, epc);
    for (int i = 1; i < 32; i++)
    {
      printf("x%d = 0x%x (%dd)\n", i, regs[i], regs[i]);
    }
  }
  
  return epc;
}

void mstatus_enable_interrupt()
{
  // Set MPIE
  uint32_t mask = (1 << 3);
  asm ("csrs mstatus, %0" :: "r"(mask));
}

void mstatus_disable_interrupt()
{
  // Set MPIE
  uint32_t mask = (1 << 3);
  asm ("csrc mstatus, %0" :: "r"(mask));
}


// PLIC device registers

typedef struct plic_context_control
{
  uint32_t priority_threshold;
  uint32_t claim_complete;
} plic_context_control_t __attribute__ ((aligned (0x1000)));

              uint32_t *const plic_priorities   =               (uint32_t* const) 0xc000000; // priorities [0..1023]
     readonly uint32_t *const plic_pending_bits =      (readonly uint32_t* const) 0xc001000; // pending bitmap registers [0..31]
              uint32_t *const plic_enables      =               (uint32_t* const) 0xc002000; // Context [0..15871], sources bitmap registers [0..31]
plic_context_control_t *const plic_ctx_controls = (plic_context_control_t* const) 0xc200000; // Priority threshold & claim / complete for context [0..15871]

// In Rocket's implementation context are harts

// CLINT device registers

typedef struct clint_time_reg
{
  uint32_t lo, hi;
} clint_time_reg_t;

                 uint32_t *const _clint_msip     =                  (uint32_t*) 0x2000000;
         clint_time_reg_t *const _clint_mtimecmp =          (clint_time_reg_t*) 0x2004000;
readonly clint_time_reg_t *const _clint_mtime    = (readonly clint_time_reg_t*) 0x200BFF8;

#define clint_msip (*_clint_msip)
#define clint_mtimecmp (*_clint_mtimecmp)
#define clint_mtime (*_clint_mtime)

void clint_connect_interrupt(int code, void (*handler_ptr)())
{
  clint_handler_vector[code] = handler_ptr;
}

void clint_machine_interrupt_enable(int code)
{
  uint32_t mask = 1 << code;
  asm ("csrs mie, %0" :: "r"(mask));
}

void clint_machine_interrupt_disble(int code)
{
  uint32_t mask = 1 << code;
  asm ("csrc mie, %0" :: "r"(mask));
}

uint64_t clint_get_mtime()
{
	volatile uint32_t lo, hi;

	/* Likewise, must guard against rollover when reading */
	do {
		hi = clint_mtime.hi;
		lo = clint_mtime.lo;
	} while (clint_mtime.hi != hi);

	return (((uint64_t)hi) << 32) | lo;
}

void clint_set_mtimecmp(uint64_t time)
{
	/* Per spec, the RISC-V MTIME/MTIMECMP registers are 64 bit,
	 * but are NOT internally latched for multiword transfers.  So
	 * we have to be careful about sequencing to avoid triggering
	 * spurious interrupts: always set the high word to a max
	 * value first.
	 */
	clint_mtimecmp.hi = 0xffffffff;
	clint_mtimecmp.lo = (uint32_t)time;
	clint_mtimecmp.hi = (uint32_t)(time >> 32);
}

void plic_set_bit(uint32_t* const target, uint32_t index)
{
  uint32_t reg_index = index >> 5;
  uint32_t bit_index = index & 0x1F;
  *target |= (1 << bit_index);
}

void plic_clear_bit(uint32_t* const target, uint32_t index)
{
  uint32_t reg_index = index >> 5;
  uint32_t bit_index = index & 0x1F;
  *target &= ~(1 << bit_index);
}

void plic_enable_for_hart(uint32_t hart_id, uint32_t irq_id)
{
  uint32_t* base = plic_enables + 32 * hart_id;
  plic_set_bit(base, irq_id);
}

void plic_set_priority_threshold(uint32_t hart_id, uint32_t priority)
{
  plic_ctx_controls[hart_id].priority_threshold = priority;
}

void plic_disable_for_hart(uint32_t hart_id, uint32_t irq_id)
{
  uint32_t* base = plic_enables + 32 * hart_id;
  plic_clear_bit(base, irq_id);
}

void plic_set_priority(uint32_t irq_id, uint32_t priority)
{
  plic_priorities[irq_id] = priority;
}

uint32_t plic_claim_irq(uint32_t hart_id)
{
  return plic_ctx_controls[hart_id].claim_complete;
}

void plic_complete_irq(uint32_t hart_id, uint32_t irq_id)
{
  plic_ctx_controls[hart_id].claim_complete = irq_id;
}

