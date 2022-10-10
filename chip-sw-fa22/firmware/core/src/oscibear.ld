/* Copyright (c) 2022 UC Berkeley */

OUTPUT_ARCH("riscv")

ENTRY(_start)

MEMORY {
  SRAM (rwx): ORIGIN = 0x80000000, LENGTH = 24K
}

SECTIONS {
  __stack_size = DEFINED(__stack_size) ? __stack_size : 0x400;
  PROVIDE(__stack_size = __stack_size);

  __heap_size = DEFINED(__heap_size) ? __heap_size : 0x800;
  PROVIDE(__heap_size = __heap_size);
  
  PROVIDE(__boot_hart = 0);

  .text : {
    . = ALIGN(4);
    KEEP(*(.text.reset))          /* Reset routine */
    
    . = ALIGN(4);
    KEEP(*(.text.isr_routine))    /* ISR routine */
    *(.text.init)                 /* Startup code */

    *(.text .text*)               /* .text sections (code) */

    KEEP (*(.init))
    KEEP (*(.fini))
    
    _etext = .;                   /* define a global symbols at end of code */
  }> SRAM
  
  .rodata : {
    . = ALIGN(4);
    *(.rodata .rodata*)           /* .rodata sections (constants, strings, etc.) */
    . = ALIGN(4);
  }> SRAM

  /* data segment */
  .data : {
    *(.data .data*)
  }> SRAM

  .sdata : {
    __global_pointer$ = . + 0x800;
    *(.srodata.cst16) *(.srodata.cst8) *(.srodata.cst4) *(.srodata.cst2) *(.srodata*)
    *(.rodata)
    *(.sdata .sdata.* .gnu.linkonce.s.*)
    . = ALIGN(4);
  }> SRAM
  
  /* bss segment */
  .sbss : {
    *(.sbss .sbss.* .gnu.linkonce.sb.*)
    *(.scommon)
  }> SRAM

  .bss (NOLOAD) : {
    *(.bss)
    *(COMMON)
  }> SRAM

  /* thread-local data segment */
  .tdata : {
    _tdata_begin = .;
    *(.tdata)
    _tdata_end = .;
  }> SRAM
  
  .tbss : {
    *(.tbss)
    _tbss_end = .;
  }> SRAM

  /* End of uninitalized data segement */
  _end = .;

}
