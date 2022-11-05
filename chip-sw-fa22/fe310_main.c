/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
  #define   __I     volatile             /** Defines "read only" permissions */
#else
  #define   __I     volatile const       /** Defines "read only" permissions */
#endif
#define     __O     volatile             /** Defines "write only" permissions */
#define     __IO    volatile             /** Defines "read / write" permissions */

/* following defines should be used for structure members */
#define     __IM     volatile const      /** Defines "read only" structure member permissions */
#define     __OM     volatile            /** Defines "write only" structure member permissions */
#define     __IOM    volatile            /** Defines "read / write" structure member permissions */

/* ================ basic types ================ */
#define SET_BITS(REG, BIT)                    ((REG) |= (BIT))
#define CLEAR_BITS(REG, BIT)                  ((REG) &= ~(BIT))
#define READ_BITS(REG, BIT)                   ((REG) & (BIT))
#define WRITE_BITS(REG, CLEARMASK, SETMASK)   ((REG) = (((REG) & (~(CLEARMASK))) | (SETMASK)))


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

#define GPIO_PIN_0				0b00000000000000000000000000000001
#define GPIO_PIN_1				0b00000000000000000000000000000010
#define GPIO_PIN_2				0b00000000000000000000000000000100
#define GPIO_PIN_3				0b00000000000000000000000000001000
#define GPIO_PIN_4				0b00000000000000000000000000010000
#define GPIO_PIN_5				0b00000000000000000000000000100000
#define GPIO_PIN_6				0b00000000000000000000000001000000
#define GPIO_PIN_7				0b00000000000000000000000010000000
#define GPIO_PIN_8				0b00000000000000000000000100000000
#define GPIO_PIN_9				0b00000000000000000000001000000000
#define GPIO_PIN_10				0b00000000000000000000010000000000
#define GPIO_PIN_11				0b00000000000000000000100000000000
#define GPIO_PIN_12  			0b00000000000000000001000000000000
#define GPIO_PIN_13  			0b00000000000000000010000000000000
#define GPIO_PIN_14  			0b00000000000000000100000000000000
#define GPIO_PIN_15  			0b00000000000000001000000000000000
#define GPIO_PIN_16				0b00000000000000010000000000000000
#define GPIO_PIN_17				0b00000000000000100000000000000000
#define GPIO_PIN_18				0b00000000000001000000000000000000
#define GPIO_PIN_19				0b00000000000010000000000000000000
#define GPIO_PIN_20				0b00000000000100000000000000000000
#define GPIO_PIN_21				0b00000000001000000000000000000000
#define GPIO_PIN_22				0b00000000010000000000000000000000
#define GPIO_PIN_23				0b00000000100000000000000000000000
#define GPIO_PIN_24				0b00000001000000000000000000000000
#define GPIO_PIN_25				0b00000010000000000000000000000000
#define GPIO_PIN_26				0b00000100000000000000000000000000
#define GPIO_PIN_27				0b00001000000000000000000000000000
#define GPIO_PIN_28  			0b00010000000000000000000000000000
#define GPIO_PIN_29  			0b00100000000000000000000000000000
#define GPIO_PIN_30  			0b01000000000000000000000000000000
#define GPIO_PIN_31  			0b10000000000000000000000000000000


#define GPIO_BASE               0x10012000
#define GPIOA_BASE              GPIO_BASE
#define GPIOA                   ((GPIO_TypeDef *)GPIOA_BASE)

#define PRCI_BASE				0x10008000

#define TL_CLK  			GPIO_PIN_0
#define TL_MOSI_DAT  		GPIO_PIN_18
#define TL_MOSI_VAL  		GPIO_PIN_19
#define TL_MOSI_RDY  		GPIO_PIN_20
#define TL_MISO_DAT  		GPIO_PIN_21
#define TL_MISO_VAL  		GPIO_PIN_22
#define TL_MISO_RDY  		GPIO_PIN_23

#define DEBUG_0				GPIO_PIN_1
#define DEBUG_1				GPIO_PIN_2


/// ==============================================
#include <metal/machine.h>

// METAL_SIFIVE_FE310_G000_PRCI_10008000_BASE_ADDRESS is defined in
// <metal/machine/platform.h> but that's not used here (who would
// want to use a decimal number for an address?)
#define PLLCFG          *(volatile uint32_t*)0x10008008
#define BITS(idx, val)  ((val) << (idx))

#define PLLR_2 1
#define PLLQ_2 1

// PLLCFG register bit indexes
#define PLLR_I        0U
#define PLLF_I        4U
#define PLLQ_I        10U
#define PLLSEL_I      16U
#define PLLREFSEL_I   17U
#define PLLBYPASS_I   18U
#define PLLLOCK_I     31U


// Supported clock: 64, 128, 256, 320 MHz
int cpu_clock_config(int frequency) {

    uint32_t cfg_temp = 0;

    if (frequency != 64 && frequency != 128 && frequency != 256 && frequency != 320) return 0;

    if (frequency == 320){
		/* There is a 16 MHz crystal oscillator HFXOSC on the board */
		cfg_temp |= BITS(PLLREFSEL_I, 1);     // Drive PLL from 16 MHz HFXOSC
		cfg_temp |= BITS(PLLR_I, PLLR_2);     // Divide ratio. R=2 for 8 MHz out
		cfg_temp |= BITS(PLLF_I, 80/2 - 1U);  // Multiply ratio. 8 MHz x 40 is 640 MHz out
		cfg_temp |= BITS(PLLQ_I, PLLQ_2);     // Divide 640MHz with 2 to get 320 MHz
		// PLLSEL_I = 0    : PLL is not driving hfclk for now
		// PLLBYPASS_I = 0 : Enables PLL
		PLLCFG = cfg_temp;
    }
    else{
		/* There is a 16 MHz crystal oscillator HFXOSC on the board */
		cfg_temp |= BITS(PLLREFSEL_I, 1);     	// Drive PLL from 16 MHz HFXOSC.

		cfg_temp |= BITS(PLLR_I, 1U);     		// Divide ratio.
												// R="1U" is treated as a divide ratio of 2.
												// This gives 16MHz / 2 = 8MHz.
												// This is within range for refr.

		cfg_temp |= BITS(PLLF_I, 31U);  		// Multiply ratio.
												// F="31U" gives a multiply ratio of 2 * ("31U" + 1) = 64.
												// This gives 8MHz * 64 = 512MHz.
												// This is within range for vco.

		if (frequency == 256) cfg_temp |= BITS(PLLQ_I, 1U);     		// Divide again.
												// Q="1U" gives a division ratio of 2.
												// This gives 512MHz / 2 = 256MHz.
												// This is within range for pllout.
		if (frequency == 128) cfg_temp |= BITS(PLLQ_I, 2U);     		// Divide again.
												// Q="2U" gives a division ratio of 4.
												// This gives 512MHz / 4 = 128MHz.
												// This is within range for pllout.

		if (frequency == 64) cfg_temp |= BITS(PLLQ_I, 3U);     		// Divide again.
												// Q="3U" gives a division ratio of 8.
												// This gives 512MHz / 8 = 64MHz.
												// This is within range for pllout.
		PLLCFG = cfg_temp;
    }


    while ( PLLCFG & BITS(PLLLOCK_I, 1) == 0) {} // Wait until PLL locks
    PLLCFG |= BITS(PLLSEL_I, 1);          // Let PLL drive hfclk

    return 1;
}
/// ==============================================


#define TL_CH_A_OPCODE_GET        0x04
#define TL_CH_A_OPCODE_PULFULLDATA        0x00

#define TL_SERDES_LAST_SIZE       0
#define TL_SERDES_LAST_OFFSET     (TL_SERDES_LAST_SIZE)
#define TL_SERDES_MASK_SIZE       8
#define TL_SERDES_MASK_OFFSET     (TL_SERDES_LAST_OFFSET + TL_SERDES_MASK_SIZE)
#define TL_SERDES_CORRUPT_SIZE    1
#define TL_SERDES_CORRUPT_OFFSET  (TL_SERDES_MASK_OFFSET + TL_SERDES_CORRUPT_SIZE)
#define TL_SERDES_DATA_SIZE       64
#define TL_SERDES_DATA_OFFSET     (TL_SERDES_CORRUPT_OFFSET + TL_SERDES_DATA_SIZE)
#define TL_SERDES_ADDRESS_SIZE    32
#define TL_SERDES_ADDRESS_OFFSET  (TL_SERDES_DATA_OFFSET + TL_SERDES_ADDRESS_SIZE)
#define TL_SERDES_SOURCE_SIZE     4
#define TL_SERDES_SOURCE_OFFSET   (TL_SERDES_ADDRESS_OFFSET + TL_SERDES_SOURCE_SIZE)
#define TL_SERDES_SIZE_SIZE       4
#define TL_SERDES_SIZE_OFFSET     (TL_SERDES_SOURCE_OFFSET + TL_SERDES_SIZE_SIZE)
#define TL_SERDES_PARAM_SIZE      3
#define TL_SERDES_PARAM_OFFSET    (TL_SERDES_SIZE_OFFSET + TL_SERDES_PARAM_SIZE)
#define TL_SERDES_OPCODE_SIZE     3
#define TL_SERDES_OPCODE_OFFSET   (TL_SERDES_PARAM_OFFSET + TL_SERDES_OPCODE_SIZE)
#define TL_SERDES_CHANID_SIZE     3
#define TL_SERDES_CHANID_OFFSET   (TL_SERDES_OPCODE_OFFSET + TL_SERDES_CHANID_SIZE)
#define TL_SERDES_TOTAL_SIZE      TL_SERDES_CHANID_OFFSET


typedef struct {
	uint32_t chanid;
	uint32_t opcode;
	uint32_t param;
	uint32_t size;
	uint32_t source;
  uint32_t address;
  uint64_t data;
  uint32_t corrupt;
  uint32_t mask;
  uint32_t last;
  char buffer[256];
} TileLinkFrame;


typedef struct {
  TileLinkFrame tx_frame;
  TileLinkFrame rx_frame;

  int32_t tx_bit_offset;
  uint32_t tx_finished;
  uint32_t tx_pending;

  uint32_t rx_bit_offset;
  uint32_t rx_finished;
  uint32_t rx_pending;
} TileLinkController;

TileLinkController tl;

void TL_serialize(TileLinkFrame *frame) {
  for (uint32_t i=0; i<TL_SERDES_LAST_SIZE; i+=1) {
    frame->buffer[i] = (frame->last >> i) & 0b1;
  }
  for (uint32_t i=0; i<TL_SERDES_MASK_SIZE; i+=1) {
    frame->buffer[i+TL_SERDES_LAST_OFFSET] = (frame->mask >> i) & 0b1;
  }
  for (uint32_t i=0; i<TL_SERDES_CORRUPT_SIZE; i+=1) {
    frame->buffer[i+TL_SERDES_MASK_OFFSET] = (frame->corrupt >> i) & 0b1;
  }
  for (uint32_t i=0; i<TL_SERDES_DATA_SIZE; i+=1) {
    frame->buffer[i+TL_SERDES_CORRUPT_OFFSET] = (frame->data >> i) & 0b1;
  }
  for (uint32_t i=0; i<TL_SERDES_ADDRESS_SIZE; i+=1) {
    frame->buffer[i+TL_SERDES_DATA_OFFSET] = (frame->address >> i) & 0b1;
  }
  for (uint32_t i=0; i<TL_SERDES_SOURCE_SIZE; i+=1) {
    frame->buffer[i+TL_SERDES_ADDRESS_OFFSET] = (frame->source >> i) & 0b1;
  }
  for (uint32_t i=0; i<TL_SERDES_SIZE_SIZE; i+=1) {
    frame->buffer[i+TL_SERDES_SOURCE_OFFSET] = (frame->size >> i) & 0b1;
  }
  for (uint32_t i=0; i<TL_SERDES_PARAM_SIZE; i+=1) {
    frame->buffer[i+TL_SERDES_SIZE_OFFSET] = (frame->param >> i) & 0b1;
  }
  for (uint32_t i=0; i<TL_SERDES_OPCODE_SIZE; i+=1) {
    frame->buffer[i+TL_SERDES_PARAM_OFFSET] = (frame->opcode >> i) & 0b1;
  }
  for (uint32_t i=0; i<TL_SERDES_CHANID_SIZE; i+=1) {
    frame->buffer[i+TL_SERDES_OPCODE_OFFSET] = (frame->chanid >> i) & 0b1;
  }
}

void TL_GET(TileLinkController *tl, uint32_t address) {
  tl->tx_frame.chanid  = 0;
//  tl->tx_frame.opcode  = TL_CH_A_OPCODE_GET;  // get
  tl->tx_frame.opcode = 0;
  tl->tx_frame.param   = 0;
  tl->tx_frame.size    = 2;
  tl->tx_frame.source  = 0;
  tl->tx_frame.address = address;
  tl->tx_frame.data    = 0x0000000000000000;
  tl->tx_frame.corrupt = 0;
  tl->tx_frame.mask    = 0b00001111;
  tl->tx_frame.last    = 1;
}

int main() {
	cpu_clock_config(320);
	CLEAR_BITS(GPIOA->INPUT_EN, 0xFFFFFFFF);
	CLEAR_BITS(GPIOA->OUTPUT_EN, 0xFFFFFFFF);
	SET_BITS(GPIOA->INPUT_EN, TL_CLK | TL_MOSI_RDY | TL_MISO_DAT | TL_MISO_VAL);
	SET_BITS(GPIOA->OUTPUT_EN, TL_MOSI_DAT | TL_MOSI_VAL | TL_MISO_RDY);
	SET_BITS(GPIOA->OUTPUT_EN, DEBUG_0 | DEBUG_1);

	SET_BITS(GPIOA->OUTPUT_VAL, TL_MISO_RDY);

	uint32_t counter = 0;
	uint32_t prev_clk = 0;

	uint32_t tl_chanid = 0;
	uint32_t tl_opcode = 0;

	TL_GET(&tl, 0x00020000);
	TL_serialize(&tl.tx_frame);


	while (1) {
		uint32_t tl_clk = READ_BITS(GPIOA->INPUT_VAL, TL_CLK);
		if (!tl_clk && prev_clk) {
//			SET_BITS(GPIOA->OUTPUT_VAL, DEBUG_0);

			if (tl.tx_bit_offset == 0) {
				SET_BITS(GPIOA->OUTPUT_VAL, TL_MOSI_VAL);
			}
			else if (tl.tx_bit_offset == 122) {
				CLEAR_BITS(GPIOA->OUTPUT_VAL, TL_MOSI_VAL);
				tl.tx_bit_offset = -100;
			}
			if (tl.tx_bit_offset >= 0 && tl.tx_frame.buffer[tl.tx_bit_offset]) {
				SET_BITS(GPIOA->OUTPUT_VAL, TL_MOSI_DAT);
			}
			else {
				CLEAR_BITS(GPIOA->OUTPUT_VAL, TL_MOSI_DAT);
			}

			tl.tx_bit_offset += 1;
		}
//		else {
//			CLEAR_BITS(GPIOA->OUTPUT_VAL, DEBUG_0);
//		}
		prev_clk = tl_clk;
	}
}

