
#include "main.h"


#define ROCC_INSTRUCTION_0_R_R(x, rs1, rs2, func7)                                   \
  {                                                                                  \
    asm volatile(                                                                    \
        ".insn r " STR(CAT(CUSTOM_, x)) ", " STR(0x3) ", " STR(func7) ", x0, %0, %1" \
        :                                                                            \
        : "r"(rs1), "r"(rs2));                                                       \
  }

int main() {
  HAL_init();

  printf("test\n");

  asm volatile(".insn r CUSTOM_0, 7, 42, a0, a1, a2");
  while (1) {
  }
}
