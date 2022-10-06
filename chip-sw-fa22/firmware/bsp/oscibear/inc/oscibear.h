
#ifndef __OSCIBEAR_H
#define __OSCIBEAR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "rv_core.h"

/**
 * @brief Interrupt Number Definition, according to the selected device 
 */

typedef enum {
  UserSoftware_IRQn         = 0,
  SupervisorSoftware_IRQn   = 1,
  HypervisorSoftware_IRQn   = 2,
  MachineSoftware_IRQn      = 3,
  UserTimer_IRQn            = 4,
  SupervisorTimer_IRQn      = 5,
  HypervisorTimer_IRQn      = 6,
  MachineTimer_IRQn         = 7,
  UserExternal_IRQn         = 8,
  SupervisorExternal_IRQn   = 9,
  HypervisorExternal_IRQn   = 10,
  MachineExternal_IRQn      = 11,
} IRQn_Type;


void HAL_CORE_enableInterrupt();

void HAL_CORE_enableIRQ(IRQn_Type IRQn);


typedef struct {
  __IO uint32_t MSIP;                           /** MSIP Registers (1 bit wide) */
  uint32_t RESERVED0[4095];
  __IO uint64_t MTIMECMP;                       /** MTIMECMP Registers */
  uint32_t RESERVED1[8188];
  __IO uint64_t MTIME;                          /** Timer Register */
} CLINT_TypeDef;



typedef struct {
  __IO uint32_t priority_threshold;
  __IO uint32_t claim_complete;
} PLIC_ContextControl_TypeDef;

typedef struct {
  __IO uint32_t priorities[1024];
  __I  uint32_t pendings[1024];
  __IO uint32_t enables[1024];
} PLIC_TypeDef;

// because the maximum struct size is 65535, we need to split PLIC content
typedef struct {
  PLIC_ContextControl_TypeDef context_controls[1024];
} PLIC_Extra_TypeDef;

typedef struct {
  __I  uint32_t INPUT_VAL;                      /** pin value */
  __IO uint32_t INPUT_EN;                       /** pin input enable */
  __IO uint32_t OUTPUT_EN;                      /** Pin output enable */
  __IO uint32_t OUTPUT_VAL;                     /** Output value */
  __IO uint32_t PUE;                            /** Internal pull-up enable */
  __IO uint32_t DS;                             /** Pin drive strength */
  __IO uint32_t RISE_IE;                        /** Rise interrupt enable */
  __IO uint32_t RISE_IP;                        /** Rise interrupt pending */
  __IO uint32_t FALL_IE;                        /** Fall interrupt enable */
  __IO uint32_t FALL_IP;                        /** Fall interrupt pending */
  __IO uint32_t HIGH_IE;                        /** High interrupt pending */
  __IO uint32_t HIGH_IP;                        /** High interrupt pending */
  __IO uint32_t LOW_IE;                         /** Low interrupt pending */
  __IO uint32_t LOW_IP;                         /** Low interrupt pending */
  __IO uint32_t OUT_XOR;                        /** Output XOR (invert) */
} GPIO_TypeDef;

typedef struct {
} QSPI_TypeDef;

typedef struct {
  __IO uint32_t TXDATA;                         /** Transmit data register */
  __I  uint32_t RXDATA;                         /** Receive data register */
  __IO uint32_t TXCTRL;                         /** Transmit control register */
  __IO uint32_t RXCTRL;                         /** Receive control register */
  __IO uint32_t IE;                             /** UART interrupt enable */
  __I  uint32_t IP;                             /** UART interrupt pending */
  __IO uint32_t DIV;                            /** Baud rate divisor */
} UART_TypeDef;

typedef struct {
  __IO uint32_t INST;               // 0x8000
  __IO uint32_t ADDITIONAL_DATA;    // 0x8004

  __IO uint32_t STATUS0;            // 0x8008
  __IO uint32_t STATUS1;            // 0x800C
  __IO uint32_t STATUS2;            // 0x8010
  __IO uint32_t STATUS3;            // 0x8014
  __IO uint32_t STATUS4;            // 0x8018
  
  __IO uint8_t  TRIM_G0;            // 0x801C
  __IO uint8_t  TRIM_G1;            // 0x801D
  __IO uint8_t  TRIM_G2;            // 0x801E
  __IO uint8_t  TRIM_G3;            // 0x801F
  __IO uint8_t  TRIM_G4;            // 0x8020
  __IO uint8_t  TRIM_G5;            // 0x8021
  __IO uint8_t  TRIM_G6;            // 0x8022
  __IO uint8_t  TRIM_G7;            // 0x8023
  
  __IO uint8_t  MIXER_R1_R0;        // 0x8024
  __IO uint8_t  MIXER_R3_R2;        // 0x8025
  
  __IO uint16_t I_VGA_ATTEN_VALUE;          // 0x8026
  __IO uint8_t  I_VGA_ATTEN_RESET;          // 0x8028
  __IO uint8_t  I_VGA_ATTEN_USE_AGC;        // 0x8029
  __IO uint8_t  I_VGA_ATTEN_SAMPLE_WINDOW;  // 0x802A
  __IO uint8_t  I_VGA_ATTEN_IDEAL_P2P;      // 0x802B
  __IO uint8_t  I_VGA_ATTEN_GAIN;           // 0x802C
  __IO uint8_t  I_FILTER_R1_R0;     // 0x802D
  __IO uint8_t  I_FILTER_R3_R2;     // 0x802E
  __IO uint8_t  I_FILTER_R5_R4;     // 0x802F
  __IO uint8_t  I_FILTER_R7_R6;     // 0x8030
  __IO uint8_t  I_FILTER_R9_R8;     // 0x8031

  __IO uint16_t Q_VGA_ATTEN_VALUE;          // 0x8032
  __IO uint8_t  Q_VGA_ATTEN_RESET;          // 0x8034
  __IO uint8_t  Q_VGA_ATTEN_USE_AGC;        // 0x8035
  __IO uint8_t  Q_VGA_ATTEN_SAMPLE_WINDOW;  // 0x8036
  __IO uint8_t  Q_VGA_ATTEN_IDEAL_P2P;      // 0x8037
  __IO uint8_t  Q_VGA_ATTEN_GAIN;           // 0x8038
  __IO uint8_t  Q_FILTER_R1_R0;     // 0x8039
  __IO uint8_t  Q_FILTER_R3_R2;     // 0x803A
  __IO uint8_t  Q_FILTER_R5_R4;     // 0x803B
  __IO uint8_t  Q_FILTER_R7_R6;     // 0x803C
  __IO uint8_t  Q_FILTER_R9_R8;     // 0x803D

  __IO uint8_t  I_DCO_USE_DCO;      // 0x803E
  __IO uint8_t  I_DCO_RESET;        // 0x803F
  __IO uint8_t  I_DCO_GAIN;         // 0x8040

  __IO uint8_t  Q_DCO_USE_DCO;      // 0x8041
  __IO uint8_t  Q_DCO_RESET;        // 0x8042
  __IO uint8_t  Q_DCO_GAIN;         // 0x8043
  
  __IO uint8_t  DAC_T0;             // 0x8044
  __IO uint8_t  DAC_T1;             // 0x8045
  __IO uint8_t  DAC_T2;             // 0x8046
  __IO uint8_t  DAC_T3;             // 0x8047
  
  __IO uint16_t ENABLE_DEBUG;       // 0x8048
  
  __IO uint16_t MUX_DBG_IN;         // 0x804A
  __IO uint16_t MUX_DBG_OUT;        // 0x804C
  
  __IO uint8_t  ENABLE_RX_I;        // 0x804E
  __IO uint8_t  ENABLE_RX_Q;        // 0x804F
  
  // __IO uint16_t IMAGE_REJECTION_OP; // 0x8048
  __IO uint32_t LUT_CMD;            // 0x8050

  __IO uint32_t RXERROR_MESSAGE;    // 0x8054
  __IO uint32_t RXFINISH_MESSAGE;   // 0x8058
  __IO uint32_t TXERROR_MESSAGE;    // 0x805C
  __IO uint32_t FIR_CMD;            // 0x8060
} BASEBAND_TypeDef;


/* ================ memory map ================ */
#define DEBUG_CONTROLLER_BASE   0x00000000
#define BOOT_SELECT_BASE        0x00002000
#define ERROR_DEVICE_BASE       0x00003000
#define BASEBAND_BASE           0x00008000
#define BOOTROM_BASE            0x00010000
#define TILE_RESET_CTRL_BASE    0x00100000
#define CLINT_BASE              0x02000000
#define PLIC_BASE               0x0C000000
#define LBWIF_RAM_BASE          0x10000000
#define GPIO_BASE               0x10012000
#define QSPI_BASE               0x10040000
#define FLASH_BASE              0x20000000
#define UART_BASE               0x54000000
#define DTIM_BASE               0x80000000

#define GPIOA_BASE              GPIO_BASE
#define QSPI0_BASE              QSPI_BASE
#define UART0_BASE              UART_BASE

#define CLINT                   ((CLINT_TypeDef *)CLINT_BASE)
#define PLIC                    ((PLIC_TypeDef *)PLIC_BASE)
#define PLIC_EXTRA              ((PLIC_Extra_TypeDef *)(PLIC_BASE + 0x00200000U))
#define GPIOA                   ((GPIO_TypeDef *)GPIOA_BASE)
#define UART0                   ((UART_TypeDef *)UART0_BASE)
#define BASEBAND                ((BASEBAND_TypeDef *)BASEBAND_BASE)


#define UART_RXCTRL_RXEN_POS                    (0U)
#define UART_RXCTRL_RXEN_MSK                    (0x1UL << UART_RXCTRL_RXEN_POS)
#define UART_RXCTRL_RXCNT_POS                   (16U)
#define UART_RXCTRL_RXCNT_MSK                   (0x7UL << UART_RXCTRL_RXCNT_POS)
#define UART_TXCTRL_TXEN_POS                    (0U)
#define UART_TXCTRL_TXEN_MSK                    (0x1UL << UART_TXCTRL_TXEN_POS)
#define UART_TXCTRL_NSTOP_POS                   (1U)
#define UART_TXCTRL_NSTOP_MSK                   (0x1UL << UART_TXCTRL_NSTOP_POS)
#define UART_TXCTRL_TXCNT_POS                   (16U)
#define UART_TXCTRL_TXCNT_MSK                   (0x7UL << UART_RXCTRL_RXCNT_POS)


#define MIE_USIE_POS                  0x00U
#define MIE_USIE_MSK                  (1U << MIE_USIE_POS)
#define MIE_SSIE_POS                  0x01U
#define MIE_SSIE_MSK                  (1U << MIE_SSIE_POS)
#define MIE_VSSIE_POS                 0x02U
#define MIE_VSSIE_MSK                 (1U << MIE_VSSIE_POS)
#define MIE_MSIE_POS                  0x03U
#define MIE_MSIE_MSK                  (1U << MIE_MSIE_POS)
#define MIE_UTIE_POS                  0x04U
#define MIE_UTIE_MSK                  (1U << MIE_UTIE_POS)
#define MIE_STIE_POS                  0x05U
#define MIE_STIE_MSK                  (1U << MIE_STIE_POS)
#define MIE_VSTIE_POS                 0x06U
#define MIE_VSTIE_MSK                 (1U << MIE_VSTIE_POS)
#define MIE_MTIE_POS                  0x07U
#define MIE_MTIE_MSK                  (1U << MIE_MTIE_POS)
#define MIE_UEIE_POS                  0x08U
#define MIE_UEIE_MSK                  (1U << MIE_UEIE_POS)
#define MIE_SEIE_POS                  0x09U
#define MIE_SEIE_MSK                  (1U << MIE_SEIE_POS)
#define MIE_VSEIE_POS                 0x0AU
#define MIE_VSEIE_MSK                 (1U << MIE_VSEIE_POS)
#define MIE_MEIE_POS                  0x0BU
#define MIE_MEIE_MSK                  (1U << MIE_MEIE_POS)
#define MIE_SGEIE_POS                 0x0CU
#define MIE_SGEIE_MSK                 (1U << MIE_SGEIE_POS)

#define MIE_USIP_POS                  0x00U
#define MIE_USIP_MSK                  (1U << MIE_USIP_POS)
#define MIP_SSIP_POS                  0x01U
#define MIP_SSIP_MSK                  (1U << MIP_SSIP_POS)
#define MIP_VSSIP_POS                 0x02U
#define MIP_VSSIP_MSK                 (1U << MIP_VSSIP_POS)
#define MIP_MSIP_POS                  0x03U
#define MIP_MSIP_MSK                  (1U << MIP_MSIP_POS)
#define MIE_UTIP_POS                  0x04U
#define MIE_UTIP_MSK                  (1U << MIE_UTIP_POS)
#define MIP_STIP_POS                  0x05U
#define MIP_STIP_MSK                  (1U << MIP_STIP_POS)
#define MIP_VSTIP_POS                 0x06U
#define MIP_VSTIP_MSK                 (1U << MIP_VSTIP_POS)
#define MIP_MTIP_POS                  0x07U
#define MIP_MTIP_MSK                  (1U << MIP_MTIP_POS)
#define MIP_SEIP_POS                  0x09U
#define MIP_SEIP_MSK                  (1U << MIP_SEIP_POS)
#define MIP_VSEIP_POS                 0x0AU
#define MIP_VSEIP_MSK                 (1U << MIP_VSEIP_POS)
#define MIP_MEIP_POS                  0x0BU
#define MIP_MEIP_MSK                  (1U << MIP_MEIP_POS)
#define MIP_SGEIP_POS                 0x0CU
#define MIP_SGEIP_MSK                 (1U << MIP_SGEIP_POS)


#ifdef __cplusplus
}
#endif

#endif /* __OSCIBEAR_H */