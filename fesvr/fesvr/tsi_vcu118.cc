// See LICENSE for license details.
//
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <assert.h>
#include <math.h>

#include "verbose.h"
#include "tsi_vcu118.h"

#include "vcu118_peripherals.h"

/**
 * Write a register using the MMIO
 *
 * @param addr address to write to (assumed to be a byte address)
 * @param data uint32_t data to write to the address
 */
void tsi_vcu118_t::write_reg(uint64_t addr, uint64_t data) {
  verbose_print(verbose_level_t::VERBOSE_MMIO, "\t" BLU "Writing " RESET "to MMIO reg: 0x%x, with data: 0x%x, %d\n", addr, data, data);

  // align addr to 64b
  dev_vaddr[addr >> 3] = data;
}

/**
 * Read a register using the MMIO
 *
 * @param addr address to read from (assumed to be a byte address)
 */
uint64_t tsi_vcu118_t::read_reg(uint64_t addr) {
  // align addr to 64b
  uint64_t data = dev_vaddr[addr >> 3];

  verbose_print(verbose_level_t::VERBOSE_MMIO, "\t" GRN "Reading " RESET "from MMIO reg: 0x%x, got data: 0x%x, %d\n", addr, data, data);
  return data;
}

/**
 * Destructor
 */
tsi_vcu118_t::~tsi_vcu118_t() {}

/**
 * Constructor
 *
 * @param args arguments to the constructor (mainly for tsi base)
 */ tsi_vcu118_t::tsi_vcu118_t(const std::vector<std::string>& args)
  : tsi_t(args)
{
  //printf("Starting HTIF...\n");

  int fd = open("/dev/mem", O_RDWR|O_SYNC);
  assert(fd != -1);

  assert(TSI_BASE_REGION_SIZE <= sysconf(_SC_PAGESIZE));

  dev_vaddr = (uintptr_t*)mmap(0, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, TSI_BASE_ADDR);
  assert(dev_vaddr != MAP_FAILED);

  //printf("Mapped %ld bytes to vaddr %p\n", sysconf(_SC_PAGESIZE), dev_vaddr);

  //printf("Done starting HTIF\n");
}

/**
 * Read a chunk of target memory
 *
 * @param taddr target address
 * @param nbytes number of bytes to read
 * @param dst where to write the read data
 */
void tsi_vcu118_t::read_chunk(addr_t taddr, size_t nbytes, void* dst)
{
  uint32_t *result = static_cast<uint32_t*>(dst);
  size_t len = nbytes / sizeof(uint32_t);

  // write the "read" command
  while (TSI_TX_FULL) {}
  write_reg(TSI_TX_ADDR, SAI_CMD_READ);

  while (TSI_TX_FULL) {}
  write_reg(TSI_TX_ADDR, taddr);

  while (TSI_TX_FULL) {}
  write_reg(TSI_TX_ADDR, 0);

  while (TSI_TX_FULL) {}
  write_reg(TSI_TX_ADDR, len == 1 ? 0 : (len - 1));

  while (TSI_TX_FULL) {}
  write_reg(TSI_TX_ADDR, 0);

  // read the response data from the "read" command
  for (size_t i = 0; i < len; i++) {
    // RX is empty if the MSB is set, so we can check if it is less than 0
    int64_t rx_value = -1;
    while (rx_value < 0) {
      rx_value = (int64_t) read_reg(TSI_RX_ADDR);
    }
    result[i] = (uint32_t) rx_value;
  }
}

/**
 * Write a chunk of target memory
 *
 * @param taddr target address
 * @param nbytes number of bytes to write
 * @param src where to get the bytes to write to target memory
 */
void tsi_vcu118_t::write_chunk(addr_t taddr, size_t nbytes, const void* src)
{
  const uint32_t *src_data = static_cast<const uint32_t*>(src);
  size_t len = nbytes / sizeof(uint32_t);

  // write the "write" command
  while (TSI_TX_FULL) {}
  write_reg(TSI_TX_ADDR, SAI_CMD_WRITE);

  while (TSI_TX_FULL) {}
  write_reg(TSI_TX_ADDR, taddr);

  while (TSI_TX_FULL) {}
  write_reg(TSI_TX_ADDR, 0);

  while (TSI_TX_FULL) {}
  write_reg(TSI_TX_ADDR, len == 1 ? 0 : (len - 1));

  while (TSI_TX_FULL) {}
  write_reg(TSI_TX_ADDR, 0);

  // write the data for the "write" command
  for (size_t i = 0; i < len; i++) {
    while (TSI_TX_FULL) {}
    write_reg(TSI_TX_ADDR, src_data[i]);
  }
}
