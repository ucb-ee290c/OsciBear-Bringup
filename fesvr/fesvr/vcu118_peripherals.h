#ifndef VCU118_PERIPHERALS_H
#define VCU118_PERIPHERALS_H

/** TSI Peripheral **/
// mmio region for the tsi widget
#define TSI_BASE_ADDR                  0x64006000
#define TSI_BASE_REGION_SIZE                  0x10

// mmio offsets and addresses
#define TSI_TX_OFFSET                         0x0
#define TSI_RX_OFFSET                         0x8
#define TSI_TX_ADDR               (TSI_TX_OFFSET)
#define TSI_RX_ADDR               (TSI_RX_OFFSET)

/** TSI Peripheral **/

#endif
