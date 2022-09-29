#ifndef __TSI_ZEDBOARD_H
#define __TSI_ZEDBOARD_H

#include "tsi.h"
#include <set>

class tsi_vcu118_t : public tsi_t {
 public:
  tsi_vcu118_t(const std::vector<std::string>& args);
  ~tsi_vcu118_t();

  // Write to a particular register mapped (currently only supports the TSI MMIO)
  void write_reg(uint64_t addr, uint64_t data);
  uint64_t read_reg(uint64_t addr);

 protected:
  void read_chunk(addr_t taddr, size_t nbytes, void* dst) override;
  void write_chunk(addr_t taddr, size_t nbytes, const void* src) override;

 private:
  //TSI
  context_t host;
  context_t* target;
  //VCU118
  volatile uintptr_t* dev_vaddr;
};

#define TSI_TX_FULL (((int64_t) read_reg(TSI_TX_ADDR)) < 0)
#define TSI_RX_EMPTY (((int64_t) read_reg(TSI_RX_ADDR)) < 0)

#endif
