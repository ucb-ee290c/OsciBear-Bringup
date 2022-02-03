#ifndef _INCLUDE_EE290C_DEVICES
#define _INCLUDE_EE290C_DEVICES

#include <string.h>
#include <stdint.h>

// ============================================================================
//  Drivers for the UART
// ----------------------------------------------------------------------------
//  References:
//  https://github.com/sifive/riscv-zephyr/blob/master/drivers/serial/uart_sifive.c
//  https://sifive.cdn.prismic.io/sifive/4d063bf8-3ae6-4db6-9843-ee9076ebadf7_fe310-g000.pdf
// ============================================================================

// Device registers
struct ee290c_uart_registers
{
  uint32_t tx; // Transmit data register
  uint32_t rx; // Receive data register
  uint32_t txctrl; // Transmit control register
  uint32_t rxctrl; // Receive control register
  uint32_t ie; // UART Interrupt enable
  uint32_t ip; // UART Interrupt pending
  uint32_t div; // Baud rate divisor
};

// Constants
#define EE90C_UART_DEFAULT_ADDR 0x54000000

// Register masks
#define EE290C_UART_TX_FULL (1 << 31)
#define EE290C_UART_RX_EMPTY (1 << 31)
#define EE290C_UART_DATA (0xFF)
#define EE290C_UART_TXCTRL_EN (0x1)
#define EE290C_UART_RXCTRL_EN (0x1)
#define EE290C_UART_IRQ_TX (1 << 0) // TX Interrupt enable / pending
#define EE290C_UART_IRQ_RX (1 << 1) // RX Interrupt enable / pending

// Device
struct ee290c_uart
{
  struct ee290c_uart_registers* registers;
  uint32_t clk_freq;
  uint32_t baud_rate;
};

// Functions
void ee290c_uart_init(uint32_t addr, struct ee290c_uart* device);

// Return bytes read / write
int ee290c_uart_fifo_read(struct ee290c_uart* device, uint8_t* data, const int size);
int ee290c_uart_fifo_write(struct ee290c_uart* device, uint8_t* data, const int size);

// Write char / string (with spin wait)
void ee290c_uart_putc(struct ee290c_uart* device, char c);
void ee290c_uart_puts(struct ee290c_uart* device, char* string);

// ============================================================================
//  Interrupt stuff
// ----------------------------------------------------------------------------
//  References:
//  https://sifive.cdn.prismic.io/sifive/0d163928-2128-42be-a75a-464df65e04e0_sifive-interrupt-cookbook.pdf
// ============================================================================

#define MAX_CLINT_INTERRUPTS 16

#define INT_CODE_USER_SOFTWARE 0
#define INT_CODE_SUPERVISOR_SOFTWARE 1
#define INT_CODE_MACHINE_SOFTWARE 3
#define INT_CODE_USER_TIMER 4
#define INT_CODE_SUPERVISOR_TIMER 5
#define INT_CODE_MACHINE_TIMER 7
#define INT_CODE_USER_EXTERNAL 8
#define INT_CODE_SUPERVISOR_EXTERNAL 9
#define INT_CODE_MACHINE_EXTERNAL 11

#define readonly const volatile

void plic_set_bit(uint32_t* const target, uint32_t index);
void plic_clear_bit(uint32_t* const target, uint32_t index);

void plic_enable_for_hart(uint32_t hart_id, uint32_t irq_id);
void plic_disable_for_hart(uint32_t hart_id, uint32_t irq_id);

void plic_set_priority(uint32_t irq_id, uint32_t priority);
void plic_set_priority_threshold(uint32_t hart_id, uint32_t priority);

uint32_t plic_claim_irq(uint32_t hart_id);
void plic_complete_irq(uint32_t hart_id, uint32_t irq_id);

void mstatus_enable_interrupt();
void mstatus_disable_interrupt();

void clint_connect_interrupt(int code, void (*handler_ptr)());

void clint_machine_interrupt_enable(int code);
void clint_machine_interrupt_disble(int code);

uint64_t clint_get_mtime();
void clint_set_mtimecmp(uint64_t time);

#endif
